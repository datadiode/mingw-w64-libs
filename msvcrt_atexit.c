#include "common.h"
#include <stdio.h>
#include <malloc.h>
#include <process.h>

#if MSVCRT_VERSION < 90
// Some graceful fallbacks (slightly off topic)
alternatename(_initterm_e, _initterm)
alternatename(_invalid_parameter_noinfo, _purecall)
#endif

#if MSVCRT_VERSION >= 140 // UCRT

static _onexit_table_t table;

void init_atexit()
{
    _initialize_onexit_table(&table);
}

void term_atexit()
{
    _execute_onexit_table(&table);
    _flushall(); // See MinGW's __dll_exit() for what this is good for
}

int atexit(void(*pfn)(void))
{
    return _register_onexit_function(&table, (_onexit_t)pfn);
}

#else

_onexit_t __dllonexit(_onexit_t func, _PVFV **start, _PVFV **end);

static _PVFV *begin = NULL;
static _PVFV *end = NULL;

void init_atexit()
{
    end = begin = calloc(1, sizeof(_PVFV));
}

void term_atexit()
{
    while (end > begin) (*--end)();
    free(begin);
    end = begin = NULL;
    _flushall(); // See MinGW's __dll_exit() for what this is good for
}

int atexit(void(*pfn)(void))
{
    return __dllonexit((_onexit_t)pfn, &begin, &end) ? 0 : -1;
}

#endif
