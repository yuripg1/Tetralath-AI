#ifndef MACROS_H
#define MACROS_H

#ifndef __has_attribute
    #define __has_attribute(x) 0
#endif

#if __has_attribute(hot) || defined(__GNUC__)
  #define HOT __attribute__((hot))
#else
  #define HOT
#endif

#if __has_attribute(aligned) || defined(__GNUC__)
  #define ALIGN_TO(n) __attribute__((aligned(n)))
#else
  #define ALIGN_TO(n)
#endif

#ifndef __has_builtin
    #define __has_builtin(x) 0
#endif

#if __has_builtin(__builtin_expect) || defined(__GNUC__)
  #define EXPECT(x) __builtin_expect((x), 1L)
  #define DO_NOT_EXPECT(x) __builtin_expect((x), 0L)
#else
  #define EXPECT(x) (x)
  #define DO_NOT_EXPECT(x) (x)
#endif

#endif
