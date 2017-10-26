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

#include "powermac.h"

resource 'vers' (1) {
  0x02, 0x00, release, 0x00,
  verUS,
  "2.0",
  "2.0, Copyright \251 Paul Humphreys 2015-2017"
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
    "About QueryCSV...",  /* use ... instead of � for compatibility with non-roman systems */
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
    "Open...", noIcon, "O", noMark, plain;  /* use ... instead of � for compatibility with non-roman systems */
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
    "Multilingual Text Editor is unavailable on this system";
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
      "Version 2.0\n\251 Paul Humphreys 2015-2017"
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
  {40, 20, 150, 260},
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
    {80, 150, 100, 230},
    Button {
      enabled,
      "OK"
    },
    /* [2] */
    {10, 60, 60, 230},
    StaticText {
      disabled,
      "Error. ^0."
    },
    /* [3] */
    {8, 8, 40, 40},
    Icon {
      disabled,
      2
    }
  }
};

resource 'STR ' (rFontPrefStr, preload, purgeable )
{
  "                                                                                                                                                                                                                                                              "
};

resource 'STR ' (rFontSizePrefStr, preload, purgeable )
{
  "2   " /* 10 point */
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
  $"0090 0000 0000 0001 0E52 6571 7569 7265"            /* .�.......Require */
  $"6420 5375 6974 652C 4576 656E 7473 2074"            /* d Suite,Events t */
  $"6861 7420 6576 6572 7920 6170 706C 6963"            /* hat every applic */
  $"6174 696F 6E20 7368 6F75 6C64 2073 7570"            /* ation should sup */
  $"706F 7274 7265 7164 0001 0001 0000 0000"            /* portreqd........ */
  $"0000 0000"                                          /* .... */
};

data 'ICN#' (128) {
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 1FFF FF00"
  $"1000 0100 1FFF FF00 1000 0100 1000 0100"
  $"13FF 8100 1000 0100 11FF 8100 1000 3F80"
  $"107F C0C0 1000 8040 11FF 3020 1001 C810"
  $"10FE 7F8F 1002 3007 1001 0007 1000 8007"
  $"1FFF E007 0000 1FE7 0000 001F 0000 0007"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 1FFF FF00"
  $"1FFF FF00 1FFF FF00 1FFF FF00 1FFF FF00"
  $"1FFF FF00 1FFF FF00 1FFF FF00 1FFF FF80"
  $"1FFF FFC0 1FFF FFC0 1FFF FFE0 1FFF FFF0"
  $"1FFF FFFF 1FFF FFFF 1FFF FFFF 1FFF FFFF"
  $"1FFF FFFF 0000 1FFF 0000 001F 0000 0007"
};
data 'icl8' (128) {
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
  $"FFFF FFFF FFFF FFFF 0000 0000 0000 0000"
  $"0000 00FF 2B2B 2B2B 2B2B 2B2B 2B2B 2B2B"
  $"2B2B 2B2B 2B2B 2BFF 0000 0000 0000 0000"
  $"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
  $"FFFF FFFF FFFF FFFF 0000 0000 0000 0000"
  $"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
  $"F5F5 F5F5 F5F5 F5FF 0000 0000 0000 0000"
  $"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
  $"F5F5 F5F5 F5F5 F5FF 0000 0000 0000 0000"
  $"0000 00FF F5F5 FFFF FFFF FFFF FFFF FFFF"
  $"FFF5 F5F5 F5F5 F5FF 0000 0000 0000 0000"
  $"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
  $"F5F5 F5F5 F5F5 F5FF 0000 0000 0000 0000"
  $"0000 00FF F5F5 F5FF FFFF FFFF FFFF FFFF"
  $"FFF5 F5F5 F5F5 F5FF 0000 0000 0000 0000"
  $"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
  $"F5F5 FFFF FFFF FFFF F500 0000 0000 0000"
  $"0000 00FF F5F5 F5F5 F5FF FFFF FFFF FFFF"
  $"FFFF 0808 0808 0808 FF00 0000 0000 0000"
  $"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
  $"FF08 0808 0808 0808 08FF 0000 0000 0000"
  $"0000 00FF F5F5 F5FF FFFF FFFF FFFF FFFF"
  $"0808 FFFF 0808 0808 0808 FF00 0000 0000"
  $"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
  $"FFFF 0000 FF08 0808 0808 08FF 0000 0000"
  $"0000 00FF F5F5 F5F5 FFFF FFFF FFFF FF08"
  $"08FF FFFF FFFF FFFF FF08 0808 FFFF FFFF"
  $"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 FF08"
  $"0808 FFFF 0808 0808 0808 0808 08FF FFFF"
  $"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5FF"
  $"0808 0808 0808 0808 0808 0808 08FF FFFF"
  $"0000 00FF F5F5 F5F5 F5F5 F5F5 F5F5 F5F5"
  $"FF08 0808 0808 0808 0808 0808 08FF FFFF"
  $"0000 00FF FFFF FFFF FFFF FFFF FFFF FFFF"
  $"FFFF FF08 0808 0808 0808 0808 08FF FFFF"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 00FF FFFF FFFF FFFF FF08 08FF FFFF"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 00FF FFFF FFFF"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 00FF FFFF"
};

