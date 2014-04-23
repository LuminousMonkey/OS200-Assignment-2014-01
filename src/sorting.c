/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 */

#include <assert.h>
#include <stddef.h>

#include "process_entry.h"
#include "sorting.h"

// Forward Declarations
static const struct LinkedListNode *sjf(const struct LinkedListNode *const node1,
                                        const struct LinkedListNode *const node2);

void fifo_sort(struct LinkedList *const list,
               struct ProcessEntry *const process_table) {

  assert(list != NULL);
  assert(process_table != NULL);

  reset_list_iterator(list);

  int current_index = 0;

  while(has_value(list)) {
    assert(current_index < list->count);

    struct ProcessEntry *current_node = node_value(list);

    init_process_entry(process_table + current_index,
                       current_node->arrival_time,
                       current_node->burst_time);

    next_list_item(list);
    ++current_index;
  }
}

void sjf_sort(struct LinkedList *const list,
              struct ProcessEntry *const process_table) {

  assert(list != NULL);
  assert(process_table != NULL);

  int current_index = 0;

  // We always expect at least one entry in the list.
  assert(list->head);

  // Save the original list count, removing elements updates it.
  int num_of_entries = list->count;

  /*
   * Insertion sort.
   *
   * We are probably getting nearly sorted data, but this is just
   * easier for the linked list to array conversion too.
   */
  while (current_index < num_of_entries) {

    assert(list->head);

    // Start with our min.
    reset_list_iterator(list);
    const struct LinkedListNode *smallest_entry_node = list->current_node;
    next_list_item(list);

    // Loop through looking for anything smaller.
    while(has_value(list)) {
      smallest_entry_node = sjf(smallest_entry_node, list->current_node);
      next_list_item(list);
    }

    init_process_entry(&process_table[current_index],
                       smallest_entry_node->process.arrival_time,
                       smallest_entry_node->process.burst_time);

    // This will free the memory held by the node, that definitely
    // breaks const.
    remove_from_list(list, (struct LinkedListNode *)smallest_entry_node);

    ++current_index;
  }
}

/*
 * SJF
 *
 * Given to process entries, it will a pointer to the one that is
 * considered the SJF.
 *
 * Operates on nodes, not process entries, because it's easier if we
 * have a reference to the node so we can remove it from the list more
 * easily.
 */
static const struct LinkedListNode *sjf(const struct LinkedListNode *const node1,
                                  const struct LinkedListNode *const node2) {
  const struct LinkedListNode *result = node1;

  if (node2->process.arrival_time == node1->process.arrival_time) {
    if (node2->process.burst_time < node1->process.burst_time) {
      result = node2;
    }
  } else if (node2->process.arrival_time < node1->process.arrival_time) {
    result = node2;
  }

  return result;
}
