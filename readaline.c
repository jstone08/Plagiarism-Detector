#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include <except.h>
#define INIT_SIZE 200



/*
 *     readaline.c
 *     by Jordan Stone, 9/17/16
 *     HW1 - Files, Pictures, and Interfaces
 *
 *     This program(function) reads a line from an input file stream. It 
 *     counts the number of bytes in the line and returns the number of 
 *     bytes to its caller. In addition it sets the pointer in its argument
 *     to the first character of the line.   
 */ 

/***************************** DECLARATION *********************************/
size_t readaline(FILE *inputfp, char **datapp);

Except_T NULL_arguments = {"one or more args are NULL"};
Except_T read_error = {"error reading in file"};
Except_T memory_fail = {"could not allocate memory"};

/**************************** IMPLEMENTATION ********************************/
/* Name: readaline
 * Arguments: pointer to the file, datapp (pointer to first element of array)
 * Purpose: reads in a line from inputted file and stores in in datapp
 */
size_t readaline(FILE *inputfp, char **datapp)
{       
        TRY
                if (datapp == NULL || inputfp == NULL) {
                        RAISE(NULL_arguments);  
                }        /* raises exception if args are NULL */

                char *line_of_chars = (char *) malloc(INIT_SIZE);

                if (line_of_chars == NULL) {
                        RAISE(memory_fail);
                }        /* raises exception if memory cannot be allocated */

                int cap = INIT_SIZE; 
                int i = 0;
                char next_char = fgetc(inputfp);
                
                if (next_char == EOF) {    /* checks if empty file */
                        *datapp = NULL;
                        free(line_of_chars);
                        return 0;
                }

                while (next_char != '\n' && next_char != EOF) {
                        if (i == cap) {
                                cap *= 2;    /* expands array */
                                line_of_chars = (char *) 
                                        realloc(line_of_chars, cap);
                        }
                        line_of_chars[i] = next_char;
                        next_char = fgetc(inputfp);
                        i++;
                }
                if (ferror(inputfp) != 0) { /*raise reading error exception*/
                        RAISE(read_error);
                }

                line_of_chars[i] = '\0';   /* makes string null-terminated */
                i++;                    
                
                *datapp = line_of_chars;
        
                return i;
                
        EXCEPT(NULL_arguments)
                exit(EXIT_FAILURE);
        EXCEPT(memory_fail)
                exit(EXIT_FAILURE);
        EXCEPT(read_error)
                exit(EXIT_FAILURE);
        END_TRY;
        return EXIT_FAILURE;
}
