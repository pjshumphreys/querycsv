#include "querycsv.h"
#include "sql.h"
#include "lexer.h"

void readQuery(
    char *queryFileName,
    struct qryData *query
  ) {
  FILE *queryFile = NULL;
  void *scanner;
  struct inputTable *currentInputTable;
  struct columnReference *currentReferenceWithName;
  struct resultColumn *currentResultColumn;
  struct sortingList *currentSortingList;
  struct columnRefHashEntry *currentHashEntry;

  int initialEncoding = ENC_UNKNOWN;
  int inputTableIndex = 2, i;
  int parserReturn = 0;

  MAC_YIELD

  /* read the query file and create the data structures we'll need */
  /* ///////////////////////////////////////////////////////////// */

  /* attempt to open the input file */

  queryFile = skipBom(queryFileName, NULL, &initialEncoding);
  if(queryFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
  }

  /* setup the initial values in the queryData structure */
  query->getCodepoints = chooseGetter(initialEncoding);
  query->inputEncoding = ENC_UNKNOWN;
  query->parseMode = 0;   /* specify we want to just read the file data for now */
  query->commandMode = 0;
  query->useGroupBy = FALSE;
  query->columnCount = 0;
  query->hiddenColumnCount = 0;
  query->recordCount = 0;
  query->groupCount = 0;
  query->params = 0;
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
    queryFile = fopen(queryFileName, "r");

    if(queryFile == NULL) {
      fputs(TDB_COULDNT_OPEN_INPUT, stderr);
      exit(EXIT_FAILURE);
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

        return;
      }

      /* otherwise continue processing */
    } break;

    case 1: {
      /* the input script contained a syntax error. show message and exit */
      fputs(TDB_PARSER_SYNTAX, stderr);
      exit(EXIT_FAILURE);
    } break;

    case 2: {
      /* the input script parsing exhausted memory storage space. show message and exit */
      fputs(TDB_PARSER_USED_ALL_RAM, stderr);
      exit(EXIT_FAILURE);
    } break;

    default: {
      /* an unknown error occured when parsing the input script. show message and exit */
      /* (this shouldn't ever happen but you never know) */
      fputs(TDB_PARSER_UNKNOWN, stderr);
      exit(EXIT_FAILURE);
    } break;
  }

  query->newLine = query->outputFileName ? "\r\n" : "\n";

  /* set query->firstInputTable to actually be the first input table. */
  query->firstInputTable = currentInputTable =
    (
      query->secondaryInputTable != NULL ?
      query->secondaryInputTable :
      query->firstInputTable
    )->nextInputTable;

  /* set the index columns for every table other than the first */
  while(currentInputTable->nextInputTable != query->firstInputTable) {
    currentInputTable = currentInputTable->nextInputTable;
    currentInputTable->fileIndex = inputTableIndex++;
  }

  /* cut the circularly linked list */
  currentInputTable->nextInputTable = NULL;

  /* set the secondary input table to NULL as we'll be re-using it */
  /* to store the current input table when getting matching records */
  query->secondaryInputTable = NULL;

  /* for every column reference in the hash table, fix up the nextReferenceWithName fields */
  for(i=0; i<query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {
      if(currentHashEntry->content->nextReferenceWithName) {
        currentReferenceWithName = currentHashEntry->content->nextReferenceWithName->nextReferenceWithName;
        currentHashEntry->content->nextReferenceWithName->nextReferenceWithName = NULL;
        currentHashEntry->content->nextReferenceWithName = currentReferenceWithName;
      }

      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }

  /* clean up the re-entrant flex scanner */
  yylex_destroy(scanner);

  /* TODO: use a hash table of the tables names to ensure they are all unique. */
  /* cause an error and exit the program if any table names are non-unique. */

  /* get set up for the second stage (populating the rest of the qryData structure using the cached data from stage 1) */
  query->parseMode = 1;

  /* rewind the file. Can't use fseek though as it doesn't work on CC65 */
  fclose(queryFile);
  queryFile = fopen(queryFileName, "r");
  if(queryFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
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
      fputs(TDB_PARSER_SYNTAX, stderr);
      exit(EXIT_FAILURE);
    } break;

    case 2: {
      /* the input script parsing exhausted memory storage space. show message and exit */
      fputs(TDB_PARSER_USED_ALL_RAM, stderr);
      exit(EXIT_FAILURE);
    } break;

    default: {
      /* an unknown error occured when parsing the input script. show message and exit */
      /* (this shouldn't ever happen but you never know) */
      fputs(TDB_PARSER_UNKNOWN, stderr);
      exit(EXIT_FAILURE);
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
}
