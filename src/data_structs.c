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
    newFile->hash = "";
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
    printf("\n");
}

void freeFileInfo(fileInfo *file) {
    if (file != NULL) {
        free(file->filename);
        free(file->path);
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
    printf("Hash Table:\n\n");
    for (int i = 0; i < ht->size; i++) {
        printf("Bucket %d:\n", i);
        fileInfo *current = ht->buckets[i]->head;
        while (current != NULL) {
            printFileInfo(current);
            current = current->next;
        }
        printf("\n");
    }
    printf("\n");
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