/*
  this program enables the user to run a sql like select statement stored in a file that can retrieve data from multiple csv files and generate another csv file of the results.

  There are also a bunch of additional utility features that simplify using csv files
  from msdos batch file scripts, and one that can retrieve the current date in ISO8601 format
*/

#include "querycsv.h"
#include "sql.h"
#include "lexer.h"

void reallocMsg(char *failureMessage, void** mem, size_t size) {
  void * temp = NULL;
  if((temp = realloc(*mem, size)) == NULL) {
    fputs(failureMessage, stderr);
    exit(EXIT_FAILURE);
  }

  *mem = temp;
}

//free memory and set the pointer to NULL. this function should hopefully get inlined
void strFree(char ** str) {
  free(*str);
  *str = NULL;
}

//checks whether the end of file is the next character in the stream.
//Used by getMatchingRecord
int endOfFile(FILE* stream) {
  int c;

  c = fgetc(stream);
  ungetc(c, stream);
  
  return c != EOF;
}

//get localtime, gmtime and utc offset string from a time_t. allocate/free memory as needed
//any of the last three parameters can be skipped by passing null
int tztime_d(time_t *now, struct tm *local, struct tm *utc, char **output) {
  struct tm * lcl;
  struct tm * gm;

  int hourlcl;
  int hourutc;
  int difference;
  int hour_difference;
  int minute_difference;

  char* output2;
  char* format = "+%02i%02i";

  if(now == NULL) {
    return FALSE;
  }

  if(local != NULL) {
    lcl = local;

    memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
  }
  else if (output != NULL) {
    lcl = (struct tm *)malloc(sizeof(struct tm));

    if(lcl == NULL) {
      return FALSE;
    }

    memcpy((void*)lcl, (void*)localtime(now), sizeof(struct tm));
  } 

  if(utc != NULL) {
    gm = utc;

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }
  else if(output != NULL) {
    gm = (struct tm *)malloc(sizeof(struct tm));

    if(gm == NULL) {
      if(local == NULL) {
        free((void*)lcl);
      }

      return FALSE;
    }

    memcpy((void*)gm, (void*)gmtime(now), sizeof(struct tm));
  }

  if(output != NULL) {
    hourlcl = lcl->tm_hour;
    hourutc = gm->tm_hour;
    
    output2 = (char*)realloc((void*)*output, 6*sizeof(char));
    
    if(output2 == NULL) {
      if(local == NULL) {
        free((void*)lcl);
      }

      if(utc == NULL) {
        free((void*)gm);
      }

      return FALSE;
    }
    *output = output2;

    if(lcl->tm_year > gm->tm_year) {
      hourlcl+=24;
    }
    else if (gm->tm_year > lcl->tm_year) {
      hourutc+=24;
    }
    else if(lcl->tm_yday > gm->tm_yday) {
      hourlcl+=24;
    }
    else if (gm->tm_yday > lcl->tm_yday) {
      hourutc+=24;
    }

    difference = ((hourlcl - hourutc)*60)+lcl->tm_min - gm->tm_min;
    minute_difference = difference % 60;
    hour_difference = (difference - minute_difference)/60;

    if(hour_difference < 0) {
      format = "%03i%02i";
    }

    snprintf(*output, 6, format, hour_difference, minute_difference);
  }
  
  if(local == NULL) {
    free((void*)lcl);
  }

  if(utc == NULL) {
    free((void*)gm);
  }

  return TRUE;
}

//write a formatted string into a string buffer. allocate/free memory as needed
int snprintf_d(char** str, char* format, ...) {
  FILE * pFile;
  va_list args;
  size_t newSize;
  char* newStr;
  
  //Check sanity of inputs and open /dev/null so that we can
  //get the space needed for the new string. There's unfortunately no
  //easier way to do this that uses only ISO functions. Filenames can only
  //be portably specified in terms of the user's codepage as well :(
  if(str == NULL || format == NULL || (pFile = fopen(DEVNULL, "wb")) == NULL) {
    return FALSE;
  }

  //get the space needed for the new string
  va_start(args, format);
  newSize = (size_t)(vfprintf(pFile, format, args)+1); //plus L'\0'
  va_end(args);

  //close the file. We don't need to look at the return code as we were writing to /dev/null 
  fclose(pFile);

  //Create a new block of memory with the correct size rather than using realloc
  //as any old values could overlap with the format string. quit on failure
  if(newSize < 2 ||
  (newStr = (char*)malloc(newSize*sizeof(char))) == NULL) {
    return FALSE;
  }

  //do the string formatting for real
  va_start(args, format);
  vsnprintf(newStr, newSize, format, args);
  va_end(args);

  //ensure null termination of the string
  newStr[newSize] = L'\0';

  //free the old contents of the output if present
  strFree(str);

  //set the output pointer to the new pointer location
  *str = newStr;
  
  //everything occurred successfully
  return TRUE;
}

//format a date into a string. allocate/free memory as needed
int strftime_d(char** ptr, char* format, struct tm* timeptr) {
  size_t length = 32; //starting value
  size_t length2 = 0;
  char *output = NULL;
  char *output2 = NULL;

  if(ptr == NULL || format == NULL) {
    return FALSE;
  }

  while (length2 == 0) {
    output2 = realloc((void*)output, length*sizeof(char));

    if(output2 == NULL) {
      strFree(&output);

      return FALSE;
    }
    output = output2;

    //calling strftime using the buffer we created
    length2 = strftime(output, length, format, timeptr);

    //double the allocation length to use if we need to try again
    length *= 2;  
  }

  //shrink the allocated memory to fit the returned length
  output2 = realloc((void*)output, (length2+1)*sizeof(char));

  //quit if the shrinking didn't work successfully
  if(output2 == NULL) {
    strFree(&output);

    return FALSE;
  }

  //free the contents of ptr then update it to point to the string we've built up
  strFree(ptr);
  *ptr = output2;

  //everything completed successfully
  return TRUE;
}

//append a character into a string with a given length, using realloc
int strAppend(char c, char** value, size_t* strSize) {
  char* temp;

  //validate inputs
  //increase value length by 1 character

  //update the string pointer
  //increment strSize
  if(strSize != NULL) {
	if(value != NULL) {
	  if((temp = realloc(*value, (*strSize)+1)) != NULL) {
		*value = temp;

		//store the additional character
		(*value)[*strSize] = c;
	  }
	  else {
		return FALSE;
	  }
	}

    (*strSize)++;
  }

  return TRUE;
}

