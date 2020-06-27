#pragma output protect8080
#pragma output nofileio
#include <compress/zx7.h>
extern unsigned char chooseCLib[];
extern int newStack;

void main(void) {
  /* decompress c lib chooser code from the embedded array into address 0x4000 */
  dzx7_turbo(((unsigned int *)chooseCLib), ((unsigned char *)16384));

  /* jp to the code */
  __asm
    ld hl, (cleanup+6)
    ld (_newStack), hl
    jp 16384
  __endasm;
}
