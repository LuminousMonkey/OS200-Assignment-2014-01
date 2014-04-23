/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * Section three of the assignment.
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "debugging.h"
#include "rr_scheduler.h"
#include "scheduler.h"
#include "sjf_scheduler.h"
#include "thread.h"
#include "user_input.h"

#define NUM_THREADS 2

static void init_data(struct SharedData *const restrict data,
                      const size_t buf_size);

static void wait_for_schedulers(struct SharedData *const restrict shared_data);

static Scheduler setup_sched_thread(struct SharedData *const restrict shared_data,
                                    int *const restrict thread_number);

static void write_result_to_buffer(struct SharedData *const restrict shared_data,
                                   struct SchedulerAverages averages,
                                   const int thread_number);

void *run_sched_thread(void *shared_data_in);

int main(void) {
  pthread_t sched_threads[NUM_THREADS];
  const int BUFFER_SIZE = 100;
  struct SharedData shared_data;

  init_data(&shared_data, BUFFER_SIZE);

  debug_print("P: Creating threads.\n");
  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&sched_threads[i], NULL, &run_sched_thread, &shared_data);
  }

  /*
   * The scheduler threads have been started, however, we can't
   * continue until we know they're ready.
   */
  wait_for_schedulers(&shared_data);

  /*
   * By the time we've reached here, the scheduler threads will be
   * waiting on the input.
   */

  // Grab control of the input mutex, we should be uncontended because
  // the threads are waiting on us.
  debug_print("Trying to grab input_mutex.\n");
  pthread_mutex_lock(&shared_data.input_mutex);
  debug_print("Got input mutex.\n");

  /*
   * By the time we get here, the scheduler threads will be ready to
   * check for input, either they're waiting for the cond to be
   * signaled, or they're just about to try and grab the mutex and
   * check.
   */
  while (file_from_user(shared_data.input_buffer, BUFFER_SIZE)) {

    // We now have our filename from the user.
    // Let all the waiting schedulers know.
    shared_data.input_ready = true;

    debug_print("P: Broadcasting input ready.\n");
    pthread_cond_broadcast(&shared_data.input_cond);

    // And unlock the mutex on the input buffer so they will read from
    // it.
    pthread_mutex_unlock(&shared_data.input_mutex);

    debug_print("P: Waiting on schedulers to read input.\n");
    // Now we need to wait for the threads to be ready reading the
    // input. But we have to make sure that we grab the output buffer
    // first.
    wait_for_schedulers(&shared_data);

    /*
     * Our schedulers have read the input, they may have already
     * produced the output and might be waiting on the output buffer.
     * Since the parent thread is here, it means that the scheduler
     * threads will be running, or blocked on output. This means we
     * need to mark input_ready as false so they don't try reading the
     * file again when we do unblock them on the output buffer.
     */
    debug_print("P: Clearing input ready.\n");
    pthread_mutex_lock(&shared_data.input_mutex);
    shared_data.input_ready = false;
    pthread_mutex_unlock(&shared_data.input_mutex);

    debug_print("P: Input ready cleared.\n");


    /*
     * The parent thread needs to lock the mutex on the schedulers
     * ready. When we have all the data, then the schedulers_ready
     * variable is what the scheduler's set to indicate they're ready
     * for input, if we don't grab it now, then we a scheduler thread
     * finishes here, it might so indicate it's ready messing this
     * test up.
     */
    pthread_mutex_lock(&shared_data.scheduler_ready_mutex);

    while (shared_data.schedulers_ready != NUM_THREADS) {

      /*
       * Still don't have all the results yet, check if a thread has
       * something for us.
       */
      pthread_mutex_lock(&shared_data.output_mutex);

      while (!shared_data.output_ready) {
        /*
         * Nothing for us yet, so block, waiting until something is
         * ready.
         */
        debug_print("P: Waiting on output buffer.\n");
        pthread_cond_wait(&shared_data.output_cond,
                          &shared_data.output_mutex);
      }

      // Something is in the output buffer.
      printf("%s", shared_data.output_buffer);

      /*
       * We've got a result from a scheduler, increase the counter so
       * we'll exit the loop when they're all done.
       */
      shared_data.schedulers_ready++;
      shared_data.output_ready = false;

      // Signal that the output buffer is ready to any other threads
      // waiting.
      pthread_cond_signal(&shared_data.output_cond);
      pthread_mutex_unlock(&shared_data.output_mutex);
    }

    /*
     * Before we get back to the input loop, we need to clear off.
     */
    shared_data.schedulers_ready = 0;
    pthread_mutex_unlock(&shared_data.scheduler_ready_mutex);

    /*
     * At this stage, all the scheduler threads have given us their
     * output. They could be locked, waiting on some input, or they
     * might not have got there yet, we just have to grab the input
     * mutex.
     */
    debug_print("P: Trying to grab input_mutex.\n");
    pthread_mutex_lock(&shared_data.input_mutex);
    debug_print("P: Got input mutex.\n");
  }

  shared_data.quit = true;
  pthread_cond_broadcast(&shared_data.input_cond);
  pthread_mutex_unlock(&shared_data.input_mutex);

  pthread_exit(0);

  return EXIT_SUCCESS;
}

