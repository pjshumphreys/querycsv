int isNumberWithGetByteLength(
    unsigned char *offset,
    int *lastMatchedBytes,
    int firstChar
) {
  unsigned char decimalNotFound = TRUE;
  unsigned char *string = offset;
  unsigned char c = 0;

  MAC_YIELD

  if (*offset >= '0' && *offset <= '9') {
    c = 1;
  }
  else if(firstChar) {
    if(*offset == '.' && *(offset+1) >= '0' && *(offset+1) <= '9') {
      c = 1;
    }
    else if (*offset == '-' || *offset == '+') {
      if(*(offset+1) >= '0' && *(offset+1) <= '9') {
        c = 1;
      }
      else if (*(offset+1) == '.' && *(offset+2) >= '0' && *(offset+2) <= '9') {
        c = 1;
      }
    }
  }

  if(c) {
    if(*string == '-' || *string == '+') {
      string++;
    }

    do {
      c = *string;

      if(c >= '0' && c <= '9') {
        string++;
        continue;
      }

      if(decimalNotFound) {
        if(c == '.' || c == ',') {
          decimalNotFound = FALSE;

          string++;
          continue;
        }
      }

      break;
    } while(1);

    *(lastMatchedBytes) += (string-offset-1);

    return TRUE;
  }

  return FALSE;
}
