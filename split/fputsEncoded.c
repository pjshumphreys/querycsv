int fputsEncoded(char *str, struct qryData *query) {
  size_t bytesStored = 0;
  char *encoded;
  int retval = 0;
  int offset = query->outputOffset;

  MAC_YIELD

  switch(query->outputEncoding) {
    /* Tasword 2 format accounting stuff */
    case ENC_TSW: {
      /* Tasword 2 is a file only representation. switch to regular zx spectrum format if outputting to the screen */
      if(query->outputFile == stdout || query->outputFile == stderr) {
        encoded = d_charsetEncode(str, ENC_ZX, &bytesStored, query);
        retval = fwrite(encoded, 1, bytesStored, query->outputFile);
        free(encoded);
      }
      else {
        encoded = d_charsetEncode(str, ENC_TSW, &bytesStored, query);

        if(bytesStored) { /* 2 if statements here as z88dk can't handle && and || in the same expression */
          if(bytesStored == 1 || (unsigned char)(encoded[bytesStored-2]) != 143) { /* 143 = escape newline and pseudo EOF characters */
            /* if the output string ends in an encoded newline character... */
            if((unsigned char)(encoded[bytesStored-1]) == 128) {
              /* replaces newlines with spaces if PRM_REMOVE is set */
              if(query->params & PRM_REMOVE) {
                encoded[bytesStored-1] = ' ';
              }

              if(
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
        }

        /* tasword 2 format files must not exceed 20480 bytes (320 x 64 byte lines) as they would overrun
         * into tasword's machine code. Abort the program if we try to generate this much output */
        if((offset + bytesStored) > 20480) {
          fputs(TDB_FILE_SIZE_EXCEEDED, stderr);
          exit(EXIT_FAILURE);
        }

        retval = fwrite(encoded, 1, bytesStored, query->outputFile);
        free(encoded);
      }
    } break;

    /* account for embedded \0 bytes in utf-16 and utf-32 */
    case ENC_UTF16LE:
    case ENC_UTF16BE:
    case ENC_UTF32LE:
    case ENC_UTF32BE: {
      if(offset == 0 && query->outputFile != stdout) {
        /* utf-16 and utf-32 files being written to disk always get a bom */
        encoded = d_charsetEncode("\xEF\xBB\xBF", query->outputEncoding, &bytesStored, query);
        retval = fwrite(encoded, 1, bytesStored, query->outputFile);
        free(encoded);
        offset = -retval;
        bytesStored = 0;
      }

      encoded = d_charsetEncode(str, query->outputEncoding, &bytesStored, query);
      retval += fwrite(encoded, 1, bytesStored, query->outputFile);
      free(encoded);
    } break;

    case ENC_PETSCII: {
      if(offset == 0 && query->outputFile != stdout && query->params & PRM_BOM) {
        /* write a pseudo load address at the start of the output file */
        retval = fwrite("\x01\x08", 1, 2, query->outputFile);
        offset = -retval;
        bytesStored = 0;
      }

      encoded = d_charsetEncode(str, ENC_PETSCII, &bytesStored, query);
      retval += fwrite(encoded, 1, bytesStored, query->outputFile);
      free(encoded);
    } break;

    case ENC_UTF8: {
      if(offset == 0 && query->outputFile != stdout && query->params & PRM_BOM) {
        /* write a pseudo load address at the start of the output file */
        retval = fwrite("\xEF\xBB\xBF", 1, 3, query->outputFile);
        offset = -retval;
        bytesStored = 0;
      }

      if(!(query->params & (PRM_INSERT | PRM_REMOVE))) {
        retval = fwrite(str, 1, strlen(str), query->outputFile);
        break;
      }
    } /* fall thru */

    default: {
      encoded = d_charsetEncode(str, query->outputEncoding, &bytesStored, query);
      retval += fwrite(encoded, 1, bytesStored, query->outputFile);
      free(encoded);
    } break;
  }

  offset += retval;
  query->outputOffset = offset;

  return retval;
}
