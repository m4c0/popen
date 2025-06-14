#pragma leco tool
#define POPEN_ERROR(x) printf("error: %s\n", (x))
#define POPEN_IMPLEMENTATION
#include "popen.h"

int main(int argc, char ** argv) {
  if (argc > 1) {
    printf("now ");
    return 3;
  }

  FILE *out[8];
  FILE *err[8];
  void *pids[8];
  for (int i = 0; i < 8; i++) {
    pids[i] = proc_open(out + i, err + i, argv[0], "x", 0);
  }

  for (int i = 0, max = 8; i < 8; i++) {
    int n = max;
    int res = proc_wait_any(pids, &n);

    char buf[1024];
    while (fgets(buf, sizeof(buf), out[n])) {
      printf("> %s", buf);
    }
    printf("popen ended %d\n", res);

    fclose(out[n]);
    fclose(err[n]);

    // We have to do this "dance" because of Windows
    pids[n] = pids[max - 1];
    out[n] = out[max - 1];
    err[n] = err[max - 1];
  }

  return 0;
}
