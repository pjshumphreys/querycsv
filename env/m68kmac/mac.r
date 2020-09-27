#include "SysTypes.r"
#include "Types.r"

/* these defines are used to set enable/disable flags of a menu */

#ifndef CREATOR
  #define CREATOR 'qcsv'
#endif

#define AllItems  0b1111111111111111111111111111111 /* 31 flags */
#define NoItems   0b0000000000000000000000000000000
#define MenuItem1 0b0000000000000000000000000000001
#define MenuItem2 0b0000000000000000000000000000010
#define MenuItem3 0b0000000000000000000000000000100
#define MenuItem4 0b0000000000000000000000000001000
#define MenuItem5 0b0000000000000000000000000010000
#define MenuItem6 0b0000000000000000000000000100000
#define MenuItem7 0b0000000000000000000000001000000
#define MenuItem8 0b0000000000000000000000010000000
#define MenuItem9 0b0000000000000000000000100000000
#define MenuItem10  0b0000000000000000000001000000000
#define MenuItem11  0b0000000000000000000010000000000
#define MenuItem12  0b0000000000000000000100000000000

#include "mac.h"

resource 'vers' (1) {
  0x01, 0x00, release, 0x00,
  verUS,
  "1.0",
  "1.0, Copyright \251 Paul Humphreys 2015-2020"
};

resource 'WIND' (rDocWindow, preload, purgeable) {
  {64, 60, 314, 460},
  zoomDocProc, invisible, goAway, 0x0, "QueryCSV", noAutoCenter
};

resource 'CNTL' (rVScroll, preload, purgeable) {
  {-1, 385, 236, 401},
  0, visible, 0, 0, scrollBarProc, 0, ""
};

resource 'CNTL' (rHScroll, preload, purgeable) {
  {235, -1, 251, 386},
  0, visible, 0, 0, scrollBarProc, 0, ""
};

resource 'MBAR' (kMBarDisplayed, preload)
{
  {
    mApple,
    mFile,
    mEdit,
    mFont,
    mSize
  }
};
resource 'MENU' (mApple, preload) {
  mApple, textMenuProc,
  AllItems & ~MenuItem2,  /* Disable dashed line, enable About and DAs */
  enabled, apple,
  {
    "About QueryCSV...",  /* use ... instead of … for compatibility with non-roman systems */
    noicon, nokey, nomark, plain;
    "-",
    noicon, nokey, nomark, plain;
  }
};

resource 'MENU' (mFile, preload)
{
  mFile,
  textMenuProc,
  AllItems,
  enabled,
  "File",
  {
    "Open...", noIcon, "O", noMark, plain;  /* use ... instead of … for compatibility with non-roman systems */
    "Quit", noIcon, "Q", noMark, plain;
  }
};

resource 'MENU' (mEdit, preload)
{
  mEdit,
  textMenuProc,
  NoItems,
  enabled,
  "Edit",
  {
    "Copy", noicon, "C", nomark, plain;
    "Select All", noicon, "A", nomark, plain;
  }
};

resource 'MENU' (mFont, preload)
{
  mFont,
  textMenuProc,
  AllItems,
  enabled,
  "Font",
  {

  }
};

resource 'MENU' (mSize, preload)
{
  mSize,
  textMenuProc,
  AllItems,
  enabled,
  "Size",
  {
  "9 Point",
    noicon, nokey, nomark, plain;
  "10 Point",
    noicon, nokey, nomark, plain;
  "12 Point",
    noicon, nokey, nomark, plain;
  "14 Point",
    noicon, nokey, nomark, plain;
  "18 Point",
    noicon, nokey, nomark, plain;
  "24 Point",
    noicon, nokey, nomark, plain;
  "36 Point",
    noicon, nokey, nomark, plain;
  }
};

resource 'STR#' (kErrStrings, purgeable) {
  {
    "You must run on 512Ke or later";
    "Application Memory Size is too small";
    "Not enough memory to run QueryCSV";
    "Not enough memory to do Cut";
    "Cannot do Cut";
    "Cannot do Copy";
    "Cannot do Clear";
    "Cannot exceed 32,000 characters with Paste";
    "Not enough memory to do Paste";
    "Cannot create window";
    "Cannot exceed 32,000 characters";
    "Cannot do Paste";
    "A file is already open in QueryCSV. Close that one first";
    "Cannot initialize Multilingual Text Editor";
    "Cannot attach Textension object to window";
    "Cannot activate/deactivate";
    "Textension object document not attached to window";
    "Cannot set the font you specified";
    "Cannot set the size you specified";
    "Cannot set the style you specified";
    "Cannot set the justification you specified";
    "Cannot dispose the Textension font menu object";
    "Cannot set the word wrap option you specified"
  }
};

