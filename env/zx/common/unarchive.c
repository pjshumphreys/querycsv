/* This program will be compiled to a .tap file and run on a residos enabled emulator.
  It will read .tap blocks and convert them into files on the +3dos filesystem.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <conio.h>
#include <spectrum.h>

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

uint16_t readBytes2;
char * temp;

int main(int argc, char *argv[]) {
  char * temp2;
  char * firstBlock;
  uint16_t * readBytes;

  FILE * output = NULL;

  temp = 0xBFEF;

  clrscr();

  fputs("Press any key to start\n", stdout);
  getchar();
  fputs("\n", stdout);

  do {
    tape_load_block(temp, 16400, 0xff);
    fputs("Pause!\n", stdout);

    firstBlock = temp + strlen(temp) + 1;
    readBytes = firstBlock + 1;
    readBytes2 = *readBytes;
    temp2 = firstBlock + 3;

    switch(*firstBlock) {
      case 0:
        fprintf(stdout, "Appending %u bytes to %s\n", readBytes2, temp);
      break;

      case 1:
        if(output) {
          fclose(output);
          output = NULL;
        }

        fprintf(stdout, "Writing %u bytes to %s\n", readBytes2, temp);

        if((output = fopen(temp, "wb")) == NULL) {
          fprintf(stderr, "Couldn't write to file %s\n", temp);
          return 1;
        }
      break;

      default:
        fputs("Termination block found. exiting\n \n \n", stdout);

        if(output) {
          fclose(output);
        }
      return 0;
    }

    fwrite(temp2, 1, readBytes2, output);

    fputs("Unpause!\n", stdout);
  } while(1);

  return 0;
}
