/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 */

#include <stdio.h>

#include "file_reader.h"

enum FileError read_file(const char *const restrict filename,
                         struct LinkedList *const restrict process_list,
                         int *const restrict quantum) {

  FILE *const restrict file_to_read = fopen(filename, "r");

  enum FileError file_error = FILE_ERR_NONE;

  // Keep track of the line number, for error reporting. Starting on
  // two, because it's easy to know the quantum is wrong.
  int line_number = 1;

  if (file_to_read == NULL) {
    // Error
    file_error = FILE_ERR_OPEN;
  } else {
    // File opened.
    // First line is the quantum.
    if (fscanf(file_to_read, " %4d ", quantum) == EOF) {
      // There was an error.
      file_error = FILE_ERR_OPEN;
    } else {
      // Read the first line in.
      if (*quantum < 1) {
        file_error = FILE_ERR_QUANTUM;
      } else {
        // Should be good, read and add to list until done, or error.
        init_list(process_list);

        int arrival_time, burst_time;

        while(fscanf(file_to_read, "%4d %4d ",
                     &arrival_time, &burst_time) != EOF) {
          // Any errors we get here aren't terminal, we'll just skip
          // that entry but let the user know.
          switch (add_to_list(process_list, arrival_time, burst_time)) {
            case LIST_ERR_NONE:
              ++line_number;
              break;
            case LIST_ERR_ARRIVAL:
              fprintf(stderr, "Error with arrival time: %d\n", arrival_time);
              break;
            case LIST_ERR_BURST:
              fprintf(stderr, "Error with burst time: %d\n", burst_time);
              break;
            default:
              fprintf(stderr, "Unknow error adding process to list.\n");
          }
        }
      }
    }

    fclose(file_to_read);
  }

  return file_error;
}
