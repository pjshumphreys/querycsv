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

      case 'q':
      case 'Q':
        params |= PRM_QUOTE;
      break;

      case 'c':
      case 'C':
        params &= ~PRM_BLANK;
      break;

      case 'p':
      case 'P':
        params |= PRM_POSTGRES;
        params &= ~PRM_EURO;
        params &= ~PRM_NULL;
      break;

      case 'e':
      case 'E': {
        params &= ~PRM_POSTGRES;
        params |= PRM_EURO;
      } break;

      case 'n':
      case 'N':
        params &= ~PRM_POSTGRES;
        params |= PRM_NULL;
      break;

      case 's':
      case 'S':
        params |= PRM_SPACE;
      break;
    }

    i++;
  }

  queryData->params = params;
}
