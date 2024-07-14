#pragma leco tool

#include <stdio.h>

import popen;

int main(int argc, char **argv) {
  if (argc > 1) {
    printf("hello from child stdout\n");
    printf("how are you today?\n");
    printf("I know I'm fine because ");
    return 0;
  }

  char *args[3];
  args[0] = argv[0];
  args[1] = "x";
  args[2] = 0;

  p::proc proc{args};
  while (proc.gets()) {
    printf("> %s", proc.last_line_read());
  }
  printf("popen ended\n");
  return 0;
}
