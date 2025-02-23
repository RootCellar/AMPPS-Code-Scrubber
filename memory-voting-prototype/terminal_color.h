#ifndef TERMINAL_COLOR_H
#define TERMINAL_COLOR_H

char* color(char* color);

#define ANSI_COLOR_RED     color("\x1b[31m")
#define ANSI_COLOR_GREEN   color("\x1b[32m")
#define ANSI_COLOR_YELLOW  color("\x1b[33m")
#define ANSI_COLOR_BLUE    color("\x1b[34m")
#define ANSI_COLOR_MAGENTA color("\x1b[35m")
#define ANSI_COLOR_CYAN    color("\x1b[36m")

#define ANSI_COLOR_GRAY           color("\x1b[90m")
#define ANSI_COLOR_BRIGHT_RED     color("\x1b[91m")
#define ANSI_COLOR_BRIGHT_GREEN   color("\x1b[92m")
#define ANSI_COLOR_BRIGHT_YELLOW  color("\x1b[93m")
#define ANSI_COLOR_BRIGHT_BLUE    color("\x1b[94m")
#define ANSI_COLOR_BRIGHT_MAGENTA color("\x1b[95m")
#define ANSI_COLOR_BRIGHT_CYAN    color("\x1b[96m")

#define ANSI_COLOR_RESET   color("\x1b[0m")

#endif
