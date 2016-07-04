#include "querycsv.h"

int d_strftime(char** ptr, char* format, struct tm* timeptr)
//format a date into a string. allocate/free memory as needed
{
  size_t length = 32; //starting value
  size_t length2 = 0;
  char *output = NULL;
  char *output2 = NULL;

  if(ptr == NULL || format == NULL) {
    return FALSE;
  }

  while (length2 == 0) {
    output2 = realloc((void*)output, length*sizeof(char));

    if(output2 == NULL) {
      strFree(&output);

      return FALSE;
    }
    output = output2;

    //calling strftime using the buffer we created
    length2 = strftime(output, length, format, timeptr);

    //double the allocation length to use if we need to try again
    length *= 2;
  }

  //shrink the allocated memory to fit the returned length
  output2 = realloc((void*)output, (length2+1)*sizeof(char));

  //quit if the shrinking didn't work successfully
  if(output2 == NULL) {
    strFree(&output);

    return FALSE;
  }

  //free the contents of ptr then update it to point to the string we've built up
  strFree(ptr);
  *ptr = output2;

  //everything completed successfully
  return TRUE;
}