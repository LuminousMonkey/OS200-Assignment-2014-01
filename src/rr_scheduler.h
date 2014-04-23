/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * The round robin scheduler, will run the current process for either
 * length of quantum or remaining burst time (whichever is smaller),
 * then switch to the next process. Processes are taken in a FCFS
 * fashion, so process entry should not have been sorted beforehand,
 * we're assuming that the file being read in is correct.
 */

#ifndef RR_SCHEDULER_H_
#define RR_SCHEDULER_H_

#include "process_entry.h"

void rr_scheduler(struct ProcessEntry *const restrict process_table,
                  const int total_processes,
                  const int quantum);

#endif
