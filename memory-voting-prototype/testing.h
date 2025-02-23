#ifndef TESTING_H
#define TESTING_H

#include "stddef.h"

#define FLIP_CHANCE_MOD (1000*1000*100)

#define malloc(n) custom_malloc(n)

struct memory_correction_test_result {
  int num_flips;
  int reported_corrections;
  int unsolved_errors;
};

struct data_copies_collection {
  char* original_data;
  char** data_copies;
  int num_copies;
  int data_size;
};

void* custom_malloc(size_t n);

double roll_flip_chance();

int simulate_flips(char** data_copies, int num_copies, int data_size, double flip_rate);
struct memory_correction_test_result test_memory_correction(char* original_data, char** data_copies, int num_copies, int data_size, int cycles, double flip_rate);

int count_errors(char* original_data, char** data_copies, int num_copies, int data_size);
void copy_original_to_copies(char* original_data, char** data_copies, int num_copies, int data_size);

void cleanup_data_copy_collection(struct data_copies_collection* data_copies);
struct data_copies_collection create_data_copy_collection(int copies, int size);
void fill_copies_with_random(struct data_copies_collection data_copies);

#endif