#include "querycsv.c"

#undef fputs
#undef fprintf
#undef main

int fputs_c64(const char* str, FILE* stream) {
  if(stream == stdout || stream == stderr) {
    return fputsEncoded((char *)str, stream, ENC_PRINT);
  }

  return fputs(str, stream);
}

int fprintf_c64(FILE *stream, const char *format, ...) {
  va_list args;
  int newSize;
  int retval;
  char* newStr = NULL;

  if(stream == stdout || stream == stderr) {
    //get the space needed for the new string
    va_start(args, format);
    newSize = vsnprintf(NULL, 0, format, args); /* plus '\0' */
    va_end(args);

    //Create a new block of memory with the correct size rather than using realloc
    //as any old values could overlap with the format string. quit on failure
    if((newStr = (char*)malloc(newSize+1)) == NULL) {
      return 0;
    }

    //do the string formatting for real
    va_start(args, format);
    vsprintf(newStr, format, args);
    va_end(args);

    //ensure null termination of the string
    newStr[newSize] = '\0';

    fputsEncoded(newStr, stream, ENC_PRINT);

    free(newStr);

    return newSize-1;
  }

  va_start(args, format);
  retval = vfprintf(stream, format, args);
  va_end(args);

  return retval;
}

int main(int argc, char ** argv) {
  int temp;
  int retval = realmain(argc, argv);

  fputsEncoded(TDB_PRESS_A_KEY, stdout, ENC_PRINT);

  temp = cgetc();

  return retval;
}
