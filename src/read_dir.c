#include "headers/read_dir.h"


unsigned long hash_function(char *str) {
    // djb2 hash function by Dan Bernstein
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    return hash;
}

bool isHidden(char *filename) {
    return filename[0] == '.';
}

bool addFileHashTable(hashTable *ht, fileInfo *file) {
    char *fileHash = strSHA2(file->path);
    if (fileHash == NULL) {
        return false;
    }
    file->hash = fileHash;
    // use fileHash to determine which bucket to add the file to
    unsigned long index = hash_function(fileHash) % ht->size;
    if (ht->buckets[index]->head == NULL) {
        ht->buckets[index]->head = file;
        ht->buckets[index]->tail = file;
    } else {
        ht->buckets[index]->tail->next = file;
        ht->buckets[index]->tail = file;
    }
    ht->buckets[index]->numFiles++;
    return true;
}

Set *initSet() {
    Set *newSet = calloc(1, sizeof(Set));
    CHECK_ALLOC(newSet);
    return newSet;
}

SetCollection *initSetCollection() {
    SetCollection *sc = calloc(1, sizeof(SetCollection));
    CHECK_ALLOC(sc);
    return sc;
}

bool addFileSet(SetCollection *sc, fileInfo *file) {
    char *fileHash = file->hash;
    if (fileHash == NULL) {
        return false;
    }
    for (int i = 0; i < sc->numSets; i++) {
        if (strcmp(sc->sets[i]->hash, fileHash) == 0) {
            sc->sets[i]->numFiles++;
            sc->sets[i]->files = realloc(sc->sets[i]->files, sc->sets[i]->numFiles * sizeof(fileInfo *));
            CHECK_ALLOC(sc->sets[i]->files);
            sc->sets[i]->files[sc->sets[i]->numFiles - 1] = file;
            return true;
        }
    }
    Set *newSet = initSet();
    newSet->hash = fileHash;
    newSet->files = calloc(1, sizeof(fileInfo *));
    CHECK_ALLOC(newSet->files);
    newSet->files[0] = file;
    newSet->numFiles = 1;
    sc->sets = realloc(sc->sets, (sc->numSets + 1) * sizeof(Set *));
    CHECK_ALLOC(sc->sets);
    sc->sets[sc->numSets] = newSet;
    sc->numSets++;
    return true;
}

void freeSet(Set *set) {
    if (set != NULL) {
        if (set->files != NULL) {
            free(set->files);
        }
        free(set);
    }
}

void freeSetCollection(SetCollection *sc) {
    if (sc != NULL) {
        if (sc->sets != NULL) {
            for (int i = 0; i < sc->numSets; i++) {
                freeSet(sc->sets[i]);
            }
            free(sc->sets);
        }
        free(sc);
    }
}

void printSetCollection(SetCollection *sc) {
    printf("SET COLLECTION:\n\n");
    for (int i = 0; i < sc->numSets; i++) {
        printf("Set %d (%d) [%s]:\n", i + 1, sc->sets[i]->numFiles, sc->sets[i]->hash);
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        for (int j = 0; j < sc->sets[i]->numFiles; j++) {
            printFileInfo(sc->sets[i]->files[j]);
            printf("\n");
        }
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("\n");
    }
    printf("-------------------------------------------------------------------------------------\n");
}

void readDir(char *dirPath, hashTable *ht, SetCollection *sc, optionList *optList) {
    DIR *dir = opendir(dirPath);
    if (dir == NULL) {
        perror(dirPath);
        freeHashTable(ht);
        freeSetCollection(sc);
        freeOptionList(optList);
        exit(EXIT_FAILURE);        
    }

    // each directory entry
    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        // skip . and .. directories
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        // get full path of the file
        char *fullPath = calloc(strlen(dirPath) + strlen(entry->d_name) + 2, sizeof(char));
        CHECK_ALLOC(fullPath);
        sprintf(fullPath, "%s/%s", dirPath, entry->d_name);
        struct stat fileStatBuf;
        // if cannot get file information, report error and skip the file
        if (stat(fullPath, &fileStatBuf) == -1) {
            fprintf(stderr, "Error: Cannot get file information for %s\n", fullPath);
            free(fullPath);
            continue;
        }

        // if entry is a directory
        if (S_ISDIR(fileStatBuf.st_mode)) {
            // if the recursive flag is set, recursively read the directory
            if (getOption(optList, 'r') != NULL) {
                readDir(fullPath, ht, sc, optList);
            }
        } 
        // if entry is a regular file
        else if (S_ISREG(fileStatBuf.st_mode)) {
            // if the hidden flag is not set, skip hidden files
            if (getOption(optList, 'a') == NULL && isHidden(entry->d_name)) {
                free(fullPath);
                continue;
            }
            fileInfo *newFile = initFileInfo(entry->d_name, fullPath, fileStatBuf.st_size, fileStatBuf.st_ino);

            if (!addFileHashTable(ht, newFile)) {
                fprintf(stderr, "Error: Cannot add file %s to hash table\n", fullPath);
                freeFileInfo(newFile);
            }
            if (!addFileSet(sc, newFile)) {
                fprintf(stderr, "Error: Cannot add file %s to set collection\n", fullPath);
                freeFileInfo(newFile);
            }
        }
        free(fullPath);
    }
    closedir(dir);
}

