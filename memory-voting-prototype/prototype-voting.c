
/*
 *
 * Author: Darian Marvel
 * 1/19/2025
 *
 * Prototype memory voting and correction for AMPPS
 *
*/


// Includes

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "memory_correction.h"
#include "time.h"

// Definitions

#define FLIP_RATE (1.0/(1000.0*1000.0))
#define NUM_COPIES (3)
#define DATA_SIZE (16*1024)
#define NUM_TEST_LOOPS (1*100)
#define FLIP_CHANCE_MOD (1000*1000*100)

// Functions

double roll_flip_chance() {
  double random_value = rand() % FLIP_CHANCE_MOD;
  return random_value / (double) FLIP_CHANCE_MOD;
}

char random_bit() {
  return 1 << (rand() % BITS_IN_BYTE);
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

struct memory_correction_test_result {
  int num_flips;
  int reported_corrections;
  int unsolved_errors;
};

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
    for(int j = 0; j < data_size; j++) {
      for(int k = 0; k < num_copies; k++) {
        if(data_copies[k][j] != original_data[j]) {
          results.unsolved_errors++;
          data_copies[k][j] = original_data[j];
        }
      }
    }
  }

  return results;
}

struct data_copies_collection {
  char* original_data;
  char** data_copies;
  int num_copies;
  int data_size;
};

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
  memset(toRet.data_copies, (int)NULL, copy_pointers_list_size);

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

// Main

int main(int argc, char** argv) {

  // Setup

  // Allocate memory for arrays
  struct data_copies_collection data = create_data_copy_collection(NUM_COPIES, DATA_SIZE);
  if(data.original_data == NULL) {
    printf("Failed to create data_copies_collection\n");
    exit(1);
  }

  // Fill original_data with random values and copy to data_copies
  srand(get_time().tv_nsec);
  fill_copies_with_random(data);

  // Test it

  int unsolved_errors = 0;
  int total_flips = 0;

  double flip_rate = FLIP_RATE;
  double flips_per_cycle;
  double expected_flips;

  struct timespec start, end;
  double time_spent;
  double cycles_per_second;

  while(unsolved_errors < 1) {

    start = get_time();

    struct memory_correction_test_result results = test_memory_correction(data.original_data, data.data_copies, NUM_COPIES, DATA_SIZE, NUM_TEST_LOOPS, flip_rate);

    end = get_time();
    time_spent = timespec_difference_seconds(start, end);
    cycles_per_second = (double) NUM_TEST_LOOPS / time_spent;

    unsolved_errors += results.unsolved_errors;
    total_flips += results.num_flips;

    flips_per_cycle = (double) results.num_flips / NUM_TEST_LOOPS;
    expected_flips = NUM_COPIES * DATA_SIZE * BITS_IN_BYTE * flip_rate;

    printf("%.20f flip_rate, %d flips, %f average flips per cycle (%f expected), %f seconds, %f cycles per second\n",
      flip_rate, results.num_flips, flips_per_cycle, expected_flips, time_spent, cycles_per_second);

    flip_rate *= 1.05;

  }

  printf("%d unsolved errors\n", unsolved_errors);
  printf("%d total flips\n", total_flips);
  printf("%.20f flip_rate\n", flip_rate);

  // Cleanup

  cleanup_data_copy_collection(&data);

  return 0;

}
