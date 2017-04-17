#include "querycsv.h"

int getCsvColumn(
    FILE **inputFile,
    int inputEncoding,
    char **value,
    size_t *strSize,
    int *quotedValue,
    long *startPosition,
    int doTrim
  ) {

  long codepoints[4];
  void (*getCodepoints)(FILE *, long *, int *, int *);
  int arrLength;
  int byteLength;

  int i;
  long c;
  int state = 0;

  char *tempString = NULL;
  int canEnd = TRUE;
  int quotePossible = TRUE;
  int exitCode = 0;
  char *minSize = NULL;
  long offset = 0;

  MAC_YIELD

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
    offset = *startPosition;
  }

  if(feof(*inputFile) != 0) {
    return FALSE;
  }

  /* choose which function to use based upon the character encoding */
  switch(inputEncoding) {
    case ENC_UTF8: {
      getCodepoints = &getCodepointsUTF8;
    } break;

    case ENC_CP437: {
      getCodepoints = &getCodepointsCP437;
    } break;

    case ENC_CP850: {
      getCodepoints = &getCodepointsCP850;
    } break;

    case ENC_CP1252: {
      getCodepoints = &getCodepointsCP1252;
    } break;

    /*
    case ENC_UTF16LE: {
      getCodepoints = &getCodepointsUTF16LE;
    } break;

    case ENC_UTF16BE: {
      getCodepoints = &getCodepointsUTF16BE;
    } break;

    case ENC_UTF32LE: {
      getCodepoints = &getCodepointsUTF32LE;
    } break;

    case ENC_UTF32BE: {
      getCodepoints = &getCodepointsUTF32BE;
    } break;
    */

    case ENC_PETSCII: {
      getCodepoints = &getCodepointsPetscii;
    } break;

    case ENC_MAC: {
      getCodepoints = &getCodepointsMac;
    } break;

    default: {
      getCodepoints = &getCodepointsUTF8;
    }
  }

  do {
    /* get some codepoints from the file, usually only 1 but maybe up to 4 */
    getCodepoints(*inputFile,
      &codepoints,
      &arrLength,
      &byteLength
    );

    offset += byteLength;

    /* for each codepoint returned treat it as we previously treated bytes */
    /* read a character */
    for (i = 0; i < arrLength && exitCode == 0; i++) {
      c = codepoints[i];

      switch(state) {
        case 1: {   /* \r detected state. look for \n */
          state = 0;  /* go back to initial state */

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

          if(c == 0x0A /* '\n' */) {  /* consume a \n character after \r */
            continue;
          }
        } /* fall thru */

        case 2: { /* test for double quote literal inside a quoted string state */
          state = 0;  /* go back to initial state */

          switch(c) {
            case 0x20:  /* ' ' */
            case 0x0D:  /* '\r' */
            case 0x0A:  /* '\n' */
            case MYEOF:
            case 0x2C: {  /* ',' */
              canEnd = TRUE;
            } break;

            case 0x22: {  /* '"' */
              strAppend('"', value, strSize);
              continue;
            } break;

            default: {
              strAppend('"', value, strSize);
            } break;
          }
        } /* fall thru */

        default: {   /* initial state */
          switch(c) {
            case 0x20: { /* ' ' */
              if(!canEnd) {
                minSize = &((*value)[*strSize]);
              }
              strAppend(' ', value, strSize);
            } break;

            case 0x0D: { /* '\r' */
              state = 1;
              continue;
            } break;

            case 0x0A: { /* '\n' */
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
            } break;

            case 0x0: {
              if (quotedValue != NULL) {
                *quotedValue = TRUE;
              }
            } break;

            case MYEOF: {
              exitCode = 2;
            } break;

            case 0x22: {  /* '"' */
              canEnd = FALSE;

              if (quotedValue != NULL) {
                *quotedValue = TRUE;
              }

              if(quotePossible) {
                if(strSize != NULL) {
                  *strSize = 0;
                }

                quotePossible = FALSE;
              }
              else {
                state = 2;
                continue;
              }
            } break;

            case 0x2C: {  /* ',' */
              if(canEnd) {
                exitCode = 1;
                break;
              }
            } /* fall thru */

            default: {
              if(doTrim && quotePossible) {
                if(strSize != NULL) {
                  *strSize = 0;
                }
              }

              quotePossible = FALSE;
              strSize = strAppendUTF8(c, value, *strSize);
            } break;
          }
        } break;
      }
    }
  } while (exitCode == 0);

  if(doTrim) {
    strRTrim(value, strSize, minSize);
  }

  strAppend('\0', value, strSize);

  if(strSize != NULL) {
    (*strSize)--;
  }

  if(startPosition != NULL) {
    *startPosition = offset;
  }

  free(tempString);
  return exitCode == 1;
}
