/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * Holds all the structures that are common between the different
 * threads.
 */

#ifndef THREAD_H_
#define THREAD_H_

#include <pthread.h>
#include <stdbool.h>

/*
 * Number of threads to run.
 */
#define NUM_THREADS 8

/*
 * Our shared data for our threads, this is to contain all the data
 * shared between threads.
 *
 * The input mutex handles the input buffer, the parent thread is the
 * only one that writes to this buffer, so we only need the mutex to
 * signal when the parent has finished writing. Any number of
 * scheduler threads can read from the buffer at the same time without
 * any problems.
 *
 * Scheduler threads are only to read from the input buffer when
 * input_ready is set to true.
 *
 * The input mutex also covers the quit variable, this is set to true
 * if the user wants to quit. Since quitting is based off if the user
 * typed QUIT as input, it's directly related to the input mutex as
 * well. Any scheduler threads should check the quit Boolean first.
 *
 * When a scheduler is done, it has to write its output to the output
 * buffer, however, we don't know if the parent thread is ready, or
 * reading the buffer. So the scheduler_finished_mutex is used.
 *
 * When a scheduler thread is ready, it will try to grab the scheduler
 * finished mutex, if it succeeds, then it will check the scheduler
 * ready variable. If true, then it will release the lock, otherwise
 * it will write to the output buffer and set the scheduler ready to
 * true.
 */
struct SharedData {
  pthread_mutex_t input_mutex;
  pthread_cond_t input_cond;
  bool input_ready;
  bool quit;

  pthread_mutex_t scheduler_ready_mutex;
  pthread_cond_t scheduler_ready_cond;
  int schedulers_ready;

  pthread_mutex_t output_mutex;
  pthread_cond_t output_cond;
  bool output_ready;

  char *input_buffer;
  char *output_buffer;
};

/*
 * Forward Declarations.
 */
void *run_sched_thread(void *shared_data_in);

#endif
