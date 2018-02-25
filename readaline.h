#ifndef READALINE_H_INCLUDED
#define READALINE_H_INCLUDED

#include <stdio.h>

/*
 *   readaline - interface
 *
 *   Parameters:
 * 
 *     inputfd:    file stream must have been opened for reading
 *     datapp:     readaline will update *datapp to point to newly
 *                 allocated buffer with data (or NULL if called at eof)
 *
 *   Errors:
 * 
 *                 A Hanson exception must be raised if fd or **datapp is 
 *                 supplied as NULL,if memory cannot be allocated, or if an
 *                 error is encountered reading the input stream.
 * 
 *   Note:         caller must free (*datapp) to avoid memory leaks.
 *
 *   Note:         readline would be a less clumsy name, but there is already
 *                 a standard readline() Unix library routine, so readaline
 *                 minimizes the chance we accidentally call that when wanting 
 *                 this.
 */
size_t readaline(FILE *inputfd, char **datapp);

#endif