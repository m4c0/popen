module;
extern "C" void popen_perror(const char *);
#define POPEN_IMPLEMENTATION
#define POPEN_ERROR popen_perror
#include "popen.h"

export module popen;

namespace p {
export constexpr const auto open = proc_open;
}

extern "C" void popen_perror(const char *) {}
