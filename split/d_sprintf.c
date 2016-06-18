#include "querycsv.h"

int d_sprintf(char** str, char* format, ...)
//write a formatted string into a string buffer. allocate/free memory as needed
{
  FILE * pFile;
  size_t newSize;
  char* newStr = NULL;
  va_list args;

  //Check sanity of inputs and open /dev/null so that we can
  //get the space needed for the new string. There's unfortunately no
  //easier way to do this that uses only ISO functions. Filenames can only
  //be portably specified in terms of the user's codepage as well :(
  if(str == NULL || format == NULL || (pFile = fopen(DEVNULL, "wb")) == NULL) {
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

  //do the string formatting for real. vsnprintf doesn't seem to be available on Lattice C
  va_start(args, format);
  vsprintf(newStr, format, args);
  va_end(args);

  //ensure null termination of the string
  newStr[newSize] = '\0';

  //free the old contents of the output if present
  free(*str);

  //set the output pointer to the new pointer location
  *str = newStr;

  //everything occurred successfully
  return newSize;
}
