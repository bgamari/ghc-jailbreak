#include <wchar.h>
#include <stdio.h>
#include "fs.h"

FILE *FS(_wfopen) (
  const wchar_t *filename,
  const wchar_t *mode
);

int FS(_open) (
  const char *filename,
  int oflag,
  int pmode
);
int FS(_wopen) (
  const wchar_t *filename,
  int oflag,
  int pmode
);

int FS(_sopen) (
  const char *filename,
  int oflag,
  int shflag,
  int pmode
);
int FS(_wsopen) (
  const wchar_t *filename,
  int oflag,
  int shflag,
  int pmode
);

FILE *FS(_fsopen) (
  const char *filename,
  const char *mode,
  int shflag
);
FILE *FS(_wfsopen) (
  const wchar_t *filename,
  const wchar_t *mode,
  int shflag
);