#include "Print.hpp"

#include <iomanip>
#include <iostream>

void print(const char* format, ...)
{
  va_list args;
  va_start(args, format);
  printf(">>> ");
  vprintf(format, args);
  printf("\n");
  va_end(args);
}
