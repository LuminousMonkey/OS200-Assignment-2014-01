/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * The SJF scheduler, when a process is completed, it will pick the
 * process with the shortest burst time that is currently waiting.
 */

#ifndef SJF_SCHEDULER_H_
#define SJF_SCHEDULER_H_

#include "process_entry.h"

/*
 * SJF Scheduler
 *
 * Takes a pointer to an array of ProcessEntries and run a SJF
 * scheduler on it, (hence the function name, funny that).
 *
 * The array being passed in is expected to be sorted.
 *
 * When the scheduler is run, it will update the process entries with
 * turnaround and waiting times. (Mutable data!)
 */
void sjf_scheduler(struct ProcessEntry *const restrict process_table,
                   const int total_processes,
                   const int quantum);

#endif
