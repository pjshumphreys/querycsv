struct expression * parse_columnRefList(struct qryData *queryData, struct expression *exp, struct columnReference *referencePtr) {
  struct expression *temp = NULL;
  struct expressionEntry *temp2 = NULL;

  MAC_YIELD

  if(queryData->parseMode != 1) {
    return NULL;
  }

  if(exp->type != EXP_ROWNUMBER) {
    temp = parse_scalarExp(queryData, NULL, EXP_ROWNUMBER, NULL);

    reallocMsg((void **)&temp2, sizeof(struct expressionEntry));
    temp2->value = exp;
    temp2->nextInList = NULL;

    temp->unionPtrs.voidPtr = (void *)temp2;
  }
  else {
    temp = exp;
  }

  if(referencePtr != NULL) {
    temp2 = (struct expressionEntry *)(temp->unionPtrs.voidPtr);

    while(temp2->nextInList != NULL) {
      temp2 = temp2->nextInList;
    }

    reallocMsg((void **)&(temp2->nextInList), sizeof(struct expressionEntry));
    temp2 = temp2->nextInList;

    /* tag the column expression onto the end of the list */
    temp2->value = parse_scalarExpColumnRef(queryData, referencePtr);
    temp2->nextInList = NULL;
  }

  return temp;
}
