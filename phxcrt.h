#include <wchar.h>
#include <stdio.h>

FILE *fopen(
   const char *filename,
   const char *mode
);
FILE *_wfopen(
   const wchar_t *filename,
   const wchar_t *mode
);

int _open(
   const char *filename,
   int oflag,
   int pmode
);
int _wopen(
   const wchar_t *filename,
   int oflag,
   int pmode
);

int _sopen(
   const char *filename,
   int oflag,
   int shflag,
   int pmode
);
int _wsopen(
   const wchar_t *filename,
   int oflag,
   int shflag,
   int pmode
);

FILE *_fsopen(
   const char *filename,
   const char *mode,
   int shflag
);
FILE *_wfsopen(
   const wchar_t *filename,
   const wchar_t *mode,
   int shflag
);