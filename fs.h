/* -----------------------------------------------------------------------------
 *
 * (c) Tamar Christina 2018
 *
 * Windows I/O routines for file opening.
 *
 * NOTE: Only modify this file in utils/fs/ and rerun configure. Do not edit
 *       this file in any other directory as it will be overwritten.
 *
 * ---------------------------------------------------------------------------*/

#pragma once

#include <stdio.h>

#if !defined(FS_NAMESPACE)
#define FS_NAMESPACE phyx
#endif

/* Play some dirty tricks to get CPP to expand correctly.  */
#define FS_FULL(ns, name) __##ns##_##name
#define prefix FS_NAMESPACE
#define FS_L(p, n) FS_FULL(p, n)
#define FS(name) FS_L(prefix, name)

#if defined(_WIN32)
#include <wchar.h>
wchar_t* FS(create_device_name) (const wchar_t*);
int FS(translate_mode) (const wchar_t*);
int FS(swopen) (const wchar_t* filename, int oflag,
                int shflag, int pmode);
FILE *FS(fwopen) (const wchar_t* filename, const wchar_t* mode);
FILE *FS(fopen) (const char* filename, const char* mode);
int FS(_stat) (const char *path, struct _stat *buffer);
int FS(_stat64) (const char *path, struct __stat64 *buffer);
int FS(_wstat) (const wchar_t *path, struct _stat *buffer);
int FS(_wstat64) (const wchar_t *path, struct __stat64 *buffer);
#else

FILE *FS(fopen) (const char* filename, const char* mode);
#endif
