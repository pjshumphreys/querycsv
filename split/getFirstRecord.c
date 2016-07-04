#include "querycsv.h"

struct resultColumnValue *getFirstRecord(
    struct resultTree *root,
    struct qryData *query
  ) {

  struct resultTree *currentResult;

  if(root == NULL) {
    return NULL;
  }

  currentResult = root;

  while(currentResult->left) {
    currentResult = currentResult->left;
  }

  return currentResult->columns;
}
