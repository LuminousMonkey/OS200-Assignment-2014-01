/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 */

#include <stdbool.h>

#include "process_entry.h"

/*
 * init_process_entry
 *
 * Given a pointer to a process entry, the arrival and burst times,
 * set up the entry.
 */
enum ProcessEntryError init_process_entry(
    struct ProcessEntry *const restrict process_entry,
    const int arrival_time,
    const int burst_time) {

  enum ProcessEntryError entry_error = PROCESS_ENTRY_ERR_NONE;

  // Check for valid values.
  if (arrival_time < 0 || burst_time < 1) {
    if (arrival_time < 0) {
      entry_error = PROCESS_ENTRY_ERR_ARRIVAL;
    } else {
      entry_error = PROCESS_ENTRY_ERR_BURST;
    }
  } else {
    // Good to go.
    process_entry->arrival_time = arrival_time;
    process_entry->burst_time = burst_time;
    process_entry->burst_time_remaining = burst_time;
    process_entry->turnaround_time = 0;
    process_entry->waiting_time = 0;
  }

  return entry_error;
}
