/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * The RR scheduler, processes are placed in a queue and executed in a
 * rotated fashion, the burst time is the smallest of the remaining
 * burst time of the process, or the quantum.
 */

#ifndef RR_SCHEDULER_H_
#define RR_SCHEDULER_H_

#include "process_entry.h"

/*
 * RR Scheduler
 *
 * Takes a pointer to an array of ProcessEntries and run a RR
 * scheduler on it, (hence the function name, funny that).
 *
 * The array being passed in is expected to be sorted.
 *
 * When the scheduler is run, it will update the process entries with
 * turnaround and waiting times. (Mutable data!)
 */
void rr_scheduler(struct ProcessEntry *const restrict process_table,
                  const int total_processes,
                  const int quantum);

#endif
