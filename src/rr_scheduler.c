/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * Implimentation of the round robin scheduler.
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>

#include "rr_scheduler.h"

// Forward defines.
static int smallest(const int num1, const int num2);
static int find_waiting(struct ProcessEntry *const restrict process_table,
                        const int total_processes,
                        const int cpu_time,
                        const int waiting);

/*
 * rr_scheduler
 *
 * All scheduling is based off the process table. RR scheduling relies
 * on using a queue to keep track of what are the next processes to
 * execute. In order to handle this, we have:
 *
 * start - Index of the process that is considered the start of the
 *         queue.
 *
 * end - Index of the process that is considered the end of the queue.
 *
 * Both of these indexes give the range of processes that are
 * currently running. To keep track of any new processes that are
 * waiting to be place at the end of the queue, there is:
 *
 * waiting - The largest index of the processes that are to be added
 *           to the end of the queue when the start is reached.
 */
void rr_scheduler(struct ProcessEntry *const restrict process_table,
                  const int total_processes,
                  const int quantum) {

  assert(process_table != NULL);
  int processes_remaining = total_processes;

  // Just skip to the CPU time for the first process.
  int cpu_time = process_table[0].arrival_time;

  int start = 0;
  int end = 0;
  int waiting = 0;

  while (processes_remaining > 0) {
    int process_to_run = start;
    bool no_process_execd = true;

    /*
     * This is the loop that runs processes in the queue.
     */
    while (process_to_run <= end) {
      if (process_table[process_to_run].burst_time_remaining != 0) {

        const int burst_or_quantum =
            smallest(process_table[process_to_run].burst_time_remaining,
                     quantum);

        cpu_time += burst_or_quantum;
        no_process_execd = false;

        process_table[process_to_run].burst_time_remaining -=
            burst_or_quantum;

        waiting = find_waiting(process_table, total_processes,
                               cpu_time, waiting);

        // At the start of the queue, move the end of the queue so the
        // waiting processes will now be executed.
        if (process_to_run == start) {
          end = waiting;
        }

        // If a process is done, remove it and figure out our results.
        if (process_table[process_to_run].burst_time_remaining < 1) {
          process_table[process_to_run].turnaround_time = cpu_time -
              process_table[process_to_run].arrival_time;
          process_table[process_to_run].waiting_time =
              process_table[process_to_run].turnaround_time -
              process_table[process_to_run].burst_time;
          --processes_remaining;
        }
      }
      ++process_to_run;
    }

    /*
     * Haven't run any process, this means the CPU is idle.
     *
     * The next process to run will be the one with the lowest arrival
     * time that still has burst time. We find it and update cpu_time
     * to it's arrival time.
     */
    if (no_process_execd) {
      int search_index = start + 1;
      int lowest_index = start + 1;

      while (search_index < total_processes) {
        if (process_table[search_index].burst_time_remaining != 0 &&
            process_table[search_index].arrival_time <
            process_table[lowest_index].arrival_time) {
          lowest_index = search_index;
        }
        ++search_index;
      }

      cpu_time = process_table[lowest_index].arrival_time;
      start = end = lowest_index;
      waiting = find_waiting(process_table, total_processes,
                             cpu_time, waiting);
    }
    no_process_execd = true;
  }
}

/*
 * FindWaiting
 *
 * Returns the last index of a process that is considered to be
 * waiting. Waiting processes are ones that are going to be added to
 * the end of the queue.
 *
 * process_table - Process table to check
 * total_processes - Total number of processes in the table.
 * cpu_time - Current cpu_time
 * waiting - Current index of the last process waiting to be added to
 *           the end of the queue.
 */
static int find_waiting(struct ProcessEntry *const restrict process_table,
                        const int total_processes,
                        const int cpu_time,
                        const int waiting) {
  int result = waiting;

  for (int index = waiting; index < total_processes; ++index) {
    if (process_table[index].burst_time_remaining != 0 &&
        process_table[index].arrival_time <= cpu_time) {
      result = index;
    }
  }

  return result;
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
