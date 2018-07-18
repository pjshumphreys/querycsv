#include "sql.h"
#include "lexer.h"

int readQuery(char *queryFileName, struct qryData *query) {
  FILE *queryFile = NULL;
  void *scanner;
  struct inputTable *currentInputTable;
  struct resultColumn *currentResultColumn;
  struct sortingList *currentSortingList;
  int initialEncoding = ENC_UNKNOWN;
  int inputTableIndex = 1;
  int parserReturn = 0;
  char* errSyntax = TDB_PARSER_SYNTAX;
  char* errRam = TDB_PARSER_USED_ALL_RAM;
  char* errUnknown = TDB_PARSER_UNKNOWN;

  int recordContinues;
  long headerByteLength;

  struct columnReference *currentReference;
  struct columnReference *newReference;
  struct inputColumn *newColumn;
  char *columnText;
  size_t columnLength;

  MAC_YIELD

  /* read the query file and create the data structures we'll need */
  /* ///////////////////////////////////////////////////////////// */

  /* attempt to open the input file */

  queryFile = skipBom(queryFileName, NULL, &initialEncoding);
  if(queryFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return EXIT_FAILURE;
  }

  /* setup the initial values in the queryData structure */
  query->getCodepoints = chooseGetter(initialEncoding);
  query->inputEncoding = ENC_UNKNOWN;
  query->parseMode = 0;   /* specify we want to just read the file data for now */
  query->hasGrouping = FALSE;
  query->commandMode = 0;
  query->useGroupBy = FALSE;
  query->columnCount = 0;
  query->hiddenColumnCount = 0;
  query->recordCount = 0;
  query->groupCount = 0;
  query->params = PRM_DEFAULT;
  query->outputFileName = NULL;
  query->outputEncoding = ENC_OUTPUT;
  query->columnReferenceHashTable = hash_createTable(32);
  query->firstInputTable = NULL;
  query->secondaryInputTable = NULL;
  query->firstResultColumn = NULL;
  query->joinsAndWhereClause = NULL;
  query->orderByClause = NULL;
  query->groupByClause = NULL;
  query->resultSet = NULL;
  query->dateString = NULL;
  query->newLine = "\n";


  /* setup reentrant flex scanner data structure */
  yylex_init_extra(query, &scanner);

  /* feed our script file into the scanner structure */
  yyset_in(queryFile, scanner);

  /* parse the script file into the query data structure. */
  /* the parser will set up the contents of qryData as necessary */
  /* check that the parsing completed sucessfully, otherwise show a message and quit */
  parserReturn = yyparse(query, scanner);

  if(parserReturn == 0 && query->inputEncoding != ENC_UNKNOWN) {
    /* the input file specified its own encoding. rewind and parse again */

    /* clean up the re-entrant flex scanner */
    yylex_destroy(scanner);

    /* rewind the file. Can't use fseek though as it doesn't work on CC65 */
    fclose(queryFile);
    queryFile = fopen(queryFileName, fopen_read);

    if(queryFile == NULL) {
      fputs(TDB_COULDNT_OPEN_INPUT, stderr);
      return EXIT_FAILURE;
    }

    /*specify the getter to use*/
    query->getCodepoints = chooseGetter(query->inputEncoding);

    /* setup reentrant flex scanner data structure */
    yylex_init_extra(query, &scanner);

    /* feed our script file into the scanner structure */
    yyset_in(queryFile, scanner);

    /* do the first parser pass again using the proper encoding */
    parserReturn = yyparse(query, scanner);
  }
  else {
    query->inputEncoding = initialEncoding;
  }

  switch(parserReturn) {
    case 0: {
      /* parsing finished sucessfully. */

      /* Quit early if a command was run */
      if(query->commandMode) {
        /* clean up the re-entrant flex scanner */
        yylex_destroy(scanner);

        return EXIT_SUCCESS;
      }

      /* otherwise continue processing */
    } break;

    case 1: {
      /* the input script contained a syntax error. show message and exit */
      fputs(errSyntax, stderr);
      return EXIT_FAILURE;
    } break;

    case 2: {
      /* the input script parsing exhausted memory storage space. show message and exit */
      fputs(errRam, stderr);
      return EXIT_FAILURE;
    } break;

    default: {
      /* an unknown error occured when parsing the input script. show message and exit */
      /* (this shouldn't ever happen but you never know) */
      fputs(errUnknown, stderr);
      return EXIT_FAILURE;
    } break;
  }

#ifdef MPW_C
  /* Macs swap 0x0D and 0x0A bytes when writing files, even if binary mode is specified */
  if(query->outputFileName != NULL && (((query->params) & PRM_MAC) == FALSE)) {
    if((((query->params) & PRM_UNIX) != FALSE)) {
      query->newLine = "\r";
    }
    else if(query->outputEncoding == ENC_CP1047) {
      query->newLine = "\205";
    }
    else {
      query->newLine = "\n\r";
    }
  }
#else
  if(query->outputFileName != NULL && (((query->params) & PRM_UNIX) == FALSE)) {
    if((((query->params) & PRM_MAC) != FALSE)) {
      query->newLine = "\r";
    }
    else if(query->outputEncoding == ENC_CP1047) {
      query->newLine = "\302\205";
    }
    else {
      query->newLine = "\r\n";
    }
  }
#endif

  /* set query->firstInputTable to actually be the first input table. */
  query->firstInputTable = currentInputTable =
    (
      query->secondaryInputTable != NULL ?
      query->secondaryInputTable :
      query->firstInputTable
    );

  /* set the index columns for every table other than the first */
  if(query->firstInputTable) {
    do {
      headerByteLength = currentInputTable->firstRecordOffset;
      recordContinues = TRUE;

      /* read csv columns until end of line occurs */
      do {
        columnLength = 0;
        columnText = NULL;
        newReference = NULL;
        newColumn = NULL;

        recordContinues = getCsvColumn(
            currentInputTable,
            &columnText,
            &columnLength,
            NULL,
            &headerByteLength,
            (query->params & PRM_TRIM),
            query->newLine
          );

        d_sprintf(&columnText, "_%s", columnText);

        /* test whether a column with this name already exists in the hashtable */
        currentReference = hash_lookupString(query->columnReferenceHashTable, columnText);

        /* if a column with this name is not already in the hash table */
        if(currentReference == NULL) {
          /* create the new column record */
          reallocMsg((void**)(&newReference), sizeof(struct columnReference));

          /* store the column name */
          newReference->referenceName = columnText;

          /* no other columns with the same name yet */
          newReference->nextReferenceWithName = NULL;

          /* insert the column into the hashtable */
          hash_addString(query->columnReferenceHashTable, columnText, newReference);
        }
        /* otherwise, add it to the linked list */
        else {
          /* we don't need another copy of this text */
          free(columnText);

          /* create the new column record */
          reallocMsg((void**)(&newReference), sizeof(struct columnReference));

          /* store the column name */
          newReference->referenceName = currentReference->referenceName;

          newReference->nextReferenceWithName = NULL;
          /* if there's only been 1 column with this name up until now */
          while(currentReference->nextReferenceWithName != NULL) {
            currentReference = currentReference->nextReferenceWithName;
          }

          currentReference->nextReferenceWithName = newReference;
        }

        newReference->referenceType = REF_COLUMN;

        reallocMsg((void**)(&newColumn), sizeof(struct inputColumn));

        newReference->reference.columnPtr = newColumn;

        newColumn->fileColumnName = newReference->referenceName;

        /* store the column's reference back to it's parent table */
        newColumn->inputTablePtr = (void*)currentInputTable;

        /* we don't yet know whether this column will actually be used in the output query, so indicate this for stage 2's use */
        newColumn->firstResultColumn = NULL;

        /* populate the columnIndex and nextColumnInTable fields */
        if(currentInputTable->firstInputColumn == NULL) {
          newColumn->columnIndex = 1;
          newColumn->nextColumnInTable = newColumn;
          currentInputTable->firstInputColumn = newColumn;
        }
        else {
          newColumn->columnIndex = (currentInputTable->firstInputColumn->columnIndex)+1;
          newColumn->nextColumnInTable = currentInputTable->firstInputColumn->nextColumnInTable;  /* this is a circularly linked list until we've finished adding records */
          currentInputTable->firstInputColumn->nextColumnInTable = newColumn;
          currentInputTable->firstInputColumn = newColumn;
        }
      } while(recordContinues);

      /* keep the current offset of the csv file as we'll need it when we're searching for matching results. avoid using ftell as it doesn't work in cc65 */
      currentInputTable->firstRecordOffset = headerByteLength;

      /* keep an easy to retrieve count of the number of columns in the csv file */
      currentInputTable->columnCount = currentInputTable->firstInputColumn->columnIndex;

      /* cut the circularly linked list of columns in this table */
      newColumn = currentInputTable->firstInputColumn->nextColumnInTable;
      currentInputTable->firstInputColumn->nextColumnInTable = NULL;
      currentInputTable->firstInputColumn = newColumn;

      currentInputTable->fileIndex = inputTableIndex++;
    } while (
        currentInputTable->nextInputTable != query->firstInputTable &&
        (currentInputTable = currentInputTable->nextInputTable)
      );

    /* cut the circularly linked list */
    currentInputTable->nextInputTable = NULL;
  }

  /* set the secondary input table to NULL as we'll be re-using it */
  /* to store the current input table when getting matching records */
  query->secondaryInputTable = NULL;

  /* clean up the re-entrant flex scanner */
  yylex_destroy(scanner);

  /* TODO: use a hash table of the tables names to ensure they are all unique. */
  /* cause an error and exit the program if any table names are non-unique. */

  /* get set up for the second stage (populating the rest of the qryData structure using the cached data from stage 1) */
  query->parseMode = 1;

  /* rewind the file. Can't use fseek though as it doesn't work on CC65 */
  fclose(queryFile);
  queryFile = fopen(queryFileName, fopen_read);
  if(queryFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return EXIT_FAILURE;
  }

  /* setup reentrant flex scanner data structure */
  yylex_init_extra(query, &scanner);

  /* feed our script file into the scanner structure */
  yyset_in(queryFile, scanner);

  /* parse the script file into the query data structure. */
  /* the parser will set up the contents of qryData as necessary */
  /* check that the parsing completed sucessfully, otherwise show a message and quit */
  switch(yyparse(query, scanner)) {
    case 0: {
      /* parsing finished sucessfully. continue processing */
    } break;

    case 1: {
      /* the input script contained a syntax error. show message and exit */
      fputs(errSyntax, stderr);
      return EXIT_FAILURE;
    } break;

    case 2: {
      /* the input script parsing exhausted memory storage space. show message and exit */
      fputs(errRam, stderr);
      return EXIT_FAILURE;
    } break;

    default: {
      /* an unknown error occured when parsing the input script. show message and exit */
      /* (this shouldn't ever happen but you never know) */
      fputs(errUnknown, stderr);
      return EXIT_FAILURE;
    } break;
  }

  currentResultColumn = query->firstResultColumn;

  /* fix the column count value just in case it's wrong (e.g. if there were hidden columns) */
  query->columnCount = currentResultColumn->resultColumnIndex+1;

  /*  cut the circularly linked list of result columns */
  query->firstResultColumn = query->firstResultColumn->nextColumnInResults;
  currentResultColumn->nextColumnInResults = NULL;

  /* cut the circularly linked list of sort by expressions */
  if(query->orderByClause != NULL) {
    currentSortingList = query->orderByClause;
    query->orderByClause = query->orderByClause->nextInList;
    currentSortingList->nextInList = NULL;
  }

  /* cut the circularly linked list of group by expressions */
  if(query->groupByClause != NULL) {
    currentSortingList = query->groupByClause;
    query->groupByClause = query->groupByClause->nextInList;
    currentSortingList->nextInList = NULL;
  }

  /* clean up the re-entrant flex scanner */
  yylex_destroy(scanner);

  /* close the query file */
  fclose(queryFile);

  return EXIT_SUCCESS;
}
