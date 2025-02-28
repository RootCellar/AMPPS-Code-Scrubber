
#include <unistd.h>

#include "terminal_color.h"

char* color(char* color) {
  return isatty(STDOUT_FILENO) ? color : "";
}