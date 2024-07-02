#ifndef READ_DIR_H
#define READ_DIR_H

#include "base.h"
#include "data_structs.h"
#include "strSHA2.h"

#include <dirent.h>
#include <sys/stat.h>


// FUNCTION PROTOTYPES

// Hash function for allocating a bucket in the hash table
extern unsigned long hash_function(char *str);

// Function to check if file is hidden
extern bool isHidden(char *filename);

// Function to add a file to the hash table
extern bool addFile(hashTable *ht, fileInfo *file);

// Function to read a directory and add files to the hash table
extern void readDir(char *dirPath, hashTable *ht, optionList *optList);


#endif // READ_DIR_H