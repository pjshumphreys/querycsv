#include "querycsv.h"

struct columnReference *hash_lookupString(
    struct columnReferenceHash *hashtable,
    char *str
) {
  struct columnRefHashEntry *list;
  unsigned int hashval = hash_compare(hashtable, str);

  MAC_YIELD

  /* Go to the correct list based on the hash value and see if str is
   * in the list.  If it is, return return a pointer to the list element.
   * If it isn't, the item isn't in the table, so return NULL.
   */
  for(list = hashtable->table[hashval]; list != NULL; list = list->nextReferenceInHash) {
    if (strcmp(str, list->referenceName) == 0) {
      return list->content;
    }
  }

  return NULL;
}
