#include "querycsv.h"

static const struct codepointToBytes codepointBytes[212] = {
  {0x00A0, 0xFF, 0xFF, 0x41, 0xCA},
  {0x00A1, 0xAD, 0xAD, 0xAA, 0xC1},
  {0x00A2, 0x9B, 0xBD, 0x4A, 0xA2},
  {0x00A3, 0x9C, 0x9C, 0xB1, 0xA3},
  {0x00A4, 0x00, 0xCF, 0x9F, 0x00},
  {0x00A5, 0x9D, 0xBE, 0xB2, 0xB4},
  {0x00A6, 0x00, 0xDD, 0x6A, 0x00},
  {0x00A7, 0x00, 0xF5, 0xB5, 0xA4},
  {0x00A8, 0x00, 0xF9, 0xBB, 0xAC},
  {0x00A9, 0x00, 0xB8, 0xB4, 0xA9},
  {0x00AA, 0xA6, 0xA6, 0x9A, 0xBB},
  {0x00AB, 0xAE, 0xAE, 0x8A, 0xC7},
  {0x00AC, 0xAA, 0xAA, 0xB0, 0xC2},
  {0x00AD, 0x00, 0xF0, 0xCA, 0x00},
  {0x00AE, 0x00, 0xA9, 0xAF, 0xA8},
  {0x00AF, 0x00, 0xEE, 0xBC, 0xF8},
  {0x00B0, 0xF8, 0xF8, 0x90, 0xA1},
  {0x00B1, 0xF1, 0xF1, 0x8F, 0xB1},
  {0x00B2, 0xFD, 0xFD, 0xEA, 0x00},
  {0x00B3, 0x00, 0xFC, 0xFA, 0x00},
  {0x00B4, 0x00, 0xEF, 0xBE, 0xAB},
  {0x00B5, 0xE6, 0xE6, 0xA0, 0xB5},
  {0x00B6, 0x00, 0xF4, 0xB6, 0xA6},
  {0x00B7, 0xFA, 0xFA, 0xB3, 0xE1},
  {0x00B8, 0x00, 0xF7, 0x9D, 0xFC},
  {0x00B9, 0x00, 0xFB, 0xDA, 0x00},
  {0x00BA, 0xA7, 0xA7, 0x9B, 0xBC},
  {0x00BB, 0xAF, 0xAF, 0x8B, 0xC8},
  {0x00BC, 0xAC, 0xAC, 0xB7, 0x00},
  {0x00BD, 0xAB, 0xAB, 0xB8, 0x00},
  {0x00BE, 0x00, 0xF3, 0xB9, 0x00},
  {0x00BF, 0xA8, 0xA8, 0xAB, 0xC0},
  {0x00C0, 0x00, 0xB7, 0x64, 0xCB},
  {0x00C1, 0x00, 0xB5, 0x65, 0xE7},
  {0x00C2, 0x00, 0xB6, 0x62, 0xE5},
  {0x00C3, 0x00, 0xC7, 0x66, 0xCC},
  {0x00C4, 0x8E, 0x8E, 0x63, 0x80},
  {0x00C5, 0x8F, 0x8F, 0x67, 0x81},
  {0x00C6, 0x92, 0x92, 0x9E, 0xAE},
  {0x00C7, 0x80, 0x80, 0x68, 0x82},
  {0x00C8, 0x00, 0xD4, 0x74, 0xE9},
  {0x00C9, 0x90, 0x90, 0x71, 0x83},
  {0x00CA, 0x00, 0xD2, 0x72, 0xE6},
  {0x00CB, 0x00, 0xD3, 0x73, 0xE8},
  {0x00CC, 0x00, 0xDE, 0x78, 0xED},
  {0x00CD, 0x00, 0xD6, 0x75, 0xEA},
  {0x00CE, 0x00, 0xD7, 0x76, 0xEB},
  {0x00CF, 0x00, 0xD8, 0x77, 0xEC},
  {0x00D0, 0x00, 0xD1, 0xAC, 0x00},
  {0x00D1, 0xA5, 0xA5, 0x69, 0x84},
  {0x00D2, 0x00, 0xE3, 0xED, 0xF1},
  {0x00D3, 0x00, 0xE0, 0xEE, 0xEE},
  {0x00D4, 0x00, 0xE2, 0xEB, 0xEF},
  {0x00D5, 0x00, 0xE5, 0xEF, 0xCD},
  {0x00D6, 0x99, 0x99, 0xEC, 0x85},
  {0x00D7, 0x00, 0x9E, 0xBF, 0x00},
  {0x00D8, 0x00, 0x9D, 0x80, 0xAF},
  {0x00D9, 0x00, 0xEB, 0xFD, 0xF4},
  {0x00DA, 0x00, 0xE9, 0xFE, 0xF2},
  {0x00DB, 0x00, 0xEA, 0xFB, 0xF3},
  {0x00DC, 0x9A, 0x9A, 0xFC, 0x86},
  {0x00DD, 0x00, 0xED, 0xBA, 0x00},
  {0x00DE, 0x00, 0xE8, 0xAE, 0x00},
  {0x00DF, 0xE1, 0xE1, 0x59, 0xA7},
  {0x00E0, 0x85, 0x85, 0x44, 0x88},
  {0x00E1, 0xA0, 0xA0, 0x45, 0x87},
  {0x00E2, 0x83, 0x83, 0x42, 0x89},
  {0x00E3, 0x00, 0xC6, 0x46, 0x8B},
  {0x00E4, 0x84, 0x84, 0x43, 0x8A},
  {0x00E5, 0x86, 0x86, 0x47, 0x8C},
  {0x00E6, 0x91, 0x91, 0x9C, 0xBE},
  {0x00E7, 0x87, 0x87, 0x48, 0x8D},
  {0x00E8, 0x8A, 0x8A, 0x54, 0x8F},
  {0x00E9, 0x82, 0x82, 0x51, 0x8E},
  {0x00EA, 0x88, 0x88, 0x52, 0x90},
  {0x00EB, 0x89, 0x89, 0x53, 0x91},
  {0x00EC, 0x8D, 0x8D, 0x58, 0x93},
  {0x00ED, 0xA1, 0xA1, 0x55, 0x92},
  {0x00EE, 0x8C, 0x8C, 0x56, 0x94},
  {0x00EF, 0x8B, 0x8B, 0x57, 0x95},
  {0x00F0, 0x00, 0xD0, 0x8C, 0x00},
  {0x00F1, 0xA4, 0xA4, 0x49, 0x96},
  {0x00F2, 0x95, 0x95, 0xCD, 0x98},
  {0x00F3, 0xA2, 0xA2, 0xCE, 0x97},
  {0x00F4, 0x93, 0x93, 0xCB, 0x99},
  {0x00F5, 0x00, 0xE4, 0xCF, 0x9B},
  {0x00F6, 0x94, 0x94, 0xCC, 0x9A},
  {0x00F7, 0xF6, 0xF6, 0xE1, 0xD6},
  {0x00F8, 0x00, 0x9B, 0x70, 0xBF},
  {0x00F9, 0x97, 0x97, 0xDD, 0x9D},
  {0x00FA, 0xA3, 0xA3, 0xDE, 0x9C},
  {0x00FB, 0x96, 0x96, 0xDB, 0x9E},
  {0x00FC, 0x81, 0x81, 0xDC, 0x9F},
  {0x00FD, 0x00, 0xEC, 0x8D, 0x00},
  {0x00FE, 0x00, 0xE7, 0x8E, 0x00},
  {0x00FF, 0x98, 0x98, 0xDF, 0xD8},
  {0x0131, 0x00, 0xD5, 0x00, 0xF5},
  {0x0152, 0x00, 0x00, 0x00, 0xCE},
  {0x0153, 0x00, 0x00, 0x00, 0xCF},
  {0x0178, 0x00, 0x00, 0x00, 0xD9},
  {0x0192, 0x9F, 0x9F, 0x00, 0xC4},
  {0x02C6, 0x00, 0x00, 0x00, 0xF6},
  {0x02C7, 0x00, 0x00, 0x00, 0xFF},
  {0x02D8, 0x00, 0x00, 0x00, 0xF9},
  {0x02D9, 0x00, 0x00, 0x00, 0xFA},
  {0x02DA, 0x00, 0x00, 0x00, 0xFB},
  {0x02DB, 0x00, 0x00, 0x00, 0xFE},
  {0x02DC, 0x00, 0x00, 0x00, 0xF7},
  {0x02DD, 0x00, 0x00, 0x00, 0xFD},
  {0x0393, 0xE2, 0x00, 0x00, 0x00},
  {0x0398, 0xE9, 0x00, 0x00, 0x00},
  {0x03A3, 0xE4, 0x00, 0x00, 0x00},
  {0x03A6, 0xE8, 0x00, 0x00, 0x00},
  {0x03A9, 0xEA, 0x00, 0x00, 0xBD},
  {0x03B1, 0xE0, 0x00, 0x00, 0x00},
  {0x03B4, 0xEB, 0x00, 0x00, 0x00},
  {0x03B5, 0xEE, 0x00, 0x00, 0x00},
  {0x03C0, 0xE3, 0x00, 0x00, 0xB9},
  {0x03C3, 0xE5, 0x00, 0x00, 0x00},
  {0x03C4, 0xE7, 0x00, 0x00, 0x00},
  {0x03C6, 0xED, 0x00, 0x00, 0x00},
  {0x2013, 0x00, 0x00, 0x00, 0xD0},
  {0x2014, 0x00, 0x00, 0x00, 0xD1},
  {0x2017, 0x00, 0xF2, 0x00, 0x00},
  {0x2018, 0x00, 0x00, 0x00, 0xD4},
  {0x2019, 0x00, 0x00, 0x00, 0xD5},
  {0x201A, 0x00, 0x00, 0x00, 0xE2},
  {0x201C, 0x00, 0x00, 0x00, 0xD2},
  {0x201D, 0x00, 0x00, 0x00, 0xD3},
  {0x201E, 0x00, 0x00, 0x00, 0xE3},
  {0x2020, 0x00, 0x00, 0x00, 0xA0},
  {0x2021, 0x00, 0x00, 0x00, 0xE0},
  {0x2022, 0x00, 0x00, 0x00, 0xA5},
  {0x2026, 0x00, 0x00, 0x00, 0xC9},
  {0x2030, 0x00, 0x00, 0x00, 0xE4},
  {0x2039, 0x00, 0x00, 0x00, 0xDC},
  {0x203A, 0x00, 0x00, 0x00, 0xDD},
  {0x2044, 0x00, 0x00, 0x00, 0xDA},
  {0x207F, 0xFC, 0x00, 0x00, 0x00},
  {0x20A7, 0x9E, 0x00, 0x00, 0x00},
  {0x20AC, 0x00, 0x00, 0x00, 0xDB},
  {0x2122, 0x00, 0x00, 0x00, 0xAA},
  {0x2202, 0x00, 0x00, 0x00, 0xB6},
  {0x2206, 0x00, 0x00, 0x00, 0xC6},
  {0x220F, 0x00, 0x00, 0x00, 0xB8},
  {0x2211, 0x00, 0x00, 0x00, 0xB7},
  {0x2219, 0xF9, 0x00, 0x00, 0x00},
  {0x221A, 0xFB, 0x00, 0x00, 0xC3},
  {0x221E, 0xEC, 0x00, 0x00, 0xB0},
  {0x2229, 0xEF, 0x00, 0x00, 0x00},
  {0x222B, 0x00, 0x00, 0x00, 0xBA},
  {0x2248, 0xF7, 0x00, 0x00, 0xC5},
  {0x2260, 0x00, 0x00, 0x00, 0xAD},
  {0x2261, 0xF0, 0x00, 0x00, 0x00},
  {0x2264, 0xF3, 0x00, 0x00, 0xB2},
  {0x2265, 0xF2, 0x00, 0x00, 0xB3},
  {0x2310, 0xA9, 0x00, 0x00, 0x00},
  {0x2320, 0xF4, 0x00, 0x00, 0x00},
  {0x2321, 0xF5, 0x00, 0x00, 0x00},
  {0x2500, 0xC4, 0xC4, 0x00, 0x00},
  {0x2502, 0xB3, 0xB3, 0x00, 0x00},
  {0x250C, 0xDA, 0xDA, 0x00, 0x00},
  {0x2510, 0xBF, 0xBF, 0x00, 0x00},
  {0x2514, 0xC0, 0xC0, 0x00, 0x00},
  {0x2518, 0xD9, 0xD9, 0x00, 0x00},
  {0x251C, 0xC3, 0xC3, 0x00, 0x00},
  {0x2524, 0xB4, 0xB4, 0x00, 0x00},
  {0x252C, 0xC2, 0xC2, 0x00, 0x00},
  {0x2534, 0xC1, 0xC1, 0x00, 0x00},
  {0x253C, 0xC5, 0xC5, 0x00, 0x00},
  {0x2550, 0xCD, 0xCD, 0x00, 0x00},
  {0x2551, 0xBA, 0xBA, 0x00, 0x00},
  {0x2552, 0xD5, 0x00, 0x00, 0x00},
  {0x2553, 0xD6, 0x00, 0x00, 0x00},
  {0x2554, 0xC9, 0xC9, 0x00, 0x00},
  {0x2555, 0xB8, 0x00, 0x00, 0x00},
  {0x2556, 0xB7, 0x00, 0x00, 0x00},
  {0x2557, 0xBB, 0xBB, 0x00, 0x00},
  {0x2558, 0xD4, 0x00, 0x00, 0x00},
  {0x2559, 0xD3, 0x00, 0x00, 0x00},
  {0x255A, 0xC8, 0xC8, 0x00, 0x00},
  {0x255B, 0xBE, 0x00, 0x00, 0x00},
  {0x255C, 0xBD, 0x00, 0x00, 0x00},
  {0x255D, 0xBC, 0xBC, 0x00, 0x00},
  {0x255E, 0xC6, 0x00, 0x00, 0x00},
  {0x255F, 0xC7, 0x00, 0x00, 0x00},
  {0x2560, 0xCC, 0xCC, 0x00, 0x00},
  {0x2561, 0xB5, 0x00, 0x00, 0x00},
  {0x2562, 0xB6, 0x00, 0x00, 0x00},
  {0x2563, 0xB9, 0xB9, 0x00, 0x00},
  {0x2564, 0xD1, 0x00, 0x00, 0x00},
  {0x2565, 0xD2, 0x00, 0x00, 0x00},
  {0x2566, 0xCB, 0xCB, 0x00, 0x00},
  {0x2567, 0xCF, 0x00, 0x00, 0x00},
  {0x2568, 0xD0, 0x00, 0x00, 0x00},
  {0x2569, 0xCA, 0xCA, 0x00, 0x00},
  {0x256A, 0xD8, 0x00, 0x00, 0x00},
  {0x256B, 0xD7, 0x00, 0x00, 0x00},
  {0x256C, 0xCE, 0xCE, 0x00, 0x00},
  {0x2580, 0xDF, 0xDF, 0x00, 0x00},
  {0x2584, 0xDC, 0xDC, 0x00, 0x00},
  {0x2588, 0xDB, 0xDB, 0x00, 0x00},
  {0x258C, 0xDD, 0x00, 0x00, 0x00},
  {0x2590, 0xDE, 0x00, 0x00, 0x00},
  {0x2591, 0xB0, 0xB0, 0x00, 0x00},
  {0x2592, 0xB1, 0xB1, 0x00, 0x00},
  {0x2593, 0xB2, 0xB2, 0x00, 0x00},
  {0x25A0, 0xFE, 0xFE, 0x00, 0x00},
  {0x25CA, 0x00, 0x00, 0x00, 0xD7},
  {0xF8FF, 0x00, 0x00, 0x00, 0xF0},
  {0xFB01, 0x00, 0x00, 0x00, 0xDE},
  {0xFB02, 0x00, 0x00, 0x00, 0xDF}
};

