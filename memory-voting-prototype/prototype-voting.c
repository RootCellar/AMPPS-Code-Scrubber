
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

#define FLIP_RATE (1.0/(1000.0*1000.0))
#define NUM_COPIES (3)
#define DATA_SIZE (16*1024)
#define NUM_TEST_LOOPS (1*1000)
#define FLIP_CHANCE_MOD (1000*1000*100)
#define BITS_IN_BYTE (8)

// Functions

struct timespec get_time() {
  struct timespec time;
  clock_gettime(CLOCK_REALTIME, &time);
  return time;
}

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

// Performs a bit-level correction of the byte at loc in data_copies
char correct_bits(char** data_copies, int num_copies, int loc) {

  char agreements[num_copies];

  char most_agreements;
  char most_agreed_value;

  char corrections = 0;

  for(int i = 0; i < BITS_IN_BYTE; i++) {

    // Reset tracked agreements
    for(int k = 0; k < num_copies; k++) agreements[k] = 0;
    most_agreements = 0;
    most_agreed_value = 0;

    char current_bit = 1 << i;

    // Search forward for data copies that agree
    // and keep track of how many are found
    for(int k = 0; k < num_copies; k++)
      for(int j = k+1; j < num_copies; j++) {
        if((data_copies[j][loc] & current_bit) == (data_copies[k][loc] & current_bit)) {
          agreements[k]++;
        }
      }

    // Find the most agreed value
    for(int k = 0; k < num_copies; k++) {
      if(agreements[k] > most_agreements) {
        most_agreements = agreements[k];
        most_agreed_value = data_copies[k][loc] & current_bit;
      }
    }

    // Correct data copies to match
    for(int k = 0; k < num_copies; k++) {
      if((data_copies[k][loc] & current_bit) != most_agreed_value) {
        data_copies[k][loc] ^= current_bit;
        corrections++;
      }
    }

  }

  return corrections;
}

// Searches through every byte and ensures that all data copies agree on
// the value of each. In the event that the data copies don't agree on
// the value of a particular byte, correct the copies of that byte
// at the bit level.
int correct_errors(char** data_copies, int num_copies, int data_size) {

  if(num_copies < 2) return -1;

  int corrections = 0;

  for(int i = 0; i < data_size; i++) {

    char value = data_copies[0][i];

    // Search forward through the data copies
    // and make sure they all agree with the current byte.
    // If they don't, perform a correction
    for(int k = 1; k < num_copies; k++) {
      if(data_copies[k][i] != value) {
        corrections += correct_bits(data_copies, num_copies, i);
      }
    }

  }

  return corrections;
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

    // Cause memory bit flips and then have the algorithm attempt
    // to correct them
    simulate_flips(data_copies, NUM_COPIES, DATA_SIZE, FLIP_RATE);
    correct_errors(data_copies, NUM_COPIES, DATA_SIZE);

    // Check to make sure the algorithm corrected every byte.
    // If it didn't, increment the unsolved error count and
    // correct it for the next iteration
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
