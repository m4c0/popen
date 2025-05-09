module;
extern "C" void popen_perror(const char *);
#define POPEN_IMPLEMENTATION
#define POPEN_ERROR popen_perror
#include "popen.h"

export module popen;
import no;

namespace p {
static constexpr const auto cstr_buf_size = 10240;

struct cstr : no::no {
  char * buf = new char[cstr_buf_size];
  ~cstr() { delete[] buf; }
};
struct file : no::no {
  FILE * h;
  ~file() { fclose(h); }
  [[nodiscard]] bool gets(cstr & s) { return fgets(s.buf, cstr_buf_size - 1, h); }
};

export class proc {
  cstr m_last_line {};
  file m_out {};
  file m_err {};
  void * m_handle;

public:
  explicit proc(auto... args)
    : m_out {}
    , m_err {}
    , m_handle { proc_open(&m_out.h, &m_err.h, args..., 0) }
  {}

  [[nodiscard]] auto wait() const { return proc_wait(m_handle); }

  [[nodiscard]] const char * last_line_read() const { return m_last_line.buf; }

  [[nodiscard]] bool gets()     { return m_out.gets(m_last_line); }
  [[nodiscard]] bool gets_err() { return m_err.gets(m_last_line); }
};
}

extern "C" void popen_perror(const char * err) { fputs(err, stderr); }
