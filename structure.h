#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/param.h>    // for MAXPATHLEN


//---------------------------------------------------------------------

//  A HELPFUL PREPROCESSOR MACRO TO CHECK IF ALLOCATIONS WERE SUCCESSFUL
#define CHECK_ALLOC(p) if(p == NULL) { perror(__func__); exit(EXIT_FAILURE); }

// DECLARATION OF strdup() FUNCTION
extern char *strdup(const char *s);

//---------------------------------------------------------------------

// DATA STRUCTURES

// FILE STRUCTURE (NAME, PATH, HASH, SIZE, INODE, NEXT)
typedef struct file {
    char *name;
    char *path;
    char *hash;
    size_t size;
    long int inode;
    struct file *next;
} file;

// HASH TABLE TO STORE FILES 

// (HASH TABLE IS AN ARRAY OF BUCKETS, EACH BUCKET IS A LINKED LIST OF FILES)
typedef struct hash_bucket {
    file *head;
    file *tail;
    int num_files_in_bucket;
} hash_bucket;

// HASH TABLE STRUCTURE (SIZE, TABLE)
typedef struct hash_table {
    int size;
    hash_bucket **table;
} hash_table;

// OPTIONS STRUCTURE (FLAG, OPTIONARGS, NUM_OPTIONARGS, NEXT)
typedef struct option_struct {
    char flag;
    char **optionargs;
    int num_optionargs;
    struct option_struct* next;
} option_struct;

// OPTION LIST STRUCTURE (OPTIONS, NUM_OPTIONS)
typedef struct option_list{
    option_struct **options;
    int num_options;
} option_list;

//---------------------------------------------------------------------

// FUNCTION PROTOTYPES

// CREATE A NEW FILE STRUCTURE
extern file *new_file(char *name, char *path, size_t size, long int inode);

// PRINT THE CONTENTS OF A FILE STRUCTURE
extern void print_file(file *f);

// FREE THE MEMORY ALLOCATED FOR A FILE STRUCTURE
extern void free_file(file *f);

// CREATE A NEW HASH TABLE
extern hash_table *new_hash_table(int size);

// PRINT THE CONTENTS OF A HASH TABLE
extern void print_hash_table(hash_table *ht);

// FREE THE MEMORY ALLOCATED FOR A HASH TABLE
extern void free_hash_table(hash_table *ht);

// NEW OPTION_LIST
extern option_list *new_option_list();

// GET OPTION FROM OPTION_LIST
extern option_struct *get_option(option_list *ol, char flag);

// ADD OPTION TO OPTION_LIST
extern void add_option(option_list *ol, char flag, char *optionarg);

// PRINT OPTION_LIST
extern void print_option_list(option_list *ol);

// FREE OPTION_LIST
extern void free_option_list(option_list *ol);
