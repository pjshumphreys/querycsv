#include "querycsv.h"

static struct hash4Entry numberEntry = { NULL, 127, 0, 0 };

struct hash4Entry *getLookupTableEntry(
    unsigned char **offset,
    unsigned char **str,
    int *lastMatchedBytes,
    void (*get)(),
    int firstChar
  ) {
  struct hash4Entry *temp = NULL, *temp2 = NULL;
  int totalBytes = 0;

  MAC_YIELD

  if(isNumberWithGetByteLength(*offset, lastMatchedBytes, firstChar)) {
    return &numberEntry;
  }
  
  while((temp = in_word_set(*offset, totalBytes+(*lastMatchedBytes)))) {
    //the match is so far holding up. 

    //keep this match for later as it may be the last one we find
    temp2 = temp;

    //add the byte length to the total
    totalBytes += *lastMatchedBytes;
    
    //get a code point
    (*((int (*)(unsigned char **, unsigned char **, int,  int *, void (*)()))get))
    (offset, str, totalBytes, lastMatchedBytes, get);
  } 

  //don't update the value passed to us if we didn't find any match at all
  if(temp2 != NULL) {
    //copy the match data into the output
    *lastMatchedBytes = totalBytes;
  }
  
  return temp2;
}
