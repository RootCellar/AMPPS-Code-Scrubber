
#include <stdlib.h>

#include "memory_correction.h"
#include "environment.h"

// Performs a bit-level correction of the byte at loc in data_copies
__attribute__ ((always_inline))
char correct_bits(char** data_copies, int num_copies, int32_t loc) {

  int i, j;

  char one_votes;
  char zero_votes;
  char current_bit = 1;

  char most_agreed_value;

  char corrections = 0;

  for(i = 0; i < BITS_IN_BYTE; i++) {

    // Reset variables
    one_votes = 0;
    zero_votes = 0;

    // Count zeros and ones votes
    for(j = 0; j < num_copies; j++) {
      if((DATA_READ(data_copies, j, loc) & current_bit) != 0) one_votes++;
      else zero_votes++;
    }

    // Find the most agreed value
    if(one_votes > zero_votes) most_agreed_value = current_bit;
    else most_agreed_value = 0;

    // Correct data copies to match
    for(j = 0; j < num_copies; j++) {
      if((DATA_READ(data_copies, j, loc) & current_bit) != most_agreed_value) {
        UNLOCK_MEMORY_SEGMENT(j);
        DATA_WRITE(data_copies, j, loc, DATA_READ(data_copies, j, loc) ^ current_bit);
        LOCK_MEMORY_SEGMENT(j);
        corrections++;
      }
    }

    current_bit <<= 1;

  }

  return corrections;
}

// Searches through every byte and ensures that all data copies agree on
// the value of each. In the event that the data copies don't agree on
// the value of a particular byte, correct the copies of that byte
// at the bit level.
int32_t correct_errors(char** data_copies, int num_copies, int32_t data_size) {

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