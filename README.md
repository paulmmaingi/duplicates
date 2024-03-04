# duplicates #

This is an implementation of the CITS2002 Project 2 2021. duplicates is a command line utility program to locate and report duplicate files in, and below, a named directory. It makes use of hard links to minimise the space taken up by storing the duplicate files. For more info, see [CITS2002 Systems Programming - Project 2 2021](https://teaching.csse.uwa.edu.au/units/CITS2002/past-projects/p2021-2/summary.php).

## Options ##

- `-h, --help`: Displays the program help message showing usage and the options.
- `-r, --recursive`: This option search directories recursively, i.e. includes subdirectories.
- `-a, --hidden`: Includes hidden files (which usually start with a '.' character in Unix/Linux file systems).
- `-q, --quiet`: Returns whether or not the given directory(ies) contain duplicates or not and how much space could be saved.
- `-f, --file <file>`: Returns files duplicate to the ones with the given name (supports multiple files having the same name).
- `-d, --hash <hash>`: Returns all the files with the given hash.
- `-l, --list`: Lists all duplicate files in sets.
- `-m, --minimise`: Minimises the memory usage by hard linking duplicate files.

## Usage ##

1. **Compile the program with make:**

    ```bash
    make
    ```

2. **Execute to detect duplicate files providing the options you need:**

    ```bash
    ./duplicates [options] directory1 directory2 [directory3 ...]
    ```