//decrement size of a string to represent right trimming whitespace 
int strRTrim(char** value, size_t* strSize) {
  char* end;
  char* str;
  int size;

  if(value == NULL || strSize == NULL) {
    return FALSE;
  }

  str = *value;
  size = *strSize;

  end = str + size - 1;
  while(end > str && (*end == ' ' || *end == '\t')) {
    end--;
    size--;
  }

  *strSize = size; 

  return TRUE;
}

char *strReplace(char *search, char *replace, char *subject) {
  char *replaced = (char*)calloc(1, 1), *temp = NULL;
  char *p = subject, *p3 = subject, *p2;
  int  found = 0;

  if(
      search == NULL ||
      replace == NULL ||
      subject == NULL ||
      strlen(search) == 0 ||
      strlen(replace) == 0 ||
      strlen(subject) == 0
    ) {
    return NULL;
  }  
  
  while((p = strstr(p, search)) != NULL) {
    found = 1;
    temp = realloc(replaced, strlen(replaced) + (p - p3) + strlen(replace));

    if(temp == NULL) {
      free(replaced);
      return NULL;
    }

    replaced = temp;
    strncat(replaced, p - (p - p3), p - p3);
    strcat(replaced, replace);
    p3 = p + strlen(search);
    p += strlen(search);
    p2 = p;
  }
  
  if (found == 1) {
    if (strlen(p2) > 0) {
      temp = realloc(replaced, strlen(replaced) + strlen(p2) + 1);

      if (temp == NULL) {
        free(replaced);
        return NULL;
      }

      replaced = temp;
      strcat(replaced, p2);
    }
  }
  else {
    temp = realloc(replaced, strlen(subject) + 1);

    if (temp != NULL) {
      replaced = temp;
      strcpy(replaced, subject);
    }
  }

  return replaced;
}

int getCsvColumn(
    FILE ** inputFile,
    char** value,
    size_t* strSize,
    int* quotedValue,
    long* startPosition
  ) {

  int c;
  char *tempString = NULL;
  int valueStarted = FALSE;
  int valueFinished = FALSE;  
  int quotedValue2 = FALSE;

  if(feof(*inputFile) != 0) {
    return FALSE;
  }

  if (quotedValue != NULL) {
    *quotedValue = FALSE;
  }

  if(strSize != NULL) {
    *strSize = 0;
  }
  
  if (value == NULL) {
    value = &tempString;
  }
  
  //read until start of value
  while (valueStarted == FALSE) {
    c = fgetc(*inputFile);
    
    switch(c) {
      case ',':
        free(tempString);
        return TRUE;
      case ' ':
      case '\t':
      break;

      case '\r':
        if((c = fgetc(*inputFile)) != '\n' && c != EOF) {
          ungetc(c, *inputFile);
        }
      case EOF:
      case '\n':
        free(tempString);
        return FALSE;
      break;

      case '"':
        if (quotedValue != NULL) {
          *quotedValue = TRUE;
        }
        
        if(startPosition != NULL) {
          *startPosition = ftell(*inputFile)-1;
        }
        
        quotedValue2 = TRUE;
        valueStarted = TRUE;
      break;
      
      default:
        ungetc(c, *inputFile);

        if(startPosition != NULL) {
          *startPosition = ftell(*inputFile);
        }

        valueStarted = TRUE;
      break;
    }
  }

  if(quotedValue2) {
    //read until end of value
    while (valueFinished == FALSE) {
      c = fgetc(*inputFile);
      
      switch(c) {
        case EOF:
          free(tempString);
          return FALSE;
        break;

        case '"':
          c = fgetc(*inputFile);
          if (c != '"') {
            valueFinished = TRUE;
            ungetc(c, *inputFile);
            strAppend('\0', value, strSize);
            if(strSize != NULL) {
              (*strSize)--;
            }
          }
          else {
            strAppend(c, value, strSize);
          }
        break;

        case '\r':
          strAppend('\n', value, strSize);
          if((c = fgetc(*inputFile)) != '\n' && c != EOF) {
            ungetc(c, *inputFile);
          }
        break;
      
        default:
          strAppend(c, value, strSize);
        break;
      }
    }

    //read until comma, newline or EOF. text here is ignored
    for ( ; ; ) {
      c = fgetc(*inputFile);
      
      switch(c) {
        case '\r':
          if((c = fgetc(*inputFile)) != '\n' && c != EOF) {
            ungetc(c, *inputFile);
          }
        case '\n':
        case EOF:
          free(tempString);
          return FALSE;
        break;
        case ',':
          free(tempString);
          return TRUE;
        default:
        break;
      }
    }
  }
  else {
    //read until comma, newline or EOF. append any other bytes to the output string
    for ( ; ; ) {
      c = fgetc(*inputFile);
      
      switch(c) {
        case '\r':
          if((c = fgetc(*inputFile)) != '\n' && c != EOF) {
            ungetc(c, *inputFile);
          }
        case '\n':
        case EOF:
        case ',':
          strRTrim(value, strSize);
          strAppend('\0', value, strSize);
          if(strSize != NULL) {
            (*strSize)--;
          }
          free(tempString);
          return c == ',';
        break;
        
        default:
          strAppend(c, value, strSize);
        break;
      }
    }
  }

  free(tempString);
  return FALSE;
}

void stringGet(unsigned char **str, struct resultColumnValue* field) {
  long offset = ftell(*(field->source));
  char* string2 = NULL;
  
  fseek(*(field->source), field->startOffset, SEEK_SET);
  fflush(*(field->source));
  
  if (field->isQuoted) {
    //can't use a shortcut to get the string value, so get it the same way we did the last time
    getCsvColumn(field->source,str,&(field->length),NULL,NULL);
  }
  else {  
    //can use a shortcut to get the string value
	reallocMsg("alloc failed", (void**)str, field->length+1);

	if(fread(*str, 1, field->length, *(field->source)) != field->length) {
      fputs("didn't read string properly\n", stderr);
      exit(EXIT_FAILURE);
    }
	
    (*str)[field->length] = '\0';
  }

  //reset the file offset as we don't know what else the file is being used for
  fseek(*(field->source), offset, SEEK_SET);
  fflush(*(field->source));  
}

FILE* openTemp(char** name) {
  int fd = -1;
  FILE *sfp;
  
  snprintf_d(name, "%s/XXXXXXXX", getenv(TEMP_VAR));
  
  if (
      (fd = mkstemp(*name)) == -1 ||
      (sfp = fdopen(fd, "wb+")) == NULL
    ) {

    if (fd != -1) {
      close(fd);
      unlink(*name);
      strFree(name);
    }
  
    fputs("Couldn't create scratchpad file", stderr);
    exit(EXIT_FAILURE);
  }
  return sfp;
}

