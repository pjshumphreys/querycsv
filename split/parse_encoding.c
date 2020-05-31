int parse_encoding2(char *encoding) {
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
        mystrnicmp("16le", matcher, 5) == 0
    ) {
      return ENC_UTF16LE;
    }

    if(mystrnicmp("16be", matcher, 5) == 0) {
      return ENC_UTF16BE;
    }

    if(
        strcmp("32", matcher) == 0 ||
        mystrnicmp("32le", matcher, 5) == 0
    ) {
      return ENC_UTF32LE;
    }

    if(mystrnicmp("32be", matcher, 5) == 0) {
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
      mystrnicmp("mac", encoding, 4) == 0 ||
      mystrnicmp("macroman", encoding, 9) == 0
  ) {
    return ENC_MAC;
  }

  else if(mystrnicmp("oem", encoding, 4) == 0) {
    return ENC_CP437;
  }

  else if(mystrnicmp("ansi", encoding, 5) == 0) {
    return ENC_CP1252;
  }

  else if(mystrnicmp("ascii", encoding, 6) == 0) {
    return ENC_ASCII;
  }

  else if(mystrnicmp("unicode", encoding, 8) == 0) {
    return ENC_UTF8;
  }

  else if(mystrnicmp("petscii", encoding, 8) == 0) {
    return ENC_PETSCII;
  }

  else if(mystrnicmp("atarist", encoding, 8) == 0) {
    return ENC_ATARIST;
  }

  else if(mystrnicmp("bbc", encoding, 4) == 0) {
    return ENC_BBC;
  }

  else if(mystrnicmp("zx", encoding, 3) == 0) {
    return ENC_ZX;
  }

  else if(mystrnicmp("tsw", encoding, 4) == 0) {
    return ENC_TSW;
  }

  fprintf(stderr, TDB_INVALID_ENCODING, encoding);

  return ENC_UNSUPPORTED;
}

int parse_encoding(char *encoding) {
  int retval = parse_encoding2(encoding);

  free(encoding);

  return retval;
}