data 'icl4' (128) {
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"000F FFFF FFFF FFFF FFFF FFFF 0000 0000"
  $"000F CCCC CCCC CCCC CCCC CCCF 0000 0000"
  $"000F FFFF FFFF FFFF FFFF FFFF 0000 0000"
  $"000F C0C0 C0C0 C0C0 C0C0 C0CF 0000 0000"
  $"000F 0C0C 0C0C 0C0C 0C0C 0C0F 0000 0000"
  $"000F C0FF FFFF FFFF F0C0 C0CF 0000 0000"
  $"000F 0C0C 0C0C 0C0C 0C0C 0C0F 0000 0000"
  $"000F C0CF FFFF FFFF F0C0 C0CF 0000 0000"
  $"000F 0C0C 0C0C 0C0C 0CFF FFFF 0000 0000"
  $"000F C0CC CFFF FFFF FFF0 2020 F000 0000"
  $"000F 0C0C 0C0C 0C0C FF02 0202 0F00 0000"
  $"000F C0CF FFFF FFFF F0FF F020 20F0 0000"
  $"000F 0C0C 0C0C 0C0F FF00 FF02 020F 0000"
  $"000F C0C0 FFFF FFF0 2FFF FFFF F020 FFFF"
  $"000F 0C0C 0C0C 0CFF 02FF 0202 0202 0FFF"
  $"000F C0C0 C0C0 C0CF F020 2020 2020 2FFF"
  $"000F 0C0C 0C0C 0C0C FF02 0202 0202 0FFF"
  $"000F FFFF FFFF FFFF FFF0 2020 2020 2FFF"
  $"0000 0000 0000 0000 000F FFFF FFFF 0FFF"
  $"0000 0000 0000 0000 0000 0000 000F FFFF"
  $"0000 0000 0000 0000 0000 0000 0000 0FFF"
};

data 'ics#' (128) {
  $"0000 0000 0000 0000 0000 3FFC 2004 3FFC"
  $"2004 2E74 208C 2BE4 20B3 2043 3FFF 0007"
  $"0000 0000 0000 0000 0000 3FFC 3FFC 3FFC"
  $"3FFC 3FFC 3FFC 3FFC 3FFF 3FFF 3FFF 0007"
};

data 'ics8' (128) {
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 FFFF FFFF FFFF FFFF FFFF FFFF 0000"
  $"0000 FF00 0000 0000 0000 0000 00FF 0000"
  $"0000 FFFF FFFF FFFF FFFF FFFF FFFF 0000"
  $"0000 FF00 0000 0000 0000 0000 00FF 0000"
  $"0000 FF00 FFFF FF00 00FF FFFF 00FF 0000"
  $"0000 FF00 0000 0000 FF00 0000 FFFF 0000"
  $"0000 FF00 FF00 FFFF FFFF FF00 00FF 0000"
  $"0000 FF00 0000 0000 FF00 FFFF 0000 FFFF"
  $"0000 FF00 0000 0000 00FF 0000 0000 FFFF"
  $"0000 FFFF FFFF FFFF FFFF FFFF FFFF FFFF"
  $"0000 0000 0000 0000 0000 0000 00FF FFFF"
};

data 'ics4' (128) {
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 0000 0000 0000 0000"
  $"0000 0000 0000 0000 00FF FFFF FFFF FF00"
  $"00F0 0000 0000 0F00 00FF FFFF FFFF FF00"
  $"00F0 0000 0000 0F00 00F0 FFF0 0FFF 0F00"
  $"00F0 0000 F000 FF00 00F0 F0FF FFF0 0F00"
  $"00F0 0000 F0FF 00FF 00F0 0000 0F00 00FF"
  $"00FF FFFF FFFF FFFF 0000 0000 0000 0FFF"
};