void parseQuery(char* queryFileName, struct qryData * query) {
  FILE * queryFile = NULL;
  void * scanner;
  struct inputTable* currentInputTable;
  struct columnReference* currentReference;
  struct columnReference* currentReferenceWithName;
  struct resultColumn* currentResultColumn;
  struct sortingList* currentSortingList;
  struct columnRefHashEntry* currentHashEntry;

  int inputTableIndex = 2, i;

  //read the query file and create the data structures we'll need
  ///////////////////////////////////////////////////////////////

  //attempt to open the input file
  queryFile = fopen(queryFileName, "r");
  if(queryFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    exit(EXIT_FAILURE);
  }

  //setup the initial values in the queryData structure
  query->parseMode = 0;   //specify we want to just read the file data for now
  query->hasGrouping = FALSE;
  query->columnCount = 0;
  query->hiddenColumnCount = 0;
  query->groupCount = 0;
  query->intoFileName = NULL;
  query->firstInputTable = NULL;
  query->secondaryInputTable = NULL;
  query->columnReferenceHashTable = create_hash_table(32);  
  query->firstResultColumn = NULL;
  query->joinsAndWhereClause = NULL;
  query->orderByClause = NULL;
  query->groupByClause = NULL;
  query->scratchpadName = NULL;
  query->scratchpad = openTemp(&(query->scratchpadName));

  //setup reentrant flex scanner data structure
  yylex_init(&scanner);

  //feed our script file into the scanner structure
  yyset_in(queryFile, scanner);

  //parse the script file into the query data structure.
  //the parser will set up the contents of qryData as necessary
  //check that the parsing completed sucessfully, otherwise show a message and quit
  switch(yyparse(query, scanner)) {
    case 0:
      //parsing finished sucessfully. continue processing
    break;
    
    case 1: 
      //the input script contained a syntax error. show message and exit
      fputs(TDB_PARSER_SYNTAX, stderr);
      exit(EXIT_FAILURE);
    break;

    case 2:
      //the input script parsing exhausted memory storage space. show message and exit
      fputs(TDB_PARSER_USED_ALL_RAM, stderr);
      exit(EXIT_FAILURE);
    break;

    default:
      //an unknown error occured when parsing the input script. show message and exit
      //(this shouldn't ever happen but you never know)
      fputs(TDB_PARSER_UNKNOWN, stderr);
      exit(EXIT_FAILURE);
    break;
  }

  //set query->firstInputTable to actually be the first input table.
  query->firstInputTable = currentInputTable =
  (query->secondaryInputTable != NULL ?
  query->secondaryInputTable :
  query->firstInputTable)->nextInputTable;

  //set the index columns for every table other than the first
  while (currentInputTable->nextInputTable != query->firstInputTable) {
    currentInputTable = currentInputTable->nextInputTable;
    currentInputTable->fileIndex = inputTableIndex++;
  }
  
  //cut the circularly linked list
  currentInputTable->nextInputTable = NULL;

  //set the secondary input table to NULL as we'll be re-using it
  //to store the current input table when getting matching records
  query->secondaryInputTable = NULL;

  //for every column reference in the hash table, fix up the nextReferenceWithName fields
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

  //clean up the re-entrant flex scanner
  yylex_destroy(scanner);

  //TODO: use a hash table of the tables names to ensure they are all unique.
  //cause an error and exit the program if any table names are non-unique.

  //get set up for the second stage (populating the rest of the qryData structure using the cached data from stage 1)
  query->parseMode = 1;
  
  //rewind the file
  fseek(queryFile, 0, SEEK_SET);

  //setup reentrant flex scanner data structure
  yylex_init(&scanner);

  //feed our script file into the scanner structure
  yyset_in(queryFile, scanner);

  //parse the script file into the query data structure.
  //the parser will set up the contents of qryData as necessary
  //check that the parsing completed sucessfully, otherwise show a message and quit
  switch(yyparse(query, scanner)) {
    case 0:
      //parsing finished sucessfully. continue processing
    break;
    
    case 1: 
      //the input script contained a syntax error. show message and exit
      fputs(TDB_PARSER_SYNTAX, stderr);
      exit(EXIT_FAILURE);
    break;

    case 2:
      //the input script parsing exhausted memory storage space. show message and exit
      fputs(TDB_PARSER_USED_ALL_RAM, stderr);
      exit(EXIT_FAILURE);
    break;

    default:
      //an unknown error occured when parsing the input script. show message and exit
      //(this shouldn't ever happen but you never know)
      fputs(TDB_PARSER_UNKNOWN, stderr);
      exit(EXIT_FAILURE);
    break;
  }

  currentResultColumn = query->firstResultColumn;

  //fix the column count value just in case it's wrong (e.g. if there were hidden columns)
  query->columnCount = currentResultColumn->resultColumnIndex+1; 

  // cut the circularly linked list of result columns
  query->firstResultColumn = query->firstResultColumn->nextColumnInResults;
  currentResultColumn->nextColumnInResults = NULL;

  //cut the circularly linked list of sort by expressions
  if(query->orderByClause != NULL) {
    currentSortingList = query->orderByClause;
    query->orderByClause = query->orderByClause->nextInList;
    currentSortingList->nextInList = NULL;
  }

  //cut the circularly linked list of group by expressions
  if(query->groupByClause != NULL) {
    currentSortingList = query->groupByClause;
    query->groupByClause = query->groupByClause->nextInList;
    currentSortingList->nextInList = NULL;
  }

  //clean up the re-entrant flex scanner
  yylex_destroy(scanner);
  
  //close the query file
  fclose(queryFile);
}

void initResultSet(
    struct qryData * query,
    struct resultColumnValue** match,
    struct resultSet* results
  ) {

  results->recordCount = 0;
  results->records = NULL;

  *match = NULL;

  reallocMsg(
    "couldn't initialise resultset",
    (void**)match,
    (query->columnCount)*sizeof(struct resultColumnValue)
  );
}

void appendToResultSet(
    struct qryData * query,
    struct resultColumnValue* match,
    struct resultSet* results
  ) {

  reallocMsg(
    "couldn't append record to resultset",
    (void**)&(results->records),
    (results->recordCount+1)*(query->columnCount)*sizeof(struct resultColumnValue)
  );
  
  memcpy(
      &(results->records[results->recordCount*query->columnCount]),
      match,
      (query->columnCount)*sizeof(struct resultColumnValue)
    );

  results->recordCount++;
}

