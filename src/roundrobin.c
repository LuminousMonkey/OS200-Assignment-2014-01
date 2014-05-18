/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * Section One of the assignment, this is the mainline of the round
 * robin scheduler.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "rr_scheduler.h"
#include "scheduler.h"
#include "user_input.h"

int main() {
  const int FILENAME_SIZE = 100;
  char filename[FILENAME_SIZE];

  printf("RR Simulation: ");

  while (file_from_user(filename, FILENAME_SIZE)) {
    struct SchedulerAverages averages;

    averages = run_scheduler(filename, &rr_scheduler);

    printf("Average turnaround time=%.2f."
           "Average waiting time=%.2f\n",
           averages.turnaround_time, averages.waiting_time);

    printf("RR Simulation: ");
  }
}
