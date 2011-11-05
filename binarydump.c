/**
 * binarydump.c
 *
 * Simple command line utility that displays file contents in binary.
 * Example output:
 *
 * 0x0	01101000   01100101   01101100   01101100   
 * 0x4	01101111   00100000   01110111   01101111   
 * 0x8	01110010   01101100   01100100   00100000
 *
 * By default, it prints hexadecimal offset addresses. The '-r' option disables
 * address printing, and removes the whitespace that is normally printed between bytes.
 * The '-n' option controls the number of bytes printed per line of output.
 *
 * Author: James Russell (fizwidget)
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Function prototypes.
 */

/* 
 * Processes CLI arguments and sets global variables.
 * If args are invalid, prints usage & terminates.
 */
void process_args(int argc, char **argv);
/* Prints the contents of 'file' in binary */
void binary_dump(FILE *file);
/* Prints an individual byte in binary */
void print_byte(unsigned char byte);

/*
 * Global variables.
 */

int chunksize = 4;          // Number of bytes to print per line
int formatting_enabled = 1; // Print addresses & whitespace?
char *filename = NULL;      // Name of file to be printed

/*
 * Program entry point.
 */

int main (int argc, char **argv)
{
    process_args(argc, argv);
    FILE *file;
    
    // Did the user specify a file?
    if (filename == NULL) {
        // No, default to standard input
        file = stdin;
    } else {
        // Yes, attempt to open the file
        file = fopen(filename, "rb");
        if (!file) {
            printf("error: could not open file %s\n", filename);
            exit(EXIT_FAILURE);
        }
    }
    
    // Display file
    binary_dump(file);
    // Close file
    fclose(file);
    
    return 0;
}

void process_args(int argc, char **argv)
{
    
    // For each argument (ignoring argv[0])
    int i;
    for (i = 1; i < argc; i++) {
         char *arg = argv[i];
         
         // Is this argument an option?
         if (*arg == '-') {
             arg++; // Skip over '-'
             
             // Determine type of option
             if (*arg == 'h') {
                 // Help option [-h | -help]
                 printf("usage: bindump [FILE] [-n chunksize] [-r]\n");
                 exit(EXIT_FAILURE);
                 
             } else if (*arg == 'n') {
                 // Chunksize option [-n chunksize]
                 
                 if (++i < argc) {
                     int tmp = atoi(argv[i]);
                     if (tmp) chunksize = tmp; // Must be non-zero
                     
                 } else {
                     // Error, chunksize not specified
                     printf("error: option \"-n\" given, but chunksize not specified.\n");
                     exit(EXIT_FAILURE);
                 }
                 
             } else if (*arg == 'r') {
                 // Raw option [-r]
                 formatting_enabled = 0;
                 
             } else {
                 printf("error: unrecognised option \"-%s\"\n", arg);
                 exit(EXIT_FAILURE);
             }
             
         } else {
             // File specified
             if (filename) {
                 printf("error: more than one file specified\n");
                 exit(EXIT_FAILURE);
             } else {
                 filename = arg; 
             }
         }
     }
}

void binary_dump(FILE *file)
{
    char *buff = malloc(chunksize); // 'chunksize' bytes read into buffer each iteration
    int offset = 0;                 // Track offset from start of file
    int num_bytes_read;             // Number of bytes read in a single iteration
    
    // For each chunk of bytes
    while ((num_bytes_read = fread(buff, 1, chunksize, file))) {
        
        if (formatting_enabled) {
            printf("0x%X\t", offset);
        }
        
        offset += num_bytes_read;
        
        // For each byte that was read
        int i;
        for (i = 0; i < num_bytes_read; i++) {
            print_byte(buff[i]);
            if (formatting_enabled) printf("   ");
        }
        
        printf("\n");
    }
    
    // Print final offset (i.e. size of file)
    if (formatting_enabled) {
        printf("0x%X\n", offset);
    }
}

void print_byte(unsigned char byte)
{
    unsigned char tmp = 0x80;
    do printf("%i", (byte & tmp) ? 1 : 0);
    while (tmp >>= 1);
}