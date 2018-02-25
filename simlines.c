#include <stdio.h>
#include <stdlib.h>
#include "readaline.h"
#include <atom.h>
#include <table.h>
#include <ctype.h>
#include <seq.h>
#include <except.h>
#include <assert.h>


/*
 *     simlines.c
 *     by Jordan Stone, 9/17/16
 *     Files, Pictures, and Interfaces
 *
 *     This program reads in multiple files and outputs lines that are 
 *     present more than once in any of the files. It prints out the line,
 *     the file that the line was present in, and the corresponding line 
 *     number.
 */ 

Except_T cannot_open = {"one or more files cannot be opened"};

/*struct stored in tables*/
typedef struct match_group {
        char *clean_line;
        Seq_T *files;
        int count;
        char was_counted;
} match_group;

/*struct to print out file names*/
typedef struct file_name_data {
        int argc;
        char **argv;
        int mg_nums;
} file_name_data;

/***************************** DECLARATIONS *********************************/
void fill_table(file_name_data *file_names, FILE *input_files[]); 

void clean_array(int bytes, char **datapp);

file_name_data *initialize_file_name_data(int argc, char *argv[]);

match_group *initialize_match_group(int argc, char *datapp);

void print_similar_lines(const void *key, void **value, void *file_names);

void free_data(const void *key, void **value, void *file_names);

/**************************** IMPLEMENTATION ********************************/
int main(int argc, char *argv[])
{
        FILE *input_files[argc - 1];
        for (int i = 1; i < argc; i++) {   /* opens all files, stores them */
                input_files[i-1] = fopen(argv[i], "rb");
                assert(input_files[i-1] != NULL);
        }
      
        file_name_data *file_names = initialize_file_name_data(argc, argv); 
        fill_table(file_names, input_files);
        free(file_names);

        return EXIT_SUCCESS;
}

/* Name: fill_table
 * Arguments: file_name_data struct, array of opened inputted files 
 * Purpose: creates a hanson table and adds match group structs into each 
 *          element of the table
 */
void fill_table(file_name_data *file_names, FILE *input_files[])
{
        Table_T table = Table_new(50, NULL, NULL);      /* new table */

        int file_num = 1;          /* keeps track of file being read */
        for (int i = 0; i < file_names->argc - 1; i++) {
                int line_counter = 1;
                char **datapp = malloc(sizeof(char*));  /* init. datapp */
                assert(datapp != NULL);
                int bytes = readaline(input_files[i], datapp);

                while (bytes != 0) {
                        clean_array(bytes, datapp);
                        const char *key = Atom_string(*datapp);

                                /* checks if element at key is unoccupied */
                        if (Table_get(table, key) == NULL) {
                                match_group *mg = 
                                initialize_match_group(file_names->argc,
                                                                  *datapp);
                                int *line_num =  malloc(sizeof(int *));
                                assert(line_num != NULL);
                                *line_num = line_counter;
                                Seq_addhi(mg->files[file_num], line_num);
                                Table_put(table, key, mg);
                        }       /* checks if element at key is occupied */
                        else {
                                match_group *temp = Table_get(table, key);
                                temp->count++;
                                int *line_num = malloc(sizeof(int *));
                                assert(line_num != NULL);
                                *line_num = line_counter;
                                Seq_addhi(temp->files[file_num], line_num);

                                /* does not print if spaces or empty lines */
                                if (temp->was_counted == 0 && 
                                        *temp->clean_line != ' ' && 
                                        *temp->clean_line != '\0') {
                                        file_names->mg_nums++;
                                        temp->was_counted = 1;
                                }
                                free(*datapp);
                        }
                        line_counter++;
                        bytes = readaline(input_files[i], datapp);
                }
                free(*datapp);
                file_num++;
                free(datapp);
        }
        Table_map(table, print_similar_lines, (void*) file_names);
        Table_map(table, free_data, (void*) file_names);
        Table_free(&table);
}

