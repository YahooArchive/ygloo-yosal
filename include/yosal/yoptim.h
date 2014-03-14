#ifndef _YOSAL_YOPTIM_H
#define	_YOSAL_YOPTIM_H

#ifdef	__cplusplus
extern "C" {
#endif


#undef YOPTIMIZE_SIZE
#undef YOPTIMIZE_SPEED
#undef YINLINE

#if defined(DEBUG) || !defined(NDEBUG)
#  define YOPTIMIZE_SIZE
#  define YOPTIMIZE_SPEED
#  define YINLINE
#else
#  if defined(__clang__)
#    define YOPTIMIZE_SIZE
#    define YOPTIMIZE_SPEED
#  else
#    define YOPTIMIZE_SIZE __attribute__((optimize("Os")))
#    define YOPTIMIZE_SPEED __attribute__((optimize("O3")))
#endif
#  ifndef YINLINE
#    if defined(__GNUC__)
#      define YINLINE __attribute__((always_inline)) inline
#    elif defined(_MSC_VER)
#      define YINLINE __forceinline
#    else
#      define YINLINE
#    endif
#  endif
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* _YOSAL_YOPTIM_H */

