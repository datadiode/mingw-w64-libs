#include "common.h"

#if MSVCRT_VERSION < 90

typedef struct _iobuf
{
    char *_ptr;
    int _cnt;
    char *_base;
    int _flag;
    int _file;
    int _charbuf;
    int _bufsiz;
    char *_tmpfname;
} FILE;

// Have _nolock functions gracefully fall back to their locking counterparts
alternatename(_fread_nolock, fread)
alternatename(_fwrite_nolock, fwrite)
alternatename(_fseek_nolock, fseek)
alternatename(_fseeki64_nolock, _fseeki64)
alternatename(_ftell_nolock, ftell)
alternatename(_ftelli64_nolock, _ftelli64)
alternatename(_fflush_nolock, fflush)
alternatename(_fclose_nolock, fclose)
alternatename(_ungetwc_nolock, ungetwc)
alternatename(_getc_nolock, _fgetc_nolock)
alternatename(_getwc_nolock, _fgetwc_nolock)
alternatename(_putc_nolock, _fputc_nolock)
alternatename(_putwc_nolock, _fputwc_nolock)

// Offer an opportunity to speed up buffered (W)CHAR-wise IO
// Inspired by https://github.com/dlang/dmd/blob/master/druntime/src/rt/msvc.d
// Copyright Digital Mars 2015
// SPDX-License-Identifier: BSL-1.0

IMP(int, _fputc_nolock)(int c, FILE *fp)
{
    extern int fputc(int c, FILE *fp);
    if (fp->_cnt >= sizeof(CHAR))
    {
        *(CHAR UNALIGNED *)fp->_ptr = (CHAR)c;
        fp->_ptr += sizeof(CHAR);
        fp->_cnt -= sizeof(CHAR);
        return (CHAR)c;
    }
    else
    {
        return fputc(c, fp);
    }
}

IMP(int, _fgetc_nolock)(FILE *fp)
{
    extern int fgetc(FILE *fp);
    if (fp->_cnt >= sizeof(CHAR))
    {
        CHAR c = *(CHAR UNALIGNED *)fp->_ptr;
        fp->_ptr += sizeof(CHAR);
        fp->_cnt -= sizeof(CHAR);
        return c;
    }
    else
    {
        return fgetc(fp);
    }
}

IMP(int, _fputwc_nolock)(int c, FILE *fp)
{
    extern int fputwc(int c, FILE *fp);
    if (fp->_cnt >= sizeof(WCHAR))
    {
        *(WCHAR UNALIGNED *)fp->_ptr = (WCHAR)c;
        fp->_ptr += sizeof(WCHAR);
        fp->_cnt -= sizeof(WCHAR);
        return (WCHAR)c;
    }
    else
    {
        return fputwc(c, fp);
    }
}

IMP(int, _fgetwc_nolock)(FILE *fp)
{
    extern int fgetwc(FILE *fp);
    if (fp->_cnt >= sizeof(WCHAR))
    {
        WCHAR c = *(WCHAR UNALIGNED *)fp->_ptr;
        fp->_ptr += sizeof(WCHAR);
        fp->_cnt -= sizeof(WCHAR);
        return c;
    }
    else
    {
        return fgetwc(fp);
    }
}

#endif
