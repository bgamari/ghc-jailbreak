#include <stdio.h>
#include <io.h>
#include "fs.h"

int main ()
{
  volatile int f = sopen ("E:\\ghc-dev\\msys64\\tmp\\ccSGClgk.s", 769, 0, 0);
  volatile int g = FS(sopen) ("E:\\ghc-dev\\msys64\\tmp\\ccSGClgk.s", 769, 0, 0);

  printf ("Hello World!%d %d\n", f, g);
  return 0;
}