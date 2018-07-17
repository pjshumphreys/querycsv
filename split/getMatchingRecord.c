int getMatchingRecord(struct qryData *query, struct resultColumnValue *match) {
  struct inputTable *currentInputTable;
  struct inputColumn *currentInputColumn;
  struct resultColumn *currentResultColumn;
  struct resultColumnValue columnOffsetData;
  struct resultColumnParam matchParams;
  int recordHasColumn;
  int doLeftRecord = FALSE;

  MAC_YIELD

  matchParams.ptr = match;
  matchParams.params = query->params;

  /* if secondaryInputTable is NULL then */
  /* the query hasn't returned any results yet. */
  /* needed as this function should continue where it left off next time */
  if(query->secondaryInputTable == NULL) {
    if(query->firstInputTable != NULL) {
      query->secondaryInputTable = query->firstInputTable;
    }
    /* if there's no tables specified at all just return 1 row as we're probably just outputting constants */
    else {
      if(query->recordCount != 0) {
        return FALSE;
      }
      else {
        /* do calculated columns */
        getCalculatedColumns(query, match, FALSE);

        if(!walkRejectRecord(
          1, /* 1 means all tables and *CALCULATED* columns */
          query->joinsAndWhereClause,
          &matchParams
        )) {
          return TRUE;
        }
        else {
          return FALSE;
        }
      }
    }
  }

  currentInputTable = query->secondaryInputTable;

  /* get the offsets of each column in turn and confirm whether the criteria for a match is met */
  /* ****************************************************************************************** */

  do {  /* tables */
    while(
        endOfFile(currentInputTable->fileStream) ||
        (
          currentInputTable->isLeftJoined &&
          currentInputTable->noLeftRecord &&
          (doLeftRecord = TRUE)
        )
      ) {   /* records */

      /* reset the flag that says the column values ran out */
      recordHasColumn = TRUE;

      for(
          currentInputColumn = currentInputTable->firstInputColumn;
          currentInputColumn != NULL;
          currentInputColumn = currentInputColumn->nextColumnInTable
        ) {  /* columns */

        /* if we haven't yet reached the end of a record, get the next column value. */
        /* if it returns false we use an empty string for the value instead */

        if(recordHasColumn == TRUE && !doLeftRecord) {
          columnOffsetData.value = NULL;
          recordHasColumn = getCsvColumn(
              &(currentInputTable->fileStream),
              currentInputTable->fileEncoding,
              &(columnOffsetData.value),
              &(columnOffsetData.length),
              &(columnOffsetData.isQuoted),
              NULL,
              (query->params & PRM_TRIM) == 0,
              query->newLine
            );

          /* these values should actually be set depending on whether the value was quoted or not */
          /* if the value is quoted we should probably also NFD normalise it before writing to the scratchpad */
          columnOffsetData.isNormalized = FALSE;
          columnOffsetData.isNull = FALSE;
        }

        /* construct an empty column reference. */
        else {
          columnOffsetData.isNull = doLeftRecord;
          columnOffsetData.startOffset = 0;
          columnOffsetData.length = 0;
          columnOffsetData.isQuoted = FALSE;
          columnOffsetData.isNormalized = TRUE; /* an empty string needs no unicode normalization */
          columnOffsetData.value = mystrdup("");
        }

        /* put the values retrieved into each of the columns in the output match */
        for(
            currentResultColumn = currentInputColumn->firstResultColumn;
            currentResultColumn != NULL;
            currentResultColumn = currentResultColumn->nextColumnInstance
          ) {

          free(match[currentResultColumn->resultColumnIndex].value);

          memcpy(
              &(match[currentResultColumn->resultColumnIndex]),
              &columnOffsetData,
              sizeof(struct resultColumnValue)
            );
        }
      }
      /* end of columns */

      /* consume any remaining column data that may exist in this record */
      if(recordHasColumn == TRUE && !doLeftRecord) {
        while(getCsvColumn(
            &(currentInputTable->fileStream),
            currentInputTable->fileEncoding,
            NULL,
            NULL,
            NULL,
            NULL,
            (query->params & PRM_TRIM) == 0,
            query->newLine
          )) {
          /* do nothing */
        }
      }

      /* TRUE means the record was rejected. FALSE means the record hasn't yet been rejected */
      if(walkRejectRecord(
          currentInputTable->fileIndex,
          query->joinsAndWhereClause,
          &matchParams
        )) {
        if(doLeftRecord) {
          doLeftRecord = FALSE;
          break;
        }
        else {
          /* go to next record */
          continue;
        }
      }
      else if(currentInputTable->nextInputTable == NULL) {
        /* there was a record match for this table */
        currentInputTable->noLeftRecord = FALSE;

        /* mark every record in every table as having a match, */
        /* even if using a special left join one */
        currentInputTable = query->firstInputTable;

        while((currentInputTable) != (query->secondaryInputTable)) {
          currentInputTable->noLeftRecord = FALSE;
          currentInputTable = currentInputTable->nextInputTable;
        }

        currentInputTable = query->secondaryInputTable;

        /* do calculated columns */
        getCalculatedColumns(query, match, FALSE);

        if(!walkRejectRecord(
          currentInputTable->fileIndex+1, /* +1 means all tables and *CALCULATED* columns */
          query->joinsAndWhereClause,
          &matchParams
        )) {
          return TRUE;
        }
      }
      else {
        /* there are more columns still to check in the next file */
        currentInputTable = query->secondaryInputTable = currentInputTable->nextInputTable;
      }

      doLeftRecord = FALSE;
    }
    /* end of records */

    /* rewind the file, but skip the column headers line */
    fclose(currentInputTable->fileStream);

    if((currentInputTable->fileStream = fopen(currentInputTable->fileName, fopen_read)) == NULL) {
      fputs(TDB_COULDNT_OPEN_INPUT, stderr);
      exit(EXIT_FAILURE);
    }

    myfseek(currentInputTable->fileStream, currentInputTable->firstRecordOffset, SEEK_SET);
    currentInputTable->noLeftRecord = TRUE;

    /* go back up the list of tables. */
    if(currentInputTable->fileIndex == 1) {
      currentInputTable = NULL;
    }
    else {
      currentInputTable = query->firstInputTable;

      while(currentInputTable->nextInputTable != query->secondaryInputTable) {
        currentInputTable = currentInputTable->nextInputTable;
      }

      query->secondaryInputTable = currentInputTable;
    }

  } while (currentInputTable != NULL);
  /* end of tables */

  /* all data scanned. no more matches */
  return FALSE;
}
