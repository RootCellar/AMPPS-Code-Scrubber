
#include "stdio.h"

#include "util.h"
#include "memory_correction.h"
#include "terminal_color.h"

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
    printf("Code Copy #%d: ", i);
    for(int k = 0; k < collection.data_size; k++) {
      print_bits(collection.data_copies[i][k]);
      printf("  ");
    }
    printf("\n");
  }
}

void print_correct_errors_demo(int num_copies, size_t len, struct data_copies_collection collection) {
  printf("%s\n Before Correction:\n\n%s", ANSI_COLOR_BRIGHT_CYAN, ANSI_COLOR_RESET);
  print_data_copies_bits(collection);

  int fixes = correct_errors(collection.data_copies, num_copies, (int)len);

  printf("%s\n After Correction:\n\n%s", ANSI_COLOR_BRIGHT_CYAN, ANSI_COLOR_RESET);
  print_data_copies_bits(collection);

  printf("\n");
  printf("%d fixes\n", fixes);
}