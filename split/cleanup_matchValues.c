#include "querycsv.h"

void cleanup_matchValues(
    struct qryData *query,
    struct resultColumnValue **match
  ) {

  struct columnRefHashEntry *currentHashEntry;
  struct columnReference *currentReference;
  struct resultColumn *currentResultColumn;

  int i;

  //for each column in the output result set ...
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {

      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        //... check if the current column in the result set is a grouped one
        if(
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL
          ) {
          free(match[currentResultColumn->resultColumnIndex]->value);
        }

        currentReference = currentReference->nextReferenceWithName;
      }

      //go to the next reference in the hash table
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }

  free(*match);
  *match = NULL;
}
