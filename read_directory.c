#include "read_directory.h"

// FUNCTION DEFINITIONS

bool is_hidden_file(const char *name) {
    return name[0] == '.';
}

unsigned long hash_function(char *str) {
    // djb2 hash function by Dan Bernstein
    
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void add_file_to_hash_table(hash_table *ht, file *f) {
    char *hash = strSHA2(f->path);
    f->hash = hash;
    int index = hash_function(hash) % ht->size;
    if(ht->table[index]->head == NULL) {
        ht->table[index]->head = f;
        ht->table[index]->tail = f;
    } else {
        ht->table[index]->tail->next = f;
        ht->table[index]->tail = f;
    }
    ht->table[index]->num_files_in_bucket++;
}

void add_file_to_dict(dict *d, file *f) {
    if(d->name == NULL) {
        d->name = strdup(f->name);
        CHECK_ALLOC(d->name);
        d->path = strdup(f->path);
        CHECK_ALLOC(d->path);
        d->hash = strdup(f->hash);
        CHECK_ALLOC(d->hash);
        d->size = f->size;
        d->inode = f->inode;
        d->next = NULL;
        return;
    }
    dict *d1 = malloc(sizeof(dict));
    CHECK_ALLOC(d1);
    d1->name = strdup(f->name);
    CHECK_ALLOC(d1->name);
    d1->path = strdup(f->path);
    CHECK_ALLOC(d1->path);
    d1->hash = strdup(f->hash);
    CHECK_ALLOC(d1->hash);
    d1->size = f->size;
    d1->inode = f->inode;
    d1->next = d->next;
    d->next = d1;
}

void print_dict(dict *d) {
    dict *d1 = d;
    printf("DICTIONARY:\n\n");
    while(d1 != NULL) {
        printf("file_name: %s\n", d1->name);
        printf("file_path: %s\n", d1->path);
        printf("file_hash: %s\n", d1->hash);
        printf("file_size: %ld bytes\n", d1->size);
        printf("file_inode: %ld\n", d1->inode);
        printf("\n");
        d1 = d1->next;
    }
}

void free_dict(dict *d) {
    dict *d1;
    while(d != NULL) {
        d1 = d;
        d = d->next;
        free(d1);
    }
}

void read_directory(char *path, hash_table *ht, option_list *ol, dict *d) {
    DIR *dir;
    struct dirent *entry;

    dir = opendir(path);
    if (dir == NULL) {
        perror(path);
        exit(EXIT_FAILURE);
    }

    char fullpath[MAXPATHLEN];
    while ((entry = readdir(dir)) != NULL) {
        struct stat sb;
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        if (stat(fullpath, &sb) == -1) {
            perror(fullpath);
            exit(EXIT_FAILURE);
        }

        // if the entry is a directory
        if (S_ISDIR(sb.st_mode)) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            // recursive call to read_directory if -r option is set
            if(get_option(ol, 'r') != NULL) {
                read_directory(fullpath, ht, ol, d);
            }
        } 
        // if the entry is a regular file
        else if (S_ISREG(sb.st_mode))
        {
            // hidden files are not added to the hash table if -a option is not set
            if(is_hidden_file(entry->d_name) && get_option(ol, 'a') == NULL) {
                continue;
            }
            file *f = new_file(entry->d_name, fullpath, sb.st_size, sb.st_ino);
            add_file_to_hash_table(ht, f);
            add_file_to_dict(d, f);    
        }
    }
    closedir(dir);
}

file *get_files_with_hash(hash_table *ht, char *hash) {
    int index = hash_function(hash) % ht->size;
    file *f = ht->table[index]->head;
    if(f == NULL) {
        return NULL;
    }
    file *files = NULL;
    while(f != NULL) {
        if(strcmp(f->hash, hash) == 0) {
            file *f1 = new_file(f->name, f->path, f->size, f->inode);
            f1->hash = strdup(f->hash);
            CHECK_ALLOC(f1->hash);
            f1->next = files;
            files = f1;
        }
        f = f->next;
    }
    return files;
}

file *get_files_from_dict(dict *d, char *name) {
    file *files = NULL;
    dict *d1 = d;
    while(d1 != NULL) {
        if(strcmp(d1->name, name) == 0) {
            file *f = new_file(d1->name, d1->path, d1->size, d1->inode);
            f->hash = d1->hash;
            f->next = files;
            files = f;
        }
        d1 = d1->next;
    }
    return files;
}

void get_files_with_name(hash_table *ht, char *name, dict *d) {
    file *files = get_files_from_dict(d, name);
    file *f = files;
    while(f != NULL) {
        if(f->hash != NULL) {
            printf("Files duplicate to: %s\n\n", f->path);
            print_file_array(remove_file_from_array(get_files_with_hash(ht, f->hash), f->path));
        }
        f = f->next;
    }    
}

