#include "structure.h"

// FUNCTION DEFINITIONS

file *new_file(char *name, char *path, size_t size, long int inode) {
    file *f = malloc(sizeof(file));
    CHECK_ALLOC(f);
    f->name = strdup(name);
    CHECK_ALLOC(f->name);
    f->path = strdup(path);
    CHECK_ALLOC(f->path);
    f->hash = "";
    f->size = size;
    f->inode = inode;
    f->next = NULL;
    return f;
}

void print_file(file *f) {
    printf("file_name: %s\n", f->name);
    printf("file_path: %s\n", f->path);
    printf("file_hash: %s\n", f->hash);
    printf("file_size: %ld bytes\n", f->size);
    printf("file_inode: %ld\n", f->inode);
    if(f->next != NULL) {
        printf("next_file: %s\n", f->next->name);
    } else {
        printf("next_file: NULL\n");
    }
    printf("\n");
}

void free_file(file *f) {
    free(f->name);
    free(f->path);
    free(f);
}

hash_table *new_hash_table(int size) {
    hash_table *ht = malloc(sizeof(hash_table));
    CHECK_ALLOC(ht);
    ht->size = size;
    ht->table = calloc(size, sizeof(hash_bucket *));
    CHECK_ALLOC(ht->table);
    for(int i = 0; i < size; i++) {
        ht->table[i] = calloc(1, sizeof(hash_bucket));
        CHECK_ALLOC(ht->table[i]);
    }
    return ht;
}

void print_hash_table(hash_table *ht) {
    printf("HASH TABLE:\n\n");
    for(int i = 0; i < ht->size; i++) {
        printf("bucket %d:\n\n", i);
        file *f = ht->table[i]->head;
        while(f != NULL) {
            print_file(f);
            f = f->next;
        }
        printf("\n");
    }
    printf("\n");
}

void free_hash_table(hash_table *ht) {
    for(int i = 0; i < ht->size; i++) {
        file *f = ht->table[i]->head;
        while(f != NULL) {
            file *temp = f;
            f = f->next;
            free_file(temp);
        }
        free(ht->table[i]);
    }
    free(ht->table);
    free(ht);
}

option_list *new_option_list() {
    option_list *ol = malloc(sizeof(option_list));
    CHECK_ALLOC(ol);
    ol->options = malloc(sizeof(option_struct *));
    CHECK_ALLOC(ol->options);
    ol->num_options = 0;
    return ol;
}

option_struct *get_option(option_list *ol, char flag) {
    for(int i = 0; i < ol->num_options; i++) {
        if(ol->options[i]->flag == flag) {
            return ol->options[i];
        }
    }
    return NULL;
}

void add_option(option_list *ol, char flag, char *optionarg) {
    // if the option already exists, add the optionarg to the optionargs array
    option_struct *opt = get_option(ol, flag);
    if(opt != NULL && optionarg != NULL) {
        opt->optionargs = realloc(opt->optionargs, (opt->num_optionargs + 1) * sizeof(char *));
        CHECK_ALLOC(opt->optionargs);
        opt->optionargs[opt->num_optionargs] = strdup(optionarg);
        CHECK_ALLOC(opt->optionargs[opt->num_optionargs]);
        opt->num_optionargs++;
        return;
    }
    // option doesnt exist, create a new option
    ol->options = realloc(ol->options, (ol->num_options + 1) * sizeof(option_struct *));
    CHECK_ALLOC(ol->options);
    ol->options[ol->num_options] = calloc(1, sizeof(option_struct));
    CHECK_ALLOC(ol->options[ol->num_options]);
    ol->options[ol->num_options]->flag = flag;
    if(optionarg != NULL) {
        ol->options[ol->num_options]->optionargs = malloc(sizeof(char *));
        CHECK_ALLOC(ol->options[ol->num_options]->optionargs);
        ol->options[ol->num_options]->optionargs[0] = strdup(optionarg);
        CHECK_ALLOC(ol->options[ol->num_options]->optionargs[0]);
        ol->options[ol->num_options]->num_optionargs = 1;
    }
    ol->num_options++;
}

void print_option_list(option_list *ol) {
    if(ol->num_options > 0){
        printf("OPTIONS(%d):\n\n", ol->num_options);
        for(int i = 0; i < ol->num_options; i++) {
            printf("%d. flag: %c\n", i+1, ol->options[i]->flag);
            if(ol->options[i]->optionargs != NULL) {
                for(int j = 0; j < ol->options[i]->num_optionargs; j++) {
                    printf("\targ%d: %s\n", j+1, ol->options[i]->optionargs[j]);
                }
            }
        }
        printf("--------------------------------------------------\n\n");
    }
}

void free_option_list(option_list *ol) {
    for(int i = 0; i < ol->num_options; i++) {
        if(ol->options[i]->optionargs != NULL) {
            for(int j = 0; j < ol->options[i]->num_optionargs; j++) {
                free(ol->options[i]->optionargs[j]);
            }
            free(ol->options[i]->optionargs);
        }
        free(ol->options[i]);
    }
    free(ol->options);
    free(ol);
}