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
 * The '-r' option disables address printing, and removes the whitespace between columns.
 * The '-n' option sets the number of bytes printed per line.
 * If no file is specified, standard input is read.
 *
 * Contributors:
 * - James Russell (fizwidget)
 */

#include <stdlib.h>
#include <stdio.h>

/*
 * Function prototypes.
 */

/* Prints the contents of 'file' in binary */
void binary_dump(FILE *file);
/* Prints an individual byte in binary */
void print_byte(unsigned char b);
/* Processes CLI arguments & sets global vars; terminates prog if args invalid. */
void process_args(int argc, char **argv);

/*
 * Global variables.
 */

int bytes_per_line = 4;         // Number of bytes to print per line
int formatting_enabled = 1;     // Print addresses & whitespace? (boolean)
char *filename = NULL;          // Name of file to be printed

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
            fprintf(stderr, "error: could not open file %s\n", filename);
            exit(EXIT_FAILURE);
        }
    }
    
    binary_dump(file);
    fclose(file);
    
    return 0;
}

void binary_dump(FILE *file)
{
    char *byte_buff = malloc(bytes_per_line);   // Stores however many bytes will be printed per line
    int offset = 0;                             // Track offset from start of file
    int num_bytes_read;                         // Number of bytes read by fread()
    int i;
    
    // While more data remains, read bytes into buffer
    while ((num_bytes_read = fread(byte_buff, 1, bytes_per_line, file))) {
        
        if (formatting_enabled) {
            // Print hexadecimal offset address
            printf("0x%X\t", offset);
        }
        
        offset += num_bytes_read;
        
        // Print each byte in the buffer
        for (i = 0; i < num_bytes_read; i++) {
            print_byte(byte_buff[i]);
            if (formatting_enabled) printf("   ");
        }
        
        printf("\n");
    }
    
    if (formatting_enabled) {
        // Print final offset (i.e. size of file)
        printf("0x%X\n", offset);
    }
    
    free(byte_buff);
}

void print_byte(unsigned char byte)
{
    /*
     * Algorithm:
     * 1. Initialise bitmask to 10000000 (80 in hex).
     * 2. Use bitmask to extract bit from 'byte'.
     * 3. If extracted bit == 1 then print 1, else print 0.
     * 4. Right-shift bitmask; jump to step 2 if bitmask is non-zero.
     */
     
    unsigned char mask = 0x80;
    do printf("%i", (mask & byte) ? 1 : 0);
    while (mask >>= 1);
}

void process_args(int argc, char **argv)
{
    /*
     * There are no required arguments.
     * Optional arguments:
     * [-h | -help]         Print usage & exit
     * [FILE]               Specify the file to be printed (if not specified, program defaults to stdin)
     * [-n bytesPerLine]    Set number of bytes to print per line
     * [-r | -raw]          Disable printing of address column & remove whitespace
     */
    
    char *arg;
    int i;
    
    // For each argument (ignoring argv[0])
    for (i = 1; i < argc; i++) {
         arg = argv[i]; 
         
         // Does this argument start with '-'?
         if (*arg == '-') {
             arg++; // Skip over '-'
             
             // Determine type of argument
             if (*arg == 'h') {
                 // Help option [-h | -help]
                 printf("Usage:\n");
                 printf("   binarydump [FILE] [-n bytesPerLine] [-r | -raw]\n");
                 printf("\n");
                 printf("Raw mode prints nothin' but the bits (address offsets & whitespace will NOT be printed).\n");
                 printf("If no file is specified, standard input will be read.\n");
                 
                 exit(EXIT_SUCCESS);
                 
             } else if (*arg == 'n') {
                 // Bytes per line option [-n bytesPerLine]
                 i++;
                 if (i < argc) {
                     int tmp = atoi(argv[i]);
                     if (tmp != 0) bytes_per_line = tmp;
                     
                 } else {
                     // Error, 'bytesPerLine' not specified
                     fprintf(stderr, "error: option \"-n\" given, but count not specified.\n");
                     exit(EXIT_FAILURE);
                 }
                 
             } else if (*arg == 'r') {
                 // Raw option [-r | -raw]
                 formatting_enabled = 0;
                 
             } else {
                 fprintf(stderr, "error: unrecognised option \"-%s\"\n", arg);
                 exit(EXIT_FAILURE);
             }
             
         } else {
             // File specified
             if (filename != NULL) {
                 fprintf(stderr, "error: more than one file specified\n");
                 exit(EXIT_FAILURE);
             } else {
                 filename = arg; 
             }
         }
     }
}