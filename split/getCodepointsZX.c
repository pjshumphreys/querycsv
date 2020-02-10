static const unsigned QCSV_SHORT zx[16] = {
  0x25A1, /* white square */
  0x259D, /* Quadrant upper right */
  0x2598, /* Quadrant upper left */
  0x2580, /* Upper half block */

  0x2597, /* Quadrant lower right */
  0x2590, /* Right half block */
  0x259A, /* Quadrant upper left and lower right */
  0x259C, /* Quadrant upper left and upper right and lower right */

  0x2596, /* Quadrant lower left */
  0x259E, /* Quadrant upper right and lower left */
  0x258C, /* Left half block */
  0x259B, /* Quadrant upper left and upper right and lower left */

  0x2584, /* Lower half block */
  0x259F, /* Quadrant upper right and lower left and lower right */
  0x2599, /* Quadrant upper left and lower left and lower right */
  0x2588  /* Full block */
};



void getCodepointsZXCommon(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength,
    int isTsw
) {
  int c;

  MAC_YIELD

  if(stream == NULL) {
    *arrLength = *byteLength = 0;
    return;
  }

  *arrLength = *byteLength = 1;

  /* Don't return any other bytes after MYEOF has been returned */
  if(codepoints[0] == MYEOF) {
    return;
  }

  c = fgetc(stream);

  if(isTsw) {
    if(c == 0x80) {
      codepoints[0] = 0x0A;
      return;
    }

    if(c < 0x20) {
      codepoints[0] = 0x20;
      return;
    }

    if(c == 0x8f) {
      *byteLength = 2;

      switch((c = fgetc(stream))) {
        case 0x80: {
          codepoints[0] = 0x25A1; /* white square */
        } return;

        case 0x8f: {
          codepoints[0] = 0x2588; /* Full block */
        } return;

        default: {
          codepoints[0] = MYEOF;
        } return;
      }
    }
  }

  if(c == EOF) {
    codepoints[0] = MYEOF;
    return;
  }

  if(c < 0x7F) {
    if(c == 0x5E) {
      codepoints[0] = 0x2191;
      return;
    }

    if(c == 0x60) {
      codepoints[0] = 0xA3;
      return;
    }

    codepoints[0] = (long)c;
  }
  else {
    if(c > 0x8F) {
      codepoints[0] = 0xFFFD;
      return;
    }

    if(c > 0x7F) {
      codepoints[0] = zx[c - 0x80];
      return;
    }

    codepoints[0] = 0xA9;
  }
}

void getCodepointsZX(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength
) {
  getCodepointsZXCommon(
    stream,
    codepoints,
    arrLength,
    byteLength,
    FALSE
  );
}

void getCodepointsTSW(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength
) {
  getCodepointsZXCommon(
    stream,
    codepoints,
    arrLength,
    byteLength,
    TRUE
  );
}
