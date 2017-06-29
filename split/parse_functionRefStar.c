#include "querycsv.h"

struct expression *parse_functionRefStar(
    struct qryData *queryData,
    long aggregationType
  ) {

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  if(aggregationType != GRP_COUNT) {
    /* I don't think in sql you can aggregate an aggregate. */
    /* therefore we should error out if we get to this point */
    fputs("only count(*) is valid",stderr);
    exit(EXIT_FAILURE);
  }

  return parse_functionRef(
    queryData,
    GRP_STAR,
    parse_scalarExpLiteral(queryData, ""),
    FALSE
  );
}
