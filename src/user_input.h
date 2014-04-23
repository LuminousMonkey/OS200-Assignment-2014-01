/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * For reading a single string in from the user.
 */

#ifndef USER_INPUT_H_
#define USER_INPUT_H_

#include <stdbool.h>
#include <stddef.h>

#define USER_QUIT_STRING "QUIT"

/*
 * File from user
 *
 * Takes a pointer to a buffer and the max size of that buffer. Will
 * prompt the user and place the input into the buffer and return
 * true. If the user enters in QUIT, then it will return false, don't
 * rely on the contents of the buffer.
 */
bool file_from_user(char *const restrict filename, const size_t size);

#endif