void exp_divide(char** value, double leftVal, double rightVal) {
  if(rightVal == 0.0) {
    *value = strdup("Infinity");
  }
  else {
    snprintf_d(value, "%g", leftVal/rightVal);
  }
}

void exp_uminus(char** value, double leftVal) {
  if(leftVal == 0.0) {
    *value = strdup("0");
  }
  else {
    snprintf_d(value, "%g", leftVal*-1);
  }
}


//getValue can be passed a scalar expression's abstract syntax
//tree and it will evaluate it and turn it into a literal string of text characters.
//the evaluated value must be freed later though.
void getValue(
    struct expression* expressionPtr,
    struct resultColumnValue * match
  ) {
  struct expression * calculatedField;
  
  long offset;
  
  switch(expressionPtr->type) {
    case EXP_COLUMN: {
      //get the value of the first instance in the result set of
      //this input column (it should have just been filled out with a
      //value for the current record)
      //TODO: test and fix the behaviour here
      stringGet(
          &(expressionPtr->value),
          &(match[
            ((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->
            firstResultColumn->resultColumnIndex
          ])
        );
    } break;

    case EXP_LITERAL: {
      expressionPtr->value = strdup((char *)expressionPtr->unionPtrs.voidPtr);
    } break;

    case EXP_CALCULATED: {
      calculatedField = ((struct expression*)(expressionPtr->unionPtrs.voidPtr));

      getValue(
          calculatedField,
          match
        );

      expressionPtr->value = strdup(calculatedField->value);

      strFree(&(calculatedField->value));
    } break;

    case EXP_GROUP: {
      expressionPtr->value = strdup(*((char **)expressionPtr->unionPtrs.voidPtr));
    } break;

    case EXP_UPLUS: {
       getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );

      snprintf_d(
          &(expressionPtr->value),
          "%g",
          strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)
        );
        
      strFree(&(expressionPtr->unionPtrs.leaves.leftPtr->value));
    } break;

    case EXP_UMINUS: {
      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );

      exp_uminus(
          &(expressionPtr->value),
          strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)
        );
        
      strFree(&(expressionPtr->unionPtrs.leaves.leftPtr->value));
    } break;
    
    default: {
      if(expressionPtr->type > EXP_CONCAT) {
        expressionPtr->value = strdup("");
        break;
      }

      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );
      
      getValue(
          expressionPtr->unionPtrs.leaves.rightPtr,
          match
        );

      switch(expressionPtr->type){
        case EXP_PLUS:
          snprintf_d(
              &(expressionPtr->value),
              "%g",
              strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)+
              strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL)
            );
        break;

        case EXP_MINUS:
          snprintf_d(
              &(expressionPtr->value),
              "%g",
              strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)-
              strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL)
            );
        break;

        case EXP_MULTIPLY:
          snprintf_d(
              &(expressionPtr->value),
              "%g",
              strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)*
              strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL)
            );
        break;

        case EXP_DIVIDE:
          exp_divide(
              &(expressionPtr->value),
              strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL),
              strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL)
            );
        break;

        case EXP_CONCAT:
          snprintf_d(
              &(expressionPtr->value),
              "%s%s",
              expressionPtr->unionPtrs.leaves.leftPtr->value,
              expressionPtr->unionPtrs.leaves.rightPtr->value
            );
        break;
      }
      
      strFree(&(expressionPtr->unionPtrs.leaves.leftPtr->value));
      strFree(&(expressionPtr->unionPtrs.leaves.rightPtr->value));
    } break;
  }
}

//TRUE means the record was rejected.  FALSE means the record should not yet be rejected
int walkRejectRecord(
    int currentTable,
    struct expression* expressionPtr,
    struct resultColumnValue * match
  ) {

  int retval,i;
  struct atomEntry * currentAtom;

  if(expressionPtr == NULL || currentTable < expressionPtr->minTable) {
    return FALSE;
  }

  if(expressionPtr->type >= EXP_EQ && expressionPtr->type <= EXP_GTE){
    getValue(expressionPtr->unionPtrs.leaves.leftPtr, match);
    getValue(expressionPtr->unionPtrs.leaves.rightPtr, match);

    retval = strCompare(
        &(expressionPtr->unionPtrs.leaves.leftPtr->value),
        &(expressionPtr->unionPtrs.leaves.rightPtr->value),
        expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive != 0,
        (void (*)())&getUnicodeChar,
        (void (*)())&getUnicodeChar
      );

    strFree(&(expressionPtr->unionPtrs.leaves.leftPtr->value));
    strFree(&(expressionPtr->unionPtrs.leaves.rightPtr->value));
    
    switch(expressionPtr->type) {
      case EXP_EQ:
        return retval != 0;
      case EXP_NEQ:
        return retval == 0;
      case EXP_LT:
        return retval != -1;
      case EXP_LTE:
        return retval == 1;
      case EXP_GT:
        return retval != 1;
      case EXP_GTE:
        return retval == -1;
    }
  }
  else if(expressionPtr->type == EXP_AND) {
    return
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) ||
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
  }
  else if(expressionPtr->type == EXP_OR) {
    return 
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) &&
        walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
  }
  else if(expressionPtr->type <= EXP_NOT) {
    return walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) == 0;
  }
  else if(expressionPtr->type == EXP_IN || expressionPtr->type == EXP_NOTIN) {
    getValue(expressionPtr->unionPtrs.leaves.leftPtr, match);

    for(
        i = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->index,
          currentAtom = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->nextInList;
        i != currentAtom->index;
        currentAtom = currentAtom->nextInList
      ) {
      if(strCompare(
        &(expressionPtr->unionPtrs.leaves.leftPtr->value),
        &(currentAtom->content),
        expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive != 0,
        (void (*)())&getUnicodeChar,
        (void (*)())&getUnicodeChar
      ) == 0) {
        strFree(&(expressionPtr->unionPtrs.leaves.leftPtr->value));
        return expressionPtr->type == EXP_NOTIN;  //FALSE
      }
    }

    strFree(&(expressionPtr->unionPtrs.leaves.leftPtr->value));
    return expressionPtr->type != EXP_NOTIN;  //TRUE
  }
  
  return FALSE;
}

