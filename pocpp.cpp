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

  FILE *out;
  FILE *err;

  char *args[3];
  args[0] = argv[0];
  args[1] = "x";
  args[2] = 0;

  int res = p::open(args, &out, &err);
  printf("popen returned %d\n", res);

  char buf[1024];
  while (fgets(buf, sizeof(buf), out)) {
    printf("> %s", buf);
  }
  printf("popen ended\n");

  fclose(out);
  fclose(err);

  return 0;
}
