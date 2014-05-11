/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * The scheduler threads, these are the threads that do the actual
 * calculation, reading the input from the parent thread, then writing
 * to the output buffer when completed.
 *
 * Their flow of control is in a few stages, with the transition from
 * each stage being controlled by the parent thread.
 *
 * Check the assignment.c for details.
 */

#include <pthread.h>
#include <stdio.h>

#include "rr_scheduler.h"
#include "scheduler.h"
#include "sjf_scheduler.h"
#include "thread.h"

// Strings for displaying scheduler type.
#define RR_STR "RR"
#define SJF_STR "SJF"

/*
 * Forward declarations.
 */
static Scheduler setup_sched_thread(struct SharedData *const restrict shared_data,
                                    int *const restrict thread_number);

static void write_result_to_buffer(struct SharedData *const restrict shared_data,
                                   struct SchedulerAverages averages,
                                   const int thread_number);

void *run_sched_thread(void *shared_data_in) {
  struct SharedData *const restrict shared_data = shared_data_in;

  int thread_number;

  /*
   * Here we figure out which scheduler we're going to run and
   * increment the scheduler's ready variable so the parent thread
   * knows that the threads are waiting on input.
   */
  Scheduler scheduler_to_run = setup_sched_thread(shared_data,
                                                  &thread_number);

  // Wait until there's input, or the user wants to quit.
  pthread_mutex_lock(&shared_data->input_mutex);

  // User doesn't want to quit.
  while (!shared_data->quit) {
    while (!shared_data->input_ready && !shared_data->quit) {
      pthread_cond_wait(&shared_data->input_cond, &shared_data->input_mutex);
    }

    if (!shared_data->quit) {
      // We have some input. We can release the mutex on the input
      // straight away, we only read it, and the main thread won't
      // change it until it gets the result from us.
      pthread_mutex_unlock(&shared_data->input_mutex);

      // While the parent thread is waiting for us, it will not touch
      // anything, so we don't have to lock.
      struct SchedulerAverages averages =
          run_scheduler(shared_data->input_buffer, scheduler_to_run);

      // We do however, need to tell the input thread that we've got the input.
      pthread_mutex_lock(&shared_data->scheduler_ready_mutex);
      shared_data->schedulers_ready++;

      pthread_cond_signal(&shared_data->scheduler_ready_cond);
      pthread_mutex_unlock(&shared_data->scheduler_ready_mutex);

      write_result_to_buffer(shared_data, averages, thread_number);

      /*
       * Wrote to the output buffer, this means we're going back to
       * trying to ready the input. Try to lock the input mutex before
       * we ready anything.
       */
      pthread_mutex_lock(&shared_data->input_mutex);
    }
  }

  // User wants to quit
  pthread_mutex_unlock(&shared_data->input_mutex);

  pthread_exit(NULL);
}

static Scheduler setup_sched_thread(struct SharedData *const restrict shared_data,
                                    int *const restrict thread_number) {
  pthread_mutex_lock(&shared_data->scheduler_ready_mutex);
  *thread_number = shared_data->schedulers_ready;

  Scheduler scheduler_to_run;
  if ((shared_data->schedulers_ready++ % 2) != 0) {
    scheduler_to_run = rr_scheduler;
  } else {
    scheduler_to_run = sjf_scheduler;
  }

  pthread_cond_signal(&shared_data->scheduler_ready_cond);
  pthread_mutex_unlock(&shared_data->scheduler_ready_mutex);

  return scheduler_to_run;
}

static void write_result_to_buffer(struct SharedData *const restrict shared_data,
                                   struct SchedulerAverages averages,
                                   const int thread_number) {

  // We have a result now, so we let the parent thread know.
  pthread_mutex_lock(&shared_data->output_mutex);

  // Make sure we haven't grabbed the mutex before the parent thread.
  while (shared_data->output_ready) {
    pthread_cond_wait(&shared_data->output_cond,
                      &shared_data->output_mutex);
  }

  char *scheduler_type;

  if (thread_number % 2 != 0) {
    scheduler_type = RR_STR;
  } else {
    scheduler_type = SJF_STR;
  }

  // We put our result string into the output buffer, and let the
  // parent thread know.
  sprintf(shared_data->output_buffer,
          "%s:\t"
          "Average Waiting: %f. "
          "Average Turnaround: %f\n",
          scheduler_type,
          averages.waiting_time,
          averages.turnaround_time);

  shared_data->output_ready = true;

  pthread_cond_broadcast(&shared_data->output_cond);

  // We've sent a signal
  pthread_mutex_unlock(&shared_data->output_mutex);
}
