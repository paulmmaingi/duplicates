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
        d->next = NULL;
    } else {
        dict *d1 = malloc(sizeof(dict));
        CHECK_ALLOC(d1);
        d1->name = strdup(f->name);
        CHECK_ALLOC(d1->name);
        d1->path = strdup(f->path);
        CHECK_ALLOC(d1->path);
        d1->hash = strdup(f->hash);
        CHECK_ALLOC(d1->hash);
        d1->size = f->size;
        d1->next = d->next;
        d->next = d1;
    }

}

void print_dict(dict *d) {
    printf("DICTIONARY:\n\n");
    while(d != NULL) {
        printf("file_name: %s\n", d->name);
        printf("file_path: %s\n", d->path);
        printf("file_hash: %s\n", d->hash);
        printf("file_size: %ld bytes\n", d->size);
        printf("\n");
        d = d->next;
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

        if (S_ISDIR(sb.st_mode)) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            // recursive call to read_directory if -r option is set
            if(get_option(ol, 'r') != NULL) {
                read_directory(fullpath, ht, ol, d);
            }
        } 
        else if (S_ISREG(sb.st_mode))
        {
            // hidden files are not added to the hash table if -a option is not set
            if(is_hidden_file(entry->d_name) && get_option(ol, 'a') == NULL) {
                continue;
            }
            file *f = new_file(entry->d_name, fullpath, sb.st_size);
            add_file_to_hash_table(ht, f);
            add_file_to_dict(d, f);    
        }
    }
    
    closedir(dir);
}

file *get_files_with_hash(hash_table *ht, char *hash) {
    int index = hash_function(hash) % ht->size;
    file *f = ht->table[index]->head;
    file *files = NULL;
    while(f != NULL) {
        if(strcmp(f->hash, hash) == 0) {
            file *f1 = new_file(f->name, f->path, f->size);
            f1->hash = f->hash;
            f1->next = files;
            files = f1;
        }
        f = f->next;
    }
    return files;
}

file *get_files_from_dict(dict *d, char *name) {
    file *files = NULL;
    while(d != NULL) {
        if(strcmp(d->name, name) == 0) {
            file *f = new_file(d->name, d->path, 0);
            f->hash = d->hash;
            f->next = files;
            files = f;
        }
        d = d->next;
    }
    return files;
}

void get_files_with_name(hash_table *ht, char *name, dict *d) {
    file *files = get_files_from_dict(d, name);
    file *f = files;
    while(f != NULL) {
        print_file_array(get_files_with_hash(ht, f->hash));
        f = f->next;
    }    
}

void print_file_array(file *f) {
    printf("-------------------------------------------------\n");
    while(f != NULL) {
        print_file(f);
        f = f->next;
    }
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
    printed_hashes *seen_files = malloc(sizeof(printed_hashes));
    CHECK_ALLOC(seen_files);
    seen_files->hashes = malloc(sizeof(char *));
    CHECK_ALLOC(seen_files->hashes);
    seen_files->num_hashes = 0;

    file *unique_files = NULL;
    dict *d1 = d;
    while(d1 != NULL) {
        if(!is_in_printed_hashes(seen_files, d1->hash)) {
            file *f = get_files_with_hash(ht, d1->hash);
            file *f1 = new_file(f->name, f->path, f->size);
            f1->hash = f->hash;
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
    free(seen_files->hashes);
    free(seen_files);
    return unique_files;
}

void list_duplicates(dict *d, hash_table *ht) {
    file *unique_files = get_unique_files(ht, d);
    file *f = unique_files;
    while(f != NULL) {
        file *f1 = get_files_with_hash(ht, f->hash);
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

void default_print(hash_table *ht, dict *d) {
    // WORK OUT NUMBER OF FILES
    int num_files = 0;

    for(int i = 0; i<ht->size; i++) {\
        num_files += ht->table[i]->num_files_in_bucket;
    }
    printf("Number of files found: %d\n", num_files);

    // WORK OUT TOTAL SIZE OF FILES
    size_t total_size = 0;

    for(int i = 0; i<ht->size; i++) {
        file *f = ht->table[i]->head;
        while(f != NULL) {
            total_size += f->size;
            f = f->next;
        }
    }
    printf("Total size of files found: %ld bytes\n", total_size);
    
    // WORK OUT NUMBER OF UNIQUE FILES, ADD TO UNIQUE FILES ARRAY
    file *unique_files = get_unique_files(ht, d);
    // print_file_array(unique_files);
    file *f = unique_files;
    int num_unique_files = 0;
    size_t total_unique_size = 0;
    while(f != NULL) {
        num_unique_files++;
        total_unique_size += f->size;
        f = f->next;
    }
    printf("Number of unique files found: %d\n", num_unique_files);
    printf("Total size of unique files found: %ld bytes\n", total_unique_size);
    free_file(unique_files);
}
        