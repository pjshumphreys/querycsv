/* This program will be compiled to a .tap file and run on a residos enabled emulator.
  It will read .tap blocks and convert them into files on the +3dos filesystem.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <conio.h>
#include <spectrum.h>

#define freeAndZero(p) { free(p); p = 0; }

uint16_t readBytes2;
char * temp;

int main(int argc, char *argv[]) {
  char * temp2;
  char * temp3 = NULL;
  char * firstBlock;
  uint16_t * readBytes;
  int c;
  uint16_t bytesOut;

  FILE * output = NULL;

  fputs("\nChoose destination drive (A-P)\n"
        "or press any other key to use\n"
        "current drive\n", stdout);

  temp = (char *)(0xBFEF);

  c = getchar() & 0x5f;

  if(c < 'A' || c > 'P') {
    c = 'T';
  }

  fputs("\n", stdout);

  do {
    tape_load_block(temp, 16400, 0xff);
    fputs("Pause!\n", stdout);

    firstBlock = temp + strlen(temp) + 1;
    readBytes = (uint16_t *)(firstBlock + 1);
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

        if(c != 'T') {
          temp3 = malloc(strlen(temp)+3);
          sprintf(temp3, "%c:%s", c, 0xBFEF);
        }
        else {
          temp3 = temp;
        }

        fprintf(stdout, "Writing %u bytes to %s\n", readBytes2, temp3);

        if((output = fopen(temp3, "wb")) == NULL) {
          fprintf(stderr, "Couldn't write to file %s\n", temp3);
          return 1;
        }

        if(c != 'T') {
          freeAndZero(temp3);
        }
      break;

      default:
        fputs("Termination block found. exiting\n \n \n", stdout);

        if(output) {
          fclose(output);
        }
      return 0;
    }

    if(readBytes2 < 200) {
      for(bytesOut = 0; bytesOut < readBytes2; bytesOut++) {
        printf("%03d ", temp2[bytesOut]);
        fputc(temp2[bytesOut], output);
      }
    }
    else {
      fwrite(temp2, 1, readBytes2, output);
    }

    /* residos files may need a soft eof */
    if(readBytes2 % 128 != 0) {
      fputs("Adding soft-EOF\n", stdout);
      fputc(0x1a, output);
    }

    fputs("Unpause!\n", stdout);
  } while(1);

  return 0;
}
