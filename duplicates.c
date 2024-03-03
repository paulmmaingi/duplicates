#include "duplicates.h"
// need to add delete by linking and also removing links also handle collisions in hash table and test and debug
struct option long_options[] = {
    {"help", no_argument, NULL, 'h'},  // works
    {"recursive", no_argument, NULL, 'r'}, // works
    {"hidden", no_argument, NULL, 'a'}, // works
    {"quiet", no_argument, NULL, 'q'},
    {"file", required_argument, NULL, 'f'}, // works
    {"hash", required_argument, NULL, 'd'}, // works
    {"list", no_argument, NULL, 'l'}, // works
    {"delete", no_argument, NULL, 'x'},
    {NULL, 0, NULL, 0}
};

#define OPTLIST "hraqf:d:lx"

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
    fprintf(stderr, "  -x, --delete\t\tDelete all duplicate files\n");
    exit(EXIT_FAILURE);
}

void debug(hash_table *ht, dict *d) {
    print_hash_table(ht);
    print_dict(d);
}


int main(int argc, char *argv[]) {
    char* progname = argv[0];
    if (argc < 2) {
        usage(progname);
    }
    
    option_list *ol = new_option_list();
    int opt;
    
    while ((opt = getopt_long(argc, argv, OPTLIST, long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                usage(argv[0]);
                break;
            case 'r':
                add_option(ol, 'r', NULL);
                break;
            case 'a':
                add_option(ol, 'a', NULL);
                break;
            case 'q':
                add_option(ol, 'q', NULL);
                break;
            case 'f':
                add_option(ol, 'f', optarg);
                break;
            case 'd':
                add_option(ol, 'd', optarg);
                break;
            case 'l':
                add_option(ol, 'l', NULL);
                break;
            case 'x':
                add_option(ol, 'x', NULL);
                break;
            default:
                usage(argv[0]);
                break;
        }
    }

    print_option_list(ol);
    

    hash_table *ht = new_hash_table(5); // prime number avoids collisions
    dict *d = calloc(1, sizeof(dict));

    // for loop to read all directories provided
    for(int i = optind; i < argc; i++) {
        read_directory(argv[i], ht, ol, d);
    }
    if(get_option(ol, 'q') == NULL && get_option(ol, 'd') == NULL && get_option(ol, 'f') == NULL && get_option(ol, 'l') == NULL){
        default_print(ht, d);
    }
    if(get_option(ol, 'd') != NULL) {
        for(int i = 0; i < get_option(ol, 'd')->num_optionargs; i++) {
            file *f = get_files_with_hash(ht, get_option(ol, 'd')->optionargs[i]);
            if(f != NULL){
                printf("Files with hash: '%s'\n\n", get_option(ol, 'd')->optionargs[i]);
                print_file_array(f);
            }
        }
    }
    if(get_option(ol, 'f') != NULL) {
        for(int i = 0; i < get_option(ol, 'f')->num_optionargs; i++) {
            get_files_with_name(ht, get_option(ol, 'f')->optionargs[i], d);
        }
    }
    if(get_option(ol, 'l') != NULL) {
        list_duplicates(d, ht);
    }

    // debug(ht, d);

    free_dict(d);
    free_hash_table(ht);
    free_option_list(ol);
    return 0;
}