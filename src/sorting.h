/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * Description:
 *   Different sorting methods that take a linked list of process
 *   entries and stores the result into an array of process entries.
 */

#ifndef SORTING_H_
#define SORTING_H_

#include "linked_list.h"
#include "process_entry.h"

/*
 * Selection sort
 *
 * Takes a pointer to a list which contains the arrival and burst
 * times as read from a file. Will update the given process table so
 * it contains the process entries sorted in order of arrival time.
 * This is destructive to the list passed in.
 *
 * list - Pointer to list with items loaded.
 * process_table - Pointer to allocated array of process entries.
 */
void selection_sort(struct LinkedList *const list,
                    struct ProcessEntry *const process_table);

#endif
