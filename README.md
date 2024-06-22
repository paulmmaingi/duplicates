# duplicates

`duplicates` is a command-line utility designed to identify and report duplicate files within a specified directory and its subdirectories. By leveraging hard links, it optimizes storage space for duplicate files. This tool was developed as an implementation of the CITS2002 Project 2, 2021. For more detailed information, visit [CITS2002 Systems Programming - Project 2 2021](https://teaching.csse.uwa.edu.au/units/CITS2002/past-projects/p2021-2/summary.php).

## Options

- `-h, --help`: Display help information, including usage and options.
- `-r, --recursive`: Search directories recursively, including all subdirectories.
- `-a, --hidden`: Include hidden files (typically prefixed with a '.' in Unix/Linux systems).
- `-q, --quiet`: Report the presence of duplicates and potential space savings without listing the duplicates.
- `-f, --file <file>`: Identify duplicates of the specified file(s), supporting multiple files with the same name.
- `-d, --hash <hash>`: Find files matching the specified hash value.
- `-l, --list`: List sets of duplicate files.
- `-m, --minimise`: Reduce memory usage by creating hard links for duplicate files.

## Getting Started

### Compilation

Compile the program using the provided Makefile:

```bash
make
```

### Execution

Run `duplicates` with your desired options to find duplicate files across one or more directories:

```bash
./duplicates [options] directory1 [directory2 ...]
```
