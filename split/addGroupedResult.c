void addGroupedResult(
    struct qryData *query,
    struct resultColumnValue *match
) {
  struct resultColumnParam matchParams;

  MAC_YIELD

  matchParams.ptr = match;
  matchParams.params = query->params;

  /* fix up the calculated columns that need it */
  getGroupedColumns(query);

  /* calculate remaining columns that make use of aggregation */
  getCalculatedColumns(query, match, TRUE);

  /* Apply the aggregate "having" clause filters  */
  if(!walkRejectRecord(
    query->joinsAndWhereClause->minTable+1, /* +1 means all tables and *CALCULATED* columns */
    query->joinsAndWhereClause,
    &matchParams
  )) {
    query->recordCount++;

    /* append the record to the new result set */
    query->useGroupBy = FALSE;
    tree_insert(query, match, &(query->resultSet));
    query->useGroupBy = TRUE;
  }
}
