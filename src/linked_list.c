/*
 * PDM300 - Homework 1
 *
 * Author: Mike Aldred
 *
 * Description:
 *   Simple linked list implementation for OS 200 assignment.
 *
 * Comments for functions are in the header file, as that is what is
 * "public". Not a full linked list implementation, just enough to do
 * what we need.
 *
 * Check linked_list.h for interface details.
 */

#include "linked_list.h"

#include <assert.h>
#include <stdlib.h>

// Forward decs
static void remove_head_from_list(struct LinkedList *const restrict list);

void init_list(struct LinkedList *const restrict list) {
  list->head = NULL;
  list->tail = NULL;
  list->current_node = NULL;
  list->count = 0;
}

void destroy_list(struct LinkedList *const restrict list) {
  while (list->head != NULL) {
    remove_head_from_list(list);
  }

  // Reset
  init_list(list);
}

int add_to_list(struct LinkedList *const restrict list,
                const int arrival_time,
                const int burst_time) {

  enum LinkedListError error = LIST_ERR_NONE;

  struct LinkedListNode *const restrict new_list_node =
      calloc(1, sizeof(struct LinkedListNode));

  // If we can't allocate memory this small the sort of trouble that
  // this means isn't something this program can handle.
  assert(new_list_node != NULL);

  enum ProcessEntryError process_entry_error =
      init_process_entry(&new_list_node->process, arrival_time, burst_time);

  /*
   * If there are any errors, then free the new list node and return
   * the error.
   */
  switch (process_entry_error) {
    case PROCESS_ENTRY_ERR_ARRIVAL:
      error = LIST_ERR_ARRIVAL;
      free(new_list_node);
      break;
    case PROCESS_ENTRY_ERR_BURST:
      error = LIST_ERR_BURST;
      free(new_list_node);
      break;
    case PROCESS_ENTRY_ERR_NONE:
      new_list_node->next = NULL;

      if (list->tail != NULL) {
        // Elements exist in the list already.
        new_list_node->previous = list->tail;
        list->tail->next = new_list_node;
        list->tail = new_list_node;
      } else {
        // Empty list
        list->tail = list->head = new_list_node;
      }
      list->count++;
      break;
    default:
      // We ever hit this fix it.
      assert(false);
  }

  return error;
}

void remove_from_list(struct LinkedList *const restrict list,
                      struct LinkedListNode *const to_remove) {

  assert(list != NULL);
  assert(to_remove != NULL);

  if (to_remove->previous == NULL) {
    // It's the head
    if (to_remove->next == NULL) {
      // And the tail
      list->head = NULL;
      list->tail = NULL;
    } else {
      // Just the head.
      list->head = to_remove->next;
      list->head->previous = NULL;
    }
  } else if (to_remove->next == NULL) {
    // It's just the tail
    list->tail = to_remove->previous;
    list->tail->next = NULL;
  } else {
    to_remove->previous->next = to_remove->next;
    to_remove->next->previous = to_remove->previous;
  }

  // Update the iterator if we're removing a node it's on.
  if (to_remove == list->current_node) {
    if (to_remove->next != NULL) {
      list->current_node = to_remove->next;
    } else {
      list->current_node = list->head;
    }
  }

  free(to_remove);
  --list->count;
}

void reset_list_iterator(struct LinkedList *const restrict list) {
  list->current_node = list->head;
}

bool has_value(const struct LinkedList *const restrict list) {
  bool result = false;

  if (list->current_node != NULL) {
    result = true;
  }

  return result;
}

void next_list_item(struct LinkedList *const restrict list) {
  if (list->current_node != NULL) {
    list->current_node = list->current_node->next;
  }
}

struct ProcessEntry *node_value(struct LinkedList *const restrict list) {
  struct ProcessEntry *result = NULL;

  if (list->current_node != NULL) {
    result = &list->current_node->process;
  }

  return result;
}

/*
 * Remove head from list.
 *
 * Given a pointer to a linked list, remove the head of the list and
 * replace it with the next element in the list, if present.
 *
 * This will also free the ProcessEntry item.
 */
static void remove_head_from_list(struct LinkedList *const restrict list) {
  if (list->head != NULL) {
    struct LinkedListNode *old_head = list->head;

    if (old_head == list->tail) {
      list->head = NULL;
      list->tail = NULL;
    } else {
      list->head = old_head->next;
    }

    free(old_head);
  }
}