resource 'ALRT' (rAboutAlert, purgeable) {
  {121, 136, 221, 376}, /* 240x100 */
  rAboutAlert,
  { /* array: 4 elements */
    /* [1] */
    OK, visible, silent,
    /* [2] */
    OK, visible, silent,
    /* [3] */
    OK, visible, silent,
    /* [4] */
    OK, visible, silent
  },
  centerMainScreen
};
resource 'DITL' (rAboutAlert, purgeable) {
  { /* array DITLarray: 3 elements */
    /* [1] */
    {70, 150, 90, 230},
    Button {
      enabled,
      "OK"
    },
    /* [2] */
    {10, 60, 65, 230},
    StaticText {
      disabled,
      "Version 1.0\n\251 Paul Humphreys 2015-2020"
    },
    /* [3] */
    {8, 8, 40, 40},
    Icon {
      disabled,
      1
    }
  }
};

resource 'ALRT' (rUserAlert, purgeable) {
  {65, 70, 166, 426},
  rUserAlert,
  { /* array: 4 elements */
    /* [1] */
    OK, visible, silent,
    /* [2] */
    OK, visible, silent,
    /* [3] */
    OK, visible, silent,
    /* [4] */
    OK, visible, silent
  },
  centerMainScreen
};

resource 'DITL' (rUserAlert, purgeable) {
  { /* array DITLarray: 3 elements */
    /* [1] */
    {70, 280, 90, 344},
    Button {
      enabled,
      "OK"
    },
    /* [2] */
    {10, 60, 65, 344},
    StaticText {
      disabled,
      "Error. ^0."
    },
    /* [3] */
    {8, 13, 40, 45},
    Icon {
      disabled,
      2
    }
  }
};

data 'STR ' (rFontPrefStr)
{
  $"064D 6F6E 6163 6F20 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 2020"
  $"2020 2020 2020 2020 2020 2020 2020 20"
};

resource 'STR ' (rFontSizePrefStr, preload, purgeable )
{
  "3   " /* 12 point */
};

resource 'STR ' (rZoomPrefStr, preload, purgeable )
{
  "0" /* not zoomed */
};


resource 'BNDL' (128) {
  CREATOR,  0,
  {
    'FREF',
    {
      0, 128,   /* APPL */
      1, 129    /* **** */
    },
    'ICN#',
    {
      0, 128,   /* APPL */
      1, 128    /* **** */
    },
  }
};

resource 'FREF' (128) {
  'APPL', 0,  ""
};

resource 'FREF' (129) {
  '****', 0,  ""
};

resource 'open' (128) {   /* for the Translation Manager aka Easy Open */
  CREATOR,
  {
    '****'
  }
};

data 'aete' (0, "Apple Events Terminology") {
  $"0090 0000 0000 0001 0E52 6571 7569 7265"            /* .ê.......Require */
  $"6420 5375 6974 652C 4576 656E 7473 2074"            /* d Suite,Events t */
  $"6861 7420 6576 6572 7920 6170 706C 6963"            /* hat every applic */
  $"6174 696F 6E20 7368 6F75 6C64 2073 7570"            /* ation should sup */
  $"706F 7274 7265 7164 0001 0001 0000 0000"            /* portreqd........ */
  $"0000 0000"                                          /* .... */
};

