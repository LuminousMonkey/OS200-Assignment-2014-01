/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "sjf_scheduler.h"

// Forward defines.
static int sjf_next_process(
    const struct ProcessEntry *const restrict process_table,
    const int number_of_processes,
    const int cpu_time);

/*
 * SJF Scheduler
 *
 * The quantum parameter is not intended to be used, it's only so the
 * function signature matches.
 */
void sjf_scheduler(struct ProcessEntry *const restrict process_table,
                   const int total_processes,
                   const int quantum) {

  assert(process_table != NULL);
  int cpu_time = process_table[0].arrival_time;
  int processes_remaining = total_processes;

  while (processes_remaining > 0) {
    int next_process = sjf_next_process(process_table,
                                        total_processes,
                                        cpu_time);

    // Skip any time not spent processing.
    if (process_table[next_process].arrival_time > cpu_time) {
      cpu_time = process_table[next_process].arrival_time;
    }

    cpu_time += process_table[next_process].burst_time_remaining;

    process_table[next_process].burst_time_remaining = 0;

    process_table[next_process].turnaround_time = cpu_time -
        process_table[next_process].arrival_time;
    process_table[next_process].waiting_time =
        process_table[next_process].turnaround_time -
        process_table[next_process].burst_time;
    --processes_remaining;
  }
}

static int sjf_next_process(
    const struct ProcessEntry *const restrict process_table,
    const int number_of_processes,
    const int cpu_time) {

  int next_process = 0;

  bool keep_searching = true;
  while (keep_searching == true &&
         next_process < number_of_processes) {
    if (process_table[next_process].burst_time_remaining == 0) {
      ++next_process;
    } else {
      keep_searching = false;
    }
  }

  int index = 1;

  while (index < number_of_processes) {
    if (process_table[index].burst_time_remaining != 0 &&
        process_table[index].arrival_time <= cpu_time &&
        process_table[index].burst_time_remaining <
        process_table[next_process].burst_time_remaining) {

      next_process = index;
    }

    ++index;
  }

  return next_process;
}
