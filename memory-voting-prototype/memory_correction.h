#ifndef MEMORY_CORRECTION_H
#define MEMORY_CORRECTION_H

#include <stdint.h>

#define SEEK_TYPE char
#define BITS_IN_BYTE (8)

char correct_bits(char** data_copies, int num_copies, int32_t loc);
int32_t correct_errors(char** data_copies, int num_copies, int32_t data_size);

#endif