
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

#include "unit_tests.h"
#include "memory_correction.h"
#include "testing.h"

// Main

int main(int argc, char** argv) {

  SECTION("correct_bits");

  TEST_NAME("Two copies fix a fully flipped third copy");
  {
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

  show_test_results();

  return 0;
}
