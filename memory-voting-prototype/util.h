#ifndef UTIL_H
#define UTIL_H

#include "testing.h"

void print_bits(char c);
void print_data_copies_bits(struct data_copies_collection collection);
void print_correct_errors_demo(int num_copies, size_t len, struct data_copies_collection collection);

#endif