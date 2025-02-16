
#ifndef TIME_H
#define TIME_H

#include <time.h>
#include <sys/time.h>

#define ONE_BILLION (1000000000)

double timespec_to_double(struct timespec time) {
  double nanoseconds_in_seconds = (double) time.tv_nsec / (double) ONE_BILLION;
  double seconds = time.tv_sec;
  return seconds + nanoseconds_in_seconds;
}

double timespec_difference_seconds(struct timespec start, struct timespec end) {
  // Shrink tv_sec down to the actual integer difference between them
  // Helps protect time measurements from floating point roundoff because
  // tv_sec is a large number
  end.tv_sec -= start.tv_sec;
  start.tv_sec = 0;

  return timespec_to_double(end) - timespec_to_double(start);
}

struct timespec get_time() {
  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  return time;
}

#endif //TIME_H
