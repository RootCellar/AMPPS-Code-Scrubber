
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
#include "environment.h"
#include "testing.h"
#include "time.h"

// Definitions

#define FLIP_RATE (1.0/(1000.0*1000.0))
#define NUM_COPIES (3)
#define DATA_SIZE (16*1024)
#define NUM_TEST_LOOPS (1*100)

// Functions

char random_bit() {
  return 1 << (rand() % BITS_IN_BYTE);
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

  set_environment_memory_segments(NUM_COPIES);

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
