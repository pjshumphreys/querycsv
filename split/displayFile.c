void displayFile(void) {
  char *fileName = NULL;
  char *encoding = NULL;
  char *temp = NULL;
  FILE * theFile;
  long c;
  struct inputTable table;
  int byteLength;
  int byteLength2;
  int i;
  int outputEnc = ENC_PRINT;
  char* bytes = NULL; /* a pointer to constant data *OR* some allocated data for utf-16 */
  void (*getBytes)(long, char **, int *);
  char utf16Bytes[4];

  switch(outputEnc) {
    case ENC_CP1252: {
      getBytes = getBytesCP1252;
    } break;

    case ENC_CP437: {
      getBytes = getBytesCP437;
    } break;

    case ENC_CP850: {
      getBytes = getBytesCP850;
    } break;

    case ENC_MAC: {
      getBytes = getBytesMac;
    } break;

    case ENC_PETSCII: {
      getBytes = getBytesPetscii;
    } break;

    case ENC_CP1047: {
      getBytes = getBytesCP1047;
    } break;

    case ENC_ATARIST: {
      getBytes = getBytesAtariST;
    } break;

    case ENC_BBC: {
      getBytes = getBytesBBC;
    } break;

    case ENC_UTF16LE: {
      getBytes = getBytesUtf16Le;
      bytes = (char *)utf16Bytes;
    } break;

    case ENC_UTF16BE: {
      getBytes = getBytesUtf16Be;
      bytes = (char *)utf16Bytes;
    } break;

    default: {
      getBytes = getBytesCP1252;
    } break;
  }

  fputs(TDB_FILENAME, stdout);

  fgets_d(&fileName, stdin);
  #ifdef __CC65__
    temp = petsciiToUtf8(fileName);
    free(fileName);
    fileName = temp;
    temp = NULL;
  #endif

  if((theFile = fopen(fileName, fopen_read)) == NULL) {
    fputs(TDB_NOT_FOUND, stdout);
  }
  else {
    table.fileStream = theFile;
    table.cpIndex = table.arrLength = 0;

    fputs(TDB_ENCODING, stdout);

    fgets_d(&encoding, stdin);
    #ifdef __CC65__
      temp = petsciiToUtf8(encoding);
      free(encoding);
      encoding = temp;
      temp = NULL;
    #endif

    if(strcmp(encoding, "") == 0) {
      table.fileEncoding = ENC_INPUT;
    }
    else {
      table.fileEncoding = parse_encoding2(encoding);

      if(table.fileEncoding == ENC_UNSUPPORTED) {
        table.fileEncoding = ENC_UTF8;
      }
    }

    /* initalise the "get a codepoint" data structures */
    getNextCodepoint(&table);

    while((c = getCurrentCodepoint(&table, &byteLength)) != MYEOF) {
      getBytes(c, &bytes, &byteLength2);

      /* for each byte returned, call strAppend */
      for(i=0; i < byteLength2; i++) {
        fputc(bytes == NULL ? ((char)c) : bytes[i], stdout);
      }

      getNextCodepoint(&table);
    }

    fclose(theFile);
  }

  free(fileName);
  free(encoding);
}