void defaultPrint(SetCollection *sc, optionList *optList) {
    int totalFiles = 0;
    size_t totalSize = 0;
    int totalUniqueFiles = 0;
    size_t totalUniqueSize = 0;
    for (int i = 0; i < sc->numSets; i++) {
        totalFiles += sc->sets[i]->numFiles;
        totalUniqueFiles++;
        totalUniqueSize += sc->sets[i]->files[0]->size;

        ino_t *encounteredInodes = calloc(sc->sets[i]->numFiles, sizeof(ino_t));
        CHECK_ALLOC(encounteredInodes);
        int numEncounteredInodes = 0;
        for (int j = 0; j < sc->sets[i]->numFiles; j++) {
            bool isEncountered = false;
            for (int k = 0; k < numEncounteredInodes; k++) {
                if (encounteredInodes[k] == sc->sets[i]->files[j]->inode) {
                    isEncountered = true;
                    break;
                }
            }
            if (!isEncountered) {
                encounteredInodes[numEncounteredInodes] = sc->sets[i]->files[j]->inode;
                numEncounteredInodes++;
            }
        }
        free(encounteredInodes);
        totalSize += sc->sets[i]->files[0]->size * numEncounteredInodes;
    }
    
    if (getOption(optList, 'q') == NULL) {
        printf("Total files found: %d\n", totalFiles);
        printf("Total size of all files found: %zu bytes ~ %zu KB ~ %zu MB\n", totalSize, totalSize / 1024, totalSize / 1024 / 1024);
        printf("Total unique files found: %d\n", totalUniqueFiles);
        printf("Total size of unique files found: %zu bytes ~ %zu KB ~ %zu MB\n", totalUniqueSize, totalUniqueSize / 1024, totalUniqueSize / 1024 / 1024);
        totalSize > totalUniqueSize ? printf("Potential space savings: %zu bytes ~ %zu KB ~ %zu MB (%.2f%%)\n", totalSize - totalUniqueSize, (totalSize - totalUniqueSize) / 1024, (totalSize - totalUniqueSize) / 1024 / 1024, (double)(totalSize - totalUniqueSize) / totalSize * 100) : printf("No potential space savings\n");
    } else {
        if (totalSize > totalUniqueSize) {
            printf("Duplicate files found. Can save %zu bytes ~ %zu KB ~ %zu MB (%.2f%% potential space savings) [redundant files: %d] [unique files: %d, total files: %d]\n", totalSize - totalUniqueSize, (totalSize - totalUniqueSize) / 1024, (totalSize - totalUniqueSize) / 1024 / 1024, (double)(totalSize - totalUniqueSize) / totalSize * 100, totalFiles - totalUniqueFiles, totalUniqueFiles, totalFiles);
        } else {
            if (totalFiles > totalUniqueFiles) {
                printf("No duplicate files found. %d files are hard linked. [unique files: %d, total files: %d]\n", totalFiles - totalUniqueFiles, totalUniqueFiles, totalFiles);
            } else {
                printf("No duplicate files found. [unique files: %d, total files: %d]\n", totalUniqueFiles, totalFiles);
            }
        }
    }
}

void listDuplicatesWithHash(char *hash, hashTable *ht) {
    unsigned long index = hash_function(hash) % ht->size;
    fileInfo *current = ht->buckets[index]->head;
    if (current == NULL) {
        printf("No duplicate files with hash %s found\n", hash);
        return;
    } else{
        printf("DUPLICATE FILES WITH HASH %s:\n\n", hash);
        while (current != NULL) {
            if (strcmp(current->hash, hash) == 0) {
                printf("%s\t[inode: %lu, size: %zu bytes ~ %zu KB ~ %zu MB]\n", current->path, current->inode, current->size, current->size / 1024, current->size / 1024 / 1024);
                printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            }
            current = current->next;
        }   
        printf("-------------------------------------------------------------------------------------\n");
    }
}

void listDuplicatesToFileNamed(char *filename, SetCollection *sc, hashTable *ht) {
    char *targetHash = NULL;
    for (int i = 0; i < sc->numSets; i++) {
        for (int j = 0; j < sc->sets[i]->numFiles; j++) {
            if (strcmp(sc->sets[i]->files[j]->filename, filename) == 0) {
                targetHash = sc->sets[i]->hash;
                break;
            }
        }
    }
    if (targetHash != NULL) {
        unsigned long index = hash_function(targetHash) % ht->size;
        fileInfo *current = ht->buckets[index]->head;
        if (current == NULL || current->next == NULL) {
            printf("No duplicate files to %s found\n", filename);
            return;
        } else {
            printf("DUPLICATE FILES TO %s:\n\n", filename);
            while (current != NULL) {
                if (strcmp(current->hash, targetHash) == 0 && strcmp(current->filename, filename) != 0) {
                    printf("%s\t[inode: %lu, size: %zu bytes ~ %zu KB ~ %zu MB]\n", current->path, current->inode, current->size, current->size / 1024, current->size / 1024 / 1024);
                    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                }
                current = current->next;
            }
            printf("-------------------------------------------------------------------------------------\n");
        }
    } else {
        printf("No file named %s found\n", filename);
        return;
    }
}