void getCalculatedColumns(
    struct qryData* query,
    struct resultColumnValue* match,
    int runAggregates
  ) {

  int i, j;
  struct columnReference* currentReference;
  struct resultColumn* currentResultColumn;
  struct columnRefHashEntry* currentHashEntry;

  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {
      
      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            currentReference->reference.calculatedPtr.expressionPtr->containsAggregates == runAggregates &&

            //get the current result column
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL
          ) {
            
          //start setting column value fields
          j = currentResultColumn->resultColumnIndex;
          match[j].isQuoted = FALSE;
          match[j].isNormalized = FALSE;
          match[j].source = &(query->scratchpad);

          //seek to the end of the scratchpad file and update the start position
          fseek(query->scratchpad, 0, SEEK_END);
          fflush(query->scratchpad);
          match[j].startOffset = ftell(query->scratchpad);

          //get expression value for this match
          getValue(currentReference->reference.calculatedPtr.expressionPtr, match);

          //store the value's length
          match[j].length = strlen(currentReference->reference.calculatedPtr.expressionPtr->value);

          //write the value to the scratchpad file
          fputs(currentReference->reference.calculatedPtr.expressionPtr->value, query->scratchpad);

          //free the expression value for this match
          strFree(&(currentReference->reference.calculatedPtr.expressionPtr->value));
        }
	  
        currentReference = currentReference->nextReferenceWithName;
      }

      //go to the next reference in the hash table
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}

