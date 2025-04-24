
#include <stdio.h>
#include <stdlib.h>

#include "memory_correction.h"
#include "environment.h"
#include "terminal_color.h"
#include "testing.h"
#include "time.h"
#include "util.h"

int main(int argc, char** argv) {

  int num_copies = 3;
  set_environment_memory_segments(num_copies);
  size_t len = 4*1;

  if(len % sizeof(SEEK_TYPE) != 0) {
    printf("%slen is not evenly divisible by SEEK_TYPE size!%s\n", ANSI_COLOR_BRIGHT_RED, ANSI_COLOR_RESET);
    exit(EXIT_FAILURE);
  }

  srand(get_time().tv_nsec);

  struct data_copies_collection collection = create_data_copy_collection(num_copies, len);

  char original_byte = 0b11001100;
  for(int i = 0; i < len; i++) collection.original_data[i] = original_byte;
  copy_original_to_copies(collection.original_data, collection.data_copies, collection.num_copies, collection.data_size);

  printf("%s\n Whole Flipped Bytes %s\n", ANSI_COLOR_BRIGHT_YELLOW, ANSI_COLOR_RESET);

  for(size_t i = 0; i < len; i++) {
    collection.data_copies[i % num_copies][i] = ~original_byte;
  }

  print_correct_errors_demo(num_copies, len, collection);

  printf("%s\n Randomly Flipped Bits %s\n", ANSI_COLOR_BRIGHT_YELLOW, ANSI_COLOR_RESET);

  for(size_t i = 0; i < len; i++) {
    for(int k = 0; k < BITS_IN_BYTE; k++) {
      collection.data_copies[rand() % num_copies][i] ^= 1 << k;
    }
  }

  print_correct_errors_demo(num_copies, len, collection);

  return 0;
}
