int fputsEncoded(char *str, struct qryData *query) {
  size_t bytesStored;
  char *encoded;
  int retval = 0;
  int offset = query->outputOffset;

  MAC_YIELD
  if(query->outputEncoding == ENC_UTF8 && !(query->params & (PRM_INSERT | PRM_REMOVE))) {
    retval = fputs(str, query->outputFile);
  }
  else switch(query->outputEncoding) {
    /* Tasword 2 format accounting stuff */
    case ENC_TSW: {
      bytesStored = 0;

      /* Tasword 2 is a file only representation. switch to regular zx spectrum format if outputting to the screen */
      if(query->outputFile == stdout) {
        encoded = d_charsetEncode(str, ENC_ZX, &bytesStored, query);
      }
      else {
        encoded = d_charsetEncode(str, ENC_TSW, &bytesStored, query);

        if(bytesStored) {
          if(bytesStored == 1 || (unsigned char)(encoded[bytesStored-2]) != 143) { /* 143 = escape newline and pseudo EOF characters */
            if(
              (unsigned char)(encoded[bytesStored-1]) == 128 &&  /* if the output string ends in an encoded newline character... */
              query->params & PRM_TASWORD && /* ...and we specified we want to space pad newlines... */
              (offset + bytesStored) % 64 != 0 /* and padding needs to be added */
            ) {
              /* add padding with spaces */
              retval = 64 - ((offset + bytesStored) % 64);

              do {
                strAppend(' ', &encoded, &bytesStored);
              } while(--retval);
            }
          }
        }

        /* tasword 2 format files must not exceed 20480 bytes (320 x 64 byte lines) as they would overrun
         * into tasword's machine code. Abort the program we try to generate this much output */
        if((offset + bytesStored) > 20480) {
          fwrite(TDB_FILE_SIZE_EXCEEDED, sizeof(char), sizeof(TDB_FILE_SIZE_EXCEEDED), stderr);
          exit(EXIT_FAILURE);
        }
      }

      retval += fwrite(encoded, sizeof(char), bytesStored, query->outputFile);
      free(encoded);
    } break;

    case ENC_PETSCII: {
      if(offset == 0) {
        /* write a pseudo load address at the start of the output file */
        retval = fwrite("\x01\x08", sizeof(char), 2, query->outputFile);
      }
    } /* fall thru */

    default: {
      bytesStored = 0;

      encoded = d_charsetEncode(str, query->outputEncoding, &bytesStored, query);
      retval += fwrite(encoded, sizeof(char), bytesStored, query->outputFile);
      free(encoded);
    } break;
  }

  offset += retval;
  query->outputOffset = offset;

  return retval;
}
