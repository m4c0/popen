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

#if 0
  p::proc proc { argv[0], "X" };
#else
  const char * args[] { argv[0], "X" };
  p::proc proc { 2, args };
#endif
  while (proc.gets()) {
    putln("> ", proc.last_line_read());
  }
  putln("popen ended with code: ", proc.wait());
  return 0;
}