file *remove_file_from_array(file *files, char *path) {
    if(files == NULL) {
        return NULL;
    }

    // If the first file is the one to remove
    if(strcmp(files->path, path) == 0) {
        file *next_file = files->next;
        free_file(files);
        return next_file;
    }

    file *f1 = files;
    file *f2 = files->next;
    while(f2 != NULL) {
        if(strcmp(f2->path, path) == 0) {
            f1->next = f2->next;
            free_file(f2);
            return files;
        }
        f1 = f2;
        f2 = f2->next;
    }

    return files;
}

void print_file_array(file *f) {
    // IMPLEMENT PRINTING INODE AND LINKED FILES SOMEHOW
    while(f != NULL) {
        printf("%s\n", f->path);
        // print_file(f);
        f = f->next;
    }
    printf("-------------------------------------------------\n");
}

bool is_in_printed_hashes(printed_hashes *ph, char *hash) {
    for(int i = 0; i < ph->num_hashes; i++) {
        if(strcmp(ph->hashes[i], hash) == 0) {
            return true;
        }
    }
    return false;
}

file *get_unique_files(hash_table *ht, dict *d) {
    printed_hashes *seen_files = calloc(1, sizeof(printed_hashes));
    CHECK_ALLOC(seen_files);
    seen_files->hashes = malloc(sizeof(char *));
    CHECK_ALLOC(seen_files->hashes);

    file *unique_files = NULL;
    dict *d1 = d;
    while(d1 != NULL) {
        if(!is_in_printed_hashes(seen_files, d1->hash)) {
            file *f1 = new_file(d1->name, d1->path, d1->size, d1->inode);
            f1->hash = strdup(d1->hash);
            CHECK_ALLOC(f1->hash);
            f1->next = unique_files;
            unique_files = f1;
            seen_files->hashes = realloc(seen_files->hashes, (seen_files->num_hashes + 1) * sizeof(char *));
            CHECK_ALLOC(seen_files->hashes);
            seen_files->hashes[seen_files->num_hashes] = strdup(d1->hash);
            CHECK_ALLOC(seen_files->hashes[seen_files->num_hashes]);
            seen_files->num_hashes++;
        }
        d1 = d1->next;
    }
    for(int i = 0; i < seen_files->num_hashes; i++) {
        free(seen_files->hashes[i]);
    }
    free(seen_files->hashes);
    free(seen_files);
    return unique_files;
}

void list_duplicates(dict *d, hash_table *ht) {
    file *unique_files = get_unique_files(ht, d);
    file *f = unique_files;
    while(f != NULL) {
        file *f1 = get_files_with_hash(ht, f->hash);
        // if there are duplicates for the file
        if(f1->next != NULL) {
            printf("DUPLICATES: ");
            while(f1 != NULL) {
                printf("%s\t", f1->path);
                f1 = f1->next;
            }
            printf("\n");
        }
        f = f->next;
    }
    free_file(unique_files);
}

void default_print(hash_table *ht, dict *d, option_list *ol) {
    int quiet = 0;
    if(get_option(ol, 'q') != NULL) {
        quiet = 1;
    }

    // WORK OUT NUMBER OF FILES
    int num_files = 0;

    for(int i = 0; i<ht->size; i++) {\
        num_files += ht->table[i]->num_files_in_bucket;
    }
    quiet == 0 ? printf("Number of files found: %d\n", num_files) : 0;
     
    // WORK OUT TOTAL SIZE OF FILES (ONLY INCLUDE HARD LINKED FILES ONCE)
    size_t total_size = 0;
    ino_t *inode_array = calloc(num_files, sizeof(ino_t));
    CHECK_ALLOC(inode_array);
    int inode_count = 0;
    dict *d1 = d;
    while(d1 != NULL) {
        bool found = false;
        for(int i = 0; i < inode_count; i++) {
            if(inode_array[i] == d1->inode) {
                found = true;
                break;
            }
        }
        if(!found) {
            inode_array[inode_count] = d1->inode;
            inode_count++;
            total_size += d1->size;
        }
        d1 = d1->next;
    }
    free(inode_array);
    quiet == 0 ? printf("Total size of files found: %ld bytes, %ld KB, %ld MB\n", total_size, total_size/1024, total_size/1024/1024) : 0;
    
    // WORK OUT NUMBER OF UNIQUE FILES, ADD TO UNIQUE FILES ARRAY
    file *unique_files = get_unique_files(ht, d);
    file *f = unique_files;

    int num_unique_files = 0;
    size_t total_unique_size = 0;

    while(f != NULL) {
        num_unique_files++;
        total_unique_size += f->size;
        f = f->next;
    }
    quiet == 0 ? printf("Number of unique files found: %d\n", num_unique_files) : 0;
    quiet == 0 ? printf("Total size of unique files found: %ld bytes, %ld KB, %ld MB\n", total_unique_size, total_unique_size/1024, total_unique_size/1024/1024) : 0;

    // QUIET MODE
    if(quiet == 1) {
        if(num_files > num_unique_files) {
            printf("Duplicate files found\n%ld bytes wasted\n", total_size - total_unique_size);
        } else {
            printf("No duplicate files found\n");
        }
    }
    free_file(unique_files);
}
        