#pragma output noprotectmsdos
#include <stdio.h>
#include <cpm.h>

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

void loadCLib(void) {
  FILE * tempFile;
  char * temp = 0x0100;

  if((tempFile = fopen(isMSX2() ? "qrycsv01.ovl" : "qrycsv02.ovl", "rb")) == NULL) {
    fputs("Couldn't open binary\n", stderr);
    return;
  }

  fread(temp, 1, 16128, tempFile);
  fclose(tempFile);
}

void main(void) {
  loadCLib();
}
