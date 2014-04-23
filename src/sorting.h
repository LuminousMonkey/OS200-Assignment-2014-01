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
 * Typedef for the sorting functions. They must match this.
 */
typedef void (*ScheduleSort)(struct LinkedList *const list,
                             struct ProcessEntry *const process_table);


/*
 * FIFO sort.
 *
 * Just transfers the contents of the linked list into the given array.
 * Intended for a round robin scheduler.
 */
void fifo_sort(struct LinkedList *const list,
               struct ProcessEntry *const process_table);

/*
 * SJF sort
 *
 * Will sort so that for jobs with the same arrival time, the shortest
 * job will be placed first.
 *
 * This is destructive to the list passed in.
 *
 * list - Pointer to list with items loaded.
 * process_table - Pointer to allocated array of process entries.
 */
void sjf_sort(struct LinkedList *const list,
              struct ProcessEntry *const process_table);

#endif
