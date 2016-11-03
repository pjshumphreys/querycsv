#include "querycsv.h"

void cleanup_matchValues(
    struct qryData *query,
    struct resultColumnValue **match
  ) {

  int i, len;

  MAC_YIELD

  /* for each column in the output result set ... */
  for(i = 0, len = query->columnCount; i < len; i++) {
    free((*match)[i].value);
  }
  
  free(*match);
  *match = NULL;
}
