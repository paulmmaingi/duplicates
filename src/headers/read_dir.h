#ifndef READ_DIR_H
#define READ_DIR_H

#include "base.h"
#include "data_structs.h"
#include "strSHA2.h"

#include <dirent.h>
#include <sys/stat.h>


// DEFINITIONS OF STRUCTS USED IN THE PROGRAM

// Set struct to store files with same hash in same set (hash, files, numFiles)
typedef struct Set {
    char *hash;
    fileInfo **files;
    int numFiles;
} Set;

// SetCollection struct to store all sets of files (sets, numSets) - linked list of sets
typedef struct SetCollection {
    Set **sets;
    int numSets;
} SetCollection;


// FUNCTION PROTOTYPES

// Hash function for allocating a bucket in the hash table
extern unsigned long hash_function(char *str);

// Function to check if file is hidden
extern bool isHidden(char *filename);

// Function to add a file to the hash table
extern bool addFileHashTable(hashTable *ht, fileInfo *file);

// Function to initialize a new set
extern Set *initSet();

// Function to initialize a new set collection
extern SetCollection *initSetCollection();

// Function to add a file to a set in the set collection
extern bool addFileSet(SetCollection *sc, fileInfo *file);

// Function to free a set
extern void freeSet(Set *set);

// Function to free a set collection
extern void freeSetCollection(SetCollection *sc);

// Function to print the contents of a set collection
extern void printSetCollection(SetCollection *sc);

// Function to read a directory and add files to the hash table
extern void readDir(char *dirPath, hashTable *ht, SetCollection *sc, optionList *optList);

// Function for the default action of the program
extern void defaultPrint(SetCollection *sc, optionList *optList);

// Function to list the relative pathnames of all files with the given hash
extern void listDuplicatesWithHash(char *hash, hashTable *ht);


#endif // READ_DIR_H