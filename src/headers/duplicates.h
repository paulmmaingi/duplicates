#ifndef DUPLICATES_H
#define DUPLICATES_H


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


// Some Macros
#define CHECK_ALLOC(ptr) if (ptr == NULL) { perror(__func__); exit(1); }

#define CHECK_ERROR_INT(rv) if (rv < 0) { fprintf(stderr, "ERROR: [file: %s\t line: %d\t func: %s] --> %s\n", __FILE__, __LINE__, __func__, strerror(errno)); exit(1); }

#define CHECK_ERROR_PTR(rv) if (rv == NULL) { fprintf(stderr, "ERROR: [file: %s\t line: %d\t func: %s] --> %s\n", __FILE__, __LINE__, __func__, strerror(errno)); exit(1); }


#endif // DUPLICATES_H