#include "headers/duplicates.h"

struct option long_options[] = {
    {"help", no_argument, NULL, 'h'}, 
    {"recursive", no_argument, NULL, 'r'},
    {"hidden", no_argument, NULL, 'a'},
    {"quiet", no_argument, NULL, 'q'},
    {"file", required_argument, NULL, 'f'},
    {"hash", required_argument, NULL, 'd'},
    {"list", no_argument, NULL, 'l'},
    {"minimise", no_argument, NULL, 'm'},
    {NULL, 0, NULL, 0}
};

#define OPTLIST "hraqf:d:lm"

void usage(char *progname) {
    fprintf(stderr, "Usage: %s [options] <directory1> <directory2> ...\n", progname);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "  -h, --help\t\tDisplay this help message\n");
    fprintf(stderr, "  -r, --recursive\tSearch directories recursively\n");
    fprintf(stderr, "  -a, --hidden\t\tSearch all files, including hidden files\n");
    fprintf(stderr, "  -q, --quiet\t\tDo not print anything to the standard output\n");
    fprintf(stderr, "  -f, --file <file>\tOnly search for files with the given name\n");
    fprintf(stderr, "  -d, --hash <hash>\tOnly search for files with the given hash\n");
    fprintf(stderr, "  -l, --list\t\tList all duplicate files\n");
    fprintf(stderr, "  -m, --minimise\tMinimise the memory usage by hard linking duplicate files\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
    char* progname = argv[0];
    if (argc < 2) {
        usage(progname);
    }

    optionList *options = initOptionList();
    int opt;
    while ((opt = getopt_long(argc, argv, OPTLIST, long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                usage(progname);
                break;
            case 'r':
                addOption(options, 'r', NULL);
                break;
            case 'a':
                addOption(options, 'a', NULL);
                break;
            case 'q':
                addOption(options, 'q', NULL);
                break;
            case 'f':
                addOption(options, 'f', optarg);
                break;
            case 'd':
                addOption(options, 'd', optarg);
                break;
            case 'l':
                addOption(options, 'l', NULL);
                break;
            case 'm':
                addOption(options, 'm', NULL);
                break;
            default:
                usage(progname);
                break;
        }
    }

    printOptionList(options);

    hashTable *ht = initHashTable(HASH_TABLE_SIZE);
    SetCollection *sc = initSetCollection();

    if (ht == NULL) {
        fprintf(stderr, "Error: Cannot allocate memory for hash table\n");
        exit(EXIT_FAILURE);
    }

    for (int i = optind; i < argc; i++) {
        readDir(argv[i], ht, sc, options);
    }

    printHashTable(ht);
    printSetCollection(sc);

    freeHashTable(ht);
    freeSetCollection(sc);

    freeOptionList(options);


    return 0;
}