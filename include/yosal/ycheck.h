#ifndef _YOSAL_YCHECK_H
#define _YOSAL_YCHECK_H 1

#if !defined(__BIONIC__)
#if defined(__GNUC__) && (__GNUC__ > 2 || (__GNUC__ == 2 && GNUC_MINOR >= 1))
#define YOSAL_HAVE_BACKTRACE 1
#include <execinfo.h>
#endif
#endif

#ifndef __has_feature
#define __has_feature(x) 0  // Compatibility with non-clang compilers.
#endif

#ifndef CLANG_ANALYZER_NORETURN
#if __has_feature(attribute_analyzer_noreturn)
#define CLANG_ANALYZER_NORETURN __attribute__((analyzer_noreturn))
#else
#define CLANG_ANALYZER_NORETURN
#endif
#endif

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#undef YCHECK
#undef YDCHECK

#ifdef NDEBUG
#define YDCHECK(e) ((void)0)
#else
#define YDCHECK(e) (!(e) ? __ycheck_noreturn(#e, __FILE__, __LINE__) : (void)0)
#endif

static inline int __ycheck(const char *e, const char *file, unsigned int line)
{
  ALOGE("%s:%u: failed assertion '%s'", file, line, e);

#if defined(YOSAL_HAVE_BACKTRACE)
  void *callstack[128];
  int frames = backtrace(callstack, sizeof(callstack)/sizeof(callstack[0]));
  char **symbols = backtrace_symbols(callstack, frames);
  if (symbols != NULL) {
    int i;
    for (i = 0; i < frames; i++) {
      ALOGE("[%d] %s", i, (symbols[i] == NULL) ? "<unknown>":symbols[i]);
    }
    free(symbols);
  }
#endif

  return 1;
}

static inline void __ycheck_noreturn(const char *e, const char *file, unsigned int line) CLANG_ANALYZER_NORETURN;
static inline void __ycheck_noreturn(const char *e, const char *file, unsigned int line)
{
  __ycheck(e, file, line);

  abort();
}

#ifdef __cplusplus
};
#endif

#endif // _YOSAL_YCHECK_H
