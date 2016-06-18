#include "querycsv.h"

int runQuery(char* queryFileName, char *params)
{
  struct qryData query;
  struct resultSet results;
  struct resultColumnValue* match;
  int recordsOutput = -1;

  readQuery(queryFileName, &query);

  readCommandLine(params, &(query.params));

  //allocates space for the first record in the record set
  initResultSet(&query, &match, &results);

  while(getMatchingRecord(&query, match)) {
    //if there is no sorting of results required and the user didn't
    //specify an output file then output the result to the screen
    if(
        query.orderByClause == NULL &&
        query.groupByClause == NULL &&
        query.intoFileName == NULL
      ) {

      //print record to stdout
    }

    //add another record to the result set
    appendToResultSet(&query, match, &results);
  }

  //perform group by operations if it was specified in the query
  if(query.hasGrouping == TRUE) {
    groupResults(&query, &results);
  }

  //sort the offsets file according to query specification
  if(query.orderByClause != NULL) {
    query.useGroupBy = FALSE;
    qsort_s(
      (void*)results.records,
      results.recordCount,
      (query.columnCount)*sizeof(struct resultColumnValue),
      recordCompare,
      (void*)(&query)
    );
  }

  //output the results to the specified file
  recordsOutput = outputResults(&query, &results);

  //free the query data structures
  cleanup_query(&query);

  //free(query.scratchpadName);

  //free the result set data structures
  free(results.records);

  free(match);

  //output the number of records returned iff there was an into clause specified
  if(recordsOutput != -1) {
    fprintf(stdout, "%d", recordsOutput);
  }

  return 0;
}
