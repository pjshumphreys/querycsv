#include "querycsv.h"

extern long codepoints[18];
extern struct hash2Entry entry;
extern struct hash2Entry* retval;
extern int i;

TABLE

int longCompare_NUMBER(const void *a, const void *b) {
  return (*((long*)a) < *((long*)b)) ? -1 : (*((long*)a) != *((long*)b) ? 1 : 0);
}

void isInHash2_NUMBER(void) {
  if((retval = (struct hash2Entry*)bsearch(
      (const void*)&(entry.codepoint),
      (const void*)&hash2innerNUMBER,
      HASH2SIZE,
      sizeof(struct hash2Entry),
      &longCompare_NUMBER
    )) != NULL) {
    entry.length = retval->length;
    
    for(i = 0; i < entry.length; i++) {
      codepoints[i] = retval->codepoints[i];
    }

    retval = &entry;
  }
}
