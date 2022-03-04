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
    case ENC_CP437:
      returnByte = lookup->cp437;
    break;

    case ENC_CP850:
      returnByte = lookup->cp850;
    break;

    case ENC_MAC:
      returnByte = lookup->mac;
    break;
  }

  if(!returnByte) {
    returnByte = 0x3f;  /* ascii question mark */
    return &returnByte;
  }

  return &returnByte;
}
