/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 */

#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "user_input.h"

bool file_from_user(char *const restrict filename, const size_t size) {
  bool result = true;

  if (fgets(filename, size, stdin)) {

    // Remove the trailing newline, it's annoying.
    size_t length = strlen(filename) - 1;
    if (filename[length] == '\n') {
      filename[length] = '\0';
    }

    // Not standard C, but POSIX.
    result = (strncasecmp(filename, USER_QUIT_STRING, size) != 0);
  }

  return result;
}
