#include "querycsv.h"

#include "hash1.h"

struct hash2Entry {
  long codepoint;
  int length;
  const long * codepoints;
};

#include "hash3.h"

struct hash4Entry {
  const char *name;
  int script;
  int index;
  int islower;
};

struct hash4Entry * in_word_set (register const char *str, register unsigned int len);
static unsigned int hash (register const char *str, register unsigned int len);
