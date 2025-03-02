
/*
 *
 * Author: Darian Marvel
 * 2/17/2025
 *
 * Unit tests for AMPPS memory correction algorithms
 *
*/


// Includes

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include "unit_tests.h"
#include "environment.h"
#include "memory_correction.h"
#include "testing.h"

// Main

int main(int argc, char** argv) {

  SECTION("correct_bits");

  TEST_NAME("Two copies fix a fully flipped third copy");
  {
    set_environment_memory_segments(3);
    struct data_copies_collection collection = create_data_copy_collection(3, 1);

    char original_data = 0b10101010;

    collection.data_copies[0][0] = original_data;
    collection.data_copies[1][0] = original_data;
    collection.data_copies[2][0] = (char) ~original_data;

    int fixes = correct_bits(collection.data_copies, 3, 0);
    TEST(fixes == 8, "correct_bits reports making 8 fixes");
    TEST(collection.data_copies[2][0] == original_data, "two copies fix a fully flipped third copy");
    TEST(collection.data_copies[0][0] == original_data, "the first copy is not modified");
    TEST(collection.data_copies[1][0] == original_data, "the second copy is not modified");
  }

  TEST_NAME("3 copies with bit flips are corrected");
  {
    set_environment_memory_segments(3);
    struct data_copies_collection collection = create_data_copy_collection(3, 1);

    char original_data = 0b10101010;

    collection.data_copies[0][0] = 0b10001111;
    collection.data_copies[1][0] = 0b11111000;
    collection.data_copies[2][0] = 0b00100010;

    int fixes = correct_bits(collection.data_copies, 3, 0);
    TEST(fixes == 8, "correct_bits reports making 8 fixes");
    TEST(collection.data_copies[0][0] == original_data, "the first copy is fully corrected");
    TEST(collection.data_copies[1][0] == original_data, "the second copy is fully corrected");
    TEST(collection.data_copies[2][0] == original_data, "the third copy is fully corrected");
  }

  TEST_NAME("5 copies with bit flips are corrected");
  {
    set_environment_memory_segments(5);
    struct data_copies_collection collection = create_data_copy_collection(5, 1);

    char original_data = 0b10101010;

    collection.data_copies[0][0] = 0b10001111;
    collection.data_copies[1][0] = 0b11111000;
    collection.data_copies[2][0] = 0b00101010;
    collection.data_copies[3][0] = 0b01100110;
    collection.data_copies[4][0] = 0b10110010;

    int fixes = correct_bits(collection.data_copies, 5, 0);
    TEST(fixes == 13, "correct_bits reports making 13 fixes");
    TEST(collection.data_copies[0][0] == original_data, "the first copy is fully corrected");
    TEST(collection.data_copies[1][0] == original_data, "the second copy is fully corrected");
    TEST(collection.data_copies[2][0] == original_data, "the third copy is fully corrected");
    TEST(collection.data_copies[3][0] == original_data, "the fourth copy is fully corrected");
    TEST(collection.data_copies[4][0] == original_data, "the fifth copy is fully corrected");
  }

  SECTION("correct_errors");

  TEST_NAME("correct_errors properly uses correct_bits to synchronize 3 copies");
  {
    set_environment_memory_segments(3);
    int num_copies = 3;
    size_t len = 8*32;
    struct data_copies_collection collection = create_data_copy_collection(num_copies, len);

    char original_byte = 0b11001100;
    memset(collection.original_data, original_byte, len);
    copy_original_to_copies(collection.original_data, collection.data_copies, collection.num_copies, collection.data_size);

    for(size_t i = 0; i < len; i++) {
      collection.data_copies[i % num_copies][i] = ~original_byte;
    }
    TEST(count_errors(collection.original_data, collection.data_copies, collection.num_copies, collection.data_size) == len, "len erroneous bytes before correct_errors");

    int fixes = correct_errors(collection.data_copies, num_copies, len);
    TEST(fixes == len * 8, "correct_errors reports making len*8 fixes");
    TEST(count_errors(collection.original_data, collection.data_copies, collection.num_copies, collection.data_size) == 0, "0 errors after correct_errors");
  }

  TEST_NAME("correct_errors properly uses correct_bits to synchronize 5 copies");
  {
    set_environment_memory_segments(5);
    int num_copies = 5;
    size_t len = 8*32;
    struct data_copies_collection collection = create_data_copy_collection(num_copies, len);

    char original_byte = 0b11001100;
    memset(collection.original_data, original_byte, len);
    copy_original_to_copies(collection.original_data, collection.data_copies, collection.num_copies, collection.data_size);

    for(size_t i = 0; i < len; i++) {
      collection.data_copies[i % num_copies][i] = ~original_byte;
    }
    TEST(count_errors(collection.original_data, collection.data_copies, collection.num_copies, collection.data_size) == len, "len erroneous bytes before correct_errors");

    int fixes = correct_errors(collection.data_copies, num_copies, len);
    TEST(fixes == len * 8, "correct_errors reports making len*8 fixes");
    TEST(count_errors(collection.original_data, collection.data_copies, collection.num_copies, collection.data_size) == 0, "0 errors after correct_errors");
  }

  show_test_results();

  return 0;
}