char* getBytesCommon(long codepoint, int key) {
  struct codepointToBytes *lookup;

  if((lookup = (struct codepointToBytes*)bsearch(
      (void *)&codepoint,
      (void *)codepointBytes,
      212,
      sizeof(struct codepointToBytes),
      compareCodepoints
    )) == NULL) {
    returnByte = 0;
  }
  else switch(key) {
    case 0:
      returnByte = lookup->cp437;
    break;

    case 1:
      returnByte = lookup->cp850;
    break;

    case 2:
      returnByte = lookup->cp1047;

      return &returnByte; /* don't do the if statement below for cp1047 */
    break;

    case 3:
      returnByte = lookup->mac;
    break;
  }

  if(!returnByte) {
    returnByte = 0x3f;  /* ascii question mark */
    return &returnByte;
  }

  return &returnByte;
}

void getBytesCP437(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    if(codepoint < 0x80) {
      *bytes = NULL;
      return;
    }

    *bytes = getBytesCommon(codepoint, 0);
  }
}

void getBytesCP850(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    if(codepoint < 0x80) {
      *bytes = NULL;
      return;
    }

    *bytes = getBytesCommon(codepoint, 1);
  }
}

void getBytesMac(
    long codepoint,
    char **bytes,
    int *byteLength
) {
  if(byteLength != NULL && bytes != NULL) {
    *byteLength = 1;

    if(codepoint < 0x80) {
      *bytes = NULL;
      return;
    }

    *bytes = getBytesCommon(codepoint, 3);
  }
}

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
