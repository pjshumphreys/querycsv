static const unsigned QCSV_SHORT petscii[128] = {
  0x0040, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
  0x0068, 0x0069, 0x006A, 0x006B, 0x006C, 0x006D, 0x006E, 0x006F,
  0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
  0x0078, 0x0079, 0x007A, 0x005B, 0x00A3, 0x005D, 0x2191, 0x2190,
  0x2500, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
  0x0048, 0x0049, 0x004A, 0x004B, 0x004C, 0x004D, 0x004E, 0x004F,
  0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
  0x0058, 0x0059, 0x005A, 0x253C, 0xFFFD, 0x2502, 0x2592, 0x2591,
  0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
  0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0x000A, 0xFFFD, 0xFFFD,
  0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
  0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD, 0xFFFD,
  0x00A0, 0x258C, 0x2584, 0x2594, 0x005F, 0x258F, 0xFFFD, 0x2595,
  0xFFFD, 0x2593, 0x007C, 0x251C, 0x2597, 0x2514, 0x2510, 0x2582,
  0x250C, 0x2534, 0x252C, 0x2524, 0x258E, 0x258D, 0xFFFD, 0xFFFD,
  0xFFFD, 0x2583, 0x2713, 0x2596, 0x259D, 0x2518, 0x2598, 0x259A
};

void getCodepointsPetscii(
    FILE *stream,
    long *codepoints,
    int *arrLength,
    int *byteLength
) {
  int c;

  MAC_YIELD

  if(stream == NULL) {
    *arrLength = *byteLength = 0;
    return;
  }

  *arrLength = *byteLength = 1;

  if((c = fgetc(stream)) == EOF) {
    *byteLength = 0;
    codepoints[0] = MYEOF;
    return;
  }

  if(c < 65) {
    codepoints[0] = (long)c;
    return;
  }

  if(c > 191) {
    codepoints[0] = (long)(0xFFFD);
    return;
  }

  codepoints[0] = (long)(petscii[c-64]);
}
