
#include <stdlib.h>

#include "memory_correction.h"

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
  if(data_size % sizeof(SEEK_TYPE) != 0) return -1;

  SEEK_TYPE** data_copies_cast = (SEEK_TYPE**) data_copies;

  int corrections = 0;

  for(size_t i = 0; i < data_size / sizeof(SEEK_TYPE); i++) {

    char value = data_copies_cast[0][i];

    // Search forward through the data copies
    // and make sure they all agree with the current byte.
    // If they don't, perform a correction
    for(int k = 1; k < num_copies; k++) {
      if(data_copies_cast[k][i] != value) {

        for(size_t j = 0; j < sizeof(SEEK_TYPE); j++) {
          corrections += correct_bits(data_copies, num_copies, i * sizeof(SEEK_TYPE) + j);
        }

        k = num_copies;
      }
    }

  }

  return corrections;
}