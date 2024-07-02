#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H


#include "base.h"

#include <sys/types.h>


// DEFINITIONS OF STRUCTS USED IN THE PROGRAM

// Struct to store file info in a linked list (filename, path, hash, size, inode, next)
typedef struct fileInfo {
    char *filename;
    char *path;
    char *hash;
    size_t size;
    ino_t inode;
    struct fileInfo *next;
} fileInfo;

// Hash table struct which will store buckets of fileInfo structs, each bucket/linked list is a set of duplicate files with the same hash

// Bucket struct to store a linked list of fileInfo structs (head, tail, numFiles)
typedef struct bucket {
    fileInfo *head;
    fileInfo *tail;
    int numFiles;
} bucket;

// Hash table struct to store buckets of fileInfo structs (buckets, size)
typedef struct hashTable {
    bucket **buckets;
    int size;
} hashTable;

// Option struct (flag, args, numArgs)
typedef struct _option {
    char flag;
    char **args;
    int numArgs;
} _option;

// Struct to store the command line options and their args (options, numOptions)
typedef struct optionList {
    _option *options;
    int numOptions;
} optionList;


// FUNCTION PROTOTYPES

// Function to initialize a new fileInfo struct
extern fileInfo *initFileInfo(char *filename, char *path, size_t size, ino_t inode);

// Function to print the contents of a fileInfo struct
extern void printFileInfo(fileInfo *file);

// Function to free the memory allocated for a fileInfo struct
extern void freeFileInfo(fileInfo *file);

// Function to initialize a new bucket struct (linked list of fileInfo structs)
extern bucket *initBucket();

// Function to initialize a new hashTable struct (array of bucket structs)
extern hashTable *initHashTable(int size);

// Function to print the contents of a hashTable struct
extern void printHashTable(hashTable *ht);

// Function to free the memory allocated for a hashTable struct
extern void freeHashTable(hashTable *ht);

// Function to initialize a new optionList struct
extern optionList *initOptionList();

// Function to print the contents of an optionList struct
extern void printOptionList(optionList *optList);

// Function to free the memory allocated for an optionList struct
extern void freeOptionList(optionList *optList);

// Function to get an option from an optionList struct
extern _option *getOption(optionList *optList, char flag);

// Function to add a new option to an optionList struct
extern bool addOption(optionList *optList, char flag, char *arg);


#endif // DATA_STRUCTS_H