#include "querycsv.h"

int isCombiningChar(long codepoint)
{
  #include "hash3.h"

  struct hash3Entry * result;

  if(
      codepoint < 0x300 ||
      (
        codepoint > 0x309A && (
          codepoint < 0xA66F || (
            codepoint < 0xFB1E &&
            codepoint > 0xABED
          )
        )
      )
  ) {
    return 0;
  }

  result = (struct hash3Entry*)bsearch(
      (const void*)&codepoint,
      (const void*)&hash3EntryMap,
      745,
      sizeof(struct hash3Entry),
      &combiningCharCompare
    );

  return (result == NULL ? 0 : result->order);
}
