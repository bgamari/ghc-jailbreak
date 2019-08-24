#include <stdio.h>
#include <io.h>
#include <windows.h>
#include "fs.h"
#include <errno.h>
#include <fcntl.h>

int main ()
{
  //volatile int f = sopen ("E:\\ghc-dev\\msys64\\tmp\\ccSGClgk.s", 769, 0, 0);
  //volatile int g = FS(sopen) ("E:\\ghc-dev\\msys64\\tmp\\ccSGClgk.s", 769, 0, 0);

  volatile int f = sopen ("E:\\ghc-dev\\msys64\\tmp\\ccSGClgk.s", _O_BINARY | _O_CREAT | _O_RDWR | _O_TRUNC, 0, 0);
  volatile DWORD x1 = GetLastError ();
  errno_t err;
  _get_errno( &err );
  volatile int g = FS(sopen) ("E:\\ghc-dev\\msys64\\tmp\\ccSGClgk.s", _O_BINARY | _O_CREAT | _O_RDWR | _O_TRUNC, 0, 0);
  volatile DWORD x2 = GetLastError ();

  printf ("Hello Worldsa!%d (%d) %d\n", f, err, g);
  return 0;
}