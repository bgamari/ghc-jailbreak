CC=gcc
CFLAGS=-O0 -g -DUNICODE -D__USE_MINGW_ANSI_STDIO=1 -std=c99 -municode \
       -DWINVER=0x0601 -D_WIN32_WINNT=0x0601
LDFLAGS=-ldbghelp

iat-patcher: fs.c iat-patcher.c
	$(CC) $(CFLAGS) iat-patcher.c fs.c -o iat-patcher.exe $(LDFLAGS)