void readParams(struct qryData *queryData, char *string) {
  int params;
  char* i;

  MAC_YIELD

  if(queryData->parseMode == 1 || string == NULL) {
    return;
  }

  params = PRM_DEFAULT;
  i = string;

  while(*i) {
    switch(*i) {
      case 'd':
      case 'D':
        params &= ~PRM_MAC;
        params &= ~PRM_UNIX;
      break;

      case 'u':
      case 'U':
        params &= ~PRM_MAC;
        params |= PRM_UNIX;
      break;

      case 'm':
      case 'M':
        params |= PRM_MAC;
        params &= ~PRM_UNIX;
      break;

      case 'b':
      case 'B':
        params |= PRM_BOM;
      break;

      case 's':
      case 'S':
        params |= PRM_SPACE;
      break;

      case 'a':
      case 'A':
        params |= PRM_QUOTE;
      break;

      case 'k':
      case 'K':
        params |= PRM_TRIM;
      break;

      case 'i':
      case 'I': {
        switch(*(i+1)) {
          case 'c':
          case 'C':
            params &= ~PRM_BLANK;
          break;

          case 'p':
          case 'P':
            params |= PRM_POSTGRES;
          break;

          case 'n':
          case 'N':
            params |= PRM_NULL;
          break;
        }
      } break;

      case 'e':
      case 'E': {
        switch(*(i+1)) {
          case 'p':
            params |= PRM_EPOSTGRES;
            params &= ~PRM_ENULL;
          break;

          case 'n':
            params &= ~PRM_EPOSTGRES;
            params |= PRM_ENULL;
          break;
        }
      } break;
    }

    i++;
  }

  queryData->params = params;
}
