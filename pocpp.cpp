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

  p::proc proc { argv[0], "X" };
  while (proc.gets()) {
    putln("> ", proc.last_line_read());
  }
  putln("popen ended with code: ", proc.wait());
  return 0;
}
