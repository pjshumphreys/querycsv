int readInputOptions(struct qryData *queryData, char *string) {
  int params;
  char* i;

  MAC_YIELD

  if(queryData->parseMode == 1 || string == NULL) {
    return 0;
  }

  params = PRM_BLANK;
  i = string;

  while(*i) {
    switch(*i) {
      case 'p':
      case 'P':
        params &= ~PRM_EURO;
        params |= PRM_POSTGRES;
      break;

      case 'c':
      case 'C':
        params &= ~PRM_BLANK;
      break;

      case 'k':
      case 'K':
        params |= PRM_TRIM;
      break;
      
      case 'e':
      case 'E':
        params &= ~PRM_POSTGRES;
        params |= PRM_EURO;
      break;

      case 'n':
      case 'N':
        params |= PRM_NULL;
      break;
    }

    i++;
  }

  return params;
}
