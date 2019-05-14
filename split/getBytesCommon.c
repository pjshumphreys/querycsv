#include "cmnmap.h"

char* getBytesCommon(long codepoint, int key) {
  struct codepointToBytes *lookup;

  MAC_YIELD

  if((lookup = (struct codepointToBytes*)bsearch(
    (void *)&codepoint,
    (void *)commonBytes,
    SIZE_COMMONBYTES,
    sizeof(struct codepointToBytes),
    compareCodepoints
  )) == NULL) {
    returnByte = 0;
  }
  else switch(key) {
    case 0:
      returnByte = lookup->cp437;
    break;

    case 1:
      returnByte = lookup->cp850;
    break;

    case 2:
      returnByte = lookup->cp1047;

      return &returnByte; /* don't do the if statement below for cp1047 */
    break;

    case 3:
      returnByte = lookup->mac;
    break;
  }

  if(!returnByte) {
    returnByte = 0x3f;  /* ascii question mark */
    return &returnByte;
  }

  return &returnByte;
}
