
#include "stdio.h"

#include "util.h"
#include "memory_correction.h"

void print_bits(char c) {
  // printf("%d ", c);
  for(int i = BITS_IN_BYTE - 1; i >= 0; i--) {
    char bit = 1 << i;
    // printf("%d ", bit);
    if((c & bit) != 0) printf("1 ");
    else printf("0 ");
  }
}

void print_data_copies_bits(struct data_copies_collection collection) {
  for(int i = 0; i < collection.num_copies; i++) {
    for(int k = 0; k < collection.data_size; k++) {
      print_bits(collection.data_copies[i][k]);
      printf("  ");
    }
    printf("\n");
  }
}