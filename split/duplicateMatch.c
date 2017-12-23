void duplicateMatch(
    struct qryData *query,
    struct resultColumnValue** match
) {
  int i = 0;
  struct resultColumnValue* newMatch = NULL;

  reallocMsg(
    (void**)&newMatch,
    (query->columnCount)*sizeof(struct resultColumnValue)
  );

  memmove(newMatch, *match, (query->columnCount)*sizeof(struct resultColumnValue));

  for(i = query->columnCount-1; i >= 0; i--) {
    newMatch[i].value = mystrdup((*match)[i].value);
  }

  *match = newMatch;
}
