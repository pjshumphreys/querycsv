int getCsvColumn(
    FILE **inputFile,
    int inputEncoding,
    char **value,
    size_t *strSize,
    int *quotedValue,
    long *startPosition,
    int doTrim,
    char * newLine
) {
  long codepoints[4];
  void (*getCodepoints)(FILE *, long *, int *, int *);
  int arrLength;
  int byteLength;
  int i;
  int c2;
  long c;
  char *tempString = NULL;
  size_t tempSize;
  int canEnd = TRUE;
  int exitCode = 0;
  int notFoundLeft = TRUE;
  size_t minLeft = 0;
  size_t minRight = 0;
  long offset = 0;
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

  if(feof(*inputFile) != 0) {
    return FALSE;
  }

  /* choose which function to use based upon the character encoding */
  getCodepoints = chooseGetter(inputEncoding);

  do {
    /* get some codepoints from the file, usually only 1 but maybe up to 4 */
    getCodepoints(*inputFile,
      codepoints,
      &arrLength,
      &byteLength
    );

    offset += byteLength;

    /* for each codepoint returned treat it as we previously treated bytes */
    /* read a character */
    for(i = 0; i < arrLength && exitCode == 0; i++) {
      c = codepoints[i];

      switch(c) {
        case 0x20: { /* ' ' */
          /* if we're inside a double quoted string and haven't yet
          found a minimum amount to left trim to then specify it now */
          if(!canEnd) {

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

        case 0x0D: { /* '\r' */
          c2 = fgetc(*inputFile);
          offset++;

          if(c2 != '\n') {
            ungetc(c2, *inputFile);
            offset--;
          }

          if(canEnd) {
            exitCode = 2;
            break;
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

        case 0x0A: { /* '\n' */
          c2 = fgetc(*inputFile);
          offset++;

          if(c2 != '\r') {
            ungetc(c2, *inputFile);
            offset--;
          }

          if(canEnd) {
            exitCode = 2;
            break;
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

        case 0x85: { /* EBCDIC newline */
          if(canEnd) {
            exitCode = 2;
            break;
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

        case 0x0: { /* csv files are a plain text format, so there shouldn't
          be any null bytes. Remove any that may appear. */
        } break;

        case MYEOF: {
          exitCode = 2;
        } break;

        case 0x22: {  /* '"' */
          if(canEnd) {
            canEnd = FALSE;

            if(quotedValue != NULL) {
              *quotedValue = TRUE;
            }
          }
          else {
            c2 = fgetc(*inputFile);
            offset++;

            if(c2 != '"') {
              ungetc(c2, *inputFile);
              offset--;

              canEnd = TRUE;
            }
            else {
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
          }
        } break;

        case 0x2C: {  /* ',' */
          if(canEnd) {
            exitCode = 1;
            break;
          }

          if(quotedValue != NULL) {
            *quotedValue = TRUE;
          }
        } /* fall thru */

        default: {
          if(notFoundLeft) {
            notFoundLeft = FALSE;
            minLeft = *strSize;
          }

          *strSize = strAppendUTF8(c, (unsigned char**)value, *strSize);
          minRight = *strSize;
        } break;
      }
    }
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
