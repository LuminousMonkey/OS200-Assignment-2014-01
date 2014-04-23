/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * Description:
 *   Functions for reading in the text file for the process details.
 */

#ifndef FILE_READER_H_
#define FILE_READER_H_

#include "linked_list.h"

enum FileError {
  FILE_ERR_NONE = 0,
  FILE_ERR_OPEN,
  FILE_ERR_QUANTUM
};

/*
 * Read File
 *
 * Takes a filename to load in, will open and parse the file, a linked
 * list of the entries read from it and the quantum that was specified
 * will be returned. Also returns a FileErrorCode of FILE_ERR_NONE if
 * no problems.
 *
 * Does not check that the number of fields per line is valid. We'll
 * consider a quantum of 0 or lower invalid, and will return an error.
 *
 * filename - String of the file to load.
 * list - Pointer to a LinkedList that has been initialised.
 * quantum - Pointer to an integer, will return the quantum specified
 *           in the file. If there was any sort of error could be set
 *           to any value.
 *
 */
enum FileError read_file(const char *const restrict filename,
                         struct LinkedList *const restrict process_list,
                         int *const restrict quantum);


#endif
