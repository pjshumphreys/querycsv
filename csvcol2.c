#include <stdio.h>

#define MYEOF -1

/* returns a number of codepoints, each as a long */
/* invalid bytes or bytes that form an overlong codepoint are treated as
a set of windows-1252 characters that are then each converted to their coresponding value in unicode */
/* the number of bytes read in the file is also returned, so you can fseek back to offset after the match later */
/* due to the nature of utf-8, commas, new lines and double quotes always appear at the end of the array */
void getCodepointsUTF8(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength
  ) {

  int byteIndex = -1;
  int c;

  if(stream == NULL) {
    *arrlength = 0;
    *bytelength = 0;
    return;
  }

  c = fgetc(stream);
  codepoints[++byteIndex] = (long)c;

  if(c == EOF) {
    codepoints[0] = MYEOF;
    return;
  }

  /* if the current byte offset is a valid utf-8 character that's not overlong or decomposable then return it */
  if(c < 0x80) {
    /* read 1 byte. no overlong checks needed as a 1 byte code can */
    /* never be overlong */
    *arrlength = 1;
    *bytelength = 1;

    codepoints[0] = (long)c;

    return;
  }
  /* ensure the current byte is the start of a valid utf-8 sequence */
  else if(c > 0xC1) {
    if (c < 0xE0) {
      /* read 2 bytes */
      c = fgetc(stream);
      codepoints[++byteIndex] = c;

      if(c != EOF && (c & 0xC0) == 0x80) {
        codepoints[0] = (codepoints[0] << 6) + codepoints[1] - 0x3080;

        *arrlength = 1;
        *bytelength = 2;

        return;
      }
    }
    else if (c < 0xF0) {
      /* read 3 bytes */
      c = fgetc(stream);
      bytes[++byteIndex] = c;

      if(
          c != EOF &&
          (c & 0xC0) == 0x80 &&
          (bytes[0] != 0xE0 || c > 0x9F)
      ) {
        c = fgetc(stream);
        codepoints[++byteIndex] = (long)c;

        if(
            c != EOF &&
            (c & 0xC0) == 0x80
        ) {
          codepoints[0] = (codepoints[0] << 12) + (codepoints[1] << 6) + codepoints[2] - 0xE2080;

          *arrlength = 1;
          *bytelength = 3;
          return;
        }
      }
    }
    else if (c < 0xF5) {
      /* read 4 bytes */
      c = fgetc(stream);
      codepoints[++byteIndex] = (long)c;

      if(
          c != EOF &&
          (c & 0xC0) == 0x80 &&
          (codepoints[0] != 0xF0 || c > 0x8F) &&
          (codepoints[0] != 0xF4 || c < 0x90) &&
      ) {
        c = fgetc(stream);
        codepoints[++byteIndex] = (long)c;

        if(c != EOF && (c & 0xC0) == 0x80) {
          c = fgetc(stream);
          codepoints[++byteIndex] = (long)c;

          if(c != EOF && (c & 0xC0) == 0x80) {
            codepoints[0] = (codepoints[0] << 18) + (codepoints[1] << 12) + (codepoints[2] << 6) + codepoints[3] - 0x3C82080;

            *arrlength = 1;
            *bytelength = 4;
            return;
          }
        }
      }
    }
  }

  //dump out the bytes matched, converting each to a separate codepoint
  if(c == EOF) {
    codepoints[byteIndex] = MYEOF;
  }

  *arrlength = *bytelength = byteIndex + 1;

  for( ; byteIndex > -1; byteIndex--) {
    //use codepage 1252 conversions where appropriate
    if(codepoints[byteIndex] > 0x7F) {
      codepoints[byteIndex] = cp1252[codepoints[byteIndex] - 0x80];
    }
  }
}

static const long cp437[128] = {
  0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
  0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
  0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
  0x00FF, 0x00D6, 0x00DC, 0x00A2, 0x00A3, 0x00A5, 0x20A7, 0x0192,
  0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
  0x00BF, 0x2310, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
  0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
  0x2555, 0x2563, 0x2551, 0x2557, 0x255D, 0x255C, 0x255B, 0x2510,
  0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x255E, 0x255F,
  0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x2567,
  0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256B,
  0x256A, 0x2518, 0x250C, 0x2588, 0x2584, 0x258C, 0x2590, 0x2580,
  0x03B1, 0x00DF, 0x0393, 0x03C0, 0x03A3, 0x03C3, 0x00B5, 0x03C4,
  0x03A6, 0x0398, 0x03A9, 0x03B4, 0x221E, 0x03C6, 0x03B5, 0x2229,
  0x2261, 0x00B1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00F7, 0x2248,
  0x00B0, 0x2219, 0x00B7, 0x221A, 0x207F, 0x00B2, 0x25A0, 0x00A0
};

