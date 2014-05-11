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
#include <string.h>

#include "scheduler.h"
#include "thread.h"
#include "user_input.h"

// Forward declarations.
static void init_data(struct SharedData *const restrict data,
                      const size_t buf_size);

static void wait_for_schedulers(struct SharedData *const restrict shared_data);

/*
 * Main
 *
 * Will start up NUM_THREADS of threads running in background for
 * schedulers. When given input from the user, informs the threads and
 * outputs the result from the thread when its finished.
 */
int main(void) {
  pthread_t sched_threads[NUM_THREADS];
  const int BUFFER_SIZE = 100;
  struct SharedData shared_data;

  init_data(&shared_data, BUFFER_SIZE);

  // Stop the child threads from going past the input stage.
  pthread_mutex_lock(&shared_data.input_mutex);

  for (int i = 0; i < NUM_THREADS; ++i) {
    pthread_create(&sched_threads[i], NULL, &run_sched_thread, &shared_data);
  }

  // Wait for all the children to be created.
  wait_for_schedulers(&shared_data);

  while (file_from_user(shared_data.input_buffer, BUFFER_SIZE)) {
    shared_data.input_ready = true;

    pthread_cond_broadcast(&shared_data.input_cond);

    // Stop the children racing ahead and putting something in the
    // output buffer before we're ready.
    pthread_mutex_lock(&shared_data.output_mutex);

    pthread_mutex_unlock(&shared_data.input_mutex);

    // Wait for all the children to have read the input buffer.
    wait_for_schedulers(&shared_data);

    pthread_mutex_lock(&shared_data.input_mutex);
    shared_data.input_ready = false;
    pthread_mutex_unlock(&shared_data.input_mutex);
    pthread_mutex_unlock(&shared_data.output_mutex);

    // Children can now start processing, so check if we have any
    // output from them.
    pthread_mutex_lock(&shared_data.scheduler_ready_mutex);

    while (shared_data.schedulers_ready != NUM_THREADS) {
      pthread_mutex_lock(&shared_data.output_mutex);

      while (!shared_data.output_ready) {
        pthread_cond_wait(&shared_data.output_cond,
                          &shared_data.output_mutex);
      }

      // Something is in the output buffer.
      printf("%s", shared_data.output_buffer);

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
    pthread_mutex_lock(&shared_data.input_mutex);
  }

  shared_data.quit = true;
  pthread_cond_broadcast(&shared_data.input_cond);
  pthread_mutex_unlock(&shared_data.input_mutex);

  pthread_exit(0);

  free(shared_data.input_buffer);
  free(shared_data.output_buffer);

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
  // Need to wait here until we know that the threads are up and running.
  // So we will block here until they're ready.
  pthread_mutex_lock(&shared_data->scheduler_ready_mutex);

  while (shared_data->schedulers_ready != NUM_THREADS ) {
    pthread_cond_wait(&shared_data->scheduler_ready_cond,
                      &shared_data->scheduler_ready_mutex);
  }

  // Scheduler threads have started, now they will be considered ready
  // when they read the input.
  shared_data->schedulers_ready = 0;
  pthread_mutex_unlock(&shared_data->scheduler_ready_mutex);
}