int getMatchingRecord(struct qryData* query, struct resultColumnValue* match) {
  struct inputTable* currentInputTable;
  struct inputColumn* currentInputColumn;
  struct resultColumn* currentResultColumn;
  struct resultColumnValue columnOffsetData;
  int recordHasColumn;
  int noLeftRecord = TRUE;

  //if secondaryInputTable is NULL then
  //the query has yet returned any results.
  //needed as this function should continue where it left off next time
  if(query->secondaryInputTable == NULL) {
    query->secondaryInputTable = query->firstInputTable;
  }

  currentInputTable = query->secondaryInputTable;

  //get the offsets of each column in turn and confirm whether the criteria for a match is met
  ////////////////////////////////////////////////////////////////////////////////////////////

  do {  //tables
    while(endOfFile(currentInputTable->fileStream)) {   //records

      //reset the flag that says the column values ran out
      recordHasColumn = TRUE;

      for (
          currentInputColumn = currentInputTable->firstInputColumn;
          currentInputColumn != NULL;
          currentInputColumn = currentInputColumn->nextColumnInTable
        ) {  //columns

        //if we haven't yet reached the end of a record, get the next column value.
        //if it returns false we use an empty string for the value instead

        if(recordHasColumn == TRUE) {
          recordHasColumn = getCsvColumn(
              &(currentInputTable->fileStream),
              NULL,
              &(columnOffsetData.length),
              &(columnOffsetData.isQuoted),
              &(columnOffsetData.startOffset)
            );

          //these values should actually be set depending on whether the value was quoted or not
          //if the value is quoted we should probably also NFD normalise it before writing to the scratchpad
          columnOffsetData.isNormalized = FALSE;
          columnOffsetData.source = &(currentInputTable->fileStream);
        }

        //construct an empty column reference.
        else {
          columnOffsetData.isQuoted = FALSE;
          columnOffsetData.isNormalized = TRUE; //an empty string needs no unicode normalization
          columnOffsetData.source = &(query->scratchpad);
        }

        //put the values retrieved into each of the columns in the output match
        for(
            currentResultColumn = currentInputColumn->firstResultColumn;
            currentResultColumn != NULL;
            currentResultColumn = currentResultColumn->nextColumnInstance
          ) {
            
          memcpy(
              &(match[currentResultColumn->resultColumnIndex]),
              &columnOffsetData,
              sizeof(struct resultColumnValue)
            );
        }
      }
      //end of columns

      //consume any remaining column data that may exist in this record
      if(recordHasColumn == TRUE) {
        while(getCsvColumn(&(currentInputTable->fileStream), NULL, NULL, NULL, NULL)) {
          //do nothing
        }
      }

      //if the current table was left joined and no matching record was
      //output then output a record containing empty strings for each column
      //in the table
      if(currentInputTable->isLeftJoined && currentInputTable->noLeftRecord) {
        //set the fields in this file to empty string then go to the next table
      }

      //TRUE means the record was rejected. FALSE means the record hasn't yet been rejected
      if(walkRejectRecord(
          currentInputTable->fileIndex,
          query->joinsAndWhereClause,
          match
        )) {
        //go to next record
        continue;
      }
      else if(currentInputTable->nextInputTable == NULL) {
        currentInputTable->noLeftRecord = FALSE;

        //do calculated columns
        getCalculatedColumns(query, match, FALSE);

        if(!walkRejectRecord(
          currentInputTable->fileIndex+1, //+1 means all tables and *CALCULATED* columns
          query->joinsAndWhereClause,
          match
        )) {
          return TRUE;
        }
      }
      else {
        currentInputTable->noLeftRecord = FALSE;

        //more columns still to check in the next file
        currentInputTable = query->secondaryInputTable = currentInputTable->nextInputTable;
      }
    }
    //end of records
    
    //rewind the file, but skip the column headers line
    fseek(currentInputTable->fileStream, currentInputTable->firstRecordOffset, SEEK_SET);
    currentInputTable->noLeftRecord = TRUE;

    //go back up the list of tables.
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
  //end of tables
  
  //all data scanned. no more matches
  return FALSE;
}

void cleanup_atomList(struct atomEntry * currentAtom) {
  struct atomEntry* next;
  while(currentAtom != NULL) {
    next = currentAtom->nextInList;

    free(currentAtom->content);
    free(currentAtom);

    currentAtom = next;
  }
}

void cleanup_expression(struct expression * currentExpression) {
  if(currentExpression != NULL) {
    switch(currentExpression->type) {
      case EXP_IN:
      case EXP_NOTIN:
        cleanup_expression(currentExpression->unionPtrs.inLeaves.leftPtr);
        cleanup_atomList(currentExpression->unionPtrs.inLeaves.lastEntryPtr);
      break;

      case EXP_COLUMN:
        //the memory used by the input column will be freed elsewhere
      break;

      case EXP_GROUP:
        //the memory used here is cleaned up elsewhere
      break;

      case EXP_LITERAL:
        free(currentExpression->unionPtrs.voidPtr);
      break;

      case EXP_CALCULATED:
        //TODO: confirm that the expression will be freed elsewhere
      break;

      case EXP_UMINUS:
      case EXP_UPLUS:
        cleanup_expression(currentExpression->unionPtrs.leaves.leftPtr);
      break;

      default:
        cleanup_expression(currentExpression->unionPtrs.leaves.leftPtr);
        cleanup_expression(currentExpression->unionPtrs.leaves.rightPtr);
      break;
    }

    free(currentExpression);
  }
}

void cleanup_columnReferences(struct columnReferenceHash * table) {
  int i;
  struct columnRefHashEntry *currentHashEntry, *nextHashEntry;
  struct columnReference *currentReference, *nextReference;

  for(i=0; i<table->size; i++) {
    currentHashEntry = table->table[i];

    while(currentHashEntry != NULL) {
      nextHashEntry = currentHashEntry->nextReferenceInHash;

      free(currentHashEntry->referenceName);
      currentReference = currentHashEntry->content;
      free(currentHashEntry);
  
      while(currentReference != NULL) {
        nextReference = currentReference->nextReferenceWithName;

        if(currentReference->referenceType == REF_COLUMN) {
          free(currentReference->reference.columnPtr);
        }

        if(currentReference->referenceType == REF_EXPRESSION) {
          cleanup_expression(currentReference->reference.calculatedPtr.expressionPtr);
        }

        free(currentReference);

        currentReference = nextReference;
      }

      currentHashEntry = nextHashEntry;
    }
  }

  free(table->table);
  free(table);
}

void cleanup_orderByClause(struct sortingList* currentSortingList) {
  struct sortingList *next;

  while(currentSortingList != NULL) {
    next = currentSortingList->nextInList;

    cleanup_expression(currentSortingList->expressionPtr);

    free(currentSortingList);
    
    currentSortingList = next;
  }
}

void cleanup_resultColumns(struct resultColumn * currentResultColumn) {
  struct resultColumn* next;
  while(currentResultColumn != NULL) {
    next = currentResultColumn->nextColumnInResults;

    free(currentResultColumn->resultColumnName);
    free(currentResultColumn);

    currentResultColumn = next;
  }
}

void cleanup_inputColumns(struct inputColumn * currentInputColumn) {
  struct inputColumn* next;
  while(currentInputColumn != NULL) {
    next = currentInputColumn->nextColumnInTable;

    //free(currentInputColumn->fileColumnName); will be done by the column references instead
    free(currentInputColumn);

    currentInputColumn = next;
  }
}

void cleanup_inputTables(struct inputTable * currentInputTable) {
  struct inputTable* next;
  while(currentInputTable != NULL) {
    next = currentInputTable->nextInputTable;

    free(currentInputTable->queryTableName);
    fclose(currentInputTable->fileStream);
    free(currentInputTable);

    currentInputTable = next;
  }
}

void cleanup_query(struct qryData * query) {
  cleanup_columnReferences(query->columnReferenceHashTable);

  cleanup_resultColumns(query->firstResultColumn);

  cleanup_orderByClause(query->groupByClause);

  cleanup_orderByClause(query->orderByClause);

  cleanup_expression(query->joinsAndWhereClause);

  cleanup_inputTables(query->firstInputTable);

  free(query->intoFileName);

  //close the open files
  fclose(query->scratchpad);
  unlink(query->scratchpadName);
  free(query->scratchpadName);
}

//compares two whole records to one another. multiple columns can be involved in this comparison.
int recordCompare(const void * a, const void * b, void * c) {
  struct sortingList* orderByClause;
  char* string1, *string2, *output1, *output2;
  int compare;

  for(
      orderByClause = (struct sortingList*)c;
      orderByClause != NULL;
      orderByClause = orderByClause->nextInList
    ) {

    //get the value of the expression using the values in record a
    getValue(
        orderByClause->expressionPtr,
        (struct resultColumnValue*)a
      );
    string1 = output1 = orderByClause->expressionPtr->value;
    orderByClause->expressionPtr->value = NULL;

    //get the value of the expression using the values in record b
    getValue(
        orderByClause->expressionPtr,
        (struct resultColumnValue*)b
      );
    string2 = output2 = orderByClause->expressionPtr->value;
    orderByClause->expressionPtr->value = NULL;

    //do the comparison of the two current expression values
    compare = strCompare(
        &output1,
        &output2,
        orderByClause->expressionPtr->caseSensitive,
        (void (*)())getUnicodeChar,
        (void (*)())getUnicodeChar
      );

    //clean up used memory. The string1 & string2 pointers might be made
    //stale (and freed automatically) by unicode NFD normalisation in
    //strCompare function
    strFree(&output1);
    strFree(&output2);

    // if the fields didn't compare as being the same, then return which was greater
    if(compare != 0) {
      return orderByClause->isDescending?-compare:compare;
    }
  }

  //all fields to compare compared equally
  return 0;
}

void updateRunningCounts(struct qryData * query, struct resultColumnValue * match) {
  struct columnRefHashEntry* currentHashEntry;
  struct columnReference* currentReference;
  struct resultColumn* currentResultColumn;
  char* tempString = NULL;
  char* tempString2 = NULL;

  int i, j;

  query->groupCount++;

  //for each column in the output result set ...
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {
      
      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        //... check if the current column in the result set is a grouped one, and increment/set the group variables in the appropriate way
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
            currentResultColumn->groupType != GRP_NONE
          ) {
          stringGet(&tempString, &(match[currentResultColumn->resultColumnIndex]));

          switch(currentResultColumn->groupType) {
            case GRP_COUNT:
              if(query->groupCount > 1) {
                for(j = 1; j < query->groupCount; j++) {
                  stringGet(&tempString2, &(match[(currentResultColumn->resultColumnIndex) - (query->columnCount)]));

                  if(strCompare(
                    &tempString,
                    &tempString2,
                    TRUE,
                    (void (*)())getUnicodeChar,
                    (void (*)())getUnicodeChar
                  ) == 0) {
                    strFree(&tempString2);
                    break;
                  }

                  strFree(&tempString2);
                }
                if(j == query->groupCount) {
                  currentResultColumn->groupNum++;
                }
              }
              else {
                currentResultColumn->groupNum++;
              }
            break;

            case GRP_AVG:
            case GRP_SUM:
              currentResultColumn->groupNum += strtod(tempString, NULL);
            break;

            case GRP_CONCAT:
              snprintf_d(
                &(currentResultColumn->groupText),
                "%s%s",
                currentResultColumn->groupText,
                tempString
              );
            break;

            case GRP_MIN:
              if(currentResultColumn->groupText == NULL || strCompare(
                  &tempString,
                  &(currentResultColumn->groupText),
                  TRUE,
                  (void (*)())getUnicodeChar,
                  (void (*)())getUnicodeChar
                ) == -1) {

                free(currentResultColumn->groupText);
                currentResultColumn->groupText = tempString;
                tempString = NULL;
                currentReference = currentReference->nextReferenceWithName;
                continue;
              }
            break;
              
            case GRP_MAX:
              if(currentResultColumn->groupText == NULL || strCompare(
                  &tempString,
                  &(currentResultColumn->groupText),
                  TRUE,
                  (void (*)())getUnicodeChar,
                  (void (*)())getUnicodeChar
                ) == 1) {

                free(currentResultColumn->groupText);
                currentResultColumn->groupText = tempString;
                tempString = NULL;
                currentReference = currentReference->nextReferenceWithName;
                continue;
              }
            break;
          }

          strFree(&tempString);
        }
        
        currentReference = currentReference->nextReferenceWithName;
      }

      //go to the next reference in the hash table
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}

