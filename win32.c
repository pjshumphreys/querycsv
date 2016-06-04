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

#define DEVNULL "NUL"
#define MAX_UTF8_PATH 780 // (_MAX_PATH)*3

int strAppendUTF8(long codepoint, unsigned char ** nfdString, int nfdLength);
int strAppend(char c, char** value, size_t* strSize);

struct dirent {
  unsigned  d_type;
  time_t    d_ctime; //-1 for FAT file systems
  time_t    d_atime; //-1 for FAT file systems
  time_t    d_mtime;
  int64_t   d_size; //64-bit size info
  char      d_name[MAX_UTF8_PATH]; 
  char      d_first; //flag for 1st time
  long      d_handle; //handle to pass to FindNext
};

typedef struct dirent DIR;

int hasUtf8 = FALSE;
int usingInput = FALSE;
int usingOutput = FALSE;
int usingError = FALSE;
HANDLE std_in;
HANDLE std_out;
HANDLE std_err;
char **argv_w32 = NULL;
char *test = NULL;

void cleanup_w32() {
  free(test);
  free(argv_w32);
}

void strAppendChars_w32(FILE* stream, char** value, size_t* strSize) {
  char c;
  char c2;
  wchar_t wc = 0;
  wchar_t wc2 = 0;
  long lwc;
  DWORD charsRead;
  size_t oldSize = *strSize;

  if(usingInput) {
    ReadConsoleW(std_in, &wc, 1, &charsRead, NULL);

    if(wc > 0xD7FF && wc < 0xDC00) {
      ReadConsoleW(std_in, &wc2, 1, &charsRead, NULL);

      if (wc2 > 0xDBFF && wc2 < 0xE000) {
        wc -= 0xD800;
        wc2 -= 0xDC00;
        lwc = (((long)wc) << 10) + wc2 + 0x010000;
        *strSize = strAppendUTF8(lwc, value, *strSize);
        fprintf_w32(stdout, "%d, ", lwc);
      }
      else {
        *strSize = strAppendUTF8(wc, value, *strSize);
        *strSize = strAppendUTF8(wc2, value, *strSize);
        fprintf_w32(stdout, "%d, ", wc);
        fprintf_w32(stdout, "%d, ", wc2);

      }
    }
    else if (wc == L'\xD') {
      *strSize = strAppendUTF8('\n', value, *strSize);
    }
    else {
      *strSize = strAppendUTF8(wc, value, *strSize);
      fprintf_w32(stdout, "%d, ", wc);
    }

   // strAppend('\0', value, strSize);
   // (*strSize)--;

   // fprintf_w32(stdout, "%", &((*value)[oldSize]));

    return;
  }

  c = fgetc(stream);

  if (c == '\r') {
    c2 = fgetc(stream);

    if(c2 != '\n') {
      strAppend(c, value, strSize);
    }

    strAppend(c2, value, strSize);
  }
  else {
    strAppend(c, value, strSize);
  }
}

