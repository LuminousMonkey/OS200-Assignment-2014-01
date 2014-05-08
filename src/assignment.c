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
#include "scheduler.h"
#include "thread.h"
#include "user_input.h"

static void init_data(struct SharedData *const restrict data,
                      const size_t buf_size);

static void wait_for_schedulers(struct SharedData *const restrict shared_data);

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

    debug_print("P: Locking output mutex.\n");
    pthread_mutex_lock(&shared_data.output_mutex);

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
    pthread_mutex_unlock(&shared_data.output_mutex);

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

  while (shared_data->schedulers_ready != NUM_THREADS ) {
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
