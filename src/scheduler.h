/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "process_entry.h"

/*
 * This is our type so we can pass in the scheduler function. Any
 * schedulers we support must have this signature, even if they don't
 * support a quantum.
 */
typedef void (*Scheduler)(struct ProcessEntry *const restrict process_table,
                          const int num_entries,
                          const int quantum);

struct SchedulerAverages {
  double turnaround_time;
  double waiting_time;
};

struct SchedulerAverages run_scheduler(const char *const restrict filename,
                                       const Scheduler scheduler_to_use);

#endif
