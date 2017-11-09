#include "querycsv.h"

int parse_encoding2(struct qryData *queryData, char *encoding) {
  char *matcher;
    
  MAC_YIELD

  if(mystrnicmp("utf", encoding, 3) == 0) {
    matcher = encoding + 3;

    if(mystrnicmp("-", matcher, 1) == 0) {
      matcher += 1;
    }

    if(strcmp("8", matcher) == 0) {
      /* utf-8, unless a BOM is found. Overlong bytes
      are always converted using windows 1252 */
      return ENC_UTF8;
    }

    if(
        strcmp("16", matcher) == 0 ||
        stricmp("16le", matcher) == 0
    ) {
      return ENC_UTF16LE;
    }

    if(stricmp("16be", matcher) == 0) {
      return ENC_UTF16BE;
    }

    if(
        strcmp("32", matcher) == 0 ||
        stricmp("32le", matcher) == 0
    ) {
      return ENC_UTF32LE;
    }

    if(stricmp("32be", matcher) == 0) {
      return ENC_UTF32BE;
    }
  }

  else if(mystrnicmp("cp", encoding, 2) == 0) {
    matcher = encoding + 2;

    if(strcmp("1252", matcher) == 0) { /* always windows 1252 */
      return ENC_CP1252;
    }

    if(strcmp("437", matcher) == 0) {  /* always CP437 */
      return ENC_CP437;
    }
    
    if(strcmp("850", matcher) == 0) {  /* always CP850 */
      return ENC_CP850;
    }

    if(strcmp("1047", matcher) == 0) {
      return ENC_CP1047;
    }
  }

  else if(
      stricmp("mac", encoding) == 0 ||
      stricmp("macroman", encoding) == 0
  ) {
    return ENC_MAC;
  }

  else if(stricmp("oem", encoding) == 0) {
    return ENC_CP437;
  }

  else if(stricmp("ansi", encoding) == 0) {
    return ENC_CP1252;
  }

  else if(stricmp("unicode", encoding) == 0) {
    return ENC_UTF8;
  }

  else if(stricmp("petscii", encoding) == 0) {
    return ENC_PETSCII;
  }

  else if(stricmp("atarist", encoding) == 0) {
    return ENC_ATARIST;
  }

  fprintf(stderr, TDB_INVALID_ENCODING);

  return ENC_UNSUPPORTED;
}

int parse_encoding(
    struct qryData *queryData,
    char *encoding
  ) {
  int retval = parse_encoding2(queryData, encoding);

  free(encoding);

  return retval;
}
