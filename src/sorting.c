/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 */

#include <assert.h>
#include <stddef.h>

#include "process_entry.h"
#include "sorting.h"

void selection_sort(struct LinkedList *const list,
              struct ProcessEntry *const process_table) {

  assert(list != NULL);
  assert(process_table != NULL);

  int current_index = 0;

  // We always expect at least one entry in the list.
  assert(list->head);

  // Save the original list count, removing elements updates it.
  int num_of_entries = list->count;

  while (current_index < num_of_entries) {

    assert(list->head);

    // Start with our min.
    reset_list_iterator(list);
    const struct LinkedListNode *smallest_entry_node = list->current_node;
    next_list_item(list);

    // Loop through looking for anything smaller.
    while(has_value(list)) {
      if (list->current_node->process.arrival_time <
          smallest_entry_node->process.arrival_time) {
        smallest_entry_node = list->current_node;
      }
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
