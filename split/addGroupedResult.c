void addGroupedResult(
    struct qryData *query,
    struct resultColumnValue *match
) {
  struct resultColumnParam matchParams;

  MAC_YIELD

  matchParams.ptr = match;
  matchParams.params = query->params;

  /* Fix up the calculated columns that need it */
  getGroupedColumns(query);

  /* Calculate remaining columns that make use of aggregation */
  getCalculatedColumns(query, match, TRUE);

  /* Apply the aggregate "having" clause filters if necessary */
  if(
    query->joinsAndWhereClause != NULL &&
    walkRejectRecord(
      query->joinsAndWhereClause->minTable+1, /* +1 means all tables and *CALCULATED* columns */
      query->joinsAndWhereClause,
      &matchParams
    )
  ) {
    cleanup_matchValues(query, &match);
    return;
  }

  query->recordCount++;

  /* Append the record to the new result set */
  query->useGroupBy = FALSE;
  tree_insert(query, match, &(query->resultSet));
  query->useGroupBy = TRUE;
}
