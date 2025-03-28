#include "environment.h"

void __lock_memory_segment(int which) {
	//
}

void __unlock_memory_segment(int which) {
	//
}

void __data_write(char** data_copies, int which, int loc, char value) {
	//
}

char __data_read(char** data_copies, int which, int loc) {
  return data_copies[which][loc];
}