char* getcwd_w32(char* buf, size_t size) {
  wchar_t * wide;
  char * retval;
  int len;

  if(buf == NULL && hasUtf8) {
    wide = _wgetcwd(NULL, 0);

    len = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);

    if((retval = (char*)malloc(sizeof(char)*len)) == NULL) {
      free(wide);
      return NULL;
    }
    
    WideCharToMultiByte(CP_UTF8, 0, wide, -1, retval, len, NULL, NULL);
  
    free(wide);
    return retval;
  }

  return getcwd(buf, size);
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
  FILE * pFile;
  
  if(
      (stream == stdout && usingOutput) ||
      (stream == stderr && usingError)
    ) {
    if(format == NULL || (pFile = fopen(DEVNULL, "wb")) == NULL) {
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

void setupWin32(int * argc, char *** argv) {
  DWORD mode;
  LPWSTR szArglist;
  int i, j;
  int sizeNeeded;
  int notInQuotes = TRUE;
  int maybeNewField = TRUE;
  int argc_w32 = 0;
   
  setmode(fileno(stdout),O_BINARY);
     
  if(IsValidCodePage(CP_UTF8)) {
    hasUtf8 = TRUE;
    std_in = GetStdHandle(STD_INPUT_HANDLE);
    std_out = GetStdHandle(STD_OUTPUT_HANDLE);
    std_err = GetStdHandle(STD_ERROR_HANDLE);
    usingOutput = (std_out != INVALID_HANDLE_VALUE && GetConsoleMode(std_out, &mode));
    usingError  = (std_err != INVALID_HANDLE_VALUE && GetConsoleMode(std_err, &mode));
    usingInput = (std_in != INVALID_HANDLE_VALUE && GetConsoleMode(std_in, &mode));

    if(usingInput) {
      //SetConsoleMode(std_in, 0);//mode & ~(ENABLE_ECHO_INPUT|ENABLE_LINE_INPUT));
    }

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

    //cut up the string. we can't use CommandLineToArgvW as it doesn't work in older versions of win32 or dos when using HXRT. behaviour should be as described on "the old new thing"
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

DIR *opendir(const char *name) {
  DIR * retval = NULL;
  wchar_t* wide;
  char * name2; 
  int len;
  struct _wfinddatai64_t temp2;

  if((retval = (DIR *)malloc(sizeof(DIR))) == NULL) {
    return NULL;
  }

  if(strcmp(name, "\\") == 0) {
    if((name = strdup("\\*.*")) == NULL) {
      return NULL;
    }
  }
  else {
    name2 = strdup(name);
    if(name2 == NULL || (name = realloc(name2, strlen(name)+5)) == NULL) {
      free(name2);
      return NULL;
    }

    strcat(name, "\\*.*"); 
  }

  if(hasUtf8) {
    len = MultiByteToWideChar(CP_UTF8, 0, name, -1, NULL, 0);
  
    if((wide = (wchar_t*)malloc(sizeof(wchar_t)*len)) == NULL) {
      free(retval);
      return NULL;
    }
    
    MultiByteToWideChar(CP_UTF8, 0, name, -1, wide, len);
    
    retval->d_handle = _wfindfirsti64(wide, &temp2);

    free(wide);
  }
  else {
    retval->d_handle = _findfirsti64(name, (struct _finddatai64_t *)&temp2);
  }
  
  if(retval->d_handle != -1) {
    if(hasUtf8) {
      WideCharToMultiByte(CP_UTF8, 0, temp2.name, -1, &(retval->d_name), MAX_UTF8_PATH, NULL, NULL);
    }
    else {
      snprintf((char*)(&(retval->d_name)), MAX_UTF8_PATH, "%s", temp2.name);
    }

    retval->d_type  = temp2.attrib;
    retval->d_ctime = temp2.time_create;
    retval->d_atime = temp2.time_access;
    retval->d_mtime = temp2.time_write;
    retval->d_size  = temp2.size;
    retval->d_first = TRUE; //first file
  }

  return retval;
}

struct dirent *readdir(DIR * inval) {
  struct _wfinddatai64_t temp2;

  if(inval->d_handle == -1) {
    return NULL;
  }

  if(inval->d_first == TRUE) {
    inval->d_first = FALSE;
    return inval;
  }
  
  if(hasUtf8) {
    if(_wfindnexti64(inval->d_handle, &temp2) == -1) {
      return NULL;
    }
    
    WideCharToMultiByte(CP_UTF8, 0, temp2.name, -1, inval->d_name, MAX_UTF8_PATH, NULL, NULL);
  }
  else {
    if(_findnexti64(inval->d_handle, &temp2) == -1) {
      return NULL;
    }
    
    snprintf(inval->d_name, MAX_UTF8_PATH, "%s", temp2.name);
  }

  inval->d_type  = temp2.attrib;
  inval->d_ctime = temp2.time_create;
  inval->d_atime = temp2.time_access;
  inval->d_mtime = temp2.time_write;
  inval->d_size  = temp2.size;

  return inval;
}

int closedir(DIR * inval) {
  _findclose(inval->d_handle);
  free(inval);
  return 0;
}