data 'ICN#' (128) {
  $"0000 0000 003F FE00 01FF FFC0 07FF FFF0"
  $"0FFE 1FF8 1FFD FFFC 1FFD FFFC 1FFD FFFC"
  $"07FE 1FF0 11FF FFC4 003F FE00 102A AA04"
  $"01D5 55C0 16A8 0AB4 0D51 E558 1AAA 0AAC"
  $"1551 C554 0AA8 2AA8 1553 C554 03A8 0AE0"
  $"1055 5504 003F FE00 11C0 01C4 0600 0030"
  $"1802 200C 1002 2004 1001 4004 0801 4008"
  $"0600 8030 01C0 01C0 003F FE00 0000 0000"
  $"0000 0000 003F FE00 01FF FFC0 07FF FFF0"
  $"0FFF FFF8 1FFF FFFC 1FFF FFFC 1FFF FFFC"
  $"1FFF FFFC 1FFF FFFC 1FFF FFFC 1FFF FFFC"
  $"1FFF FFFC 1FFF FFFC 1FFF FFFC 1FFF FFFC"
  $"1FFF FFFC 1FFF FFFC 1FFF FFFC 1FFF FFFC"
  $"1FFF FFFC 1FFF FFFC 1FFF FFFC 1FFF FFFC"
  $"1FFF FFFC 1FFF FFFC 1FFF FFFC 0FFF FFF8"
  $"07FF FFF0 01FF FFC0 003F FE00 0000 0000"
};

data 'icl8' (128) {
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 FFFF FFFF FFFF"
  $"FFFF FFFF FFFF FF00 0000 0000 0000 0000"
  $"0000 0000 0000 00FF FFFF 7F7F 7F7F 7F7F"
  $"7F7F 7F7F 7F7F 7FFF FFFF 0000 0000 0000"
  $"0000 0000 00FF FF7F 7F7F 7F7F 7F7F 7F7F"
  $"7F7F 7F7F 7F7F 7F7F 7F7F FFFF 0000 0000"
  $"0000 0000 FF7F 7F7F 7F7F 7F7F 7F7F 7F00"
  $"0000 007F 7F7F 7F7F 7F7F 7F7F FF00 0000"
  $"0000 00FF 7F7F 7F7F 7F7F 7F7F 7F7F 007F"
  $"7F7F 7F7F 7F7F 7F7F 7F7F 7F7F 7FFF 0000"
  $"0000 00FF 7F7F 7F7F 7F7F 7F7F 7F7F 007F"
  $"7F7F 7F7F 7F7F 7F7F 7F7F 7F7F 7FFF 0000"
  $"0000 00F6 FF7F 7F7F 7F7F 7F7F 7F7F 007F"
  $"7F7F 7F7F 7F7F 7F7F 7F7F 7F7F FFF6 0000"
  $"0000 0000 F6FF FF7F 7F7F 7F7F 7F7F 7F00"
  $"0000 007F 7F7F 7F7F 7F7F FFFF F600 0000"
  $"0000 00F6 00F6 00FF FFFF 7F7F 7F7F 7F7F"
  $"7F7F 7F7F 7F7F 7FFF FFFF 00F6 00F6 0000"
  $"0000 0000 F600 F600 F600 FFFF FFFF FFFF"
  $"FFFF FFFF FFFF FF00 F600 F600 F600 0000"
  $"0000 00F6 00F5 00F6 00F6 FAFA FAFA FAFA"
  $"FAFA FAFA FAFA FAF6 00F6 00F6 00F6 0000"
  $"0000 0000 F600 F6FF FFFF 5454 5454 5454"
  $"5454 5454 5454 54FF FFFF 0000 F600 0000"
  $"0000 00F6 00FF FF54 5454 5454 5454 547F"
  $"7F7F 7F54 5454 5454 5454 FFFF 00F6 0000"
  $"0000 0000 FF54 5454 5454 5454 5454 7F00"
  $"0000 007F 5454 5454 5454 5454 FF00 0000"
  $"0000 00FF 5454 5454 5454 5454 547F 007F"
  $"7F7F 7F7F 5454 5454 5454 5454 54FF 0000"
  $"0000 00FF 5454 5454 5454 5454 5454 7F00"
  $"0000 7F54 5454 5454 5454 5454 54FF 0000"
  $"0000 00F6 FF54 5454 5454 5454 5454 7F7F"
  $"7F7F 007F 5454 5454 5454 5454 FFF6 0000"
  $"0000 0000 F6FF FF54 5454 5454 547F 0000"
  $"0000 7F54 5454 5454 5454 FFFF F600 0000"
  $"0000 00F6 00F6 00FF FFFF 5454 5454 7F7F"
  $"7F7F 7F54 5454 54FF FFFF 00F6 00F6 0000"
  $"0000 0000 F600 F600 F600 FFFF FFFF FFFF"
  $"FFFF FFFF FFFF FF00 F600 F600 F600 0000"
  $"0000 00F6 00F6 00F6 00F6 F9F9 F9F9 F9F9"
  $"F9F9 F9F9 F9F9 F9F6 00F6 00F6 00F6 0000"
  $"0000 0000 F600 F6FF FFFF 9292 9292 9292"
  $"9292 9292 9292 92FF FFFF F600 F600 0000"
  $"0000 00F6 00FF FF92 9292 9292 9292 9F92"
  $"9292 9F92 9292 9292 9292 FFFF 00F6 0000"
  $"0000 0000 FF92 9292 9292 9292 929F 009F"
  $"929F 009F 9292 9292 9292 9292 FF00 0000"
  $"0000 00FF 9292 9292 9292 9292 929F 009F"
  $"929F 009F 9292 9292 9292 9292 92FF 0000"
  $"0000 00FF 9292 9292 9292 9292 9292 9F00"
  $"9F00 9F92 9292 9292 9292 9292 92FF 0000"
  $"0000 0000 FF92 9292 9292 9292 9292 9F00"
  $"9F00 9F92 9292 9292 9292 9292 FF00 0000"
  $"0000 0000 00FF FF92 9292 9292 9292 929F"
  $"009F 9292 9292 9292 9292 FFFF 0000 0000"
  $"0000 0000 0000 00FF FFFF 9292 9292 9292"
  $"9F92 9292 9292 92FF FFFF 0000 0000 0000"
  $"0000 0000 0000 0000 0000 FFFF FFFF FFFF"
  $"FFFF FFFF FFFF FF00 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
};

