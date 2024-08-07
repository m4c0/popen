#ifndef POPEN_H
#define POPEN_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>

extern int proc_open(char *const *cmd_line, FILE **out, FILE **err);

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

static int proc__create_process(char *cmd_line, HANDLE out, HANDLE err) {
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
    return 1;
  }

  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  CloseHandle(out);
  CloseHandle(err);
  return 0;
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
int proc_open(char *const *cmd_line, FILE **out, FILE **err) {
  HANDLE outs[2];
  HANDLE errs[2];
  if (0 != proc__create_pipes(outs, errs))
    return 1;

  char buf[10240];
  buf[0] = 0;
  while (*cmd_line) {
    if (buf[0] != 0)
      strcat_s(buf, sizeof(buf), " ");
    strcat_s(buf, sizeof(buf), *cmd_line);
    cmd_line++;
  }
  if (0 != proc__create_process(buf, outs[1], errs[1]))
    return 1;

  if (0 != proc__fdopen(outs[0], out))
    return 1;
  if (0 != proc__fdopen(errs[0], err))
    return 1;

  return 0;
}

#else // !_WIN32
#include <unistd.h>

int proc_open(char *const *cmd_line, FILE **out, FILE **err) {
  int pout[2];
  if (0 != pipe(pout))
    return -1;

  int perr[2];
  if (0 != pipe(perr))
    return -1;

  int pid = fork();
  if (pid < 0) {
    return -1;
  }
  if (pid == 0) {
    close(0);
    close(pout[0]);
    close(perr[0]);
    dup2(pout[1], 1);
    dup2(perr[1], 2);
    execvp(cmd_line[0], cmd_line);
    return -1;
  }

  close(pout[1]);
  close(perr[1]);

  *out = fdopen(pout[0], "r");
  *err = fdopen(perr[0], "r");
  return 0;
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
