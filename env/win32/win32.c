#include <windows.h>
#include <wincon.h>
#include <fcntl.h>
#include <io.h>
//#include <direct.h>
#include <stdio.h>
#include <stdlib.h>
#include <shellapi.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <wchar.h>

extern char * devNull;

char *d_charsetEncode(char* s, int encoding, size_t *bytesStored);
#define ENC_UTF16LE 8
#define ENC_CP437 1

int hasUtf8 = FALSE;
int usingOutput = FALSE;
int usingError = FALSE;
HANDLE std_out;
HANDLE std_err;
char **argv_w32 = NULL;
char *test = NULL;

void cleanup_w32(void) {
  free(test);
  free(argv_w32);
}

int fputs_w32(const char *str, FILE *stream) {
  size_t len = 0;
  wchar_t *wide = NULL;
  HANDLE hnd;
  unsigned long i;
  int retval;
  char* output = NULL;

  if(
      ((stream == stdout && usingOutput && ((hnd = std_out) || TRUE)) ||
      (stream == stderr && usingError && ((hnd = std_err) || TRUE))) &&
      (len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0))
    ) {
    if(
        (wide = (wchar_t *)malloc(len * sizeof(wchar_t))) != NULL &&
        MultiByteToWideChar(CP_UTF8, 0, str, -1, wide, len)
      ) {
      WriteConsoleW(hnd, wide, (DWORD)len, &i, NULL);

      free(wide);

      return (int)len;
    }

    free(wide);
  }

  output = d_charsetEncode((char *)str, ENC_CP437, NULL);

  retval = fputs(output, stream);

  free(output);

  return retval;
}

int fprintf_w32(FILE *stream, const char *format, ...) {
  va_list args;
  int retval;
  size_t newSize;
  char* newStr = NULL;
  FILE * pFile;

  if(
      (stream == stdout && usingOutput) ||
      (stream == stderr && usingError)
    ) {
    if(format == NULL || (pFile = fopen(devNull, "wb")) == NULL) {
      return FALSE;
    }

    //get the space needed for the new string
    va_start(args, format);
    newSize = (size_t)(vfprintf(pFile, format, args)+1); //plus L'\0'
    va_end(args);

    //close the file. We don't need to look at the return code as we were writing to /dev/null
    fclose(pFile);

    //Create a new block of memory with the correct size rather than using realloc
    //as any old values could overlap with the format string. quit on failure
    if((newStr = (char*)malloc(newSize*sizeof(char))) == NULL) {
      return FALSE;
    }

    //do the string formatting for real
    va_start(args, format);
    vsprintf(newStr, format, args);
    va_end(args);

    //ensure null termination of the string
    newStr[newSize] = '\0';

    fputs_w32(newStr, stream);

    free(newStr);

    return newSize-1;
  }

  va_start(args, format);
  retval = vfprintf(stream, format, args);
  va_end(args);

  return retval;
}

void setupWin32(int *argc, char ***argv) {
  OSVERSIONINFO osvi;
  DWORD mode;
  LPWSTR szArglist;
  int i, j;
  int sizeNeeded;
  int notInQuotes = TRUE;
  int maybeNewField = TRUE;
  int argc_w32 = 0;

  ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

  GetVersionEx(&osvi);

  /*
    test if the program is not being run using the HXRT DPMI server
    and supports the UTF-8 codepage.
    Otherwise, don't use WriteConsoleW
  */
  if(
      ((osvi.dwBuildNumber & 0xFFFF) != 2222 || osvi.szCSDVersion[0] != 0) &&
      IsValidCodePage(CP_UTF8)
    ) {
    hasUtf8 = TRUE;

    std_out = GetStdHandle(STD_OUTPUT_HANDLE);
    std_err = GetStdHandle(STD_ERROR_HANDLE);

    usingOutput = (std_out != INVALID_HANDLE_VALUE && GetConsoleMode(std_out, &mode));
    usingError  = (std_err != INVALID_HANDLE_VALUE && GetConsoleMode(std_err, &mode));

    szArglist = GetCommandLineW();

    if(szArglist == NULL) {
      fprintf_w32(stderr, "couldn't get command line\n");
      exit(EXIT_FAILURE);
    }

    sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, szArglist, -1, NULL, 0, NULL, NULL);

    if((test = (char*)malloc(sizeof(char)*sizeNeeded)) == NULL) {
      fprintf_w32(stderr, "couldn't get command line\n");
      exit(EXIT_FAILURE);
    }

    WideCharToMultiByte(CP_UTF8, 0, szArglist, -1, test, sizeNeeded, NULL, NULL);

    //cut up the string. we can't use CommandLineToArgvW as it doesn't work in older versions of win32. behaviour should be as described on "the old new thing"
    for(i = 0, j = 0; i < sizeNeeded; ) {
      switch(test[i]) {
        case '\\':
          if(maybeNewField) {
            argc_w32++;
            maybeNewField = FALSE;
          }

          if(i+2 < sizeNeeded && test[i+1] == '\\' && test[i+2] == '"') {
            test[j] = '\\';
            i+=2;
            j++;
          }
          else if(i+1 < sizeNeeded && test[i+1] == '"') {
            test[j] = '"';
            i+=2;
            j++;
          }
          else {
            test[j] = '\\';
            i++;
            j++;
          }
        break;

        case ' ':
          if(notInQuotes) {
             test[j] = '\0';
             maybeNewField = TRUE;
          }
          else {
            if(maybeNewField) {
              argc_w32++;
              maybeNewField = FALSE;
            }
            test[j] = ' ';
          }
          i++;
          j++;
        break;

        case '"':
          notInQuotes = !notInQuotes;
          i++;
        break;

        case '\0':
          i++;
        break;

        default:
          if(maybeNewField) {
            argc_w32++;
            maybeNewField = FALSE;
          }
          test[j] = test[i];
          i++;
          j++;
        break;
      }
    }

    if((argv_w32 = (char**)malloc(sizeof(char*)*argc_w32)) == NULL) {
      free(test);
      fprintf_w32(stderr, "couldn't get command line\n");
      exit(EXIT_FAILURE);
    }

    atexit(cleanup_w32);
    maybeNewField = TRUE;

    for(i = 0, j = 0; i < sizeNeeded; i++) {
      if(test[i] == '\0') {
        maybeNewField = TRUE;
      }
      else if(maybeNewField) {
        argv_w32[j] = &(test[i]);
        maybeNewField = FALSE;
        j++;
      }
    }

    *argc = argc_w32;
    *argv = argv_w32;
  }
}

FILE *fopen_w32(const char *filename, const char *mode) {
  int len;
  FILE * retval;
  wchar_t *wide = NULL;
  wchar_t *wide2 = NULL;

  if(hasUtf8) {
    len = MultiByteToWideChar(CP_UTF8, 0, filename, -1, NULL, 0);

    if((wide = (wchar_t*)malloc(sizeof(wchar_t)*len)) == NULL) {
      return NULL;
    }

    MultiByteToWideChar(CP_UTF8, 0, filename, -1, wide, len);

    len = MultiByteToWideChar(CP_UTF8, 0, mode, -1, NULL, 0);

    if((wide2 = (wchar_t*)malloc(sizeof(wchar_t)*len)) == NULL) {
      free(wide);
      return NULL;
    }

    MultiByteToWideChar(CP_UTF8, 0, mode, -1, wide2, len);

    retval = _wfopen(wide, wide2);

    free(wide);
    free(wide2);

    return retval;
  }

  return fopen(filename, mode);
}
