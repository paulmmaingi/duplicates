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
            file *f = new_file(entry->d_name, fullpath);
            add_file_to_hash_table(ht, f);
            if(get_option(ol, 'l') != NULL || get_option(ol, 'f') != NULL){
                add_file_to_dict(d, f);    
            }        
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
            file *f1 = new_file(f->name, f->path);
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
            file *f = new_file(d->name, d->path);
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