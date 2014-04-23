/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * The general operation of both schedulers is pretty much exactly the
 * same, both take an array of process entries, and update them to
 * have the calculated waiting and turnaround times. This file is
 * everything else around that is common.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "scheduler.h"

#include "file_reader.h"
#include "linked_list.h"
#include "sorting.h"
#include "user_input.h"

struct SchedulerAverages run_scheduler(const char *const filename,
                                       const Scheduler scheduler_to_use) {

  struct LinkedList process_list;
  int quantum;
  struct SchedulerAverages averages = {0.0,0.0};

  enum FileError error = read_file(filename, &process_list, &quantum);

  if (error != FILE_ERR_NONE) {
    perror("main() - File Error");
  } else {
    #ifdef DEBUG
    printf("%d entries were read.\n", process_list.count);
    printf("Quantum: %d\n\n", quantum);
    #endif

    int list_count = process_list.count;

    struct ProcessEntry *entries = calloc(sizeof(struct ProcessEntry),
                                            list_count);

    assert(entries != NULL);

    fifo_sort(&process_list, entries);

    destroy_list(&process_list);

    (*scheduler_to_use)(entries, list_count, quantum);

    int total_waiting_time = 0;
    int total_turnaround_time = 0;

    for (int i = 0; i < list_count; i++) {
      #ifdef DEBUG
      printf("Arrival Time: %d, Burst Time: %d, "
             "Turnaround: %d, Waiting: %d.\n",
             entries[i].arrival_time,
             entries[i].burst_time,
             entries[i].turnaround_time,
             entries[i].waiting_time);
      #endif
      total_waiting_time += entries[i].waiting_time;
      total_turnaround_time += entries[i].turnaround_time;
    }

    if (list_count > 0) {
      averages.waiting_time = (double) total_waiting_time / list_count;
      averages.turnaround_time = (double) total_turnaround_time / list_count;
    }

    free(entries);
    entries = NULL;
  }

  return averages;
}