static const long cp850[128] = {
  0x00C7, 0x00FC, 0x00E9, 0x00E2, 0x00E4, 0x00E0, 0x00E5, 0x00E7,
  0x00EA, 0x00EB, 0x00E8, 0x00EF, 0x00EE, 0x00EC, 0x00C4, 0x00C5,
  0x00C9, 0x00E6, 0x00C6, 0x00F4, 0x00F6, 0x00F2, 0x00FB, 0x00F9,
  0x00FF, 0x00D6, 0x00DC, 0x00F8, 0x00A3, 0x00D8, 0x00D7, 0x0192,
  0x00E1, 0x00ED, 0x00F3, 0x00FA, 0x00F1, 0x00D1, 0x00AA, 0x00BA,
  0x00BF, 0x00AE, 0x00AC, 0x00BD, 0x00BC, 0x00A1, 0x00AB, 0x00BB,
  0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x00C1, 0x00C2, 0x00C0,
  0x00A9, 0x2563, 0x2551, 0x2557, 0x255D, 0x00A2, 0x00A5, 0x2510,
  0x2514, 0x2534, 0x252C, 0x251C, 0x2500, 0x253C, 0x00E3, 0x00C3,
  0x255A, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256C, 0x00A4,
  0x00F0, 0x00D0, 0x00CA, 0x00CB, 0x00C8, 0x0131, 0x00CD, 0x00CE,
  0x00CF, 0x2518, 0x250C, 0x2588, 0x2584, 0x00A6, 0x00CC, 0x2580,
  0x00D3, 0x00DF, 0x00D4, 0x00D2, 0x00F5, 0x00D5, 0x00B5, 0x00FE,
  0x00DE, 0x00DA, 0x00DB, 0x00D9, 0x00FD, 0x00DD, 0x00AF, 0x00B4,
  0x00AD, 0x00B1, 0x2017, 0x00BE, 0x00B6, 0x00A7, 0x00F7, 0x00B8,
  0x00B0, 0x00A8, 0x00B7, 0x00B9, 0x00B3, 0x00B2, 0x25A0, 0x00A0
};

static const long cp1252[128] = {
  0x20AC, 0xFFFD, 0x201A, 0x0192, 0x201E, 0x2026, 0x2020, 0x2021,
  0x02C6, 0x2030, 0x0160, 0x2039, 0x0152, 0xFFFD, 0x017D, 0xFFFD,
  0xFFFD, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2013, 0x2014,
  0x02DC, 0x2122, 0x0161, 0x203A, 0x0153, 0xFFFD, 0x017E, 0x0178,
  0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
  0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
  0x00B0, 0x00B1, 0x00B2, 0x00B3, 0x00B4, 0x00B5, 0x00B6, 0x00B7,
  0x00B8, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00BD, 0x00BE, 0x00BF,
  0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5, 0x00C6, 0x00C7,
  0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD, 0x00CE, 0x00CF,
  0x00D0, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6, 0x00D7,
  0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DE, 0x00DF,
  0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
  0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
  0x00F0, 0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7,
  0x00F8, 0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FE, 0x00FF
};

static const long macRoman[128] = {
  0x00C4, 0x00C5, 0x00C7, 0x00C9, 0x00D1, 0x00D6, 0x00DC, 0x00E1,
  0x00E0, 0x00E2, 0x00E4, 0x00E3, 0x00E5, 0x00E7, 0x00E9, 0x00E8,
  0x00EA, 0x00EB, 0x00ED, 0x00EC, 0x00EE, 0x00EF, 0x00F1, 0x00F3,
  0x00F2, 0x00F4, 0x00F6, 0x00F5, 0x00FA, 0x00F9, 0x00FB, 0x00FC,
  0x2020, 0x00B0, 0x00A2, 0x00A3, 0x00A7, 0x2022, 0x00B6, 0x00DF,
  0x00AE, 0x00A9, 0x2122, 0x00B4, 0x00A8, 0x2260, 0x00C6, 0x00D8,
  0x221E, 0x00B1, 0x2264, 0x2265, 0x00A5, 0x00B5, 0x2202, 0x2211,
  0x220F, 0x03C0, 0x222B, 0x00AA, 0x00BA, 0x03A9, 0x00E6, 0x00F8,
  0x00BF, 0x00A1, 0x00AC, 0x221A, 0x0192, 0x2248, 0x2206, 0x00AB,
  0x00BB, 0x2026, 0x00A0, 0x00C0, 0x00C3, 0x00D5, 0x0152, 0x0153,
  0x2013, 0x2014, 0x201C, 0x201D, 0x2018, 0x2019, 0x00F7, 0x25CA,
  0x00FF, 0x0178, 0x2044, 0x20AC, 0x2039, 0x203A, 0xFB01, 0xFB02,
  0x2021, 0x00B7, 0x201A, 0x201E, 0x2030, 0x00C2, 0x00CA, 0x00C1,
  0x00CB, 0x00C8, 0x00CD, 0x00CE, 0x00CF, 0x00CC, 0x00D3, 0x00D4,
  0xF8FF, 0x00D2, 0x00DA, 0x00DB, 0x00D9, 0x0131, 0x02C6, 0x02DC,
  0x00AF, 0x02D8, 0x02D9, 0x02DA, 0x00B8, 0x02DD, 0x02DB, 0x02C7
};

