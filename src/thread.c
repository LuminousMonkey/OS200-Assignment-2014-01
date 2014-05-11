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

// Forward declarations.
static Scheduler setup_sched_thread(struct SharedData *const restrict shared_data,
                                    int *const restrict thread_number);

static void write_result_to_buffer(struct SharedData *const restrict shared_data,
                                   struct SchedulerAverages averages,
                                   const int thread_number);

/*
 * run_sched_thread
 *
 * The thread code for running the scheduler. Just expects a pointer
 * to the shared data for mutex and conditionals, the thread will then
 * wait until there's input from the user. If there is, try to read in
 * the file and get the scheduler results, putting them into the
 * output buffer for the parent thread to read.
 */
void *run_sched_thread(void *shared_data_in) {
  struct SharedData *const restrict shared_data = shared_data_in;

  int thread_number;
  Scheduler scheduler_to_run = setup_sched_thread(shared_data,
                                                  &thread_number);

  pthread_mutex_lock(&shared_data->input_mutex);

  while (!shared_data->quit) {
    while (!shared_data->input_ready && !shared_data->quit) {
      pthread_cond_wait(&shared_data->input_cond, &shared_data->input_mutex);
    }

    if (!shared_data->quit) {
      // We have some input. We can release the mutex on the input
      // straight away, we only read it, and the main thread won't
      // change it until it gets the result from us.
      pthread_mutex_unlock(&shared_data->input_mutex);

      struct SchedulerAverages averages =
          run_scheduler(shared_data->input_buffer, scheduler_to_run);

      // We do however, need to tell the input thread that we've got the input.
      pthread_mutex_lock(&shared_data->scheduler_ready_mutex);
      shared_data->schedulers_ready++;

      pthread_cond_signal(&shared_data->scheduler_ready_cond);
      pthread_mutex_unlock(&shared_data->scheduler_ready_mutex);

      write_result_to_buffer(shared_data, averages, thread_number);

      // Thread is done, wait for more input.
      pthread_mutex_lock(&shared_data->input_mutex);
    }
  }

  pthread_mutex_unlock(&shared_data->input_mutex);
  pthread_exit(NULL);

  return NULL;
}

/*
 * setup_sched_thread
 *
 * Takes a pointer to the shared mutexes and the current thread
 * number. Even numbered threads will run the SJF scheduler, others
 * will be the round robin scheduler.
 */
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

/*
 * write_result_to_buffer
 *
 * Takes in a pointer to the mutexes, the calculated averages, and the
 * thread number of the scheduler thread that created the result.
 * Waits until it can write to the output buffer and will return when
 * it finally can.
 */
static void write_result_to_buffer(struct SharedData *const restrict shared_data,
                                   struct SchedulerAverages averages,
                                   const int thread_number) {

  pthread_mutex_lock(&shared_data->output_mutex);

  // Make sure we haven't grabbed the mutex before the parent thread.
  while (shared_data->output_ready) {
    pthread_cond_wait(&shared_data->output_cond,
                      &shared_data->output_mutex);
  }

  // Set the correct prefix for the scheduler output.
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
  pthread_mutex_unlock(&shared_data->output_mutex);
}
