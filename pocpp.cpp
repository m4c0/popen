#pragma leco tool

#include <string.h>

import popen;
import print;

int main(int argc, char **argv) {
  if (argc > 1) {
    putln("hello from child stdout");
    putln("how are you today?");
    put("I know I'm fine because ");
    return 3;
  }

  char *args[3];
  args[0] = argv[0];
  args[1] = strdup("x");
  args[2] = 0;

  p::proc proc{args};
  while (proc.gets()) {
    put("> ", proc.last_line_read());
  }
  putln("popen ended with code: ", proc.wait());
  return 0;
}
