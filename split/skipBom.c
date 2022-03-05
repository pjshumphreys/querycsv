/* skips the BOM if present from a file. */
/* Also attempts to detect the file encoding for
  non iso-646 character sets if the encoding parameter is not null */
FILE *skipBom(const char *filename, long* offset, int* encoding) {
  FILE *file;
  int c;
  int internalOffset = 0;
  int i;

  MAC_YIELD

  if(offset) {
    internalOffset = *offset;
  }

  file = fopen(filename, fopen_read);

  if(file != NULL) {
    /* if a preset offset was set, skip that number of bytes of the file */
    if(internalOffset) {
      for(i = internalOffset; i; i--) {
        fgetc(file);
      }
    }

    /* skip over the bom if present */
    c = fgetc(file);

    /* if the file encoding is unknown and a 128 byte zx spectrum PLUS3DOS header is found then skip it */
    if(
        internalOffset == 0 &&
        encoding &&
        *encoding == ENC_UNKNOWN &&
        c == 'P'
    ) {
      if(
          (c = fgetc(file)) == 'L' &&
          (c = fgetc(file)) == 'U' &&
          (c = fgetc(file)) == 'S' &&
          (c = fgetc(file)) == '3' &&
          (c = fgetc(file)) == 'D' &&
          (c = fgetc(file)) == 'O' &&
          (c = fgetc(file)) == 'S'
      ) {
        internalOffset = 128;

        /* skip past the first 128 bytes (including the 8 we just read) */
        myfseek(file, 120, SEEK_SET);
      }
      else {
        fclose(file);

        file = fopen(filename, fopen_read);

        internalOffset = 0;
      }

      c = fgetc(file);
    }

    switch(c) {
      case 0: {  /* maybe UTF-32BE */
        if(
            fgetc(file) == 0 &&
            fgetc(file) == 254 &&
            fgetc(file) == 255
        ) {
          internalOffset += 4;

          if(encoding) {
            *encoding = ENC_UTF32BE;
          }
        }
      } break;

      case 1: {  /* maybe a PETSCII load address */
        if(
            fgetc(file) == 8
        ) {
          internalOffset += 2;

          if(encoding) {
            *encoding = ENC_PETSCII;
          }
        }
      } break;


      case 239: { /* maybe utf-8 bom */
        if(
            fgetc(file) == 187 &&
            fgetc(file) == 191
        ) {
          internalOffset += 3;

          if(encoding) {
            *encoding = ENC_UTF8;
          }
        }
      } break;

      case 254: { /* maybe UTF-16BE or UTF-32BE */
        if(fgetc(file) == 255) {
          internalOffset += 2;

          if(encoding) {
            *encoding = ENC_UTF16BE;
          }
        }
      } break;

      case 255: { /* maybe UTF-16LE or UTF-32LE */
        if(fgetc(file) == 254) {
          if(
            fgetc(file) == 0 &&
            fgetc(file) == 0
          ) { /* UTF-32LE */
            internalOffset += 4;

            if(encoding) {
              *encoding = ENC_UTF32LE;
            }
          }
          else {  /* UTF-16LE */
            internalOffset += 2;

            if(encoding) {
              *encoding = ENC_UTF16LE;
            }
          }
        }
      } break;

      case 133:
      case 197: { /* maybe the letters 'enc'(oding) (case insensitive) in EBCDIC */
        if(
            encoding &&
            *encoding == ENC_UNKNOWN &&
            ((c = fgetc(file)) == 149 || c == 213) && /* 'N' */
            ((c = fgetc(file)) == 131 || c == 195) /* 'C' */
        ) {
          *encoding = ENC_CP1047;
          break;
        }
      } break;
    }

    if(encoding && (*encoding == ENC_DEFAULT || *encoding == ENC_UNKNOWN)) {
      *encoding = ENC_INPUT;

      /* On the c64 build files with an unknown encoding are assumed to
        be petscii, which has a two byte header */
      #if ENC_INPUT == ENC_PETSCII
        internalOffset = 2;
      #endif
    }

    if(offset) {
      *offset = internalOffset;
    }

    /* the byte order mark was not found, and calling ungetc multiple times is not */
    /* portable (doesn't work on cc65). Therefore we just close and reopen the file */
    fclose(file);

    file = fopen(filename, fopen_read);

    if(file && internalOffset) {
      myfseek(file, internalOffset, SEEK_SET);
    }
  }

  return file;
}
