#ifndef POPEN_H
#define POPEN_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

extern void * proc_open(FILE **out, FILE **err, const char * argv0, /* null-terminated argv */...);
extern int    proc_wait(void * p);
extern int    proc_wait_any(void ** ps, int * n);

#ifdef __cplusplus
}
#endif

#ifdef POPEN_IMPLEMENTATION
#ifndef POPEN_ERROR
#define POPEN_ERROR(x)
#endif

#ifdef _WIN32
#define WIN32_MEAN_AND_LEAN
#include <io.h>
#include <windows.h>

static HANDLE proc__create_process(char *cmd_line, HANDLE out, HANDLE err) {
  PROCESS_INFORMATION pi;
  ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

  STARTUPINFO si;
  ZeroMemory(&si, sizeof(STARTUPINFO));
  si.cb = sizeof(STARTUPINFO);
  si.hStdError = err;
  si.hStdOutput = out;
  si.dwFlags = STARTF_USESTDHANDLES;

  if (!CreateProcess(NULL, cmd_line, NULL, NULL, TRUE, 0, NULL, NULL, &si,
                     &pi)) {
    POPEN_ERROR("failed to create child process");
    return NULL;
  }

  CloseHandle(pi.hThread);

  CloseHandle(out);
  CloseHandle(err);
  return pi.hProcess;
}
static int proc__create_pipes(HANDLE *outs, HANDLE *errs) {
  SECURITY_ATTRIBUTES attr;
  ZeroMemory(&attr, sizeof(SECURITY_ATTRIBUTES));
  attr.nLength = sizeof(SECURITY_ATTRIBUTES);
  attr.bInheritHandle = TRUE;
  attr.lpSecurityDescriptor = NULL;

  if (!CreatePipe(outs, outs + 1, &attr, 0)) {
    POPEN_ERROR("failed to create output pipe");
    return 1;
  }
  if (!SetHandleInformation(outs[0], HANDLE_FLAG_INHERIT, 0)) {
    POPEN_ERROR("failed to set inheritance of output pipe");
    return 1;
  }

  if (!CreatePipe(errs, errs + 1, &attr, 0)) {
    POPEN_ERROR("failed to create error pipe");
    return 1;
  }
  if (!SetHandleInformation(errs[0], HANDLE_FLAG_INHERIT, 0)) {
    POPEN_ERROR("failed to set inheritance of error pipe");
    return 1;
  }

  return 0;
}
static int proc__fdopen(HANDLE h, FILE **f) {
  int fd = _open_osfhandle((intptr_t)h, 0);
  if (fd < 0) {
    POPEN_ERROR("failed to transform Windows HANDLE into FD");
    return 1;
  }
  *f = _fdopen(fd, "r");
  if (*f == NULL) {
    POPEN_ERROR("failed to transform FD into FILE");
    return 1;
  }
  return 0;
}
void * proc_open(FILE **out, FILE **err, const char * argv0, /* null-terminated argv */...) {
  HANDLE outs[2];
  HANDLE errs[2];
  if (0 != proc__create_pipes(outs, errs)) return NULL;

  char buf[10240];
  strcpy_s(buf, sizeof(buf), argv0);

  va_list va;
  va_start(va, argv0);
  const char * arg;
  while ((arg = va_arg(va, const char *))) {
    strcat_s(buf, sizeof(buf), " ");
    strcat_s(buf, sizeof(buf), arg);
  }
  va_end(va);

  HANDLE res = proc__create_process(buf, outs[1], errs[1]);
  if (!res) return NULL;

  if (0 != proc__fdopen(outs[0], out)) return NULL;
  if (0 != proc__fdopen(errs[0], err)) return NULL;

  return res;
}

int proc__wait(void * handle) {
  HANDLE h = (HANDLE)handle;
  DWORD res = STILL_ACTIVE;
  GetExitCodeProcess(h, &res);
  CloseHandle(h);
  return res;
}
int proc_wait(void * handle) {
  WaitForSingleObject((HANDLE) handle, INFINITE);
  return proc__wait(handle);
}

int proc_wait_any(void ** handles, int * n) {
  int res = WaitForMultipleObjects(n, (HANDLE *)hs, FALSE, INFINITE);
  int got = res - WAIT_OBJECT_0;
  if (got < 0 && got >= n) return -1;

  *n = got;
  return proc__wait(handles[got]);
}

#else // !_WIN32
#include <sys/wait.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void * proc_open(FILE **out, FILE **err, const char * argv0, /* null-terminated argv */...) {
  int pout[2];
  if (0 != pipe(pout))
    return 0;

  int perr[2];
  if (0 != pipe(perr))
    return 0;

  pid_t pid = fork();
  if (pid < 0) {
    return 0;
  }
  if (pid == 0) {
    close(0);
    close(pout[0]);
    close(perr[0]);
    dup2(pout[1], 1);
    dup2(perr[1], 2);

    char ** args = (char **)malloc(sizeof(char *) * 1024);
    char ** argp = args + 1;
    args[0] = strdup(argv0);

    va_list va;
    va_start(va, argv0);
    const char * arg;
    while ((arg = va_arg(va, const char *))) *argp++ = strdup(arg);
    va_end(va);

    *argp = 0;

    execvp(argv0, args);
    return 0;
  }

  close(pout[1]);
  close(perr[1]);

  *out = fdopen(pout[0], "r");
  *err = fdopen(perr[0], "r");
  return (void *)(size_t)pid;
}

int proc_wait(void * handle) {
  int status;
  waitpid((pid_t)(size_t) handle, &status, 0);
  return WEXITSTATUS(status);
}

int proc_wait_any(void ** handles, int * n) {
  int status;
  pid_t res = wait(&status);
  for (int i = 0; i < *n; i++) {
    if (res != (pid_t)(size_t) handles[i]) continue;

    *n = i;
    return WEXITSTATUS(status);
  }
  return -1;
}

#endif // !_WIN32
#endif // POPEN_IMPLEMENTATION
#endif // POPEN_H

// MIT License
//
// Copyright (c) 2024 Eduardo Costa
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
