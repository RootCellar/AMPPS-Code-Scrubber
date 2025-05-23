#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#define LOCK_MEMORY_SEGMENT(x) __lock_memory_segment(x)
#define UNLOCK_MEMORY_SEGMENT(x) __unlock_memory_segment(x)

#define DATA_WRITE(data_copies, which, loc, bits) __data_write(data_copies, which, loc, bits)
#define DATA_READ(data_copies, which, loc) __data_read(data_copies, which, loc)

void __lock_memory_segment(int which);
void __unlock_memory_segment(int which);

void __data_write(char** data_copies, int which, int loc, char value);
char __data_read(char** data_copies, int which, int loc);

/*
 * Functions for tests on a real computer
 * these should *not* exist or be called when running on the MSP430 Microcontroller
*/

void set_environment_memory_segments(char num);
int memory_segment_is_locked(int which);
int all_memory_segments_locked();

#endif
