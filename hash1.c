#include "querycsv.h"

#include "hash1.h"

extern struct hash1Entry lookupresult;
extern long hash1Codepoints[2];
extern int i;

void getHash1(char byte) {
  lookupresult.length = hash1[byte].length;

  for(i = 0; i < lookupresult.length; i++) {
    hash1Codepoints[i] = hash1[byte].codepoints[i];
  }
}
