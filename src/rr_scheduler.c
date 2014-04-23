/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 */

#include <assert.h>
#include <stddef.h>

#include <stdio.h>

#include "rr_scheduler.h"

// Forward defines.
static int smallest(const int num1, const int num2);

void rr_scheduler(struct ProcessEntry *const restrict process_table,
                  const int total_processes,
                  const int quantum) {

  assert(process_table != NULL);

  int cpu_time = process_table[0].arrival_time;
  int processes_remaining = total_processes;
  int step = 0;

  while (processes_remaining > 0) {
    int current_index = step % total_processes;

    if (process_table[current_index].burst_time_remaining != 0) {

      if (process_table[current_index].arrival_time <= cpu_time) {
        const int burst_or_quantum =
            smallest(process_table[current_index].burst_time_remaining,
                     quantum);

        cpu_time += burst_or_quantum;
        process_table[current_index].burst_time_remaining -=
            burst_or_quantum;

        if (process_table[current_index].burst_time_remaining < 1) {
          process_table[current_index].turnaround_time = cpu_time -
              process_table[current_index].arrival_time;
          process_table[current_index].waiting_time =
              process_table[current_index].turnaround_time -
              process_table[current_index].burst_time;
          --processes_remaining;
        }
      } else {
        // Make CPU time the next largest arrival time.
        ++cpu_time;
      }
    }
    ++step;
  }
}

/*
 * Smallest
 *
 * Given two numbers, will return the smallest one that is greater
 * than zero.
 */
static int smallest(const int num1, const int num2) {
  int result = num1;

  if (num2 < num1) {
    result = num2;
  } else if (num1 == 0) {
    result = num2;
  }

  return result;
}
