int getCsvColumn(
    struct inputTable *table,
    char **value,
    size_t *strSize,
    int *quotedValue,
    long *startPosition,
    int doTrim,
    char * newLine
) {
  long c, c2;
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

  if(feof(table->fileStream)) {
    return FALSE;
  }

  if(!(table->options & PRM_POSTGRES)) {
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
            if(quotedValue != NULL) {
              *quotedValue = TRUE;
            }

            canEnd = FALSE;
          }
          else {
            state = 4;
          }
        } break;

        case 0x2C:  /* ',' */
        case 0x3B:{ /* ';'. As used as a delimiter by european dsv files */
          if(canEnd && (((table->options & PRM_EURO) && c == 0x3B) || c == 0x2C)) {
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
  }
  else {
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
            }

            exitCode = 2;
          } break;

          case 3: {  /* found '\\' */
            if(c == 0x5C) {
              strAppend('\\', value, strSize);

              offset += byteLength;
              getNextCodepoint(table);
              c = getCurrentCodepoint(table, &byteLength);

              if(c == 0x4E && quotedValue != NULL) { /* N */
                *quotedValue = TRUE;
              }
            }
            else if (c == 0x72 || c == 0x6E) {
              nlCurrent = newLine;

              while(*nlCurrent) {
                strAppend(*nlCurrent, value, strSize);
                nlCurrent++;
              }

              c2 = c;
              offset += byteLength;
              getNextCodepoint(table);
              c = getCurrentCodepoint(table, &byteLength);

              if(c == 0x5C) {
                offset += byteLength;
                getNextCodepoint(table);
                c = getCurrentCodepoint(table, &byteLength);

                if((c2 == 0x72 && c == 0x6E) || (c2 == 0x6E && c == 0x72)) {
                  offset += byteLength;
                  getNextCodepoint(table);
                  c = getCurrentCodepoint(table, &byteLength);
                }
                else {
                  continue;
                }
              }
            }
            else {
              switch(c) {
                case 0x62:  /* 'b' */
                  strAppend('\b', value, strSize);
                break;

                case 0x66:  /* 'f' */
                  strAppend('\f', value, strSize);
                break;

                case 0x74:  /* 't' */
                  strAppend('\t', value, strSize);
                break;

                case 0x76:  /* 'v' */
                  strAppend('\v', value, strSize);
                break;

                case 0x2C:  /* ',' */
                  strAppend(',', value, strSize);

                  if(quotedValue != NULL) {
                    *quotedValue = TRUE;
                  }
                break;

                default:
                  strAppend('\\', value, strSize);
                  *strSize = strAppendUTF8(c, (unsigned char**)value, *strSize);
                break;
              }

              offset += byteLength;
              getNextCodepoint(table);
              c = getCurrentCodepoint(table, &byteLength);
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

        case 0x2C: { /* ',' */
          exitCode = 1;
        } break;

        case 0x0A: { /* '\n' */
          state = 2;
        } break;

        case 0x0D: { /* '\r' */
          state = 1;
        } break;

        case 0x5C: { /* '\\' */
          state = 3;
        }

        case 0x85:  /* EBCDIC newline */
        case MYEOF: {
          exitCode = 2;
        } break;

        default: {
          *strSize = strAppendUTF8(c, (unsigned char**)value, *strSize);
        } break;
      }

      offset += byteLength;
      getNextCodepoint(table);
    } while (exitCode == 0);
  }

  strAppend('\0', value, strSize);
  (*strSize)--;

  if(startPosition != NULL) {
    *startPosition = offset;
  }

  free(tempString);
  return exitCode == 1;
}
