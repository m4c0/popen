#define POPEN_ERROR(x) printf("error: %s\n", (x))
#define POPEN_IMPLEMENTATION
#include "popen.h"

int main(int argc, char ** argv) {
  if (argc > 1) {
    printf("hello from child stdout\n");
    return 0;
  }

  FILE *out;
  FILE *err;

  char *args[3];
  args[0] = argv[0];
  args[1] = "x";
  args[2] = 0;
  printf("trying to popen\n");

  int res = proc_open(args, &out, &err);
  printf("popen returned %d\n", res);
  return 0;
}
