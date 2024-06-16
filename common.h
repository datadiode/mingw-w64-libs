#include <windows.h>

#ifdef _M_X64
#define $ ""
#define IMP(t, f) t f(), (*const __imp_##f)() = f; t f
#else
#define $ "_"
#define IMP(t, f) t f(), (*const _imp__##f)() = f; t f
#endif

#define alternatename(a, b) \
    __pragma(comment(linker, "/alternatename:" $ #a "=" $ #b)) \
    __pragma(comment(linker, "/alternatename:__imp_" $ #a "=__imp_" $ #b))
