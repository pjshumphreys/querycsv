void parse_tableFactor(
    struct qryData *queryData,
    int isLeftJoin,
    char *fileName,
    char *tableName,
    int fromOptions,
    int fileEncoding
) {
  FILE *csvFile;
  struct inputTable *newTable = NULL;
  long headerByteLength = (fromOptions & PRM_HEADER) ? 128 : 0;

  MAC_YIELD

  /* ensure we are trying to open all the files we need */
  if(queryData->parseMode != 0) {
    free(fileName);
    free(tableName);
    return;
  }

  /* try opening the file specified in the query */
  csvFile = skipBom(fileName, &headerByteLength, &fileEncoding);

  if(csvFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
  }

  /* allocate the storage space for the new table's data */
  reallocMsg((void**)(&newTable), sizeof(struct inputTable));

  /* start populating our newly created table record */
  newTable->queryTableName = tableName;
  newTable->fileName = fileName;
  newTable->fileStream = csvFile;
  newTable->options = fromOptions;
  newTable->fileEncoding = fileEncoding;
  newTable->codepoints[0] = 0;
  newTable->codepoints[1] = 0;
  newTable->codepoints[2] = 0;
  newTable->codepoints[3] = 0;
  newTable->cpIndex = newTable->arrLength = 0;

  /* initalise the "get a codepoint" data structures */
  getNextCodepoint(newTable);

  newTable->firstInputColumn = NULL;  /* the table initially has no columns */
  newTable->isLeftJoined = FALSE;
  newTable->noLeftRecord = TRUE;   /* set just for initialsation purposes */

  newTable->fileIndex = 0;  /* we have to fill the file indexes in afterwards because of left join reordering (queryData->firstInputTable->fileIndex)+1; */

  /* add the new table to the linked list in the query data */
  if(queryData->firstInputTable == NULL) {
    newTable->nextInputTable = newTable;
    queryData->firstInputTable = newTable;
  }
  else {
    if(isLeftJoin) {
      newTable->isLeftJoined = TRUE;

      if(queryData->secondaryInputTable == NULL) {
        queryData->secondaryInputTable = newTable;
        newTable->nextInputTable = queryData->firstInputTable->nextInputTable;
        queryData->firstInputTable->nextInputTable = newTable;
      }
      else {
        newTable->nextInputTable = queryData->secondaryInputTable->nextInputTable;
        queryData->secondaryInputTable->nextInputTable = newTable;
        queryData->secondaryInputTable = newTable;
      }
    }
    else {
      newTable->nextInputTable = queryData->firstInputTable->nextInputTable;  /* this is a circularly linked list until we've finished adding records */
      queryData->firstInputTable->nextInputTable = newTable;
      queryData->firstInputTable = newTable;
    }
  }

  newTable->firstRecordOffset = headerByteLength;
}
