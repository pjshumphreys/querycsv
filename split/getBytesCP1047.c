static const char cp1047LowBytes[160] = {
  0x00, 0x01, 0x02, 0x03, 0x37, 0x2D, 0x2E, 0x2F,
  0x16, 0x05, 0x25, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
  0x10, 0x11, 0x12, 0x13, 0x3C, 0x3D, 0x32, 0x26,
  0x18, 0x19, 0x3F, 0x27, 0x1C, 0x1D, 0x1E, 0x1F,
  0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,
  0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61,
  0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
  0xF8, 0xF9, 0x7A, 0x5E, 0x4C, 0x7E, 0x6E, 0x6F,
  0x7C, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7,
  0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6,
  0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6,
  0xE7, 0xE8, 0xE9, 0xAD, 0xE0, 0xBD, 0x5F, 0x6D,
  0x79, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
  0x88, 0x89, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96,
  0x97, 0x98, 0x99, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
  0xA7, 0xA8, 0xA9, 0xC0, 0x4F, 0xD0, 0xA1, 0x07,
  0x04, 0x06, 0x08, 0x09, 0x0A, 0x15, 0x14, 0x17,
  0x1A, 0x1B, 0x20, 0x21, 0x22, 0x23, 0x24, 0x28,
  0x29, 0x2A, 0x2B, 0x2C, 0x30, 0x31, 0x33, 0x34,
  0x35, 0x36, 0x38, 0x39, 0x3A, 0x3B, 0x3E, 0xFF
};

void getBytesCP1047(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  MAC_YIELD

  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    if(codepoint < 0xA0) {
      returnByte = cp1047LowBytes[codepoint];
      *bytes = &returnByte;
      return;
    }

    if(codepoint > 0xFF || !(*bytes = getBytesCommon(codepoint, 2))) {
      returnByte = 0x6f;  /* ebcidic question mark */
      *bytes = &returnByte;
    }
  }
}
