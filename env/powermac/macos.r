
#include "SysTypes.r"
#include "Types.r"

/* these #defines are used to set enable/disable flags of a menu */

#define AllItems	0b1111111111111111111111111111111	/* 31 flags */
#define NoItems		0b0000000000000000000000000000000
#define MenuItem1	0b0000000000000000000000000000001
#define MenuItem2	0b0000000000000000000000000000010
#define MenuItem3	0b0000000000000000000000000000100
#define MenuItem4	0b0000000000000000000000000001000
#define MenuItem5	0b0000000000000000000000000010000
#define MenuItem6	0b0000000000000000000000000100000
#define MenuItem7	0b0000000000000000000000001000000
#define MenuItem8	0b0000000000000000000000010000000
#define MenuItem9	0b0000000000000000000000100000000
#define MenuItem10	0b0000000000000000000001000000000
#define MenuItem11	0b0000000000000000000010000000000
#define MenuItem12	0b0000000000000000000100000000000

#include "macos.h"

resource 'MBAR' (kMBarDisplayed, preload)
{
	{
		mApple,
		mFile,
    mEdit,
    mWindow
	}
};
resource 'vers' (1) {
	0x01, 0x00, release, 0x00,
	verUS,
	"1.0",
	"1.0, Copyright \251 Paul Humphreys 2016"
};

resource 'MENU' (mApple, preload) {
	mApple,
  textMenuProc,
	AllItems & ~MenuItem2,	/* Disable dashed line, enable About and DAs */
	enabled, apple,
	{
		"About QueryCSV�",
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
		"Open�",
    noIcon, "O", noMark, plain;
		"Quit",
    noIcon, "Q", noMark, plain;
	}
};

resource 'MENU' (mEdit, preload)
{
	mEdit,
	textMenuProc,
	AllItems,
	enabled,
	"Edit",
	{
		"Copy",
		noicon, "C", nomark, plain;
    "Select All",
		noicon, "A", nomark, plain;
	}
};

resource 'MENU' (mWindow, preload)
{
	mWindow,
	textMenuProc,
	AllItems,
	enabled,
	"Window",
	{
	}
};

resource 'BNDL' (128) {
  'QCSV',
  0,
  {
    'ICN#',
    {
      0, 128
    },
    'FREF',
    {
      10, 208
    }
  }
};

resource 'FREF' (208, purgeable) { 
  'TEXT',
  0,
  ""
};

resource 'open' (300, "Native document types for NavGetFile") {
	'CPAP',
	{
		'TEXT'
	}
};

resource 'ALRT' (rUserAlert, purgeable) {
	{121, 136, 221, 376}, /* 240x100 */
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
		{70, 150, 90, 230},
		Button {
			enabled,
			"OK"
		},
		/* [2] */
		{10, 60, 65, 230},
		StaticText {
			disabled,
			"Version 1.0\n\251 Paul Humphreys 2016"
		},
		/* [3] */
		{8, 8, 40, 40},
		Icon {
			disabled,
			1
		}
	}
};
