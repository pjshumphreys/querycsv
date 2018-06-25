void initMatch(
    struct qryData *query,
    struct resultColumnValue** match
) {
  int i = 0;

  MAC_YIELD

  reallocMsg(
    (void**)match,
    (query->columnCount)*sizeof(struct resultColumnValue)
  );

  /* initialise the match values to null to avoid
  extra checks during the query run */
  for(i = query->columnCount-1; i >= 0; i--) {
    (*match)[i].value = NULL;
  }
}
