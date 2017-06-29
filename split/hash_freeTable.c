#include "querycsv.h"

void hash_freeTable(struct columnReferenceHash *hashtable) {
  int i;
  struct columnRefHashEntry *list, *temp;

  MAC_YIELD

  if (hashtable==NULL) {
    return;
  }

  /* Free the memory for every item in the table, including the
   * strings themselves.
   */
  for(i=0; i<hashtable->size; i++) {
      list = hashtable->table[i];
      while(list!=NULL) {
          temp = list;
          list = list->nextReferenceInHash;
          free(temp->referenceName);
          free(temp);
      }
  }

  /* Free the table itself */
  free(hashtable->table);
  free(hashtable);
}
