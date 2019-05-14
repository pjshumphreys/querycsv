static const struct codepointToByte cp1252Bytes[SIZE_CP1252BYTES] = {
  {0x0081, 0x81}, {0x008D, 0x8D},
  {0x008F, 0x8F}, {0x0090, 0x90},
  {0x009D, 0x9D}, {0x0152, 0x8C},
  {0x0153, 0x9C}, {0x0160, 0x8A},
  {0x0161, 0x9A}, {0x0178, 0x9F},
  {0x017D, 0x8E}, {0x017E, 0x9E},
  {0x0192, 0x83}, {0x02C6, 0x88},
  {0x02DC, 0x98}, {0x2013, 0x96},
  {0x2014, 0x97}, {0x2018, 0x91},
  {0x2019, 0x92}, {0x201A, 0x82},
  {0x201C, 0x93}, {0x201D, 0x94},
  {0x201E, 0x84}, {0x2020, 0x86},
  {0x2021, 0x87}, {0x2022, 0x95},
  {0x2026, 0x85}, {0x2030, 0x89},
  {0x2039, 0x8B}, {0x203A, 0x9B},
  {0x20AC, 0x80}, {0x2122, 0x99}
};

#ifdef __Z88DK
  static struct codepointToByte* cp1252Bytes2[SIZE_CP1252BYTES];
#endif

void getBytesCP1252(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  struct codepointToByte *lookup;

  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    if(codepoint < 0x80 || (codepoint > 0x9F && codepoint < 0x100)) {
      *bytes = NULL;
      return;
    }

#ifdef __Z88DK
    if((lookup = (struct codepointToByte*)l_bsearch(
      (void *)&codepoint,
      (void *)cp1252Bytes2,
      SIZE_CP1252BYTES,
      compareCodepoints
    )) == NULL)
#else
    if((lookup = (struct codepointToByte*)bsearch(
      (void *)&codepoint,
      (void *)cp1252Bytes,
      SIZE_CP1252BYTES,
      sizeof(struct codepointToByte),
      compareCodepoints
    )) == NULL)
#endif
    {
      returnByte = 0x3f;  /* ascii question mark */
      *bytes = &returnByte;
      return;
    }

    returnByte = lookup->byte;  /* whatever the hash table lookup returned */
    *bytes = &returnByte;
  }
}
