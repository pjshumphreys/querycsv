#include "querycsv.h"

int runQuery(char *queryFileName) {
  struct qryData query;
  struct resultColumnValue* match = NULL;
  int retval
  MAC_YIELD

  readQuery(queryFileName, &query);

  /* if a command was run, just return the result */
  if(query.commandMode) {
    /* free the query data structures */
    cleanup_query(&query);

    return query.CMD_RETVAL;
  }

  /* allocates space for the next record in the record set */
  reallocMsg(
    (void**)&match,
    (query.columnCount)*sizeof(struct resultColumnValue)
  );

  /* if there is no sorting of results required and the user didn't */
  /* specify an output file then output the results to the screen as soon as they become available */
  if(
      query.orderByClause == NULL &&
      query.outputFileName == NULL &&
      query.groupByClause == NULL
    ) {

    /* output the header */
    outputHeader(&query);

    while(getMatchingRecord(&query, match)) {
      /* print record to stdout */
      outputResult(&query, match, 0);
      match = NULL;

      reallocMsg(
        (void**)&match,
        (query.columnCount)*sizeof(struct resultColumnValue)
      );
    }

    /* the last record wasn't used */
    free(match);
    match = NULL;
  }
  else {
    if(query.groupByClause != NULL) {
      query.useGroupBy = TRUE;
    }

    while(getMatchingRecord(&query, match)) {
      /* add another record to the result set. */
      /* The match variable's allocated memory is the responsibility of the tree now */
      tree_insert(&query, match, &(query.resultSet));
      match = NULL;

      reallocMsg(
        (void**)&match,
        (query.columnCount)*sizeof(struct resultColumnValue)
      );
    }

    /* the last record wasn't used */
    free(match);
    match = NULL;

    /* perform group by operations if it was specified in the query */
    if(query.groupByClause != NULL) {
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

    if(query.outputFileName) {
      /* output the number of records returned iff there was an into clause specified */
      fprintf(stdout, "%d", query.recordCount);
      fflush(stdout);
    }
  }

  /* free the query data structures */
  cleanup_query(&query);

  return EXIT_SUCCESS;
}
