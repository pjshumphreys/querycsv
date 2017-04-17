#include "querycsv.h"

#include "cp1252.h"

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

  MAC_YIELD

  if(stream == NULL) {
    *arrLength = 0;
    *byteLength = 0;
    return;
  }

  if((c = fgetc(stream)) == EOF) {
    *arrLength = *byteLength = 1;
    codepoints[0] = MYEOF;
    return;
  }
  else {
    codepoints[++byteIndex] = (long)c;
  }

  for( ; ; ) {  /* not a real loop. We only need it to be
    able to use the break statement */
    /* if the current byte offset is a valid utf-8 character that's not
    overlong or decomposable then return it */
    if(c < 0x80) {
      /* read 1 byte. no overlong checks needed as a 1 byte code can */
      /* never be overlong */
      *arrLength = *byteLength = 1;
      return;
    }
    /* ensure the current byte is the start of a valid utf-8 sequence */
    else if(c > 0xC1) {
      if (c < 0xE0) {
        /* read 2 bytes */
        if((c = fgetc(stream)) != EOF) {
          codepoints[++byteIndex] = c;
        }
        else {
          break;
        }

        if((c & 0xC0) == 0x80) {
          codepoints[0] = (codepoints[0] << 6) + codepoints[1] - 0x3080;

          *arrLength = 1;
          *byteLength = 2;

          return;
        }

        /* prevent the potential first byte of the next valid
          utf-8 sequence also being consumed */
        ungetc(c, stream);
        byteIndex--;
      }
      else if (c < 0xF0) {
        /* read 3 bytes */
        if((c = fgetc(stream)) != EOF) {
          codepoints[++byteIndex] = c;
        }
        else {
          break;
        }

        if(
            (c & 0xC0) == 0x80 &&
            (bytes[0] != 0xE0 || c > 0x9F)
        ) {
          if((c = fgetc(stream)) != EOF) {
            codepoints[++byteIndex] = c;
          }
          else {
            break;
          }

          if((c & 0xC0) == 0x80) {
            codepoints[0] = (codepoints[0] << 12) + (codepoints[1] << 6) + codepoints[2] - 0xE2080;

            *arrLength = 1;
            *byteLength = 3;
            return;
          }
        }

        /* prevent the potential first byte of the next valid
          utf-8 sequence also being consumed */
        ungetc(c, stream);
        byteIndex--;
      }
      else if (c < 0xF5) {
        /* read 4 bytes */
        if((c = fgetc(stream)) != EOF) {
          codepoints[++byteIndex] = c;
        }
        else {
          break;
        }

        if(
            (c & 0xC0) == 0x80 &&
            (codepoints[0] != 0xF0 || c > 0x8F) &&
            (codepoints[0] != 0xF4 || c < 0x90) &&
        ) {
          if((c = fgetc(stream)) != EOF) {
            codepoints[++byteIndex] = c;
          }
          else {
            break;
          }

          if((c & 0xC0) == 0x80) {
            if((c = fgetc(stream)) != EOF) {
              codepoints[++byteIndex] = c;
            }
            else {
              break;
            }

            if((c & 0xC0) == 0x80) {
              codepoints[0] = (codepoints[0] << 18) + (codepoints[1] << 12) + (codepoints[2] << 6) + codepoints[3] - 0x3C82080;

              *arrLength = 1;
              *byteLength = 4;
              return;
            }
          }
        }

        /* prevent the potential first byte of the next valid
          utf-8 sequence also being consumed */
        ungetc(c, stream);
        byteIndex--;
      }
    }

    /* don't really loop. */
    break;
  }

  /* dump out the bytes matched, converting each to a separate codepoint */
  if(c == EOF) {
    codepoints[++byteIndex] = MYEOF;
  }

  *arrLength = *byteLength = byteIndex + 1;

  for( ; byteIndex > -1; byteIndex--) {
    /* use codepage 1252 conversions */
    codepoints[byteIndex] = cp1252[codepoints[byteIndex] - 0x80];
  }
}
