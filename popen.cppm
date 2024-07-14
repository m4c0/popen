module;
extern "C" void popen_perror(const char *);
#define POPEN_IMPLEMENTATION
#define POPEN_ERROR popen_perror
#include "popen.h"

export module popen;

namespace p {
export constexpr const auto open = proc_open;

export class proc {
  FILE *m_out{};
  FILE *m_err{};

  char m_last_line[1024]{};

public:
  explicit proc(char *const *cmd_line) { p::open(cmd_line, &m_out, &m_err); }
  ~proc() {
    fclose(m_out);
    fclose(m_err);
  }

  [[nodiscard]] const char *last_line_read() const { return m_last_line; }

  [[nodiscard]] bool gets() {
    return fgets(m_last_line, sizeof(m_last_line) - 1, m_out);
  }
  [[nodiscard]] bool gets_err(char *buffer, unsigned size) {
    return fgets(m_last_line, sizeof(m_last_line) - 1, m_err);
  }
};
}

extern "C" void popen_perror(const char *err) { fputs(err, stderr); }
