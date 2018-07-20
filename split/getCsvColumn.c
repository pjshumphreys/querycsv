int getCsvColumn(
    struct inputTable *table,
    char **value,
    size_t *strSize,
    int *quotedValue,
    long *startPosition,
    int doTrim,
    char * newLine
) {
  long c;
  int byteLength;
  char *tempString = NULL;
  size_t tempSize;
  int canEnd = TRUE;
  int exitCode = 0;
  int notFoundLeft = TRUE;
  size_t minLeft = 0;
  size_t minRight = 0;
  long offset = 0;
  int state = 0;
  char *nlCurrent = NULL;

  MAC_YIELD

  if(quotedValue != NULL) {
    *quotedValue = FALSE;
  }

  if(strSize == NULL) {
    strSize = &tempSize;
  }

  *strSize = 0;

  if(value == NULL) {
    value = &tempString;
  }

  if(startPosition != NULL) {
    offset = *startPosition;
  }

  if(feof(table->fileStream) != 0) {
    return FALSE;
  }

  do {
    /* get some codepoints from the file, usually only 1 but maybe up to 4 */
    /* read a character */
    c = getCurrentCodepoint(table, &byteLength);

    if(state != 0) {
      switch(state) {
        case 1:   /* found \r */
        case 2: { /* found \n */
          if((state == 1 && c == 0x0A) || c == 0x0D) {
            offset += byteLength;
            getNextCodepoint(table);
            c = getCurrentCodepoint(table, &byteLength);
          }
        } /* fall thru */

        case 3: { /* EBCDIC newline */
          if(canEnd) {
            exitCode = 2;
          }
          else {
            if(quotedValue != NULL) {
              *quotedValue = TRUE;
            }

            if(notFoundLeft) {
              notFoundLeft = FALSE;
              minLeft = *strSize;
            }

            nlCurrent = newLine;

            while(*nlCurrent) {
              strAppend(*nlCurrent, value, strSize);
              nlCurrent++;
            }

            minRight = *strSize;
          }
        } break;

        case 4: { /* found double quote */
          if(c != 0x22) { 
            canEnd = TRUE;
          }
          else { /* and another double quote? */
            offset += byteLength;
            getNextCodepoint(table);
            c = getCurrentCodepoint(table, &byteLength);

            if(quotedValue != NULL) {
              *quotedValue = TRUE;
            }

            if(notFoundLeft) {
              notFoundLeft = FALSE;
              minLeft = *strSize;
            }

            strAppend('"', value, strSize);
            minRight = *strSize;
          }
        } break;
      }

      state = 0;

      if(exitCode) {
        break;
      }
    }

    switch(c) {
      case 0x0: { /* csv files are a plain text format, so there shouldn't
        be any null bytes. Remove any that may appear. */
      } break;

      case 0x0A: { /* '\n' */
        state = 2;
      } break;

      case 0x0D: { /* '\r' */
        state = 1;
      } break;

      case 0x20: { /* ' ' */
        /* if we're inside a double quoted string and haven't yet
        found a minimum amount to left trim to then specify it now */
        if(canEnd == FALSE) {
          if(notFoundLeft) {
            notFoundLeft = FALSE;
            minLeft = *strSize;
          }

          strAppend(' ', value, strSize);

          minRight = *strSize;
        }
        else {
          strAppend(' ', value, strSize);
        }
      } break;

      case 0x22: {  /* '"' */
        if(canEnd) {
          canEnd = FALSE;
        }
        else {
          state = 4;
        }
      } break;

      case 0x2C: {  /* ',' */
        if(canEnd) {
          exitCode = 1;
        }
        else {
          if(quotedValue != NULL) {
            *quotedValue = TRUE;
          }
          
          if(notFoundLeft) {
            notFoundLeft = FALSE;
            minLeft = *strSize;
          }

          *strSize = strAppendUTF8(c, (unsigned char**)value, *strSize);
          minRight = *strSize;
        }
      } break;

      case 0x85: { /* EBCDIC newline */
        state = 3;
      } break;

      case MYEOF: {
        exitCode = 2;
      } break;

      default: {
        if(notFoundLeft) {
          notFoundLeft = FALSE;
          minLeft = *strSize;
        }

        *strSize = strAppendUTF8(c, (unsigned char**)value, *strSize);
        minRight = *strSize;
      } break;
    }
    
    offset += byteLength;
    getNextCodepoint(table);
  } while (exitCode == 0);

  if(doTrim) {
    if(minRight < *strSize) {
      *strSize = minRight;
    }

    if(minLeft) {
      (*strSize) -= minLeft;
      memmove(*value, &((*value)[minLeft]), *strSize);
    }
  }

  strAppend('\0', value, strSize);
  (*strSize)--;

  if(startPosition != NULL) {
    *startPosition = offset;
  }

  free(tempString);
  return exitCode == 1;
}