void getCodepoints8Bit(
    FILE *stream,
    long *codepoints,
    int *arrlength,
    int *bytelength,
    long *map
  ) {

  if(stream == NULL) {
    *arrlength = 0;
    *bytelength = 0;
    return;
  }

  int c = fgetc(stream);

  *arrlength = 1;
  *bytelength = 1;

  if(c == EOF) {
    codepoints[0] = MYEOF;
    return;
  }

  if(c < 128) {
    codepoints[0] = (long)c;
    return;
  }

  c -= 128;

  codepoints[0] = map[c];
}

void getCodepointsCP437(
    FILE *stream,
    long *codepoints,
    int *arrlength,
    int *bytelength
  ) {
  getCodepoints8Bit(
    stream,
    codepoints,
    arrlength,
    bytelength,
    cp437
  );
}

void getCodepointsCP850(
    FILE *stream,
    long *codepoints,
    int *arrlength,
    int *bytelength
  ) {
  getCodepoints8Bit(
    stream,
    codepoints,
    arrlength,
    bytelength,
    cp850
  );
}

void getCodepointsCP1252(
    FILE *stream,
    long *codepoints,
    int *arrlength,
    int *bytelength
  ) {
  getCodepoints8Bit(
    stream,
    codepoints,
    arrlength,
    bytelength,
    cp1252
  );
}

void getCodepointsMac(
    FILE *stream,
    long *codepoints,
    int *arrlength,
    int *bytelength
  ) {
  getCodepoints8Bit(
    stream,
    codepoints,
    arrlength,
    bytelength,
    macRoman
  );
}

static const long petscii[128] = {
  0x0040, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
  0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
  0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
  0x0078, 0x0079, 0x007A, 0x005B, 0x00A3, 0x005D, 0x2191, 0x2190,
  0x2500, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
  0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
  0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
  0x0058, 0x0059, 0x005A, 0x253C, 0x007C, 0x2502, 0x2592, 0x007F,
  0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
  0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x000A, 0xFFFD, 0xFFFD,
  0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
  0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
  0x00A0, 0x258C, 0x2584, 0x2594, 0x2581, 0x258F, 0x00A6, 0x2595,
  0x00A8, 0x00A9, 0x00AA, 0x251C, 0x2597, 0x2514, 0x2510, 0x2582,
  0x250C, 0x2534, 0x252C, 0x2524, 0x258E, 0x258D, 0x00B6, 0x00B7,
  0x00B8, 0x2583, 0x2713, 0x2596, 0x259D, 0x2518, 0x2598, 0x259A
};

void getCodepointsPetscii(
    FILE *stream,
    long *codepoints,
    int *arrlength,
    int *bytelength,
    long *map
  ) {

  if(stream == NULL) {
    *arrlength = 0;
    *bytelength = 0;
    return;
  }

  int c = fgetc(stream);

  *arrlength = 1;
  *bytelength = 1;

  if(c == EOF) {
    codepoints[0] = MYEOF;
    return;
  }

  if(c < 65) {
    codepoints[0] = (long)c;
    return;
  }

  if(c > 191) {
    codepoints[0] = 0xFFFD;
    return;
  }

  c-=64;

  codepoints[0] = map[c];
}