void listAllDuplicates(SetCollection *sc) {
    printf("ALL DUPLICATE FILES:\n\n");
    for (int i = 0; i < sc->numSets; i++) {

        ino_t *encounteredInodes = calloc(sc->sets[i]->numFiles, sizeof(ino_t));
        CHECK_ALLOC(encounteredInodes);
        int numEncounteredInodes = 0;
        for (int j = 0; j < sc->sets[i]->numFiles; j++) {
            bool isEncountered = false;
            for (int k = 0; k < numEncounteredInodes; k++) {
                if (encounteredInodes[k] == sc->sets[i]->files[j]->inode) {
                    isEncountered = true;
                    break;
                }
            }
            if (!isEncountered) {
                encounteredInodes[numEncounteredInodes] = sc->sets[i]->files[j]->inode;
                numEncounteredInodes++;
            }
        }
        free(encounteredInodes);

        if (sc->sets[i]->numFiles > 1) {
            printf("Set %d (%d) [%s]:", i + 1, sc->sets[i]->numFiles, sc->sets[i]->hash);
            numEncounteredInodes == 1 ? printf(" all files are hard linked\n") : printf(" %d/%d files are hard linked\n", sc->sets[i]->numFiles - numEncounteredInodes + 1, sc->sets[i]->numFiles);
            // printf("numEncounteredInodes: %d, numfiles: %d\n", numEncounteredInodes, sc->sets[i]->numFiles);
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            for (int j = 0; j < sc->sets[i]->numFiles; j++) {
                printf("%s\t[inode: %lu, size: %zu bytes ~ %zu KB ~ %zu MB]\n", sc->sets[i]->files[j]->path, sc->sets[i]->files[j]->inode, sc->sets[i]->files[j]->size, sc->sets[i]->files[j]->size / 1024, sc->sets[i]->files[j]->size / 1024 / 1024);
            }
            printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
            printf("\n");
        }
    }
    printf("-------------------------------------------------------------------------------------\n");
}

void minimiseMemoryUsage(SetCollection *sc) {
    size_t totalSize = 0;
    size_t totalUniqueSize = 0;
    int numLinks = 0;

    for (int i = 0; i < sc->numSets; i++) {     
        totalUniqueSize += sc->sets[i]->files[0]->size;

        ino_t *encounteredInodes = calloc(sc->sets[i]->numFiles, sizeof(ino_t));
        CHECK_ALLOC(encounteredInodes);
        int numEncounteredInodes = 0;
        for (int j = 0; j < sc->sets[i]->numFiles; j++) {
            bool isEncountered = false;
            for (int k = 0; k < numEncounteredInodes; k++) {
                if (encounteredInodes[k] == sc->sets[i]->files[j]->inode) {
                    isEncountered = true;
                    break;
                }
            }
            if (!isEncountered) {
                encounteredInodes[numEncounteredInodes] = sc->sets[i]->files[j]->inode;
                numEncounteredInodes++;
            }
        }
        free(encounteredInodes);
        totalSize += sc->sets[i]->files[0]->size * numEncounteredInodes;

        if (sc->sets[i]->numFiles > 1) {
            for (int j = 1; j < sc->sets[i]->numFiles; j++) {
                if (sc->sets[i]->files[j]->inode != sc->sets[i]->files[0]->inode) {
                    if (unlink(sc->sets[i]->files[j]->path) == -1) {
                        fprintf(stderr, "Error: Cannot unlink file %s\n", sc->sets[i]->files[j]->path);
                    } else {
                        if (link(sc->sets[i]->files[0]->path, sc->sets[i]->files[j]->path) == -1) {
                            fprintf(stderr, "Error: Cannot link file %s to %s\n", sc->sets[i]->files[0]->path, sc->sets[i]->files[j]->path);
                        }
                        numLinks++;                     
                    }
                } else {
                    fprintf(stderr, "File %s is already hard linked to %s. (inode: %lu). Skipping...\n", sc->sets[i]->files[j]->path, sc->sets[i]->files[0]->path, sc->sets[i]->files[0]->inode);
                }
            }
        }
    } 
    if (numLinks == 0) {
        printf("Space is already minimised\n");
        return;
    }
    printf("Total files hard linked: %d\n", numLinks);
    printf("Space saved: %zu bytes ~ %zu KB ~ %zu MB (%.2f%%)\n", totalSize - totalUniqueSize, (totalSize - totalUniqueSize) / 1024, (totalSize - totalUniqueSize) / 1024 / 1024, (double)(totalSize - totalUniqueSize) / totalSize * 100);
}