#define POPEN_ERROR(x) printf("error: %s\n", (x))
#define POPEN_IMPLEMENTATION
#include "popen.h"

int main(int argc, char ** argv) {
  if (argc > 1) {
    printf("hello from child stdout\n");
    printf("how are you today?\n");
    printf("I know I'm fine because ");
    return 3;
  }

  FILE *out;
  FILE *err;

  char *args[3];
  args[0] = argv[0];
  args[1] = "x";
  args[2] = 0;
  void * res = proc_open(args, &out, &err);
  printf("popen returned %p\n", res);

  char buf[1024];
  while (fgets(buf, sizeof(buf), out)) {
    printf("> %s", buf);
  }
  printf("popen ended %d\n", proc_wait(res));

  fclose(out);
  fclose(err);

  return 0;
}
