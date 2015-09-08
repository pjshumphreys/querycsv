#include <windows.h>
#include <wincon.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <shellapi.h>

int hasUtf8 = FALSE;
int usingOutput = FALSE;
int usingError = FALSE;
HANDLE std_out;
HANDLE std_err;
char **argv_w32 = NULL;
char *test = NULL;

void cleanup_w32() {
  free(test);
  free(argv_w32);
}

int fputs_w32(const char *str, FILE *stream) {
  int len;
  wchar_t *wide = NULL;
  HANDLE hnd;
  unsigned long i;

  if(
      (stream == stdout && usingOutput && ((hnd = std_out) || TRUE)) ||
      (stream == stderr && usingError && ((hnd = std_err) || TRUE))
    ) {
    len = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    
    if((wide = (wchar_t*)malloc(sizeof(wchar_t)*len)) == NULL) {
      return 0;
    }
    
    MultiByteToWideChar(CP_UTF8, 0, str, -1, wide, len);
    
    WriteConsoleW(hnd, wide, len-1, &i, NULL);

    free(wide);

    return len-1;
  }

  return fputs(str, stream);
}

int fprintf_w32(FILE *stream, const char *format, ...) {
  va_list args;
  int retval;
  size_t newSize;
  char* newStr = NULL;
  
  if(
      (stream == stdout && usingOutput) ||
      (stream == stderr && usingError)
    ) {
    if(format == NULL) {
      return FALSE;
    }

    //get the space needed for the new string
    va_start(args, format);
    newSize = (size_t)(vsnprintf(NULL, 0, format, args)+1); //plus '\0'
    va_end(args);

    //Create a new block of memory with the correct size rather than using realloc
    //as any old values could overlap with the format string. quit on failure
    if((newStr = (char*)malloc(newSize*sizeof(char))) == NULL) {
      return FALSE;
    }

    //do the string formatting for real
    va_start(args, format);
    vsnprintf(newStr, newSize, format, args);
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

void setupWin32(int * argc, char *** argv) {
  DWORD mode;
  LPWSTR szArglist;
  int i, j;
  int size_needed;
  int notInQuotes = TRUE;
  int maybeNewField = TRUE;
  int argc_w32 = 0;
    
  if(IsValidCodePage(CP_UTF8)) {
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

    size_needed = WideCharToMultiByte(CP_UTF8, 0, szArglist, -1, NULL, 0, NULL, NULL);

    if((test = (char*)malloc(sizeof(char)*size_needed)) == NULL) {
      fprintf_w32(stderr, "couldn't get command line\n");
      exit(EXIT_FAILURE);
    }
    
    WideCharToMultiByte(CP_UTF8, 0, szArglist, -1, test, size_needed, NULL, NULL);

    //cut up the string. we can't use CommandLineToArgvW as it doesn't work in older versions of win32 or dos when using HXRT
    for(i = 0, j = 0; i < size_needed; ) {
      switch(test[i]) {
        case '\\':
          if(maybeNewField) {
            argc_w32++;
            maybeNewField = FALSE;
          }

          if(test[i+1] == '\\') {
            test[j] = '\\';
            test[j+1] = '\\';
            i+=2;
            j+=2;
          }
          else if(test[i+1] == '"') {
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
          j++;
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
    
    for(i = 0, j = 0; i < size_needed; i++) {
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

