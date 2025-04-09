
#include <stdlib.h>

#include "memory_correction.h"
#include "environment.h"

#ifdef SCRUBBING_ON_MSP430
char agreements[NUM_CODE_COPIES];
#endif

// Performs a bit-level correction of the byte at loc in data_copies
char correct_bits(char** data_copies, int num_copies, int loc) {

  int i, j, k;

#ifndef SCRUBBING_ON_MSP430
  char agreements[num_copies];
#endif

  char most_agreements;
  char most_agreed_value;

  char corrections = 0;

  for(i = 0; i < BITS_IN_BYTE; i++) {

    // Reset tracked agreements
    for(k = 0; k < num_copies; k++) agreements[k] = 0;
    most_agreements = 0;
    most_agreed_value = 0;

    char current_bit = 1 << i;

    // Search forward for data copies that agree
    // and keep track of how many are found
    for(k = 0; k < num_copies; k++) {
      for(j = k + 1; j < num_copies; j++) {
        if((DATA_READ(data_copies, j, loc) & current_bit) == (DATA_READ(data_copies, k, loc) & current_bit)) {
          agreements[k]++;
        }
      }
    }

    // Find the most agreed value
    for(k = 0; k < num_copies; k++) {
      if(agreements[k] > most_agreements) {
        most_agreements = agreements[k];
        most_agreed_value = DATA_READ(data_copies, k, loc) & current_bit;
      }
    }

    // Correct data copies to match
    for(k = 0; k < num_copies; k++) {
      if((DATA_READ(data_copies, k, loc) & current_bit) != most_agreed_value) {
        UNLOCK_MEMORY_SEGMENT(k);
        DATA_WRITE(data_copies, k, loc, DATA_READ(data_copies, k, loc) ^ current_bit);
        LOCK_MEMORY_SEGMENT(k);
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

  SEEK_TYPE** data_copies_cast = (SEEK_TYPE**) data_copies;

  int corrections = 0;
  size_t i, j;
  int k;

  if(num_copies < 2) return -1;
  if(data_size % sizeof(SEEK_TYPE) != 0) return -1;

  for(i = 0; i < data_size / sizeof(SEEK_TYPE); i++) {

    SEEK_TYPE value = data_copies_cast[0][i];

    // Search forward through the data copies
    // and make sure they all agree with the current byte.
    // If they don't, perform a correction
    for(k = 1; k < num_copies; k++) {
      if(data_copies_cast[k][i] != value) {
        for(j = 0; j < sizeof(SEEK_TYPE); j++) {
          corrections += correct_bits(data_copies, num_copies, i * sizeof(SEEK_TYPE) + j);
        }
        k = num_copies;
      }
    }
  }

  return corrections;
}