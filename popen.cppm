module;
extern "C" void popen_perror(const char *);
#define POPEN_IMPLEMENTATION
#define POPEN_ERROR popen_perror
#include "popen.h"

export module popen;
import no;

namespace p {
export constexpr const auto open = proc_open;
export constexpr const auto wait = proc_wait;

struct file : no::no {
  FILE * h;
  ~file() { fclose(h); }
  [[nodiscard]] bool gets(char * buf, unsigned size) { return fgets(buf, size, h); }
};

export class proc {
  file m_out {};
  file m_err {};
  void * m_handle;

  char m_last_line[1024] {};

public:
  explicit proc(char *const *cmd_line) : m_handle { p::open(cmd_line, &m_out.h, &m_err.h) } {}

  [[nodiscard]] auto wait() const { return p::wait(m_handle); }

  [[nodiscard]] const char * last_line_read() const { return m_last_line; }

  [[nodiscard]] bool gets()     { return m_out.gets(m_last_line, sizeof(m_last_line) - 1); }
  [[nodiscard]] bool gets_err() { return m_err.gets(m_last_line, sizeof(m_last_line) - 1); }
};
}

extern "C" void popen_perror(const char * err) { fputs(err, stderr); }
