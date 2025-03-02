
#include <stdio.h>
#include <stdlib.h>

#include "environment.h"
#include "terminal_color.h"

#define NUM_MEMORY_SEGMENTS (num_memory_segments)
#define MAX_MEMORY_SEGMENTS (100)

#define SEGMENT_LOCKED 0
#define SEGMENT_UNLOCKED 1

#define MEMORY_SEGMENT_FAIL_FAST 1

static char num_memory_segments = 0;

char segment_locks[MAX_MEMORY_SEGMENTS] = {SEGMENT_LOCKED};

void __lock_memory_segment(int which) {
  if(which >= NUM_MEMORY_SEGMENTS || which < 0) {
    printf("\n%sAttempted to lock invalid memory segment %d%s\n",
      ANSI_COLOR_RED, which, ANSI_COLOR_RESET);
    if(MEMORY_SEGMENT_FAIL_FAST) exit(EXIT_FAILURE);
  }

  segment_locks[which] = SEGMENT_LOCKED;
}

void __unlock_memory_segment(int which) {
  if(which >= NUM_MEMORY_SEGMENTS || which < 0) {
    printf("\n%sAttempted to unlock invalid memory segment %d%s\n",
      ANSI_COLOR_RED, which, ANSI_COLOR_RESET);
    if(MEMORY_SEGMENT_FAIL_FAST) exit(EXIT_FAILURE);
  }

  segment_locks[which] = SEGMENT_UNLOCKED;
}

void __data_write(char** data_copies, int which, int loc, char value) {
  if(which >= NUM_MEMORY_SEGMENTS || which < 0) {
    printf("\n%sAttempted to write to invalid memory segment %d[%d]%s\n",
      ANSI_COLOR_RED, which, loc, ANSI_COLOR_RESET);
    if(MEMORY_SEGMENT_FAIL_FAST) exit(EXIT_FAILURE);
  }

  if(segment_locks[which] != SEGMENT_LOCKED) {
    data_copies[which][loc] = value;
  } else {
    printf("\n%sAttempted to write to memory segment %d[%d] while it was locked%s\n",
      ANSI_COLOR_RED, which, loc, ANSI_COLOR_RESET);
    if(MEMORY_SEGMENT_FAIL_FAST) exit(EXIT_FAILURE);
  }
}

char __data_read(char** data_copies, int which, int loc) {
  return data_copies[which][loc];
}

/*
 * Functions for tests on a real computer
 * these should *not* exist or be called when running on the MSP430 Microcontroller
*/

void set_environment_memory_segments(char num) {
  num_memory_segments = num;
}

int memory_segment_is_locked(int which) {
  return segment_locks[which];
}

int all_memory_segments_locked() {
  for(int i = 0; i < MAX_MEMORY_SEGMENTS; i++) {
    if(segment_locks[i] != SEGMENT_LOCKED) return 0;
  }
  return 1;
}
