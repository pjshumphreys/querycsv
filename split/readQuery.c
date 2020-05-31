#include "sql.h"
#include "lexer.h"

int readQuery(char *origFileName, struct qryData *query, int queryType) {
  FILE *queryFile = NULL;
  void *scanner = NULL;
  struct inputTable *currentInputTable;
  struct resultColumn *currentResultColumn;
  struct sortingList *currentSortingList;
  int initialEncoding = ENC_UNKNOWN;
  int inputTableIndex = 1;
  int parserReturn = 0;
  long offset = 0;
  char* queryFileName = NULL;

  /* read the query file and create the data structures we'll need */
  /* ///////////////////////////////////////////////////////////// */

  #if (defined(MICROSOFT) || defined(__unix__) || defined(__LINUX__)) && !(defined(EMSCRIPTEN) || defined(MPW_C))
    size_t strSize = 0;
    int c;

    MAC_YIELD

    /* if the input file can't be rewound then load it into a string then read from that instead */
    if(queryType == 1 && (queryFile = fopen(origFileName, fopen_read))) {
      if(fseek(queryFile, 0, SEEK_SET)) {
        queryType = 0;

        while((c = fgetc(queryFile)) != EOF) {
          strAppend(c, &queryFileName, &strSize);
        }

        strAppend(0, &queryFileName, &strSize);

        origFileName = queryFileName;
      }

      fclose(queryFile);
    }
  #else
    MAC_YIELD
  #endif

  if(queryType == 1) {
    #if (defined(MICROSOFT) || defined(__unix__) || defined(__LINUX__)) && !(defined(EMSCRIPTEN) || defined(MPW_C))
      /* on windows or posix change the current directory so filenames in the query file are relative to it */
      if(!d_fullpath(origFileName, &queryFileName)) {
        fputs(TDB_COULDNT_OPEN_INPUT, stderr);
        return EXIT_FAILURE;
      }
    #else
      queryFileName = mystrdup(origFileName);
    #endif

    /* attempt to open the input file */
    queryFile = skipBom(queryFileName, &offset, &initialEncoding);

    if(queryFile == NULL) {
      fputs(TDB_COULDNT_OPEN_INPUT, stderr);
      freeAndZero(queryFileName);
      return EXIT_FAILURE;
    }
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
  query->inputFileName = NULL;
  query->outputFileName = NULL;
  query->outputEncoding = ENC_OUTPUT;
  query->codepointsInLine = 0;
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
  if(queryType == 1) {
    yyset_in(queryFile, scanner);
  }
  else {
    yy_scan_string(origFileName, scanner);
  }

  /* parse the script file into the query data structure. */
  /* the parser will set up the contents of qryData as necessary */
  /* check that the parsing completed sucessfully, otherwise show a message and quit */
  parserReturn = yyparse(query, scanner);

  /* clean up the re-entrant flex scanner */
  yylex_destroy(scanner);
  scanner = NULL;

  /* rewind the file. Can't use fseek though as it doesn't work on CC65 */
  if(queryType == 1) {
    fclose(queryFile);

    if(parserReturn == 0 && query->inputEncoding != ENC_UNKNOWN) {
      /* the input file specified its own encoding. rewind and parse again */
      queryFile = fopen(queryFileName, fopen_read);

      if(queryFile == NULL) {
        fputs(TDB_COULDNT_OPEN_INPUT, stderr);
        free(queryFileName);
        return EXIT_FAILURE;
      }

      myfseek(queryFile, offset, SEEK_SET);

      /*specify the getter to use*/
      query->getCodepoints = chooseGetter(query->inputEncoding);

      /* setup reentrant flex scanner data structure */
      yylex_init_extra(query, &scanner);

      /* feed our script file into the scanner structure */
      yyset_in(queryFile, scanner);

      /* do the first parser pass again using the proper encoding */
      parserReturn = yyparse(query, scanner);

      /* clean up the re-entrant flex scanner */
      yylex_destroy(scanner);
      scanner = NULL;

      /* rewind the file. Can't use fseek though as it doesn't work on CC65 */
      fclose(queryFile);
    }
    else {
      query->inputEncoding = initialEncoding;
    }
  }

  switch(parserReturn) {
    case 0: {
      /* parsing finished sucessfully. */
    } break;

    default: {
      /* an unknown error occured when parsing the input script. show message and exit */
      /* (this shouldn't ever happen but you never know) */
      free(queryFileName);
    } return EXIT_FAILURE;
  }

  if(query->outputFileName == NULL) {
    query->newLine = "\n";
  }
  #if defined(MPW_C) && !defined(RETRO68)
    /* MPW swaps 0x0D and 0x0A bytes when writing files, even if binary mode is specified */
    else if(query->outputEncoding == ENC_TSW || query->outputEncoding == ENC_CP1047 || query->params & PRM_UNIX) {
      query->newLine = "\r";
    }
    else if(query->params & PRM_MAC) {
      query->newLine = "\n";
    }
    else {
      query->newLine = "\n\r";
    }
  #else
    else if(query->outputEncoding == ENC_TSW || query->outputEncoding == ENC_CP1047 || query->params & PRM_UNIX) {
      query->newLine = "\n";
    }
    else if(query->params & PRM_MAC) {
      query->newLine = "\r";
    }
    else {
      query->newLine = "\r\n";
    }
  #endif

  /* Quit early if a command was run */
  if(query->commandMode) {
    runCommand(query);
    free(queryFileName);
    return EXIT_SUCCESS;
  }

  /* otherwise continue processing */

  /* set query->firstInputTable to actually be the first input table. */
  if(query->secondaryInputTable != NULL) {
    currentInputTable = query->secondaryInputTable;
  }
  else {
    currentInputTable = query->firstInputTable;
  }

  query->firstInputTable = currentInputTable;

  /* set the index columns for every table other than the first */
  if(query->firstInputTable) {
    do {
      getTableColumns(query, currentInputTable);

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

  /* TODO: use a hash table of the tables names to ensure they are all unique. */
  /* cause an error and exit the program if any table names are non-unique. */

  /* get set up for the second stage (populating the rest of the qryData structure using the cached data from stage 1) */
  query->parseMode = 1;

  if(queryType == 1) {
    queryFile = fopen(queryFileName, fopen_read);
    free(queryFileName); /* not needed any more */

    if(queryFile == NULL) {
      fputs(TDB_COULDNT_OPEN_INPUT, stderr);
      return EXIT_FAILURE;
    }

    myfseek(queryFile, offset, SEEK_SET);
  }

  /* setup reentrant flex scanner data structure */
  yylex_init_extra(query, &scanner);

  /* feed our script file into the scanner structure */
  if(queryType == 1) {
    yyset_in(queryFile, scanner);
  }
  else {
    yy_scan_string(origFileName, scanner);
  }

  parserReturn = yyparse(query, scanner);

  /* clean up the re-entrant flex scanner */
  yylex_destroy(scanner);

  /* close the query file */
  if(queryType == 1) {
    fclose(queryFile);
  }

  /* parse the script file into the query data structure. */
  /* the parser will set up the contents of qryData as necessary */
  /* check that the parsing completed sucessfully, otherwise show a message and quit */
  switch(parserReturn) {
    case 0: {
      /* parsing finished sucessfully. continue processing */
    } break;

    default:
      /* an unknown error occured when parsing the input script. show message and exit */
      /* (this shouldn't ever happen but you never know) */
    return EXIT_FAILURE;
  }

  currentResultColumn = query->firstResultColumn;

  /* fix the column count value just in case it's wrong (e.g. if there were hidden columns) */
  query->columnCount = currentResultColumn->resultColumnIndex+1;

  /* cut the circularly linked list of result columns */
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

  return EXIT_SUCCESS;
}
