/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * Description:
 *   Linked list implimentation.
 */

#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stdbool.h>

#include "process_entry.h"

enum LinkedListError {
  LIST_ERR_NONE = 0,
  LIST_ERR_ARRIVAL,
  LIST_ERR_BURST
};

struct LinkedListNode {
  struct LinkedListNode *previous;
  struct LinkedListNode *next;
  struct ProcessEntry process;
};

struct LinkedList {
  struct LinkedListNode *head;
  struct LinkedListNode *tail;

  int count;

  // For stepping through the list.
  struct LinkedListNode *current_node;
};

/*
 * Create List
 *
 * Given a pointer to a linked list, initialise what is needed
 * for the linked list.
 */
void init_list(struct LinkedList *const restrict list);

/*
 * Destroy list
 *
 * Given a pointer to a linked list, destroy the whole list.
 */
void destroy_list(struct LinkedList *const restrict list);

/*
 * Add to list
 *
 * Description:
 *   Given a pointer to a list, and a pointer to the
 *   process information, add it to the list.
  */
int add_to_list(struct LinkedList *const restrict list,
                const int arrival_time,
                const int burst_time);

/*
 * Remove from list.
 *
 * Given a pointer to a list node, remove it.
 */
void remove_from_list(struct LinkedList *const restrict list,
                      struct LinkedListNode *const to_remove);

/*
 * The following functions are for easy iteration through the list.
 */

/*
 * Reset the internal list iterator to the start of the list.
 */
void reset_list_iterator(struct LinkedList *const restrict list);

/*
 * Given a list, returns true if there is a value to be read, NULL
 * otherwise.
 */
bool has_value(const struct LinkedList *const restrict list);

/*
 * Moves the given list onto the next item. It is expected that
 * has_value has been called first to check that there is a next value
 * to move onto.
 */
void next_list_item(struct LinkedList *const restrict list);

/*
 * Will return the ProcessEntry that the list iterator currently
 * points to. Expected that reset_list_iterator has been called to set
 * everything up.
 */
struct ProcessEntry *node_value(struct LinkedList *const restrict list);

#endif
