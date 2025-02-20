#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <stdlib.h>
#include <stdio.h>

#include "terminal_color.h"

#define FAIL_FAST 1

#define TEST_PASS_OUTPUT_COLOR ANSI_COLOR_BRIGHT_GREEN
#define TEST_FAIL_OUTPUT_COLOR ANSI_COLOR_BRIGHT_RED

static int num_tests = 0;
static int num_tests_passed = 0;

#define TEST(expr, name) handle_test_result(#expr, expr, __FILE__, __func__, __LINE__, name, 0)
#define TEST_QUIET(expr, name) handle_test_result(#expr, expr, __FILE__, __func__, __LINE__, name, 1)

#define SECTION(name) \
        do { \
        printf("\n  " "%s ** " name " ** %s" "\n\n", ANSI_COLOR_BRIGHT_YELLOW, ANSI_COLOR_RESET);\
        } while(0)

void pass_test(const char* expr, const char* file, const char* func, int line, const char* name, int quiet_pass) {
  num_tests_passed++;

  if(!quiet_pass) {
    printf("%sTest: %s -- %s -- PASS %s\n", TEST_PASS_OUTPUT_COLOR, name, expr, ANSI_COLOR_RESET);
  }
}

void fail_test(const char* expr, const char* file, const char* func, int line, const char* name) {
  printf("\n%sTest: %s -- %s -- FAILED %s\n", TEST_FAIL_OUTPUT_COLOR, name, expr, ANSI_COLOR_RESET);
  printf("%s ** %s line %d in %s() ** " "%s\n", TEST_FAIL_OUTPUT_COLOR, file, line, func, ANSI_COLOR_RESET);

  if(FAIL_FAST) {
    exit(EXIT_FAILURE);
  }
}

void handle_test_result(const char* expr, int result, const char* file, const char* func, int line, const char* name, int quiet_pass) {
  num_tests++;

  if(result) {
    pass_test(expr, file, func, line, name, quiet_pass);
  }
  else {
    fail_test(expr, file, func, line, name);
  }
}

void show_test_results() {
  printf("\n\n%s %d PASSED%s\n", TEST_PASS_OUTPUT_COLOR, num_tests_passed, ANSI_COLOR_RESET);

  int failed = num_tests - num_tests_passed;
  char* outputColor = TEST_FAIL_OUTPUT_COLOR;
  if(failed == 0) {
    outputColor = TEST_PASS_OUTPUT_COLOR;
  }

  printf("%s %d FAILED%s\n", outputColor, failed, ANSI_COLOR_RESET);
}

#endif