/* Name: clean_array
 * Arguments: number of bytes in the line, and datapp which holds a pointer 
 *            to the line
 * Purpose: cleans up all non-word charcters in the array, leaves one space
 *          in place of them
 */
void clean_array(int bytes, char **datapp)
{
        int i = 0;
        int j = 0;
        char *new_array = malloc(sizeof(char) * bytes);
        assert(new_array != NULL);

        /* loops through line, adds to new_array if alphanumeric */
        while ((*datapp)[i] != '\0' && (*datapp)[i] != EOF) {
                if (isalnum((*datapp)[i]) || (*datapp)[i] == '_') {
                        new_array[j] = (*datapp)[i];
                        j++;
                }
                /* adds space to array if prev element is not a space */
                else if (!isalnum((*datapp)[i])) {
                        if (new_array[j-1] == ' ') {
                        }
                        else { 
                                new_array[j] = ' ';
                                j++;
                        }
                }
                i++;
        }

        new_array[j] = '\0';
        free(*datapp);
        *datapp = new_array;
}

/* Name: initialize_file_name_data
 * Arguments: number of files, array of command line arguments
 * Purpose: initializes the file_name_data struct
 *
 */
file_name_data *initialize_file_name_data(int argc, char *argv[])
{
        file_name_data *fndp = malloc(sizeof(file_name_data));
        assert(fndp != NULL);
        fndp->argc = argc;
        fndp->argv = argv;
        fndp->mg_nums = 0;

        return fndp;
}

/* Name: initialize_match_group
 * Arguments: number of files, datapp
 * Purpose: initializes the match_group struct
 */
match_group *initialize_match_group(int argc, char *datapp)
{
        match_group *new_element = malloc(sizeof(match_group));
        assert(new_element != NULL);
        assert(new_element->files = malloc(sizeof(Seq_T) * argc) != NULL);
        new_element->clean_line = datapp;
        new_element->count = 1; 
        new_element->was_counted = 0;

        for (int j = 0; j < argc; j++) {
                new_element->files[j] = Seq_new(5);
        }
                
        return new_element;
}

/* Name: print_similar_lines
 * Arguments: void pointer to the current key, pointer to pointer of value
 *                        at that key, and void pointer to file_names
 * Purpose: prints out all of the match groups in the specified format
 */
void print_similar_lines(const void *key, void **value, void *file_names)
{
        match_group *mg = *value;
        int seq_len = 0;

        if (mg->count < 2) {   /* if only one occurence of line, return */
                return;
        }
        (void) key;

        /* returns if line is empty or only a space */
        if (*mg->clean_line == ' ' || *mg->clean_line == '\0') {
                return;
        }

        printf("%s\n",mg->clean_line); /*prints line*/

        file_name_data data =  *(file_name_data *)file_names;


        for (int i = 0; i < data.argc; i++) {
                seq_len = Seq_length(mg->files[i]);
                for (int j = 0; j < seq_len; j++) {
                        int *line = (int*) Seq_get(mg->files[i], j);
                        printf("%-*s %7i\n",20, data.argv[i], *line);
                }       
        }

        if (data.mg_nums > 1) {  /* prints newline unless last match group */
                printf("\n");
        }

        ((file_name_data *)file_names)->mg_nums--;

}

/* Name: free_data
 * Arguments: void pointer to the current key, pointer to pointer of value
 *            at that key, and void pointer to file_names
 * Purpose: frees all allocated data
 */
void free_data(const void *key, void **value, void *file_names)
{
        (void) key;
        match_group *mg = *value;

        /* frees entire hanson table/structs/arrays/sequences */
        for (int i = 0; i < ((file_name_data *)file_names)->argc; i++) {
                Seq_T  *seq = &(mg->files[i]);
                for(int j = 0; j < Seq_length(*seq); j++) {
                        free(Seq_get(*seq, j));
                }
                Seq_free(&(mg->files[i]));
        }

        free(mg->files);
        free(mg->clean_line);
        free(mg);       
}
