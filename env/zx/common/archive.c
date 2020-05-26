/* This program will write a named .tap file containing a bunch of named files cut up and converted into data blocks */
/* It enables me to quickly copy a bunch of files into an emulated residos filesystem */
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
  char * temp2 = NULL;

  FILE * input = NULL;
  FILE * output = NULL;
  uint32_t fileSize;
  unsigned char checksum;
  int c;
  uint16_t pageSize;
  int writeSize;
  uint16_t nameLength;
  unsigned char firstPage;

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
      if((input = fopen("/dev/null", "rb")) == NULL) {
        fprintf(stdout, "File %s doesn't exist, continuing\n", temp);
        continue;
      }

      firstPage = 2;  /* special code that tells the reader to abort */
      fileSize = 0;

      fprintf(stdout, "Appending termination block\n");

      nameLength = strlen(temp)+1;
    }
    else {
      if((input = fopen(temp, "rb")) == NULL) {
        fprintf(stdout, "File %s doesn't exist, continuing\n", temp);
        continue;
      }

      firstPage = TRUE;

      fseek(input, 0, SEEK_END);
      fileSize = ftell(input);
      fseek(input, 0, SEEK_SET);

      fprintf(stdout, "Appending file %s: %lu bytes\n", temp, fileSize);

      nameLength = strlen(temp)+1;
    }

    do {
      if((nameLength + fileSize) > 16397) {
        pageSize = 16397 - nameLength;
        fileSize -= pageSize;
      }
      else {
        pageSize = fileSize;
        fileSize = 0;
      }

      checksum = 0;

      /* write the block length (always 16400 for simplicity) */
      fputc(18, output);
      fputc(64, output);

      /* write 0xff (data block) */
      fputc(255, output);
      checksum ^= 255;

      /* write the filename */
      temp2 = temp;
      while(*temp2 != 0) {
        fputc(*temp2, output);
        checksum ^= *temp2;
        temp2++;
      }

      /* write the null terminator for the file name */
      fputc(0, output);
      checksum ^= *temp2;

      /* write 1 for replace or 0 for append */
      fputc(firstPage, output);
      checksum ^= firstPage;

      /* write how many bytes to add to the named file */
      c = pageSize & 0x00FF;
      fputc(c, output);
      checksum ^= c;
      c = (pageSize & 0xFF00) >> 8;
      fputc(c, output);
      checksum ^= c;

      /* always write the same number of bytes, but zero pad */
      writeSize = (16397 - nameLength) - pageSize;

      /* write the file contents */
      while(pageSize--) {
        c = fgetc(input);
        fputc(c, output);
        checksum ^= c;
      }

      /* write the zero padding */
      while(writeSize--) {
        fputc(0, output);
        checksum ^= 0;
      }

      /* write the checksum byte */
      fwrite(&checksum, 1, 1, output);
      firstPage = FALSE;
    } while (fileSize != 0);

    fclose(input);
  } while (strcmp(temp, "") != 0);

  fclose(output);
  freeAndZero(temp);

  return 0;
}

