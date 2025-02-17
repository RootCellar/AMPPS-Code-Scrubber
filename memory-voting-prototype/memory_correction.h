#ifndef MEMORY_CORRECTION_H
#define MEMORY_CORRECTION_H

#define SEEK_TYPE char
#define BITS_IN_BYTE (8)

char correct_bits(char** data_copies, int num_copies, int loc);
int correct_errors(char** data_copies, int num_copies, int data_size);

#endif