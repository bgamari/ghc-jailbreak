CC=gcc
CFLAGS=-O0 -g -DUNICODE -D__USE_MINGW_ANSI_STDIO=1 -std=c99 -municode \
       -DWINVER=0x0601 -D_WIN32_WINNT=0x0601
LDFLAGS=-ldbghelp -limagehlp

.PHONY: all
all: iat-patcher runtime_x64

iat-patcher: fs.c iat-patcher.c
	$(CC) $(CFLAGS) fs.c iat-patcher.c -o iat-patcher.exe $(LDFLAGS)

runtime_x64: x86_64/msvcrt.def fs.c phxcrt.c
	$(CC) -shared phxcrt.c fs.c -o x86_64/muxcrt.dll
	$(CC) -shared x86_64/msvcrt.def -o x86_64/phxcrt.dll