void setRowNumber(
  struct qryData * query,
  int columnIndex,
  double defaultValue,
  struct expressionEntry * entry,
  struct resultColumnValue * currentMatch,
  struct resultColumnValue * nextMatch
) {
  char * temp1 = NULL;
  char * temp2 = NULL;

  struct resultColumnParam matchParams;

  MAC_YIELD

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
      ftostr(&(nextMatch[columnIndex].value), 1);

      free(temp1);
      free(temp2);

      return;
    }

    free(temp1);
    free(temp2);

    entry = entry->nextInList;
  }

  ftostr(&(nextMatch[columnIndex].value), fadd(defaultValue, 1));
}
