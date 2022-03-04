#include "zxmap.h"

void getBytesZXCommon(
    long codepoint,
    char **bytes,
    int *byteLength,
    int isTSW
) {
  struct codepointToByte *lookup;

  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    /* just cast the codepoint to a byte for basic ascii */
    if(codepoint < 0x80) {
      switch(codepoint) {
        case 0x5E:    /* ^ */
        case 0x60:    /* ` */
        case 0x7F: {  /* DEL */
          returnByte = 0x3f;  /* ascii question mark */
          *bytes = &returnByte;
        } return;

        default: {
          if(codepoint > 0x1f || !isTSW) {
            *bytes = NULL;
            return;
          }
          else if(codepoint == 0x0a) {
            returnByte = 0x80;  /* tasword 2 newline */
          }
          else {
            returnByte = 0x20;  /* ascii space */
          }

          *bytes = &returnByte;
        } return;
      }
    }

    if((lookup = (struct codepointToByte*)bsearch(
      (void *)&codepoint,
      (void *)zxBytes,
      SIZE_ZXBYTES,
      sizeof(struct codepointToByte),
      compareCodepoints
    )) == NULL) {
      returnByte = 0x3f;  /* ascii question mark */
      *bytes = &returnByte;
      return;
    }

    returnByte = lookup->byte;  /* whatever the hash table lookup returned */

    if(isTSW) {
      switch((unsigned char)returnByte) {
        case 0x80:
        case 0x8f: {
          *byteLength = 2;

          (*bytes)[0] = 0x8f;
          (*bytes)[1] = returnByte;
        } return;
      }
    }

    *bytes = &returnByte;
  }
}
