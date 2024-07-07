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
        totalSize += sc->sets[i]->files[0]->size * sc->sets[i]->numFiles;
        totalUniqueFiles++;
        totalUniqueSize += sc->sets[i]->files[0]->size;
    }
    if (getOption(optList, 'q') == NULL) {
        printf("Total files found: %d\n", totalFiles);
        printf("Total size of all files found: %zu bytes ~ %zu KB ~ %zu MB\n", totalSize, totalSize / 1024, totalSize / 1024 / 1024);
        printf("Total unique files found: %d\n", totalUniqueFiles);
        printf("Total size of unique files found: %zu bytes ~ %zu KB ~ %zu MB\n", totalUniqueSize, totalUniqueSize / 1024, totalUniqueSize / 1024 / 1024);
        totalSize > totalUniqueSize ? printf("Space savings: %zu bytes ~ %zu KB ~ %zu MB (%.2f%%)\n", totalSize - totalUniqueSize, (totalSize - totalUniqueSize) / 1024, (totalSize - totalUniqueSize) / 1024 / 1024, (double)(totalSize - totalUniqueSize) / totalSize * 100) : printf("No space savings\n");
    } else {
        if (totalSize > totalUniqueSize) {
            printf("Duplicate files found. Can save %zu bytes ~ %zu KB ~ %zu MB (%.2f%% space savings) [redundant files: %d] [unique files: %d, total files: %d]\n", totalSize - totalUniqueSize, (totalSize - totalUniqueSize) / 1024, (totalSize - totalUniqueSize) / 1024 / 1024, (double)(totalSize - totalUniqueSize) / totalSize * 100, totalFiles - totalUniqueFiles, totalUniqueFiles, totalFiles);
        } else {
            if (totalFiles > totalUniqueFiles) {
                printf("No duplicate files found. %d files are hard linked. [unique files: %d, total files: %d]\n", totalFiles - totalUniqueFiles, totalUniqueFiles, totalFiles);
            } else {
                printf("No duplicate files found. [unique files: %d, total files: %d]\n", totalUniqueFiles, totalFiles);
            }
        }
    }
}