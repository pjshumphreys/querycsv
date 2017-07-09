#include "querycsv.h"

void initMatch(
    struct qryData *query,
    struct resultColumnValue** match
) {
  int i = 0;

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

int runQuery(char *queryFileName) {
  struct qryData query;
  struct resultColumnValue* match = NULL;
  MAC_YIELD

  readQuery(queryFileName, &query);

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
      query.groupByClause == NULL
    ) {

    /* output the header */
    outputHeader(&query);

    while(getMatchingRecord(&query, match)) {
      /* print record to stdout */
      outputResult(&query, match, -1);
    }

    cleanup_matchValues(&query, &match);
  }
  else {
    if(query.groupByClause != NULL) {
      query.useGroupBy = TRUE;
    }

    while(getMatchingRecord(&query, match)) {
      /* add another record to the result set. */
      /* The match variable's allocated memory is the responsibility of the tree now */
      tree_insert(&query, match, &(query.resultSet));
      duplicateMatch(&query, &match);
    }

    cleanup_matchValues(&query, &match);

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
