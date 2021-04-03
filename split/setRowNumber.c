void setRowNumber(
  struct qryData * query,
  int columnIndex,
  double defaultValue,
  struct expressionEntry * entry,
  struct resultTree * item
) {
  char * temp1 = NULL;
  char * temp2 = NULL;
  struct resultColumnValue * currentMatch = item->columns;
  struct resultColumnValue * nextMatch = NULL;

  struct resultColumnParam matchParams;

  MAC_YIELD

  /* set the right row even when true groupings are being done in addition to row_number() */
  item = item->link[1];

  while(item && item->type == TRE_SKIP) {
    if(item->link[1] == NULL) {
      return;
    }

    item = item->link[1];
  }

  nextMatch = item->columns;

  matchParams.params = query->params;

  freeAndZero(nextMatch[columnIndex].value);

  while(entry) {
    matchParams.ptr = currentMatch;
    getValue(entry->value, &matchParams);

    temp1 = entry->value->value;
    entry->value->value = NULL;

    matchParams.ptr = nextMatch;
    getValue(entry->value, &matchParams);

    temp2 = entry->value->value;
    entry->value->value = NULL;

    if(strCompare(
        (unsigned char **)&temp1,
        (unsigned char **)&temp2,
        entry->value->caseSensitive,
        (void (*)(void))&getUnicodeChar,
        (void (*)(void))&getUnicodeChar
      )) {
      ftostr(&(nextMatch[columnIndex].value), ctof(1));

      free(temp1);
      free(temp2);

      return;
    }

    free(temp1);
    free(temp2);

    entry = entry->nextInList;
  }

  ftostr(&(nextMatch[columnIndex].value), fadd(defaultValue, ctof(1)));
}
