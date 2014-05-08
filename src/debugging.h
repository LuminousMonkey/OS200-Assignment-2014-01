/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * Simple header for a debug print macro. To enable debug printing
 * just compile with DEBUG set, or change the value of DEBUG here set
 * to 1.
 */

#ifndef DEBUGGING_H_
#define DEBUGGING_H_

#include <stdio.h>

#ifndef DEBUG
#define DEBUG 0
#endif

#define debug_print(...)                                            \
  do { if (DEBUG) fprintf(stderr, __VA_ARGS__); } while (0)

#endif
