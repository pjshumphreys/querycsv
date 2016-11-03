#include "querycsv.h"

/* getUnicodeCharFast does not check for invalid or overlong bytes. */
/* it also presumes the the entire string is already in nfd form */
long getUnicodeCharFast(
    unsigned char **offset,
    unsigned char **str,
    int plusBytes,
    int *bytesMatched,
    void (*get)()
) {
  unsigned char *temp = (unsigned char *)(*(offset+plusBytes));

  MAC_YIELD
  
  if(*temp < 0x80) {
    /* return the information we obtained */
    *bytesMatched = 1;
    
    return (long)(*temp);
  }
  else if(*temp < 0xE0) {
    /* read 2 bytes */
    *bytesMatched = 2;
    
    return (long)((*(temp) << 6) + *(temp+1)) - 0x3080;
  }
  else if (*temp < 0xF0) {
    /* read 3 bytes */
    *bytesMatched = 3;
    
    return ((long)(*temp) << 12) + ((long)(*(temp+1)) << 6) + (long)(*(temp+2)) - 0xE2080;
  }
    
  /* read 4 bytes */
  *bytesMatched = 4;
  
  return ((long)(*temp) << 18) + ((long)(*(temp+1)) << 12) + ((long)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;
}
