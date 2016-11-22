#include "querycsv.h"

int yyerror(struct qryData *queryData, void *scanner, const char *msg) {
  MAC_YIELD

  fputs(msg, stderr);

  return 1;
}
