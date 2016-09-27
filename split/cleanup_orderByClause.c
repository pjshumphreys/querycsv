#include "querycsv.h"

void cleanup_orderByClause(struct sortingList *currentSortingList) {
  struct sortingList *next;

  MAC_YIELD

  while(currentSortingList != NULL) {
    next = currentSortingList->nextInList;

    cleanup_expression(currentSortingList->expressionPtr);

    free(currentSortingList);

    currentSortingList = next;
  }
}
