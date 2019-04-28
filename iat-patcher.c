#include <stdio.h>
#include "fs.h"
#include <stdbool.h>
#include <stdint.h>
#include <windows.h>
#include <dbghelp.h>
#include <stdlib.h>
#include <psapi.h>
#include <wchar.h>
#include <imagehlp.h>

#define USE_BACKUPS 1

static void
show_usage (void)
{
  printf ("(c) Tamar Christina 2018-2019.\n");
  printf (" \n");
  printf ("IAT patcher will patch the C runtime used by the GCC and binutils\n");
  printf ("binaries.  In essense it will redirect calls to posix functions\n");
  printf ("such as fopen to native Windows API calls.  It uses lazy binding\n");
  printf ("semantics to minimize the overhead of the redirection but it allows\n");
  printf ("these programs to work in a modern way.\n");
  printf ("\n");
  printf ("Usage:\n\n");
  printf ("\tiat-patcher.exe <install|uninstall> <exe>\n\n");
}

static bool
rewrite_import_address_table (void*, bool*);

extern void abort (void);

int wmain (int argc, wchar_t *argv[], wchar_t *envp[])
{
  if (argc != 3)
    {
      show_usage ();
      return 1;
    }

  bool install = false;

  if (wcsncmp (L"install", argv[1], 7) == 0 && wcslen (argv[1]) == 7)
    install = true;
  else if (wcsncmp (L"uninstall", argv[1], 9) == 0 && wcslen (argv[1]) == 9)
    install = false;
  else
    {
      show_usage ();
      return 1;
    }

  int retcode = 0;

  wchar_t* filepath = FS(create_device_name) (argv[2]);
  if (wcslen (filepath) == 0)
    goto fail;

#if defined(ONLY_CHOCOLATEY)
  if (wcsstr (filepath, L"\\chocolatey\\") == NULL)
    {
      wprintf (L"This tool can only be used to patch GHC installed via Chocolatey.\n");
      wprintf (L"Since I have detected a different install I will now abort.\n");
      abort ();
    }
#endif

  int flen = wcslen (filepath)+5;
  wchar_t* bak_filepath = malloc (flen * sizeof (wchar_t));
  if (swprintf (bak_filepath, flen, L"%ls.bak", filepath) < 0)
    goto end;
  wprintf (L"%ls => %ls\n", argv[2], filepath);

  if (!install)
    goto restore;

#if USE_BACKUPS
  wprintf (L"Creating backup...\n");
  if (!CopyFileW (filepath, bak_filepath, false))
    goto fail;
#endif

  HANDLE fHwnd
    = CreateFile (filepath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                  OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL |
                  FILE_FLAG_RANDOM_ACCESS | FILE_FLAG_WRITE_THROUGH, NULL);
  DWORD filesize = GetFileSize (fHwnd, NULL);

  if (fHwnd == INVALID_HANDLE_VALUE)
    {
      wprintf (L"Unable to open binary %ls.  aborting.\n", filepath);
      goto fail;
    }

  HANDLE mHwnd
    = CreateFileMapping (fHwnd, NULL, PAGE_READWRITE, 0, 0, NULL);

  if (mHwnd == NULL)
    {
      wprintf (L"Cannot create mapping object for %ls.  Aborting.\n", filepath);
      CloseHandle (fHwnd);
      goto fail;
    }

  void* ptr = MapViewOfFile (mHwnd, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0);
  if (ptr == NULL)
    {
      wprintf (L"Cannot create view mapping for %ls.  Aborting.\n", filepath);
      CloseHandle (fHwnd);
      CloseHandle (mHwnd);
      goto fail;
    }

  bool nothing_to_do = true;
  if (!rewrite_import_address_table (ptr, &nothing_to_do))
    {
      if (nothing_to_do)
        printf ("Terminating patching...\n");
      else
        printf ("Oops, something went wrong. Aborting...\n");
      UnmapViewOfFile (ptr);
      CloseHandle (fHwnd);
      CloseHandle (mHwnd);
      if (nothing_to_do)
        goto end;
      else
        goto fail;
    }

  printf ("Calculating new checksum...\n");
  DWORD old, new;
  PIMAGE_NT_HEADERS hdr = CheckSumMappedFile (ptr, filesize, &old, &new);
  if (hdr == NULL)
    {
      printf ("Was unable to compute new checksum. aborting.\n");
      goto fail;
    }

  printf ("Old checksum 0x%lx, New checksum 0x%lx\n", old, new);
  hdr->OptionalHeader.CheckSum = new;

  printf ("Import description table rewritten. Good to go!\n");
  UnmapViewOfFile (ptr);
  CloseHandle (fHwnd);
  CloseHandle (mHwnd);
  goto end;

fail:
  retcode = 1;
  DWORD code = GetLastError ();
  if (code != ERROR_SUCCESS)
    {
      wchar_t buf[256];
      FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                     NULL, code, MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                     buf, sizeof(buf), NULL);
      wprintf(L"Error: %ls\n", buf);
    }
restore:
#if USE_BACKUPS
  printf ("Restoring backup..\n");
  if (!MoveFileEx (bak_filepath, filepath, MOVEFILE_REPLACE_EXISTING))
    wprintf (L"Could not restore backup.  Keeping file %ls.\n", bak_filepath);
  else
    DeleteFileW (bak_filepath);
#endif
end:
  free (filepath);
  free (bak_filepath);
  return retcode;
}

static bool
rewrite_import_address_table (void* ptr, bool *nothing_to_do)
{
  *nothing_to_do = false;
  PIMAGE_NT_HEADERS ntHeader = ImageNtHeader (ptr);
  IMAGE_FILE_HEADER imgHeader = ntHeader->FileHeader;
  if (imgHeader.Machine != IMAGE_FILE_MACHINE_I386
      && imgHeader.Machine != IMAGE_FILE_MACHINE_AMD64)
    return false;

  IMAGE_OPTIONAL_HEADER optHeader = ntHeader->OptionalHeader;
  if (optHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC
      && optHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    return false;

  IMAGE_DATA_DIRECTORY importDir
    = optHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

  if (importDir.Size == 0)
    {
      printf ("No import headers.  Done.");
      return true;
    }

  PIMAGE_IMPORT_DESCRIPTOR importDesc
    = (PIMAGE_IMPORT_DESCRIPTOR) ImageRvaToVa (ntHeader, ptr,
                                               importDir.VirtualAddress, NULL);

  while (true)
    {
     char* name
        = (char*)ImageRvaToVa(ntHeader, ptr, importDesc->Name, NULL);
      if (name == NULL)
        {
          printf ("Did not find C runtime, nothing to do.\n");
          *nothing_to_do = true;
          return false;
        }

      if (strncmp ("msvcrt.dll", name, 10) == 0)
        {
          char runtime[] = "phxcrt.dll";
          printf ("Found C runtime entry (%s). Rewriting..\n", name);
          memcpy (name, runtime, 10);
          printf ("Installed new C runtime entry (%s)..\n", runtime);
          return true;
        }
      importDesc++;
    }

  return true;
}
