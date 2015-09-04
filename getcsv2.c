int getCsvColumn(
    FILE ** inputFile,
    char** value,
    size_t* strSize,
    int* quotedValue,
    long* startPosition
  ) {
  int c;
  char *tempString = NULL;
  int canEnd = TRUE;
  int quotePossible = TRUE;
  int doTrim = FALSE;
  int exitCode = 0;

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
        strAppend(' ', value, strSize);
      break;

      case '\r':
        if (quotedValue != NULL) {
          *quotedValue = TRUE;
        }
        
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
        
        if(quotePossible) {
          strSize = 0;
          quotePossible = FALSE;
          
          if (quotedValue != NULL) {
            *quotedValue = TRUE;
          }
          
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
          strSize = 0;
          
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
    strRTrim(value, strSize);
  }
  
  strAppend('\0', value, strSize);

  if(strSize != NULL) {
    (*strSize)--;
  }

  free(tempString);
  return exitCode == 1;
}
