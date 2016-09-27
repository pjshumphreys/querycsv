#include "querycsv.h"

int yyerror(struct qryData *queryData, void *scanner, const char *msg) {
  MAC_YIELD

  fprintf(stderr, msg);

  return 1;
}