data 'icl4' (128) {
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 00FF FFFF FFFF FFF0 0000 0000"
  $"0000 000F FF66 6666 6666 666F FF00 0000"
  $"0000 0FF6 6666 6665 5556 6666 66FF 0000"
  $"0000 F666 6666 6650 0005 6666 6666 F000"
  $"000F 6666 6666 6505 5556 6666 6666 6F00"
  $"000F 6666 6666 6505 6666 6666 6666 6F00"
  $"000C F666 6666 6505 5556 6666 6666 FC00"
  $"0000 0FF6 6666 6650 0005 6666 66FF 0000"
  $"000C 000F FF66 6665 5556 666F FF00 0C00"
  $"0000 0000 00FF FFFF FFFF FFF0 0000 0000"
  $"000C 0000 00DD DDDD DDDD DDD0 0000 0C00"
  $"0000 000F FF77 7777 7777 777F FF00 0000"
  $"000C 0FF7 7777 777E EEE7 7777 77FF 0C00"
  $"0000 F777 7777 77E0 000E 7777 7777 F000"
  $"000F 7777 7777 7E0E EEE7 7777 7777 7F00"
  $"000F 7777 7777 77E0 00E7 7777 7777 7F00"
  $"000C F777 7777 77EE EE0E 7777 7777 FC00"
  $"0000 0FF7 7777 7E00 00E7 7777 77FF 0000"
  $"000C 000F FF77 77EE EEE7 777F FF00 0C00"
  $"0000 0000 00FF FFFF FFFF FFF0 0000 0000"
  $"000C 0000 00DD DDDD DDDD DDD0 0000 0C00"
  $"0000 000F FF8C 8C8C 8C8C 8C8F FF00 0000"
  $"000C 0FF8 C8C8 C8D8 C8D8 C8C8 C8FF 0C00"
  $"0000 FC8C 8C8C 8D0D 8D0D 8C8C 8C8C F000"
  $"000F C8C8 C8C8 CD0D CD0D C8C8 C8C8 CF00"
  $"000F 8C8C 8C8C 8CD0 D0DC 8C8C 8C8C 8F00"
  $"0000 F8C8 C8C8 C8D0 D0D8 C8C8 C8C8 F000"
  $"0000 0FFC 8C8C 8C8D 0D8C 8C8C 8CFF 0000"
  $"0000 000F FFC8 C8C8 D8C8 C8CF FF00 0000"
  $"0000 0000 00FF FFFF FFFF FFF0 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
};

