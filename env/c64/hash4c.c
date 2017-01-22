#include "cc65iso.h"

#include "hash4c.h"

extern char hash4letter[7];
extern struct hash4Entry hash4export;

struct hash4Entry *
in_word_set_c (str, len)
     register const char *str;
     register unsigned int len;
{
  struct hash4Entry* tmp = in_word_set_ci(str, len);

  if(tmp) {
    hash4export.script = tmp->script;
    hash4export.index = tmp->index;
    hash4export.islower = tmp->islower;
    strncpy((char *)(&hash4letter), tmp->name, 7);
    return (struct hash4Entry *)(&hash4export);
  }
  
  return (struct hash4Entry *)(0);
}