static void init_data(struct SharedData *const restrict data,
                      const size_t buf_size) {

  pthread_mutex_init(&data->input_mutex, NULL);
  pthread_cond_init(&data->input_cond, NULL);
  data->input_ready = false;
  data->quit = false;

  pthread_mutex_init(&data->scheduler_ready_mutex, NULL);
  pthread_cond_init(&data->scheduler_ready_cond, NULL);
  data->schedulers_ready = 0;

  pthread_mutex_init(&data->output_mutex, NULL);
  pthread_cond_init(&data->output_cond, NULL);

  data->output_ready = false;

  data->input_buffer = calloc(1,buf_size);
  data->output_buffer = calloc(1,buf_size);
}

static void wait_for_schedulers(struct SharedData *const restrict shared_data) {
  debug_print("P: Wait for schedulers.\n");
  // Need to wait here until we know that the threads are up and running.
  // So we will block here until they're ready.
  pthread_mutex_lock(&shared_data->scheduler_ready_mutex);

  debug_print("P: Got lock on scheduler_running_mutex.\n");

  debug_print("P: There are %d schedulers running.\n",
         shared_data->schedulers_ready);

  while (shared_data->schedulers_ready != 2 ) {
    debug_print("P: Waiting on schedulers to become ready.\n");
    pthread_cond_wait(&shared_data->scheduler_ready_cond,
                      &shared_data->scheduler_ready_mutex);
    debug_print("P: There are %d schedulers running.\n",
           shared_data->schedulers_ready);
  }

  debug_print("P: Scheduler's ready.\n");

  // Scheduler threads have started, now they will be considered ready
  // when they read the input.
  shared_data->schedulers_ready = 0;
  pthread_mutex_unlock(&shared_data->scheduler_ready_mutex);
}

