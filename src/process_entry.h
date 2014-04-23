/*
 * OS200 - Assignment
 *
 * Author: Mike Aldred
 *
 * Description:
 *   Process Control Block entry, not an actual real one,
 *   but just what we're using to represent processes.
 */

#ifndef PROCESS_ENTRY_H_
#define PROCESS_ENTRY_H_

struct ProcessEntry {
  int arrival_time;
  int burst_time;
  int burst_time_remaining;
  int turnaround_time;
  int waiting_time;
};

/*
 * Possible errors we might get setting up process entries.
 */
enum ProcessEntryError {
  PROCESS_ENTRY_ERR_NONE = 0,
  PROCESS_ENTRY_ERR_ARRIVAL,
  PROCESS_ENTRY_ERR_BURST
};

/*
 * Init process entry
 *
 * Given a pointer to a process entry, and initial arrival and burst
 * times, initialise the entry to expected starting values.
 *
 * process_entry - Entry to update.
 * arrival_time - Arrival time.
 * burst_time - Burst time.
 */
enum ProcessEntryError init_process_entry(
    struct ProcessEntry *const restrict process_entry,
    const int arrival_time,
    const int burst_time);


#endif