void getGroupedColumns(struct qryData * query) {
  struct columnRefHashEntry* currentHashEntry;
  struct columnReference* currentReference;
  struct resultColumn* currentResultColumn;

  int i;

  //for each column in the output result set ...
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {
      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        //... check if the current column in the result set is a grouped one, and increment/set the group variables in the appropriate way
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
            currentResultColumn->groupType != GRP_NONE
          ) {

          //convert the aggregation types that need it back into a string
          switch(currentResultColumn->groupType) {
            case GRP_AVG:
              currentResultColumn->groupNum = currentResultColumn->groupNum/query->groupCount;
            case GRP_COUNT:
            case GRP_SUM:
              snprintf_d(&(currentResultColumn->groupText), "%g", currentResultColumn->groupNum);
            break;
          }
        }
        
        currentReference = currentReference->nextReferenceWithName;
      }

      //go to the next reference in the hash table
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}

void cleanup_groupedColumns(
    struct qryData * query
  ) {
  struct columnRefHashEntry* currentHashEntry;
  struct columnReference* currentReference;
  struct resultColumn* currentResultColumn;

  int i;

  query->groupCount = 0;

  //for each column in the output result set ...
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {
      
      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        //... check if the current column in the result set is a grouped one, and increment/set the group variables in the appropriate way
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
            currentResultColumn->groupType != GRP_NONE
          ) {
          strFree(&(currentResultColumn->groupText));
          currentResultColumn->groupNum = 0;
        }
        
        currentReference = currentReference->nextReferenceWithName;
      }

      //go to the next reference in the hash table
      currentHashEntry = currentHashEntry->nextReferenceInHash;
    }
  }
}

void groupResults(struct qryData * query, struct resultSet * results) {
  struct resultSet resultsOrig;
  struct resultColumnValue * match;
  struct resultColumnValue * previousMatch;

  int i, len;

  //backup the original result set
  memcpy(&resultsOrig, results, sizeof(struct resultSet));

  //empty the result set
  results->recordCount = 0;
  results->records = NULL;

 
  //sort the records according to the group by clause
  if(query->groupByClause != NULL) {
    qsort_s(
      (void*)resultsOrig.records,
      resultsOrig.recordCount,
      (query->columnCount)*sizeof(struct resultColumnValue),
      recordCompare,
      (void*)query->groupByClause
    );
  }
 
  //keep a reference to the current record 
  match = resultsOrig.records;

  //store a copy of the first record and initialise the running totals
  updateRunningCounts(query, match);

  //loop over each record in the result set, other than the first one
  for (i = 1, len = resultsOrig.recordCount; i <= len; i++) {
    previousMatch = match;
    match = &(resultsOrig.records[i*query->columnCount]);

    //if the current record to look at is identical to the previous one
    if(
        i == len ||
        query->groupByClause == NULL ||   //if no group by clause then every record is its own group
        recordCompare(
          (void *)previousMatch,
          (void *)match,
          (void*)query->groupByClause
        ) != 0
      ) {
      //fix up the calculated columns that need it
      getGroupedColumns(query);
      
      //calculate remaining columns that make use of aggregation
      getCalculatedColumns(query, previousMatch, TRUE);

      //free the group text strings (to prevent heap fragmentation)
      cleanup_groupedColumns(query);

      //append the record to the new result set
      appendToResultSet(query, previousMatch, results);
    }
    
    if(i < len) {
      updateRunningCounts(query, match);
    }
  }
  
  //free the old result set
  free(resultsOrig.records);
}

int needsEscaping(char* str) { 
  if(*str) {
    if(*str == ' ' || *str == '\t') {
      return TRUE;
    }
       
    while(*str) {
      if(*str == '"' || *str == '\n' || *str == ',') {
      return TRUE;
      }
      str++;
    }

    str--;
       
    if(*str == ' ' || *str == '\t') {
      return TRUE;
    }
  }
  
  return FALSE;
}

int outputResults(struct qryData * query, struct resultSet * results) {
  int recordsOutput, i, j, len, firstColumn = TRUE;
  FILE * outputFile = NULL;
  struct resultColumn* currentResultColumn;
  char *string = NULL;
  char *string2 = NULL;
  
  if(query->intoFileName) {
    outputFile = fopen(query->intoFileName, "w");
    if (outputFile == NULL) {
      fputs ("opening output file failed", stderr);
      return -1;
    }
  }
  else {
    outputFile = stdout;
  }

  firstColumn = TRUE;

  //write column headers to the output file
  for(
      currentResultColumn = query->firstResultColumn;
      currentResultColumn != NULL;
      currentResultColumn = currentResultColumn->nextColumnInResults
    ) {

    if(currentResultColumn->isHidden == FALSE) {
      if (!firstColumn) {
        fputs(", ", outputFile); 
      }
      else {
        firstColumn = FALSE;
      }

      //strip over the leading underscore
      fputs((currentResultColumn->resultColumnName)+1, outputFile);
    }
  }
  
  fputs("\n", outputFile);
  
  //dereference the string offsets and lengths and write
  //to the output file. remove any that were there just for sorting purposes
  
  //for records
  for (i = 0, len = results->recordCount; i != len; i++) {
    firstColumn=TRUE;

    //for output columns
    j=0;
    for(
        currentResultColumn = query->firstResultColumn;
        currentResultColumn != NULL;
        currentResultColumn = currentResultColumn->nextColumnInResults
      ) {

      if(currentResultColumn->isHidden == FALSE) {       
        if (!firstColumn) {
          fputs(", ", outputFile);
        }
        else {
          firstColumn = FALSE;
        }

        stringGet(&string, &(results->records[(i*(query->columnCount))+j]));

        //need to properly re-escape fields that need it
        if(needsEscaping(string)) {
          string2 = strReplace("\"","\"\"", string);
          fputs("\"", outputFile);
          fputs(string2, outputFile);
          fputs("\"", outputFile);
          strFree(&string2);
        }
        else {		
          fputs(string, outputFile);
        }
      }

      j++;
    }

    fputs("\n", outputFile);
  }

  //close the output file
  if(!(query->intoFileName)) {
    fclose(outputFile);
    recordsOutput = results->recordCount;
  }
  else {
    recordsOutput = results->recordCount;
  }

  //free used memory
  strFree(&string);

  return recordsOutput;
}