data 'ics#' (128) {
  $"07F0 1FFC 3F3E 3EFE 1F3C 07F0 1414 2ACA"
  $"3496 19AC 07F0 180C 2142 2142 188C 07F0"
  $"07F0 1FFC 3FFE 3FFE 3FFE 3FFE 3FFE 3FFE"
  $"3FFE 3FFE 3FFE 3FFE 3FFE 3FFE 1FFC 07F0"
};

data 'ics8' (128) {
  $"0000 0000 00FF FFFF FFFF FFFF 0000 0000"
  $"0000 00FF FF7F 7F7F 7F7F 7F7F FFFF 0000"
  $"0000 FF7F 7F7F 7F7F 0101 7F7F 7F7F FF00"
  $"0000 FF7F 7F7F 7F01 7F7F 7F7F 7F7F FF00"
  $"0000 F6FF FC7F 7F7F 0101 7F7F FCFF F600"
  $"0000 00F6 00FC FCFC FCFC FCFC 00F6 0000"
  $"0000 F6FF FC54 5454 7F7F 5454 FCFF F600"
  $"0000 FF54 5454 547F 0101 7F54 5454 FF00"
  $"0000 FF54 5454 547F 017F 5454 5454 FF00"
  $"0000 00FF FC54 7F01 017F 5454 FCFF 0000"
  $"0000 F600 F6FB FBFB FBFB FBFB F600 F600"
  $"0000 00FF FC92 9F92 9F92 9F92 FCFF 0000"
  $"0000 FF92 9292 9F00 9F00 9F92 9292 FF00"
  $"0000 FF92 9292 9F00 9F00 9F92 9292 FF00"
  $"0000 00FF FF92 929F 009F 9292 FFFF 0000"
  $"0000 0000 00FF FFFF FFFF FFFF 0000 0000"
};

data 'ics4' (128) {
  $"0000 0FFF FFFF 0000 000F F666 5566 FF00"
  $"00F6 6665 0056 66F0 00F6 6650 5566 66F0"
  $"00CF E665 0056 EFC0 000C 0EEE EEEE 0C00"
  $"00CF E777 EE77 EFC0 00F7 777E 00E7 77F0"
  $"00F7 777E 0E77 77F0 000F E7E0 0E77 EF00"
  $"00C0 CEEE EEEE C0C0 000F E8DD DDD8 EF00"
  $"00FC 8CD0 D0DC 8CF0 00F8 C8D0 D0D8 C8F0"
  $"000F FC8D 0D8C FF00 0000 0FFF FFFF 0000"
};

resource 'CURS' (128) {
 $"07 C0 1F F0 3F F8 5F F4 4F E4 87 C2 83 82 81 02"
 $"83 82 87 C2 4F E4 5F F4 3F F8 1F F0 07 C0",
 $"07 C0 1F F0 3F F8 7F FC 7F FC FF FE FF FE FF FE"
 $"FF FE FF FE 7F FC 7F FC 3F F8 1F F0 07 C0",
 {7, 7}
};

resource 'CURS' (129) {
 $"07 C0 19 F0 21 F8 41 FC 41 FC 81 FE 81 FE FF FE"
 $"FF 02 FF 02 7F 04 7F 04 3F 08 1F 30 07 C0",
 $"07 C0 1F F0 3F F8 7F FC 7F FC FF FE FF FE FF FE"
 $"FF FE FF FE 7F FC 7F FC 3F F8 1F F0 07 C0",
 {7, 7}
};

resource 'CURS' (130) {
 $"07 C0 18 30 20 08 70 1C 78 3C FC 7E FE FE FF FE"
 $"FE FE FC 7E 78 3C 70 1C 20 08 18 30 07 C0",
 $"07 C0 1F F0 3F F8 7F FC 7F FC FF FE FF FE FF FE"
 $"FF FE FF FE 7F FC 7F FC 3F F8 1F F0 07 C0",
 {7, 7}
};

resource 'CURS' (131) {
 $"07 C0 1F 30 3F 08 7F 04 7F 04 FF 02 FF 02 FF FE"
 $"81 FE 81 FE 41 FC 41 FC 21 F8 19 F0 07 C0",
 $"07 C0 1F F0 3F F8 7F FC 7F FC FF FE FF FE FF FE"
 $"FF FE FF FE 7F FC 7F FC 3F F8 1F F0 07 C0",
 {7, 7}
};
