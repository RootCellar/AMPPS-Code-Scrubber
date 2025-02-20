
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

  return 0;
}
