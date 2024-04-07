#define POPEN_IMPLEMENTATION
#include "popen.h"

int main(int argc, char ** argv) {
  if (argc > 1) {
    printf("hello from child stdout\n");
    return 0;
  }

  char args[3];
  args[0] = argv[0];
  args[1] = "x";
  args[2] = 0;
  return proc_open(args, &out, &err);
}
