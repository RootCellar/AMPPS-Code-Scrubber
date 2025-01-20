
/*
 *
 * Author: Darian Marvel
 * 1/19/2024
 *
 * Prototype memory voting and correction for AMPPS
 *
*/


// Includes

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Definitions

#define FLIP_RATE (0.001)
#define NUM_COPIES (3)
#define DATA_SIZE (16*1024)
#define NUM_TEST_LOOPS (1*1000)
#define FLIP_CHANCE_MOD (1000*1000)

// Functions

struct timespec get_time() {
  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  return time;
}

float roll_flip_chance() {
  float random_value = rand() % FLIP_CHANCE_MOD;
  return random_value / FLIP_CHANCE_MOD;
}

char random_bit() {
  return 1 << (rand() % 8);
}

void simulate_flips(char** data_copies, int num_copies, int data_size, float flip_rate) {

  float random_roll;

  for(int i = 0; i < num_copies; i++) {
    for(int k = 0; k < data_size; k++) {
      random_roll = roll_flip_chance();
      if(random_roll < flip_rate) {
        data_copies[i][k] ^= random_bit();
      }
    }
  }
}

void correct_errors(char** data_copies, int num_copies, int data_size) {
  
}

// Main

int main(int argc, char** argv) {

  // Setup

  // Allocate memory for arrays
  char* original_data = (char*) malloc(sizeof(char) * DATA_SIZE);
  if(original_data == NULL) {
    printf("Unable to allocate memory for original_data\n");
    return 1;
  }

  char** data_copies = (char**) malloc(sizeof(char*) * NUM_COPIES);
  if(data_copies == NULL) {
    printf("Unable to allocate memory for data_copies list\n");
    return 1;
  }

  for(int i = 0; i < NUM_COPIES; i++) {
    data_copies[i] = (char*) malloc(sizeof(char) * DATA_SIZE);
    if(data_copies[i] == NULL) {
      printf("Unable to allocate memory for memory copy %d\n", i);
      return 1;
    }
  }

  // Fill original_data with random values and copy to data_copies
  srand(get_time().tv_nsec);

  for(int i = 0; i < DATA_SIZE; i++) {
    original_data[i] = rand();
    for(int k = 0; k < NUM_COPIES; k++) {
      data_copies[k][i] = original_data[i];
    }
  }

  // Test it

  int unsolved_errors = 0;

  for(int i = 0; i < NUM_TEST_LOOPS; i++) {
    simulate_flips(data_copies, NUM_COPIES, DATA_SIZE, FLIP_RATE);
    correct_errors(data_copies, NUM_COPIES, DATA_SIZE);

    for(int k = 0; k < NUM_COPIES; k++) {
      for(int j = 0; j < DATA_SIZE; j++) {
        if(data_copies[k][j] != original_data[j]) {
          unsolved_errors++;
          data_copies[k][j] = original_data[j];
        }
      }
    }
  }

  printf("%d unsolved errors\n", unsolved_errors);

  // Cleanup

  free(original_data);

  return 0;

}
