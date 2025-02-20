
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memory_correction.h"
#include "testing.h"

#undef malloc(n)
void* custom_malloc(size_t n) {
  void* ptr = malloc(n);
  if(ptr != NULL) return ptr;
  printf("Unable to allocate %lu byte(s)\n", n);
  exit(EXIT_FAILURE);
}
#define malloc(n) custom_malloc(n)

double roll_flip_chance() {
  double random_value = rand() % FLIP_CHANCE_MOD;
  return random_value / (double) FLIP_CHANCE_MOD;
}

// Returns how many bits this function flipped during the call
int simulate_flips(char** data_copies, int num_copies, int data_size, double flip_rate) {

  double random_roll;
  int flips = 0;

  for(int i = 0; i < num_copies; i++) {
    for(int k = 0; k < data_size; k++) {
      for(int j = 0; j < BITS_IN_BYTE; j++) {
        random_roll = roll_flip_chance();
        if(random_roll < flip_rate) {
          data_copies[i][k] ^= 1 << j;
          flips++;
        }
      }
    }
  }

  return flips;
}

int count_errors(char* original_data, char** data_copies, int num_copies, int data_size) {
  int errors = 0;

  for(int j = 0; j < data_size; j++) {
    for(int k = 0; k < num_copies; k++) {
      if(data_copies[k][j] != original_data[j]) {
        errors++;
      }
    }
  }

  return errors;
}

void copy_original_to_copies(char* original_data, char** data_copies, int num_copies, int data_size) {
  for(int j = 0; j < data_size; j++) {
    for(int k = 0; k < num_copies; k++) {
      data_copies[k][j] = original_data[j];
    }
  }
}

struct memory_correction_test_result test_memory_correction(char* original_data, char** data_copies, int num_copies, int data_size, int cycles, double flip_rate) {

  struct memory_correction_test_result results =
    (struct memory_correction_test_result) {.num_flips = 0, .reported_corrections = 0, .unsolved_errors = 0};

  for(int i = 0; i < cycles; i++) {

    // Cause memory bit flips and then have the algorithm attempt
    // to correct them
    results.num_flips += simulate_flips(data_copies, num_copies, data_size, flip_rate);
    results.reported_corrections += correct_errors(data_copies, num_copies, data_size);

    // Check to make sure the algorithm corrected every byte.
    // If it didn't, increment the unsolved error count and
    // correct it for the next iteration
    results.unsolved_errors += count_errors(original_data, data_copies, num_copies, data_size);
    copy_original_to_copies(original_data, data_copies, num_copies, data_size);
  }

  return results;
}

void cleanup_data_copy_collection(struct data_copies_collection* data_copies) {
  if(data_copies->original_data != NULL) {
    free(data_copies->original_data);
    data_copies->original_data = NULL;
  }

  if(data_copies->data_copies != NULL) {
    for(int i = 0; i < data_copies->num_copies; i++) {
      if(data_copies->data_copies[i] != NULL) {
        free(data_copies->data_copies[i]);
        data_copies->data_copies[i] = NULL;
      }
    }

    free(data_copies->data_copies);
    data_copies->data_copies = NULL;
  }
}

struct data_copies_collection create_data_copy_collection(int copies, int size) {

  struct data_copies_collection toRet =
    (struct data_copies_collection) {.original_data = NULL, .data_copies = NULL, .num_copies = copies, .data_size = size};

  size_t data_bytes = sizeof(char) * size;
  size_t copy_pointers_list_size = sizeof(char*) * copies;

  toRet.original_data = (char*) malloc(data_bytes);
  if(toRet.original_data == NULL) {
    printf("Unable to allocate memory for original_data\n");
    return toRet;
  }

  toRet.data_copies = (char**) malloc(copy_pointers_list_size);
  if(toRet.data_copies == NULL) {
    printf("Unable to allocate memory for data_copies list\n");
    cleanup_data_copy_collection(&toRet);
    return toRet;
  }
  memset(toRet.data_copies, (uintptr_t)NULL, copy_pointers_list_size);

  for(int i = 0; i < copies; i++) {
    toRet.data_copies[i] = (char*) malloc(data_bytes);
    if(toRet.data_copies[i] == NULL) {
      printf("Unable to allocate memory for memory copy %d\n", i);
      cleanup_data_copy_collection(&toRet);
      return toRet;
    }
  }

  return toRet;
}

void fill_copies_with_random(struct data_copies_collection data_copies) {
  for(int i = 0; i < data_copies.data_size; i++) {
    data_copies.original_data[i] = rand();
    for(int k = 0; k < data_copies.num_copies; k++) {
      data_copies.data_copies[k][i] = data_copies.original_data[i];
    }
  }
}
