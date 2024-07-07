#ifndef BASE_H
#define BASE_H


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

// Some Macros
#define CHECK_ALLOC(ptr) if (ptr == NULL) { perror(__func__); exit(1); }

#define HASH_TABLE_SIZE 5   // Prime num avoids clustering

#endif // BASE_H