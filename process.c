#include <stdio.h>
#include <stdlib.h>

#include "win32.h"

#define FALSE 0
#define TRUE 1

void reallocMsg(char *failureMessage, void** mem, size_t size) {
  void * temp = NULL;
  if((temp = realloc(*mem, size)) == NULL) {
    fputs(failureMessage, stderr);
    exit(EXIT_FAILURE);
  }

  *mem = temp;
}

int strAppendUTF8(long codepoint, unsigned char ** nfdString, int nfdLength) {
  if (codepoint < 0x80) {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+1);

    (*nfdString)[nfdLength++] = codepoint;
  }
  else if (codepoint < 0x800) {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+2);

    (*nfdString)[nfdLength++] = (codepoint >> 6) + 0xC0;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else if (codepoint < 0x10000) {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+3);

    (*nfdString)[nfdLength++] = (codepoint >> 12) + 0xE0;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+4);

    (*nfdString)[nfdLength++] = (codepoint >> 18) + 0xF0;
    (*nfdString)[nfdLength++] = ((codepoint >> 12) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }

  return nfdLength;
}

int strAppend(char c, char** value, size_t* strSize) {
  char* temp;

  //validate inputs
  //increase value length by 1 character

  //update the string pointer
  //increment strSize
  if(strSize != NULL) {
    if(value != NULL) {
      if((temp = realloc(*value, (*strSize)+1)) != NULL) {
      *value = temp;

      //store the additional character
      (*value)[*strSize] = c;
      }
      else {
      return FALSE;
      }
    }

    (*strSize)++;
  }

  return TRUE;
}

/*
void strAppendChars(FILE* stream, char** value, size_t* strSize) {
  int c;

  c = fgetc(stream);
  
  strAppend(c, value, strSize);
}*/

//returns a utf8 encoded string, not including the closing newline character
char* readLine(FILE * stream) {
  char* retval = NULL;
  size_t strSize = 0;

  for( ; ; ) {
    strAppendChars(stdin, &retval, &strSize);
    
    if(retval[strSize-1] == '\n') {
      retval[strSize-1] == '\0';
      return retval;
    }
  }
}

int main(int argc, char * argv[]) {
  int argc2;
  char ** argv2;

  setupWin32(&argc2, &argv2);
  
  fprintf(stdout, "%s", readLine(stdin));
  return 0;
}
