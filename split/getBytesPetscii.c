#include "petmap.h"

void getBytesPetscii(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  struct codepointToByte *lookup;

  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    /* just cast the codepoint to a byte for ascii control codes and symbols */
    if(codepoint < 0x41) {
      *bytes = NULL;
      return;
    }

    if((lookup = (struct codepointToByte*)bsearch(
      (void *)&codepoint,
      (void *)petsciiBytes,
      SIZE_PETSCIIBYTES,
      sizeof(struct codepointToByte),
      compareCodepoints
    )) == NULL) {
      returnByte = 0x3f;  /* ascii question mark */
      *bytes = &returnByte;
      return;
    }

    returnByte = lookup->byte;  /* whatever the hash table lookup returned */
    *bytes = &returnByte;
  }
}
