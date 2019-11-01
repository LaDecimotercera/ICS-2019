#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buffer[1024];
  va_list ap;
  va_start(ap, fmt);
  vsprintf(buffer, fmt, ap);
  va_end(ap);
  int i = 0;
  while (buffer[i])
    _putc(buffer[i++]);
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, __INT_MAX__, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int n = vsprintf(out, fmt, args);
  va_end(args);
  return n;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

#endif
