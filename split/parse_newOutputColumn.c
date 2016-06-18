#include "querycsv.h"

struct resultColumn *parse_newOutputColumn(
    struct qryData *queryData,
    int isHidden2,
    int isCalculated2,
    char *resultColumnName2,
    int aggregationType
  ) {

  struct resultColumn* newResultColumn = NULL;

  reallocMsg(TDB_MALLOC_FAILED, (void**)(&newResultColumn), sizeof(struct resultColumn));

  //insert this new output column into the list in the query data
  if(queryData->firstResultColumn == NULL) {
    newResultColumn->resultColumnIndex = 0;
    queryData->firstResultColumn = newResultColumn;
    newResultColumn->nextColumnInResults = newResultColumn;
  }
  else {
    newResultColumn->resultColumnIndex = queryData->firstResultColumn->resultColumnIndex+1;
    newResultColumn->nextColumnInResults = queryData->firstResultColumn->nextColumnInResults;   //maintain circularly linked list for now
    queryData->firstResultColumn->nextColumnInResults = newResultColumn;
    queryData->firstResultColumn = newResultColumn;
  }
  
  //fill out the rest of the necessary fields
  newResultColumn->isHidden = isHidden2;
  newResultColumn->isCalculated = isCalculated2;
  newResultColumn->resultColumnName = resultColumnName2;
  newResultColumn->nextColumnInstance = NULL;   //TODO: this field needs to be filled out properly

  newResultColumn->groupType = aggregationType;
  newResultColumn->groupText = NULL;
  newResultColumn->groupNum = 0;
  newResultColumn->groupCount = 0;
  newResultColumn->groupingDone = FALSE;

  return newResultColumn;
}
