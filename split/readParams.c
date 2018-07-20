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
      case 'a':
      case 'A':
        params |= PRM_QUOTE;
      break;

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

      case 'n':
      case 'N':
        params |= PRM_NULL;
      break;

      case 'i':
      case 'I':
        params |= PRM_IMPORT;
      break;

      case 'e':
      case 'E':
        params |= PRM_EXPORT;
      break;

      case 'p':
      case 'P':
        params |= PRM_TRIM;
      break;
    }

    i++;
  }

  queryData->params = params;
}