int getCsvColumn(
    FILE **inputFile,
    int inputEncoding,
    char **value,
    size_t *strSize,
    int *quotedValue,
    long *startPosition,
    int doTrim
  ) {

  long codepoints[4];
  void (*getCodepoints)(FILE *, long *, int *, int *);
  int arrLength;
  int byteLength;
  long bytesMatched = 0;
  int i;
  long c;

  char *tempString = NULL;
  int canEnd = TRUE;
  int quotePossible = TRUE;
  int exitCode = 0;
  char *minSize = NULL;
  long offset = 0;

  MAC_YIELD

  if(quotedValue != NULL) {
    *quotedValue = FALSE;
  }

  if(strSize != NULL) {
    *strSize = 0;
  }

  if(value == NULL) {
    value = &tempString;
  }

  if(startPosition != NULL) {
    offset = *startPosition;
  }

  if(feof(*inputFile) != 0) {
    return FALSE;
  }

  /* switch on the character encoding to choose which function to use */
  switch(inputEncoding) {
    case ENC_UTF8: {
      getCodepoints = &getCodepointsUTF8;
    } break;

    case ENC_CP437: {
      getCodepoints = &getCodepointsCP437;
    } break;

    case ENC_CP850: {
      getCodepoints = &getCodepointsCP850;
    } break;

    case ENC_WIN1252: {
      getCodepoints = &getCodepointsCP1252;
    } break;

    case ENC_UTF16LE: {
      getCodepoints = &getCodepointsUTF16LE;
    } break;

    case ENC_UTF16BE: {
      getCodepoints = &getCodepointsUTF16BE;
    } break;

    case ENC_UTF32LE: {
      getCodepoints = &getCodepointsUTF32LE;
    } break;

    case ENC_UTF32BE: {
      getCodepoints = &getCodepointsUTF32BE;
    } break;

    case ENC_PETSCII: {
      getCodepoints = &getCodepointsPetscii;
    } break;

    case ENC_MAC: {
      getCodepoints = &getCodepointsMac;
    } break;

    default: {
      getCodepoints = &getCodepointsUTF8;
    }
  }

  for ( ; ; ) {
    /* get some codepoints from the file, usually only 1 but maybe up to 4 */
    getCodepoints(*inputFile,
      &codepoints,
      &arrLength,
      &byteLength
    );

    /* for each codepoint returned treat it as we previously treated bytes */
    /* read a character */
    for (i = 0; i < arrLength && exitCode == 0; i++) {
      offset++;
      c = codepoints[i];

    }

    if (exitCode != 0) {
      break;
    }
  }

  if(doTrim) {
    strRTrim(value, strSize, minSize);
  }

  strAppend('\0', value, strSize);

  if(strSize != NULL) {
    (*strSize)--;
  }

  if(startPosition != NULL) {
    *startPosition = offset;
  }

  free(tempString);
  return exitCode == 1;
}

/* this version doesn't check for invalid bytes or overlong codepoints, as getCodepoints will have already converted them to valid utf-8 bytes in main memory */
long getUnicodeChar(
    unsigned char **offset,
    unsigned char **str,
    int plusBytes,
    int *bytesMatched,
    void (*get)()
  ) {

  struct hash2Entry* entry = NULL;
  int bytesread = 0;
  long codepoint;
  unsigned char *temp = (unsigned char *)((*offset) + plusBytes);

  MAC_YIELD

  /* if the current byte offset is a utf-8 character
    that's not decomposable then return it */
  if(*temp < 0x80) {
    /* read 1 byte. no checks needed as a 1 byte code
      is never a combining character */
    *bytesMatched = 1;

    return (long)(*temp);
  }
  else if(*temp < 0xE0) {
    /* read 2 bytes */
    codepoint = (long)((*(temp) << 6) + *(temp+1)) - 0x3080;
    if((entry = isInHash2(codepoint))) {
      bytesread = 2;
    }
    else {
      /* otherwise return it */
      *bytesMatched = 2;

      return codepoint;
    }
  }
  else if (*temp < 0xF0) {
    /* read 3 bytes */
    codepoint = ((long)(*(temp)) << 12) + ((long)(*(temp+1)) << 6) + (*(temp+2)) - 0xE2080;

    /* the codepoint is valid. but is it decomposable? */
    if((entry = isInHash2(codepoint))) {
      bytesread = 3;
    }
    else {
      /* otherwise return it */
      *bytesMatched = 3;

      return codepoint;
    }
  }

  /* read 4 bytes */
  codepoint = (((long)(*temp)) << 18) + ((long)(*(temp+1)) << 12) + ((long)(*(temp+2)) << 6) + (*(temp+3)) - 0x3C82080;

  /* the codepoint is valid. but is it decomposable? */
  if((entry = isInHash2(codepoint))) {
    bytesread = 4;
  }
  else {
    /* otherwise return it */
    *bytesMatched = 4;

    return codepoint;
  }

  /* decomposable codepoints mean the string needs to be NFD normalized. */
  return normaliseAndGet(offset, str, plusBytes, bytesMatched, bytesread, entry);
}


int main(int argc, char**argv){

  return 0;
}
