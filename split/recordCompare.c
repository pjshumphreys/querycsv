/* compares two whole records to one another. multiple columns can be involved in this comparison. */
int recordCompare(
    const void *a,
    const void *b,
    void *c
) {
  struct resultColumnParam matchParams;
  struct sortingList *orderByClause;
  char *string1, *string2, *output1, *output2;
  int compare;

  MAC_YIELD

  matchParams.params = ((struct qryData*)c)->params;

  for(
      orderByClause = ((struct qryData*)c)->useGroupBy ?
          ((struct qryData*)c)->groupByClause :
          ((struct qryData*)c)->orderByClause;
      orderByClause != NULL;
      orderByClause = orderByClause->nextInList
    ) {

    /* get the value of the expression using the values in record a */
    matchParams.ptr = (struct resultColumnValue*)a;
    getValue(
        orderByClause->expressionPtr,
        &matchParams
      );
    string1 = output1 = orderByClause->expressionPtr->value;
    orderByClause->expressionPtr->value = NULL;

    /* get the value of the expression using the values in record b */
    matchParams.ptr = (struct resultColumnValue*)b;
    getValue(
        orderByClause->expressionPtr,
        &matchParams
      );
    string2 = output2 = orderByClause->expressionPtr->value;
    orderByClause->expressionPtr->value = NULL;

    /* do the comparison of the two current expression values */
    compare = strCompare(
        (unsigned char **)(&output1),
        (unsigned char **)(&output2),
         orderByClause->expressionPtr->caseSensitive,
        (void (*)(void))getUnicodeChar,
        (void (*)(void))getUnicodeChar
      );

    /* clean up used memory. The string1 & string2 pointers might be made */
    /* stale (and freed automatically) by unicode NFD normalisation in */
    /* strCompare function */
    freeAndZero(output1);
    freeAndZero(output2);

    /*  if the fields didn't compare as being the same, then return which was greater */
    if(compare != 0) {
      return orderByClause->isDescending ? compare : -compare;
    }
  }

  /* all fields to compare compared equally */
  return 0;
}
