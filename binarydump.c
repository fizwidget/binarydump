/**
 * binarydump.c
 *
 * Simple command line utility that displays file contents in binary.
 * Example output:
 *
 * 0x0	01101000   01100101   01101100   01101100   
 * 0x4	01101111   00100000   01110111   01101111   
 * 0x8	01110010   01101100   01100100   00100000
 * 0xC
 *
 * By default, hexadecimal offset addresses are printed in the left column.
 * The '-r' option disables address printing, and removes the whitespace
 * that is normally printed between bytes. Basically, it displays the raw bits
 * without any extra formatting.
 * The '-n' option controls the number of bytes printed per line of output.
 *
 * Contributors:
 * - James Russell (fizwidget)
 */

#include <stdlib.h>
#include <stdio.h>

/*
 * Function prototypes.
 */

/* 
 * Processes CLI arguments, setting global vars as appropriate.
 * If args are invalid, prints usage & terminates.
 */
void process_args(int argc, char **argv);
/* Prints the contents of 'file' in binary */
void binary_dump(FILE *file);
/* Prints an individual byte in binary */
void print_byte(unsigned char b);

/*
 * Global variables.
 */

int bytes_per_line = 4;     // Number of bytes to print per line
int formatting_enabled = 1; // Print addresses & whitespace?
char *filename = NULL;      // Name of file to be printed

/*
 * Function implementations.
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
    
    binary_dump(file);
    fclose(file);
    
    return 0;
}

void process_args(int argc, char **argv)
{
    /*
     * Valid arguments:
     * [-h | -help] // Prints usage
     * [FILE]       // The file to be printed (if not specified, default to stdin)
     * [-n count]   // Sets number of bytes to print per line
     * [-raw]       // Disables address column & removes whitespace
     */
    
    // For each argument (ignoring argv[0])
    int i;
    for (i = 1; i < argc; i++) {
         char *arg = argv[i];
         
         // Does this argument start with '-'?
         if (*arg == '-') {
             arg++; // Skip over '-'
             
             // Determine option
             if (*arg == 'h') {
                 // Help option [-h | -help]
                 printf("Example usage:\n");
                 printf("   binarydump\n");
                 printf("   binarydump [FILE]\n");
                 printf("   binarydump [FILE] -n [bytesPerLine]\n");
                 printf("   binarydump [FILE] -raw\n");
                 printf("\n");
                 printf("Raw mode prints the bits without any whitespace,");
                 printf(" and without\nan address offset column.");
                 printf(" If no file is specified,\nstandard input will be read.\n");
                 
                 exit(EXIT_FAILURE);
                 
             } else if (*arg == 'n') {
                 // Bytes per line option [-n count]
                 if (++i < argc) {
                     int tmp = atoi(argv[i]);
                     if (tmp) bytes_per_line = tmp; // Must be non-zero
                     
                 } else {
                     // Error, 'count' not specified
                     printf("error: option \"-n\" given, but bytes_per_line not specified.\n");
                     exit(EXIT_FAILURE);
                 }
                 
             } else if (*arg == 'r') {
                 // Raw option [-raw]
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
    char *buff = malloc(bytes_per_line);    // 'bytes_per_line' bytes read into buffer each iteration
    int offset = 0;                         // Track offset from start of file
    int num_bytes_read;                     // Number of bytes read in a single iteration
    
    // For each chunk of bytes
    while ((num_bytes_read = fread(buff, 1, bytes_per_line, file))) {
        
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

void print_byte(unsigned char b)
{
    /*
     * Algorithm used:
     * 1. Initialise 'tmp' to 10000000 (80 in hex)
     * 2. If (b AND tmp) is non-zero, print 1, else print 0.
     * 3. Right-shift tmp (after first iteration, it becomes 01000000).
     * 4. If tmp is non-zero, jump to step 2.
     */
    unsigned char tmp = 0x80;
    do printf("%i", (b & tmp) ? 1 : 0);
    while (tmp >>= 1);
}