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


#endif // BASE_H