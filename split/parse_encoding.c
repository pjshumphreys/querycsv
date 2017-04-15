#include "querycsv.h"

int parse_encoding(
    struct qryData *queryData,
    char *encoding
  ) {
  MAC_YIELD

  //don't bother checking anything until the second pass
  if(queryData->parseMode == 0) {
    return ENC_UNKNOWN;
  }

  if(stricmp("win1252", encoding) == 0) { //always windows 1252
    return ENC_WIN1252;
  }

  if(stricmp("cp437", encoding) == 0) {  //always CP437
    return ENC_CP437;
  }

  if(stricmp("cp850", encoding) == 0) {  //always CP850
    return ENC_CP850;
  }

  if(
      stricmp("utf8", encoding) == 0 ||
      stricmp("unicode", encoding) == 0) {  //utf-8, unless a BOM is found, overlong bytes are always converted using windows 1252
    return ENC_UTF8;
  }

  if(stricmp("petscii", encoding) == 0) {
    return ENC_PETSCII;
  }

  if(stricmp("mac", encoding) == 0) {
    return ENC_MAC;
  }

  fprintf(strerr, "unsupported encoding %s\n", encoding);

  return ENC_UNSUPPORTED; 
}
