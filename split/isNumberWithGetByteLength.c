int clause3(unsigned char *offset, int firstChar) {
  MAC_YIELD

  if (*offset >= '0' && *offset <= '9') {
    return TRUE;
  }

  if(firstChar) {
    if(*offset == '.' && *(offset+1) >= '0' && *(offset+1) <= '9') {
      return TRUE;
    }

    if (*offset == '-' || *offset == '+') {
      if(*(offset+1) >= '0' && *(offset+1) <= '9') {
        return TRUE;
      }

      if (*(offset+1) == '.' && *(offset+2) >= '0' && *(offset+2) <= '9') {
        return TRUE;
      }
    }
  }

  return FALSE;
}

int isNumberWithGetByteLength(
    unsigned char *offset,
    int *lastMatchedBytes,
    int firstChar
) {
  int decimalNotFound = TRUE;
  unsigned char *string = offset;

  MAC_YIELD

  if(clause3(offset, firstChar)) {
    if(*string == '-' || *string == '+') {
      string++;
    }

    while(
      (*string >= '0' && *string <= '9') ||
      (decimalNotFound && (*string == '.' || *string == ',') &&
      !(decimalNotFound = FALSE))
    ) {
      string++;
    }

    *(lastMatchedBytes)+=(string-offset)-1;

    return TRUE;
  }
  else {
    return FALSE;
  }
}