int runQuery(char* queryFileName) {
  struct qryData query;
  struct resultSet results;
  struct resultColumnValue* match;
  int recordsOutput = -1;

  parseQuery(queryFileName, &query);

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
    qsort_s(
      (void*)results.records,
      results.recordCount,
      (query.columnCount)*sizeof(struct resultColumnValue),
      recordCompare,
      (void*)query.orderByClause
    ); 
  }

  //output the results to the specified file
  recordsOutput = outputResults(&query, &results);
  
  //free the query data structures
  cleanup_query(&query);

  //free the result set data structures
  free(results.records);

  free(match);
  
  //output the number of records returned iff there was an into clause specified 
  if(recordsOutput != -1) {
    fprintf(stdout, "%d", recordsOutput);
  }
  
  return 0;
}

int getColumnCount(char* inputFileName) {
  FILE * inputFile = NULL;
  int columnCount = 1;
  
  //attempt to open the input file
  inputFile = fopen(inputFileName, "rb");
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return -1;
  }

  //read csv columns until end of line occurs
  while(getCsvColumn(&inputFile, NULL, NULL, NULL, NULL)) {
    columnCount++;
  }

  //output the number of columns we counted
  fprintf(stdout, "%d", columnCount);

  //close the input file and return
  fclose(inputFile);
  return 0;
}

int getNextRecordOffset(char* inputFileName, long offset) {
  FILE * inputFile = NULL;
  
  //attempt to open the input file
  inputFile = fopen(inputFileName, "rb");
  if(inputFile == NULL) {
    fputs("5\n"/*TDB_COULDNT_OPEN_INPUT*/, stderr);
    return -1;
  }

  //seek to offset
  if(fseek(inputFile, offset, SEEK_SET) != 0) {
    fputs(TDB_COULDNT_SEEK, stderr);
    return -1;
  }

  //read csv columns until end of line occurs
  while(getCsvColumn(&inputFile, NULL, NULL, NULL, NULL)) {
    //do nothing
  }

  //get current file position
  fprintf(stdout, "%ld", ftell(inputFile));
  
  //close the input file and return
  fclose(inputFile);
  return 0;
}

int getColumnValue(char* inputFileName, long offset, int columnIndex) {
  FILE * inputFile = NULL;
  char* output = (char*)malloc(1);
  size_t strSize = 0;
  int currentColumn = 0;

  //attempt to open the input file
  inputFile = fopen(inputFileName, "rb");
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    strFree(&output);
    return -1;
  }

  //seek to offset
  if(fseek(inputFile, offset, SEEK_SET) != 0) {
    fputs(TDB_COULDNT_SEEK, stderr);
    strFree(&output);
    return -1;
  }

  //get the text of the specified csv column (if available).
  //if it's not available we'll return an empty string 
  while(
        ++currentColumn != columnIndex ?
        getCsvColumn(&inputFile, NULL, NULL, NULL, NULL):
        (getCsvColumn(&inputFile, &output, &strSize, NULL, NULL) && FALSE)
      ) {
    //get next column
  }

  //output the value
  fputs(output, stdout);

  //free the string memory
  strFree(&output);

  //close the input file and return
  fclose(inputFile);
  return 0;
}

int getCurrentDate() {
  time_t now;
  struct tm local;

  char *output = NULL;

  //get unix epoch seconds
  time(&now);

  //get localtime tm object and utc offset string
  //(we don't want to keep gmtime though as the users clock
  // will probably be set relative to localtime)
  if(tztime_d(&now, &local, NULL, &output) == FALSE) {
    fputs(TDB_TZTIMED_FAILED, stderr);

    strFree(&output);

    return -1;
  };

  //place the utc offset in the output string.
  //%z unfortunately can't be used as it doesn't work properly
  //in some c library implementations (Watcom and MSVC)
  if(snprintf_d(&output, "%%Y-%%m-%%dT%%H:%%M:%%S%s", output) == FALSE) {
    fputs(TDB_SNPRINTFD_FAILED, stderr);

    strFree(&output);

    return -1;
  };
  
  //place the rest of the time data in the output string
  if(strftime_d(&output, output, &local) == FALSE) {
    fputs(TDB_STRFTIMED_FAILED, stderr);

    strFree(&output);

    return -1;
  }

  //print the timestamp
  fputs(output, stdout);

  //free the string data
  strFree(&output);

  //quit
  return 0;
}

int main(int argc, char* argv[]) {
  //supply a default temporary folder if none is present
  if(getenv(TEMP_VAR) == NULL) {
    putenv(DEFAULT_TEMP);
  }

  //supply some default timezone data if none is present
  if(getenv("TZ") == NULL) {
    putenv(TDB_DEFAULT_TZ);
  }

  //set the locale (among other things, this applies the
  //timezone data to the date functions)
  setlocale(LC_ALL, TDB_LOCALE);

  //identify which subcommand to run, using a very simple setup
  if(argc > 1) {
    //run a query
    if(strcmp(argv[1],"--run") == 0 && argc == 3) {
      return runQuery(argv[2]);
    }

    //get the number of columns in a file
    else if (strcmp(argv[1], "--columns") == 0 && argc == 3) {
      return getColumnCount(argv[2]);
    }
    
    //get the file offset of the start of the next record in a file
    else if (strcmp(argv[1], "--next") == 0 && argc == 4) {
      return getNextRecordOffset(argv[2], atol(argv[3]));
    }
    
    //get the unescaped value of column X of the record starting at the file offset
    else if (strcmp(argv[1], "--value") == 0 && argc == 5) {
      return getColumnValue(argv[2], atol(argv[3]), atoi(argv[4]));
    }

    //get the current date in ISO8601 format (local time with UTC offset)
    else if (strcmp(argv[1], "--date") == 0 && argc == 2) {
      return getCurrentDate();
    }
  }
  
  //something else. print an error message and quit
  fputs(TDB_INVALID_COMMAND_LINE, stderr);
  return -1;
}
