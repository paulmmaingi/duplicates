#include "strSHA2.h"
#include <dirent.h>
#include <sys/stat.h>

// DATA STRUCTURES

// DICTIONARY LIKE STRUCT TO STORE FILE INFO IN ADDITION TO HASH_TABLE FOR QUICK LOOKUP (NAME, PATH, HASH, SIZE, INODE, NEXT)
typedef struct dict {
    char *name;
    char *path;
    char *hash;
    size_t size;
    long int inode;
    struct dict *next;
} dict;

// LIST LIKE STRUCT TO STORE ALREADY PRINTED HASHES (HASHES, NUM_HASHES)
typedef struct printed_hashes {
    char **hashes;
    int num_hashes;
} printed_hashes;

// FUNCTION PROTOTYPES

// CHECK IF A FILE IS HIDDEN
extern bool is_hidden_file(const char *name);

// HASH FUNCTION TO DETERMINE WHAT BUCKET A FILE SHOULD GO INTO
extern unsigned long hash_function(char *str);

// ADD A FILE TO THE HASH TABLE
extern void add_file_to_hash_table(hash_table *ht, file *f);

// READ THE DIRECTORY AND ADD FILES TO THE HASH TABLE AND DICTIONARY
extern void read_directory(char *path, hash_table *ht, option_list *ol, dict *d);

// GET THE FILES DUPLICATE TO GIVEN HASH; RETURN ARRAY OF FILES
extern file *get_files_with_hash(hash_table *ht, char *hash);

// GET THE FILES DUPLICATE TO GIVEN FILE; PRINTING THEM IN FUNCTION
extern void get_files_with_name(hash_table *ht, char *name, dict *d);

// PRINT AN ARRAY OF FILES
extern void print_file_array(file *f);

// REMOVE A FILE FROM A FILE ARRAY
extern file *remove_file_from_array(file *files, char *path);

// ADD FILE INFO TO DICTIONARY
extern void add_file_to_dict(dict *d, file *f);

// PRINT THE DICTIONARY
extern void print_dict(dict *d);

// FREE THE MEMORY ALLOCATED FOR THE DICTIONARY
extern void free_dict(dict *d);

// GET FILE INFO FROM DICTIONARY - MIGHT HAVE MULTIPLE FILES WITH SAME NAME, RETURN ARRAY OF FILES
extern file *get_files_from_dict(dict *d, char *name);

// HELPER FUNCTION TO GET UNIQUE FILES
extern file *get_unique_files(hash_table *ht, dict *d);

// LIST ALL DUPLICATE FILES - ONLY IF FILE IS A DUPLICATE THEN LIST THE DUPLICATES IN ONE LINE
extern void list_duplicates(dict *d, hash_table *ht);

// CHECK IF A HASH IS IN THE PRINTED HASHES
extern bool is_in_printed_hashes(printed_hashes *ph, char *hash);

// DEFAULT PRINTING FUNCTION
extern void default_print(hash_table *ht, dict *d, option_list *ol);

// MINIMISING FUNCTION (UNLINKS THE FILES AND CREATES HARD LINKS WITH THE SAME NAME)
extern void minimise(hash_table *ht, dict *d);