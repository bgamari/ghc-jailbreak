#include <stdlib.h>
#include <wchar.h>
#include "fs.h"
#include "phxcrt.h"

FILE *FS(_wfopen) (
  const wchar_t *filename,
  const wchar_t *mode)
{
  return FS(fwopen) (filename, mode);
}

int FS(_open) (
  const char *filename,
  int oflag,
  int pmode)
{
  size_t len = mbstowcs (NULL, filename, 0);
  wchar_t *w_filename = malloc (sizeof (wchar_t) * (len + 1));
  mbstowcs (w_filename, filename, len);
  w_filename[len] = L'\0';

  int result = FS(_wopen) (w_filename, oflag, pmode);
  free (w_filename);
  return result;
}

int FS(_wopen) (
  const wchar_t *filename,
  int oflag,
  int pmode)
{
  return FS(swopen) (filename, oflag, 0, pmode);
}

int FS(_sopen) (
  const char *filename,
  int oflag,
  int shflag,
  int pmode)
{
  size_t len = mbstowcs (NULL, filename, 0);
  wchar_t *w_filename = malloc (sizeof (wchar_t) * (len + 1));
  mbstowcs (w_filename, filename, len);
  w_filename[len] = L'\0';

  int result = FS(_wsopen) (w_filename, oflag, shflag, pmode);
  free (w_filename);
  return result;
}

int FS(_wsopen) (
  const wchar_t *filename,
  int oflag,
  int shflag,
  int pmode)
{
  return FS(swopen) (filename, oflag, shflag, pmode);
}

FILE *FS(_fsopen) (
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

  FILE *result = FS(_wfsopen) (w_filename, w_mode, shflag);
  free (w_filename);
  free (w_mode);
  return result;
}

FILE *FS(_wfsopen) (
  const wchar_t *filename,
  const wchar_t *mode,
  int shflag)
{
  int oflag = FS(translate_mode) (mode);
  int fd = FS(swopen) (filename, oflag, shflag, 0);
  return _wfdopen (fd, mode);
}