# popen

Single-header multi-platform popen-ish C/C++ library

If you only require STDOUT redirection, you are better using `popen`/`_popen`
because they are available natively.

This library is useful if you also want STDERR (or if you want to suppress it).
