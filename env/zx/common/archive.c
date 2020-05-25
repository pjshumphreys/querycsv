#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define FALSE 0
#define TRUE 1

#define freeAndZero(p) { free(p); p = 0; }

int d_fgets(char** ws, FILE* stream) {
  char buf[80];
  char* newWs = NULL;
  char* potentialNewWs = NULL;
  size_t totalLength = (size_t)0;
  size_t potentialTotalLength = (size_t)0;
  size_t bufferLength;

  /* check sanity of inputs */
  if(ws == NULL) {
    return FALSE;
  }

  /* try reading some text from the file into the buffer */
  while(fgets((char *)&(buf[0]), 80, stream) != NULL) {

    /* get the length of the string in the buffer */
    bufferLength = strlen((char *)&(buf[0]));

    /*
      add it to the potential new length.
      this might not become the actual new length if the realloc fails
    */
    potentialTotalLength+=bufferLength;

    /*
      try reallocating the output string to be a bit longer
      if it fails then set any existing buffer to the return value and return true
    */
    if((potentialNewWs = (char*)realloc(newWs, ((potentialTotalLength+1)*sizeof(char)))) == NULL) {

      /* if we've already retrieved some text */
      if(newWs != NULL) {

        /* ensure null termination of the string */
        newWs[totalLength] = '\0';

        /* set the output string pointer and return true */
        if(*ws) {
          free(*ws);
        }
        *ws = newWs;

        return TRUE;
      }

      /*
        otherwise no successful allocation was made.
        return false without modifying the output string location
      */
      return FALSE;
    }

    /* copy the buffer data into potentialNewWs */
    memcpy(potentialNewWs+totalLength, &buf, bufferLength*sizeof(char));

    /* the potential new string becomes the actual one */
    totalLength = potentialTotalLength;
    newWs = potentialNewWs;

    /* if the last character is '\n' (ie we've reached the end of a line) then return the result */
    if(newWs[totalLength-1] == '\n') {

      /* ensure null termination of the string */
      newWs[totalLength-1] = '\0';

      /* set the output string pointer and return true */
      if(*ws) {
        free(*ws);
      }

      *ws = newWs;

      return TRUE;
    }
  }

  /* if we've already retrieved some text */
  if(newWs != NULL) {
    /* ensure null termination of the string */
    newWs[totalLength] = '\0';

    /* set the output string point and return true */
    if(*ws) {
      free(*ws);
    }
    *ws = newWs;

    return TRUE;
  }

  /*
    otherwise no successful allocation was made.
    return false without modifying the output string location
  */
  return FALSE;
}

int main(int argc, char *argv[]) {
  char * temp = NULL;

  FILE * input = NULL;
  FILE * output = NULL;
  uint32_t fileSize;
  int c;

  fputs("Output filename?\n", stdout);
  d_fgets(&temp, stdin);

  if((output = fopen(temp, "wb")) == NULL) {
    fprintf(stderr, "Couldn't open %s for writing\n", temp);
    return 1;
  }

  do {
    fputs("Append input filename? (press enter to exit)\n", stdout);

    d_fgets(&temp, stdin);

    if(strcmp(temp, "") == 0) {
      break;
    }

    if((input = fopen(temp, "rb")) == NULL) {
      fprintf(stdout, "File %s doesn't exist, continuing\n", temp);
      continue;
    }

    fileSize = 0;

    fseek(input, 0, SEEK_END);
    fileSize = ftell(input);
    fseek(input, 0, SEEK_SET);

    fprintf(stdout, "Appending file %s: %lu bytes\n", temp, fileSize);
    fwrite(temp, 1, strlen(temp)+1, output);
    fwrite(&fileSize, sizeof(uint32_t), 1, output);

    while(fileSize--) {
      fputc(fgetc(input), output);
    }

    fclose(input);
  } while (1);

  fclose(output);
  freeAndZero(temp);

  return 0;
}
