struct expression *parse_functionRef(
    struct qryData *queryData,
    int aggregationType,
    struct expression *expressionPtr,
    int isDistinct
) {
  struct expression *expressionPtr2 = NULL;
  struct resultColumn *columnPtr = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  if(aggregationType != GRP_NONE) {
    queryData->hasGrouping = TRUE;

    if(isDistinct) {
      aggregationType+=GRP_STAR;
    }

    if(expressionPtr->containsAggregates) {
      /* I don't think in sql you can aggregate an aggregate. */
      /* therefore we should error out if we get to this point */
      fputs(TDB_AGGREG_AGGREG, stderr);
      exit(EXIT_FAILURE);
    }
  }

  /* parse_expCommaList is used to put an expression into a */
  /* new, hidden, calculated column in the output result set */
  columnPtr = parse_expCommaList(queryData, expressionPtr, NULL, aggregationType);   /* parse_exp_commalist returns an output column pointer */

  /* we then want to create a new expression node that references this new column */
  /* create an expression node that references the new hidden column */
  reallocMsg((void**)(&expressionPtr2), sizeof(struct expression));

  expressionPtr2->type = EXP_GROUP;
  expressionPtr2->value = NULL;

  expressionPtr2->unionPtrs.voidPtr = (void *)(columnPtr);  /* the expression nodes reference points directly to the hidden column */
  expressionPtr2->minColumn = expressionPtr->minColumn;
  expressionPtr2->minTable = expressionPtr->minTable;

  /* change the ref type back up the expression tree to be calculated later */
  expressionPtr2->containsAggregates = (aggregationType != GRP_NONE);

  return expressionPtr2;
}
