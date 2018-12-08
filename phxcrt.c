#include <fs.h>
#include <stdio.h>
#include <wchar.h>
#include "phxcrt.h"

FILE *fopen(
  const char *filename,
  const char *mode)
{
  return FS(fopen) (filename, mode);
}

FILE *_wfopen(
  const wchar_t *filename,
  const wchar_t *mode)
{
  return FS(fwopen) (filename, mode);
}

int _open(
  const char *filename,
  int oflag,
  int pmode)
{
  size_t len = mbstowcs (NULL, filename, 0);
  wchar_t *w_filename = malloc (sizeof (wchar_t) * (len + 1));
  mbstowcs (w_filename, filename, len);
  w_filename[len] = L'\0';

  int result = _wopen (w_filename, oflag, pmode);
  free (w_filename);
  return result;
}

int _wopen(
  const wchar_t *filename,
  int oflag,
  int pmode)
{
  return FS(swopen) (filename, oflag, 0, pmode);
}

int _sopen(
  const char *filename,
  int oflag,
  int shflag,
  int pmode)
{
  size_t len = mbstowcs (NULL, filename, 0);
  wchar_t *w_filename = malloc (sizeof (wchar_t) * (len + 1));
  mbstowcs (w_filename, filename, len);
  w_filename[len] = L'\0';

  int result = _wsopen (w_filename, oflag, shflag, pmode);
  free (w_filename);
  return result;
}

int _wsopen(
  const wchar_t *filename,
  int oflag,
  int shflag,
  int pmode)
{
  return FS(swopen) (filename, oflag, shflag, pmode);
}

FILE *_fsopen(
  const char *filename,
  const char *mode,
  int shflag)
{
  size_t len = mbstowcs (NULL, filename, 0);
  wchar_t *w_filename = malloc (sizeof (wchar_t) * (len + 1));
  mbstowcs (w_filename, filename, len);
  w_filename[len] = L'\0';

  len = mbstowcs (NULL, mode, 0);
  wchar_t *w_mode = malloc (sizeof (wchar_t) * (len + 1));
  mbstowcs (w_mode, mode, len);
  w_mode[len] = L'\0';

  FILE *result = _wfsopen (w_filename, w_mode, shflag);
  free (w_filename);
  free (w_mode);
  return result;
}

FILE *_wfsopen(
  const wchar_t *filename,
  const wchar_t *mode,
  int shflag)
{
  int oflag = FS(translate_mode) (mode);
  return FS(swopen) (filename, oflag, shflag, 0);
}