void *run_sched_thread(void *shared_data_in) {
  struct SharedData *const restrict shared_data = shared_data_in;

  int thread_number;

  debug_print("\tStarting scheduler thread.\n");

  /*
   * Here we figure out which scheduler we're going to run and
   * increment the scheduler's ready variable so the parent thread
   * knows that the threads are waiting on input.
   */
  Scheduler scheduler_to_run = setup_sched_thread(shared_data,
                                                  &thread_number);

  debug_print("\t%d - Trying to get input_mutex.\n", thread_number);

  // Wait until there's input, or the user wants to quit.
  pthread_mutex_lock(&shared_data->input_mutex);
  debug_print("\t%d - Got input mutex.\n", thread_number);

  // User doesn't want to quit.
  while (!shared_data->quit) {

    debug_print("\t%d - Not quitting yet.\n", thread_number);
    // There's no input for the thread yet, so we wait.
    while (!shared_data->input_ready && !shared_data->quit) {
      debug_print("\t%d - Input not ready yet, waiting.\n", thread_number);
      pthread_cond_wait(&shared_data->input_cond, &shared_data->input_mutex);
    }

    if (!shared_data->quit) {
      debug_print("\t%d - Input ready.\n", thread_number);
      // We have some input. We can release the mutex on the input
      // straight away, we only read it, and the main thread won't
      // change it until it gets the result from us.
      pthread_mutex_unlock(&shared_data->input_mutex);

      // We do however, need to tell the input thread that we've got the input.
      debug_print("\t%d - Trying to get Scheduler Ready Mutex.\n", thread_number);
      pthread_mutex_lock(&shared_data->scheduler_ready_mutex);
      shared_data->schedulers_ready++;
      debug_print("\t%d - %d schedulers ready.\n", thread_number,
                  shared_data->schedulers_ready);
      pthread_cond_signal(&shared_data->scheduler_ready_cond);
      debug_print("\t%d - Signalled scheduler ready cond.\n", thread_number);
      pthread_mutex_unlock(&shared_data->scheduler_ready_mutex);

      debug_print("\t%d - Running Scheduler.\n", thread_number);

      // While the parent thread is waiting for us, it will not touch
      // anything, so we don't have to lock.
      struct SchedulerAverages averages =
          run_scheduler(shared_data->input_buffer, scheduler_to_run);

      debug_print("\t%d - Trying to write to output buffer.\n", thread_number);
      write_result_to_buffer(shared_data, averages, thread_number);
      debug_print("\t%d - Wrote to output buffer.\n", thread_number);

      /*
       * Wrote to the output buffer, this means we're going back to
       * trying to ready the input. Try to lock the input mutex before
       * we ready anything.
       */
      debug_print("\t%d - Grabbing input mutex before main thread loop.\n",
                  thread_number);
      pthread_mutex_lock(&shared_data->input_mutex);
    }
  }

  // User wants to quit
  pthread_mutex_unlock(&shared_data->input_mutex);

  pthread_exit(NULL);
}

static Scheduler setup_sched_thread(struct SharedData *const restrict shared_data,
                                    int *const restrict thread_number) {
  debug_print("\tSetting up scheduler thread.\n");

  // Let the parent thread know we're ready.
  pthread_mutex_lock(&shared_data->scheduler_ready_mutex);
  debug_print("\tIncreasing schedulers running by 1.\n");

  Scheduler scheduler_to_run;
  if (shared_data->schedulers_ready++ == 1) {
    scheduler_to_run = rr_scheduler;
    *thread_number = 1;
    debug_print("\t%d - Round robin scheduler.\n", *thread_number);
  } else {
    scheduler_to_run = sjf_scheduler;
    *thread_number = 2;
    debug_print("\t%d - SJF scheduler.\n", *thread_number);
  }

  debug_print("\t%d - Signalling to parent number of scheduler's changed.\n",
         *thread_number);

  pthread_cond_signal(&shared_data->scheduler_ready_cond);
  pthread_mutex_unlock(&shared_data->scheduler_ready_mutex);

  return scheduler_to_run;
}

static void write_result_to_buffer(struct SharedData *const restrict shared_data,
                                   struct SchedulerAverages averages,
                                   const int thread_number) {

  debug_print("\t%d - Trying to lock output mutex.\n", thread_number);
  // We have a result now, so we let the parent thread know.
  pthread_mutex_lock(&shared_data->output_mutex);

  debug_print("\t%d - Got output mutex.\n", thread_number);

  // Make sure we haven't grabbed the mutex before the parent thread.
  while (shared_data->output_ready) {
    debug_print("\t%d - Other thread is writing to output.\n", thread_number);
    pthread_cond_wait(&shared_data->output_cond,
                      &shared_data->output_mutex);
    debug_print("\t%d - We have the output buffer.\n", thread_number);
  }

  // We put our result string into the output buffer, and let the
  // parent thread know.
  sprintf(shared_data->output_buffer,
          "T: %d. "
          "Average Waiting: %f. "
          "Average Turnaround: %f\n",
          thread_number,
          averages.waiting_time,
          averages.turnaround_time);

  shared_data->output_ready = true;

  debug_print("T: %d - Signalling average result ready.\n", thread_number);
  pthread_cond_signal(&shared_data->output_cond);

  // We've sent a signal
  pthread_mutex_unlock(&shared_data->output_mutex);
}
