#include "duplicates.h"
// help, recursive, hidden, quiet. need to add file, hash, list, delete also handle collisions in hash table
struct option long_options[] = {
    {"help", no_argument, NULL, 'h'}, 
    {"recursive", no_argument, NULL, 'r'},
    {"hidden", no_argument, NULL, 'a'},
    {"quiet", no_argument, NULL, 'q'},
    {"file", required_argument, NULL, 'f'},
    {"hash", required_argument, NULL, 'd'},
    {"list", no_argument, NULL, 'l'},
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

    if(get_option(ol, 'q') == NULL && get_option(ol, 'd') == NULL && get_option(ol, 'f') == NULL && get_option(ol, 'l') == NULL && get_option(ol, 'x') == NULL){
        print_option_list(ol);
    }

        
    
    hash_table *ht = new_hash_table(5); // prime number avoids collisions

    // for loop to read all directories provided
    for(int i = optind; i < argc; i++) {
        read_directory(argv[i], ht, ol);
    }
    if(get_option(ol, 'q') == NULL && get_option(ol, 'd') == NULL && get_option(ol, 'f') == NULL && get_option(ol, 'l') == NULL && get_option(ol, 'x') == NULL) {
        print_hash_table(ht);
    }
    if(get_option(ol, 'd') != NULL) {
        for(int i = 0; i < get_option(ol, 'd')->num_optionargs; i++) {
            file *f = get_files_with_hash(ht, get_option(ol, 'd')->optionargs[i]);
            print_file_array(f);
        }
    }
  
    
    free_hash_table(ht);
    free_option_list(ol);
    return 0;
}