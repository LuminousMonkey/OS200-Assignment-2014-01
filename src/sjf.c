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

#include "scheduler.h"
#include "sjf_scheduler.h"
#include "user_input.h"

int main() {
  const int FILENAME_SIZE = 100;
  char filename[FILENAME_SIZE];

  while (file_from_user(filename, FILENAME_SIZE)) {
    struct SchedulerAverages averages;

    averages = run_scheduler(filename, &sjf_scheduler);

    printf("Average turnaround time=%f."
           "Average waiting time=%f\n",
           averages.turnaround_time, averages.waiting_time);
  }
}