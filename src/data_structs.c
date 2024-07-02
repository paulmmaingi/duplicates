#include "headers/data_structs.h"


fileInfo *initFileInfo(char *filename, char *path, size_t size, ino_t inode) {
    fileInfo *newFile = calloc(1, sizeof(fileInfo));
    CHECK_ALLOC(newFile);
    newFile->filename = strdup(filename);
    CHECK_ALLOC(newFile->filename);
    newFile->path = strdup(path);
    CHECK_ALLOC(newFile->path);
    newFile->size = size;
    newFile->inode = inode;
    newFile->hash = NULL;
    newFile->next = NULL;
    return newFile;
}

void printFileInfo(fileInfo *file) {
    printf("Filename: %s\n", file->filename);
    printf("Path: %s\n", file->path);
    printf("Size: %zu\n", file->size);
    printf("Inode: %lu\n", file->inode);
    printf("Hash: %s\n", file->hash);
    if (file->next != NULL) {
        printf("Next: %s\n", file->next->filename);
    } else {
        printf("Next: NULL\n");
    }
}

void freeFileInfo(fileInfo *file) {
    if (file != NULL) {
        free(file->filename);
        free(file->path);
        free(file->hash);
        free(file);
    }
}

bucket *initBucket() {
    bucket *newBucket = calloc(1, sizeof(bucket));
    CHECK_ALLOC(newBucket);
    return newBucket;
}

hashTable *initHashTable(int size) {
    hashTable *newHashTable = calloc(1, sizeof(hashTable));
    CHECK_ALLOC(newHashTable);
    newHashTable->size = size;
    newHashTable->buckets = calloc(size, sizeof(bucket *));
    CHECK_ALLOC(newHashTable->buckets);
    for (int i = 0; i < size; i++) {
        newHashTable->buckets[i] = initBucket();
    }
    return newHashTable;
}

void printHashTable(hashTable *ht) {
    printf("HASH TABLE:\n\n");
    for (int i = 0; i < ht->size; i++) {
        printf("Bucket %d (%d):\n", i, ht->buckets[i]->numFiles);
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        fileInfo *current = ht->buckets[i]->head;
        while (current != NULL) {
            printFileInfo(current);
            printf("\n");
            current = current->next;
        }
        printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
        printf("\n");
    }
    printf("-------------------------------------------------------------------------------------\n");
}

void freeHashTable(hashTable *ht) {
    for (int i = 0; i < ht->size; i++) {
        fileInfo *current = ht->buckets[i]->head;
        while (current != NULL) {
            fileInfo *temp = current;
            current = current->next;
            freeFileInfo(temp);
        }
        free(ht->buckets[i]);
    }
    free(ht->buckets);
    free(ht);
}

optionList *initOptionList() {
    optionList *newOptionList = calloc(1, sizeof(optionList));
    CHECK_ALLOC(newOptionList);
    return newOptionList;
}

void printOptionList(optionList *ol) {
    if (ol->numOptions > 0) {
        printf("-------------------------------------------------------------------------------------\n");
        printf("OPTIONS(%d):\n\n", ol->numOptions);
        for (int i = 0; i < ol->numOptions; i++) {
            printf("%d. flag: %c\n", i+1, ol->options[i].flag);
            if (ol->options[i].numArgs > 0) {
                for (int j = 0; j < ol->options[i].numArgs; j++) {
                    printf("\targ%d: %s\n", j+1, ol->options[i].args[j]);
                }
            }
        }
        printf("-------------------------------------------------------------------------------------\n");
    }
}

void freeOptionList(optionList *ol) {
    if (ol != NULL) {
        for (int i = 0; i < ol->numOptions; i++) {
            for (int j = 0; j < ol->options[i].numArgs; j++) {
                free(ol->options[i].args[j]);
            }
            free(ol->options[i].args);
        }
        free(ol->options);
        free(ol);
    }
}

_option *getOption(optionList *ol, char flag) {
    for (int i = 0; i < ol->numOptions; i++) {
        if (ol->options[i].flag == flag) {
            return &ol->options[i];
        }
    }
    return NULL;
}

bool addOption(optionList *ol, char flag, char *arg) {
    // if option already exists, add arg to the args array; if arg is NULL skip
    _option *opt = getOption(ol, flag);
    if (opt != NULL) {
        if (arg != NULL) {
            char **newArgs = realloc(opt->args, (opt->numArgs + 1) * sizeof(char *));
            CHECK_ALLOC(newArgs);
            opt->args = newArgs;
            opt->args[opt->numArgs] = strdup(arg);
            CHECK_ALLOC(opt->args[opt->numArgs]);
            opt->numArgs++;
        }
        return true;
    }
    // if option does not exist, create a new option
    _option *newOption = realloc(ol->options, (ol->numOptions + 1) * sizeof(_option));
    CHECK_ALLOC(newOption);
    ol->options = newOption;
    ol->options[ol->numOptions].flag = flag;
    ol->options[ol->numOptions].args = calloc(1, sizeof(char *));
    CHECK_ALLOC(ol->options[ol->numOptions].args);
    if (arg != NULL) {
        ol->options[ol->numOptions].args[0] = strdup(arg);
        CHECK_ALLOC(ol->options[ol->numOptions].args[0]);
        ol->options[ol->numOptions].numArgs = 1;
    } else {
        ol->options[ol->numOptions].numArgs = 0;
    }
    ol->numOptions++;
    return true;
}