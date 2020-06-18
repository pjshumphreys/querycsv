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
  int tempFile;
  char * temp = 0x0100;
  char * filename = isMSX2() ? "qrycsv02.ovl" : "qrycsv01.ovl";

  if((tempFile = open(filename, O_RDONLY, 0)) == -1) {
    fprintf(stderr, "Couldn't open %s\n", filename);
    return;
  }

  read(tempFile, temp, 16128);
  close(tempFile);
}

void main(void) {
  loadCLib();
}
