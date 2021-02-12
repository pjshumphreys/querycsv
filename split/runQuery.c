int runQuery(char *queryFileName, int queryIsntString) {
  struct qryData query;
  struct resultColumnValue* match = NULL;

  MAC_YIELD

  if(readQuery(queryFileName, &query, queryIsntString) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  /* if a command was run, just return the result */
  if(query.commandMode) {
    /* free the query data structures */
    cleanup_query(&query);

    return query.CMD_RETVAL;
  }

  /* allocates space for the next record in the record set */
  initMatch(&query, &match);

  /* if there is no sorting of results required and the user didn't */
  /* specify an output file then output the results to the screen as soon as they become available */
  if(
      query.orderByClause == NULL &&
      query.outputFileName == NULL &&
      !query.hasGrouping
    ) {

    /* output the header */
    outputHeader(&query);

    while(getMatchingRecord(&query, match)) {
      /* print record to stdout */
      query.recordCount++;
      outputResult(&query, match, -1, NULL);
    }

    cleanup_matchValues(&query, &match);

    if(query.outputFile == stdout) {
      fputsEncoded(query.newLine, &query);
    }
  }
  else {
    if(query.hasGrouping) {
      query.useGroupBy = TRUE;
    }

    while(getMatchingRecord(&query, match)) {
      /* add another record to the result set. */
      /* The match variable's allocated memory is the responsibility of the tree now */
      query.recordCount++;
      tree_insert(&query, match, &(query.resultSet));
      duplicateMatch(&query, &match);
    }

    cleanup_matchValues(&query, &match);

    /* perform group by operations if it was specified in the query */
    if(query.hasGrouping) {
      groupResults(&query);
      query.useGroupBy = FALSE;
    }

    /* output the results to the specified file */
    outputHeader(&query);

    /* output each record */
    tree_walkAndCleanup(
      &query,
      &(query.resultSet),
      &outputResult
    );

    if(query.outputFile == stdout) {
      fputsEncoded(query.newLine, &query);
    }

    if(query.outputFileName) {
      /* output the number of records returned iff there was an into clause specified */
      fprintf(stdout, D_STRING "\n", query.recordCount);
      fflush(stdout);
    }
  }

  /* free the query data structures */
  cleanup_query(&query);

  return EXIT_SUCCESS;
}
