#pragma output noprotectmsdos
#include <stdio.h>
#include <cpm.h>
int isMSX2(void);
const int newStack = 0;  /* this will be set to the initial stack pointer on startup by the program that loads this one */

void main(void) {
  static int tempFile;
  char * filename = isMSX2() ? "qrycsv02.ovl" : "qrycsv01.ovl";

  if((tempFile = open(filename, O_RDONLY, 0)) == -1) {
    fprintf(stderr, "Couldn't find %s", filename);
    return;
  }

  read(tempFile, 256, 16128);
  close(tempFile);

  /* pop any extra stack values we needed to get to this point then jump to the real program */
  __asm
    ld sp, (_newStack)
    jp 256
  __endasm;
}

int isMSX2(void) {
  if(bdos(CPM_VERS, 0) == 0x22) {
    __asm
  ld a, 1
  ld c, 0x6f
  call 0x0005
  or a
  jr nz, NOTMSX2
  ld a, b
  cp 2
  jr c, NOTMSX2
  ld hl, 1
  ret
.NOTMSX2
    __endasm;
  }

  return 0;
}
