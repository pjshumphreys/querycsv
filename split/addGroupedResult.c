void addGroupedResult(
    struct qryData *query,
    struct resultColumnValue *match
) {
  MAC_YIELD

  /* fix up the calculated columns that need it */
  getGroupedColumns(query);

  /* calculate remaining columns that make use of aggregation */
  getCalculatedColumns(query, match, TRUE);

  query->groupCount++;

  /* append the record to the new result set */
  query->useGroupBy = FALSE;
  tree_insert(query, match, &(query->resultSet));
  query->useGroupBy = TRUE;
}
