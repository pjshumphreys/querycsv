/*
  this program enables the user to run a sql like select statement stored in a file that can retrieve data from multiple csv files and generate another csv file of the results.

  There are also a bunch of additional utility features that simplify using csv files
  from msdos batch file scripts, and one that can retrieve the current date in ISO8601 format
*/

#include "sql.h"
#include "lexer.h"
//#include "querycsv.h"

/*skips the BOM if present from a file */
FILE * fopen_skipBom(const char * filename) {
  FILE * file;
  int c, c2, c3;

  file = fopen(filename, "rb");

  if (file != NULL) {
    if((c = fgetc(file)) == 239) {
      if((c2 = fgetc(file)) == 187) {
        if((c3 = fgetc(file)) == 191) {
          return file; 
        }
        
        ungetc(c3, file);
      }

      ungetc(c2, file);
    }

    ungetc(c, file);
  }

  return file;
}

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

    sprintf(*output, format, hour_difference, minute_difference);
  }
  
  if(local == NULL) {
    free((void*)lcl);
  }

  if(utc == NULL) {
    free((void*)gm);
  }

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

//write a formatted string into a string buffer. allocate/free memory as needed
int sprintf_d(char** str, char* format, ...) {
  FILE * pFile;
  size_t newSize;
  char* newStr = NULL;
  va_list args;
  
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
  if((newStr = (char*)malloc(newSize*sizeof(char))) == NULL) {
    return FALSE;
  }

  //do the string formatting for real. vsnprintf doesn't seem to be available on Lattice C
  va_start(args, format);
  vsprintf(newStr, format, args);
  va_end(args);

  //ensure null termination of the string
  newStr[newSize] = '\0';

  //free the old contents of the output if present
  free(*str);

  //set the output pointer to the new pointer location
  *str = newStr;
  
  //everything occurred successfully
  return newSize;
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
int strRTrim(char** value, size_t* strSize, char* minSize) {
  char* end;
  char* str;
  int size;

  if(value == NULL || strSize == NULL || *value == NULL) {
    return FALSE;
  }

  str = *value;
  size = *strSize;

  end = str + size - 1;
  while(end > str && end != minSize && (*end == ' ' || *end == '\t')) {
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
    long* startPosition,
    int doTrim
  ) {
  int c;
  char *tempString = NULL;
  int canEnd = TRUE;
  int quotePossible = TRUE;
  int exitCode = 0;
  char* minSize = NULL;

  if(quotedValue != NULL) {
    *quotedValue = FALSE;
  }

  if(strSize != NULL) {
    *strSize = 0;
  }

  if(value == NULL) {
    value = &tempString;
  }

  if(startPosition != NULL) {
    *startPosition = ftell(*inputFile);
  }

  if(feof(*inputFile) != 0) {
    return FALSE;
  }

  //read a character
  do {
    c = fgetc(*inputFile);

    switch(c) {
      case ' ':
        if(!canEnd) {
          minSize = &((*value)[*strSize]);
        }
        strAppend(' ', value, strSize);
      break;

      case '\r':
        if((c = fgetc(*inputFile)) != '\n' && c != EOF) {
          ungetc(c, *inputFile);
        }
        else if (c == EOF) {
          exitCode = 2;
          break;
        }

      case '\n':
        if(canEnd) {
          exitCode = 2;
          break;
        }
        else {
          if (quotedValue != NULL) {
            *quotedValue = TRUE;
          }
          strAppend('\r', value, strSize);
          strAppend('\n', value, strSize);
        }
      break;

      case '\0':
        if (quotedValue != NULL) {
          *quotedValue = TRUE;
        }
      break;

      case EOF:
        exitCode = 2;
        break;
      break;

      case '"':
        canEnd = FALSE;

        if (quotedValue != NULL) {
          *quotedValue = TRUE;
        }
        
        if(quotePossible) {
          if(strSize != NULL) {
            *strSize = 0;
          }
          
          quotePossible = FALSE;          
          
          if(startPosition != NULL) {
            *startPosition = ftell(*inputFile)-1;
          }
        }
        else {
          c = fgetc(*inputFile);

          switch(c) {
            case ' ':
            case '\r':
            case '\n':
            case EOF:
            case ',':
              canEnd = TRUE;
              ungetc(c, *inputFile);
            break;

            case '"':
              strAppend('"', value, strSize);
            break;

            default:
              strAppend('"', value, strSize);
              ungetc(c, *inputFile);
            break;
          }
        }
      break;

      case ',':
        if(canEnd) {
          exitCode = 1;
          break;
        }

      default:
        if(doTrim && quotePossible) {
          if(strSize != NULL) {
            *strSize = 0;
          }
          
          if(startPosition != NULL) {
            *startPosition = ftell(*inputFile)-1;
          }
        }
        
        quotePossible = FALSE;
        strAppend(c, value, strSize);
      break;
    }
  } while (exitCode == 0);

  if(doTrim) {
    strRTrim(value, strSize, minSize);
  }
  
  strAppend('\0', value, strSize);

  if(strSize != NULL) {
    (*strSize)--;
  }

  free(tempString);
  return exitCode == 1;
}

void stringGet(unsigned char **str, struct resultColumnValue* field, int params) {
  long offset = ftell(*(field->source));
  
  fseek(*(field->source), field->startOffset, SEEK_SET);
  fflush(*(field->source));
  
  if (field->isQuoted) {
    //can't use a shortcut to get the string value, so get it the same way we did the last time
    getCsvColumn(field->source,str,&(field->length),NULL,NULL, (params & PRM_TRIM) == 0);
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

void parseCommandLine(char* string, int * params) {
  if(string && params) {
    while(*string) {
      switch(*string) {
        case 'p':
        case 'P':
          *params |= PRM_TRIM;
        break;

        case 't':
        case 'T':
          *params |= PRM_SPACE;        
        break;
        case 'i':
        case 'I':
          *params |= PRM_IMPORT;
        break;
        
        case 'e':
        case 'E':
          *params |= PRM_EXPORT;
        break;

        case 'b':
        case 'B':
          *params |= PRM_BOM;
        break;
      }
      string++;
    }
  }
}

void parseQuery(char* queryFileName, struct qryData * query) {
  FILE * queryFile = NULL;
  void * scanner;
  struct inputTable* currentInputTable;
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
  if((query->scratchpad = tmpfile()) == NULL) {
    fclose(queryFile);
    fputs("Couldn't create scratchpad file", stderr);
    exit(EXIT_FAILURE);
  }

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
  query->params = 0;

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
    sprintf_d(value, "%g", leftVal/rightVal);
  }
}

void exp_uminus(char** value, double leftVal) {
  if(leftVal == 0.0) {
    *value = strdup("0");
  }
  else {
    sprintf_d(value, "%g", leftVal*-1);
  }
}

//getValue can be passed a scalar expression's abstract syntax
//tree and it will evaluate it and turn it into a literal string of text characters.
//the evaluated value must be freed later though.
void getValue(
    struct expression* expressionPtr,
    struct resultColumnParam * match
  ) {
  struct expression * calculatedField;
  struct resultColumn * column;
  struct resultColumnValue * field;

  expressionPtr->leftNull = FALSE;

  switch(expressionPtr->type) {
    case EXP_COLUMN: {
      //get the value of the first instance in the result set of
      //this input column (it should have just been filled out with a
      //value for the current record)

      field = &(match->ptr[
          ((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->
          firstResultColumn->resultColumnIndex
        ]);
      
      if(field->leftNull) {
        expressionPtr->leftNull = TRUE;
        expressionPtr->value = strdup("");
      }
      else {
        stringGet(&(expressionPtr->value), field, match->params);
      }
    } break;

    case EXP_LITERAL: {
      expressionPtr->value = strdup((char *)expressionPtr->unionPtrs.voidPtr);
    } break;

    case EXP_CALCULATED: {
      calculatedField = ((struct expression*)(expressionPtr->unionPtrs.voidPtr));

      getValue(calculatedField, match);

      expressionPtr->leftNull = calculatedField->leftNull;
      expressionPtr->value = strdup(calculatedField->value);

      strFree(&(calculatedField->value));
    } break;

    case EXP_GROUP: {
      column = (struct resultColumn *)(expressionPtr->unionPtrs.voidPtr);
      if(column->groupingDone) {
        field = &(match->ptr[column->resultColumnIndex]);

        if(field->leftNull == FALSE) {
          stringGet(&(expressionPtr->value), field, match->params);
          break;
        }
      }
      else if(column->groupText != NULL) {
        expressionPtr->value = strdup(column->groupText);
        break;
      }

      expressionPtr->leftNull = TRUE;
      expressionPtr->value = strdup("");
    } break;

    case EXP_UPLUS: {
      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );

      if(expressionPtr->unionPtrs.leaves.leftPtr->leftNull) {
        expressionPtr->leftNull = TRUE;
        expressionPtr->value = strdup("");
      }
      else {
        sprintf_d(
            &(expressionPtr->value),
            "%g",
            strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)
          );
      }
      
      strFree(&(expressionPtr->unionPtrs.leaves.leftPtr->value));
    } break;

    case EXP_UMINUS: {
      getValue(
          expressionPtr->unionPtrs.leaves.leftPtr,
          match
        );
        
      if(expressionPtr->unionPtrs.leaves.leftPtr->leftNull) {
        expressionPtr->leftNull = TRUE;
        expressionPtr->value = strdup("");
      }
      else {
        exp_uminus(
            &(expressionPtr->value),
            strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)
          );
      }
      
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

      if(
          expressionPtr->unionPtrs.leaves.leftPtr->leftNull ||
          expressionPtr->unionPtrs.leaves.rightPtr->leftNull
        ) {
        expressionPtr->leftNull = TRUE;
        expressionPtr->value = strdup("");
      }
      else {
        switch(expressionPtr->type){
          case EXP_PLUS:
            sprintf_d(
                &(expressionPtr->value),
                "%g",
                strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)+
                strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL)
              );
          break;

          case EXP_MINUS:
            sprintf_d(
                &(expressionPtr->value),
                "%g",
                strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)-
                strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL)
              );
          break;

          case EXP_MULTIPLY:
            sprintf_d(
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
            sprintf_d(
                &(expressionPtr->value),
                "%s%s",
                expressionPtr->unionPtrs.leaves.leftPtr->value,
                expressionPtr->unionPtrs.leaves.rightPtr->value
              );
          break;
        }
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
    struct resultColumnParam * match
  ) {

  int retval,i;
  struct atomEntry * currentAtom;

  if(expressionPtr == NULL || currentTable < expressionPtr->minTable) {
    return FALSE;
  }

  if(expressionPtr->type >= EXP_EQ && expressionPtr->type <= EXP_GTE){
    getValue(expressionPtr->unionPtrs.leaves.leftPtr, match);
    getValue(expressionPtr->unionPtrs.leaves.rightPtr, match);

    if(
        expressionPtr->unionPtrs.leaves.leftPtr->leftNull ||
        expressionPtr->unionPtrs.leaves.rightPtr->leftNull
      ) {

      strFree(&(expressionPtr->unionPtrs.leaves.leftPtr->value));
      strFree(&(expressionPtr->unionPtrs.leaves.rightPtr->value));

      return FALSE;
    }
    
    retval = strCompare(
        &(expressionPtr->unionPtrs.leaves.leftPtr->value),
        &(expressionPtr->unionPtrs.leaves.rightPtr->value),
        expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive,
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

    if(expressionPtr->unionPtrs.leaves.leftPtr->leftNull) {
      strFree(&(expressionPtr->unionPtrs.leaves.leftPtr->value));
      return FALSE;
    }
    
    for(
        i = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->index,
          currentAtom = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->nextInList;
        i != currentAtom->index;
        currentAtom = currentAtom->nextInList
      ) {
      if(strCompare(
        &(expressionPtr->unionPtrs.leaves.leftPtr->value),
        &(currentAtom->content),
        expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive,
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
  struct resultColumnParam matchParams;

  matchParams.ptr = match;
  matchParams.params = query->params;

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
          getValue(currentReference->reference.calculatedPtr.expressionPtr, &matchParams);

          //store the value's length
          if(currentReference->reference.calculatedPtr.expressionPtr->leftNull) {
            match[j].length = 0;
            match[j].leftNull = TRUE;
          }
          else {
            match[j].leftNull = FALSE;
            match[j].length = strlen(currentReference->reference.calculatedPtr.expressionPtr->value);

            //write the value to the scratchpad file
            fputs(currentReference->reference.calculatedPtr.expressionPtr->value, query->scratchpad);
          }
          
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
  struct resultColumnParam matchParams;
  int recordHasColumn;
  int doLeftRecord = FALSE;

  matchParams.params = query->params;
  matchParams.ptr = match;

  //if secondaryInputTable is NULL then
  //the query hasn't returned any results yet.
  //needed as this function should continue where it left off next time
  if(query->secondaryInputTable == NULL) {
    query->secondaryInputTable = query->firstInputTable;
  }

  currentInputTable = query->secondaryInputTable;

  //get the offsets of each column in turn and confirm whether the criteria for a match is met
  ////////////////////////////////////////////////////////////////////////////////////////////

  do {  //tables
    while(
        endOfFile(currentInputTable->fileStream) ||
        (
          currentInputTable->isLeftJoined &&
          currentInputTable->noLeftRecord &&
          (doLeftRecord = TRUE)
        )
      ) {   //records

      //reset the flag that says the column values ran out
      recordHasColumn = TRUE;

      for (
          currentInputColumn = currentInputTable->firstInputColumn;
          currentInputColumn != NULL;
          currentInputColumn = currentInputColumn->nextColumnInTable
        ) {  //columns

        //if we haven't yet reached the end of a record, get the next column value.
        //if it returns false we use an empty string for the value instead

        if(recordHasColumn == TRUE && !doLeftRecord) {
          recordHasColumn = getCsvColumn(
              &(currentInputTable->fileStream),
              NULL,
              &(columnOffsetData.length),
              &(columnOffsetData.isQuoted),
              &(columnOffsetData.startOffset),
              (query->params & PRM_TRIM) == 0
            );

          //these values should actually be set depending on whether the value was quoted or not
          //if the value is quoted we should probably also NFD normalise it before writing to the scratchpad
          columnOffsetData.isNormalized = FALSE;
          columnOffsetData.source = &(currentInputTable->fileStream);
          columnOffsetData.leftNull = FALSE;
        }

        //construct an empty column reference.
        else {
          columnOffsetData.leftNull = doLeftRecord;
          columnOffsetData.startOffset = 0;
          columnOffsetData.length = 0;
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
      if(recordHasColumn == TRUE && !doLeftRecord) {
        while(getCsvColumn(&(currentInputTable->fileStream), NULL, NULL, NULL, NULL, (query->params & PRM_TRIM) == 0)) {
          //do nothing
        }
      }

      //TRUE means the record was rejected. FALSE means the record hasn't yet been rejected 
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
          //go to next record
          continue;
        }
      }
      else if(currentInputTable->nextInputTable == NULL) {
        //there was a record match for this table
        currentInputTable->noLeftRecord = FALSE;

        //mark every record in every table as having a match,
        //even if using a special left join one
        currentInputTable = query->firstInputTable;
        
        while((currentInputTable) != (query->secondaryInputTable)) {
          currentInputTable->noLeftRecord = FALSE;
          currentInputTable = currentInputTable->nextInputTable;
        }

        currentInputTable = query->secondaryInputTable;

        //do calculated columns
        getCalculatedColumns(query, match, FALSE);

        if(!walkRejectRecord(
          currentInputTable->fileIndex+1, //+1 means all tables and *CALCULATED* columns
          query->joinsAndWhereClause,
          &matchParams
        )) {
          return TRUE;
        }
      }
      else {
        //there are more columns still to check in the next file
        currentInputTable = query->secondaryInputTable = currentInputTable->nextInputTable;
      }

      doLeftRecord = FALSE;
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
}

//compares two whole records to one another. multiple columns can be involved in this comparison.
int recordCompare(const void * a, const void * b, void * c) {
  struct sortingList* orderByClause;
  char* string1, *string2, *output1, *output2;
  int compare;

  struct resultColumnParam matchParams;
  matchParams.params = ((struct qryData*)c)->params;
  
  for(
      orderByClause = ((struct qryData*)c)->useGroupBy?((struct qryData*)c)->groupByClause:((struct qryData*)c)->orderByClause;
      orderByClause != NULL;
      orderByClause = orderByClause->nextInList
    ) {

    //get the value of the expression using the values in record a
    matchParams.ptr = (struct resultColumnValue*)a;
    getValue(
        orderByClause->expressionPtr,
        &matchParams
      );
    string1 = output1 = orderByClause->expressionPtr->value;
    orderByClause->expressionPtr->value = NULL;

    //get the value of the expression using the values in record b
    matchParams.ptr = (struct resultColumnValue*)b;
    getValue(
        orderByClause->expressionPtr,
        &matchParams
      );
    string2 = output2 = orderByClause->expressionPtr->value;
    orderByClause->expressionPtr->value = NULL;

    //do the comparison of the two current expression values
    compare = strCompare(
        &output1,
        &output2,
        2,//orderByClause->expressionPtr->caseSensitive,
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
  struct resultColumnValue* field;
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
          field = &(match[currentResultColumn->resultColumnIndex]);

          if(field->leftNull == FALSE) {
            stringGet(&tempString, field, query->params);

            if(currentResultColumn->groupType > GRP_STAR) {
              if(query->groupCount > 1) {
                for(j = 1; j < query->groupCount; j++) {
                  stringGet(&tempString2, &(match[(currentResultColumn->resultColumnIndex) - (query->columnCount)]), query->params);

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
              }
              else {
                j = query->groupCount;
              }
            }

            switch(currentResultColumn->groupType) {
              case GRP_DIS_COUNT:
                if(j == query->groupCount) {
                  currentResultColumn->groupCount++;
                }
              break;

              case GRP_COUNT:
                currentResultColumn->groupCount++;
              break;

              case GRP_DIS_AVG:
              case GRP_DIS_SUM:
                if(j == query->groupCount) {
                  currentResultColumn->groupCount++;
                  currentResultColumn->groupNum += strtod(tempString, NULL);
                }
              break;

              case GRP_AVG:
              case GRP_SUM:
                currentResultColumn->groupCount++;
                currentResultColumn->groupNum += strtod(tempString, NULL);
              break;

              case GRP_DIS_CONCAT:
                if(j == query->groupCount) {
                  sprintf_d(
                      &(currentResultColumn->groupText),
                      "%s%s",
                      currentResultColumn->groupText,
                      tempString
                    );
                }
              break;

              case GRP_CONCAT:
                sprintf_d(
                    &(currentResultColumn->groupText),
                    "%s%s",
                    currentResultColumn->groupText,
                    tempString
                  );
              break;

              case GRP_MIN:
              case GRP_DIS_MIN:
                if(currentResultColumn->groupText == NULL || strCompare(
                    &tempString,
                    &(currentResultColumn->groupText),
                    2,//TRUE,
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
              case GRP_DIS_MAX:
                if(currentResultColumn->groupText == NULL || strCompare(
                    &tempString,
                    &(currentResultColumn->groupText),
                    2,//TRUE,
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
            case GRP_DIS_AVG:
              currentResultColumn->groupNum = currentResultColumn->groupNum /
                currentResultColumn->groupCount;
            case GRP_SUM:
            case GRP_DIS_SUM:
              sprintf_d(&(currentResultColumn->groupText), "%g", currentResultColumn->groupNum);
            break;
            case GRP_COUNT:
            case GRP_DIS_COUNT:
              sprintf_d(&(currentResultColumn->groupText), "%d", currentResultColumn->groupCount);
            break;
            case GRP_STAR:
              sprintf_d(&(currentResultColumn->groupText), "%d", query->groupCount);
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
    struct qryData * query,
    struct resultColumnValue * match
  ) {
  struct columnRefHashEntry* currentHashEntry;
  struct columnReference* currentReference;
  struct resultColumn* currentResultColumn;

  int i,j;

  query->groupCount = 0;

  //for each column in the output result set ...
  for(i = 0; i < query->columnReferenceHashTable->size; i++) {
    currentHashEntry = query->columnReferenceHashTable->table[i];

    while(currentHashEntry != NULL) {
      
      currentReference = currentHashEntry->content;

      while(currentReference != NULL) {
        //... check if the current column in the result set is a grouped one
        if(
            currentReference->referenceType == REF_EXPRESSION &&
            (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
            currentResultColumn->groupType != GRP_NONE
          ) {

          //write groupText value into the scratchpad,
          //so we can order by aggregate columns later
          ////////////////////////////////////////////
          
          //start setting column value fields
          j = currentResultColumn->resultColumnIndex;
          match[j].isQuoted = FALSE;
          match[j].isNormalized = FALSE;
          match[j].source = &(query->scratchpad);

          //seek to the end of the scratchpad file and update the start position
          fseek(query->scratchpad, 0, SEEK_END);
          fflush(query->scratchpad);
          match[j].startOffset = ftell(query->scratchpad);

          //store the value's length
          if(currentResultColumn->groupText == NULL) {
            match[j].length = 0;
            match[j].leftNull = TRUE;
          }
          else {
            match[j].leftNull = FALSE;
            match[j].length = strlen(currentResultColumn->groupText);

            //write the value to the scratchpad file
            fputs(currentResultColumn->groupText, query->scratchpad);
          }
          
          strFree(&(currentResultColumn->groupText));
          currentResultColumn->groupNum = 0;
          currentResultColumn->groupCount = 0;
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
  struct resultColumn * currentResultColumn;
  
  int i, len;

  //backup the original result set
  memcpy(&resultsOrig, results, sizeof(struct resultSet));

  //empty the result set
  results->recordCount = 0;
  results->records = NULL;

  //sort the records according to the group by clause
  if(query->groupByClause != NULL) {
    query->useGroupBy = TRUE;
    qsort_s(
      (void*)resultsOrig.records,
      resultsOrig.recordCount,
      (query->columnCount)*sizeof(struct resultColumnValue),
      recordCompare,
      (void*)query
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
    query->useGroupBy = TRUE;
    if(
        (query->groupByClause != NULL &&   //if no group by clause then every record is part of one group
        recordCompare(
          (void *)previousMatch,
          (void *)match,
          (void *)query
        ) != 0) ||
        i == len
      ) {
      //fix up the calculated columns that need it
      getGroupedColumns(query);
      
      //calculate remaining columns that make use of aggregation
      getCalculatedColumns(query, previousMatch, TRUE);

      //free the group text strings (to prevent heap fragmentation)
      cleanup_groupedColumns(query, previousMatch);

      //append the record to the new result set
      appendToResultSet(query, previousMatch, results);
    }
    
    if(i < len) {
      updateRunningCounts(query, match);
    }
  }
  
  //free the old result set
  free(resultsOrig.records);

  currentResultColumn = query->firstResultColumn;
  while(currentResultColumn != NULL) {
    currentResultColumn->groupingDone = TRUE;
    currentResultColumn = currentResultColumn->nextColumnInResults;
  }
}

int needsEscaping(char* str, int params) {
  if(
    str == NULL ||
    strcmp(str, (((params & PRM_EXPORT) == 0)?"\\N":"\\N")) == 0 ||
    *str == ' ' ||
    *str == '\t') {
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
  
  return FALSE;
}

int outputResults(struct qryData * query, struct resultSet * results) {
  int recordsOutput = -1, i, j, len, firstColumn = TRUE;
  FILE * outputFile = NULL;
  struct resultColumn* currentResultColumn;
  struct resultColumnValue* field;

  char *string = NULL;
  char *string2 = NULL;
  char *string3 = (((query->params) & PRM_SPACE) != 0)?",":", ";
  
  if(query->intoFileName) {
    outputFile = fopen(query->intoFileName, "wb");
    if (outputFile == NULL) {
      fputs ("opening output file failed", stderr);
      return -1;
    }
  }
  else {    
    outputFile = stdout;
  }

  if(((query->params) & PRM_BOM) != 0) {
    fputs("\xEF\xBB\xBF", outputFile);
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
        fputs(string3, outputFile); 
      }
      else {
        firstColumn = FALSE;
      }

      //strip over the leading underscore
      fputs((currentResultColumn->resultColumnName)+1, outputFile);
    }
  }
  
  fputs("\r\n", outputFile);
  
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
          fputs(string3, outputFile);
        }
        else {
          firstColumn = FALSE;
        }
        
        field = &(results->records[(i*(query->columnCount))+j]);

        switch(field->leftNull) {
          case TRUE:
            if(((query->params) & PRM_EXPORT) != 0) {
              fputs("\\N", outputFile);
            }
          break;
          
          default:
            stringGet(&string, field, query->params);

            //need to properly re-escape fields that need it
            if(*string == '\0') {
              fputs("\"\"", outputFile);  //empty string always needs escaping
            }
            else if(needsEscaping(string, query->params)) {
              string2 = strReplace("\"","\"\"", string);
              fputs("\"", outputFile);
              fputs(string2, outputFile);
              fputs("\"", outputFile);
              strFree(&string2);
            }
            else {		
              fputs(string, outputFile);
            }
          break;
        }
      }

      j++;
    }

    fputs("\r\n", outputFile);
  }

  //close the output file
  if(query->intoFileName) {
    fclose(outputFile);
    recordsOutput = results->recordCount;
  }

  //free used memory
  strFree(&string);

  return recordsOutput;
}

int runQuery(char* queryFileName, char *params) {
  struct qryData query;
  struct resultSet results;
  struct resultColumnValue* match;
  int recordsOutput = -1;
  
  parseQuery(queryFileName, &query);

  parseCommandLine(params, &(query.params));

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

int getColumnCount(char* inputFileName) {
  FILE * inputFile = NULL;
  int columnCount = 1;
  
  //attempt to open the input file
  inputFile = fopen_skipBom(inputFileName);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return -1;
  }

  //read csv columns until end of line occurs
  while(getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE)) {
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
  inputFile = fopen_skipBom(inputFileName);
  if(inputFile == NULL) {
    fputs(TDB_COULDNT_OPEN_INPUT, stderr);
    return -1;
  }

  //seek to offset
  if(fseek(inputFile, offset, SEEK_SET) != 0) {
    fputs(TDB_COULDNT_SEEK, stderr);
    return -1;
  }

  //read csv columns until end of line occurs
  while(getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE)) {
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
  inputFile = fopen_skipBom(inputFileName);
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
        getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE):
        (getCsvColumn(&inputFile, &output, &strSize, NULL, NULL, TRUE) && FALSE)
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
  if(sprintf_d(&output, "%%Y-%%m-%%dT%%H:%%M:%%S%s", output) == FALSE) {
    fputs(TDB_SPRINTFD_FAILED, stderr);

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
  int argc2;
  char ** argv2;
    
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

  argc2 = argc;
  argv2 = argv;

  #if WINDOWS
    setupWin32(&argc2, &argv2);
  #endif

  //identify which subcommand to run, using a very simple setup
  if(argc2 > 1) {
    //get the number of columns in a file
    if (strcmp(argv2[1], "--columns") == 0 && argc2 == 3) {
      return getColumnCount(argv2[2]);
    }
    
    //get the file offset of the start of the next record in a file
    else if (strcmp(argv2[1], "--next") == 0 && argc2 == 4) {
      return getNextRecordOffset(argv2[2], atol(argv2[3]));
    }
    
    //get the unescaped value of column X of the record starting at the file offset
    else if (strcmp(argv2[1], "--value") == 0 && argc2 == 5) {
      return getColumnValue(argv2[2], atol(argv2[3]), atoi(argv2[4]));
    }

    //get the unescaped trimmed value of column X of the record starting at the file offset
    else if (strcmp(argv2[1], "--trimval") == 0 && argc2 == 5) {
      return getColumnValue(argv2[2], atol(argv2[3]), atoi(argv2[4]));
    }

    //get the current date in ISO8601 format (local time with UTC offset)
    else if (strcmp(argv2[1], "--date") == 0 && argc2 == 3) {
      if(strcmp(argv2[2], "now") == 0) {
        return getCurrentDate();
      }
      else {
        fputs("not supported", stderr);

        return -1;
      }
    }

    //run a query
    else if(argc2 == 3) {
      return runQuery(argv2[2], argv2[1]);
    }

    else if(argc2 == 2) {
      return runQuery(argv2[1], NULL);
    }
  }
  else {
    //launch interactive mode
  }
  
  //something else. print an error message and quit
  fputs(TDB_INVALID_COMMAND_LINE, stderr);

  return -1;
}
