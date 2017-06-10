/*
  macos.c
  Macintosh Toolbox/Carbon support functions
  using only API compatible with System 1.0

  Use Macintosh Programmer's Workshop to build
  using the Makefile in this folder.

  Compiles for:
    m68k: Toolbox, System 1 - Mac OS 9.2.2
    ppc: Carbon CFM, Mac OS 8.1 - Mac OS X v10.6
    i386: Carbon Mach-O, Mac OS X v10.4 - 10.10

 */

#define OLDROUTINENAMES 1

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>

#if TARGET_API_MAC_CARBON

#if __MACH__

#include <Carbon/Carbon.h>
#else
#include <Carbon.h>
#endif

#else

#include <stdio.h>
#include <Quickdraw.h>
#include <MacWindows.h>
#include <Dialogs.h>
#include <Menus.h>
#include <ToolUtils.h>
#include <Devices.h>
#include <Drag.h>
#include <AppleEvents.h>
#include <Gestalt.h>
#include <DiskInit.h>
#include <Resources.h>
#include <TextUtils.h>
#include <Menus.h>
#include <ControlDefinitions.h>
#include <Scrap.h>
#endif

#include <MacTextEditor.h>

#include <SIO.h>

#define TARGET_API_MAC_TOOLBOX (!TARGET_API_MAC_CARBON)
#if TARGET_API_MAC_TOOLBOX
#define GetWindowPort(w) w
QDGlobals qd;   /* qd is needed by the Macintosh runtime */
#endif

#include "mac.h"

enum {
  sApplicationName = 1,
  sTranslationLockedErr,
  sTranslationErr,
  sOpeningErr,
  sReadErr,       // 5
  sWriteToBusyFileErr,
  sBusyOpen,
  sChooseFile,
  sChooseFolder,
  sChooseVolume,      // 10
  sCreateFolder,
  sChooseObject,
  sSaveCopyMessage,
  slSavePromptIndex,
  slClosingIndex,     // 15
  slQuittingIndex,
  sAddRemoveTitle,
  sLowMemoryErr
};

/*
  A DocumentRecord contains the WindowRecord for one of our document windows,
  as well as the TEHandle for the text we are editing. Other document fields
  can be added to this record as needed. For a similar example, see how the
  Window Manager and Dialog Manager add fields after the GrafPort.
*/
typedef struct {
  WindowRecord  docWindow;
  TEHandle      docTE;
  ControlHandle docVScroll;
  ControlHandle docHScroll;
  ProcPtr       docClik;
} DocumentRecord, *DocumentPeek;

struct lineOffsets {
  int lineLength;
  struct lineOffsets *nextLine;
};

struct lineOffsets *firstLine = NULL;
struct lineOffsets *lastLine = NULL;
SInt32 textUsed = 0;
int lineHeight2 = 10;

#define APP_NAME_STRING "\pQueryCSV"
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 320
#define BITMAP_SIZE 16
#define rAppStringsID     128
#define kOpenPrefKey      1
#define kMaxDocumentCount   100   // maximum number of documents allowed

#define TRUE 1
#define FALSE 0

/*
  Define TopLeft and BotRight macros for convenience. Notice the implicit
  dependency on the ordering of fields within a Rect
*/
#define TopLeft(aRect) (* (Point *) &(aRect).top)
#define BotRight(aRect) (* (Point *) &(aRect).bottom)

/* TODO: do something about these ugly global variables. What are they even for anyway? */
WindowPtr mainWindowPtr;
const short appleM = 0;
const short fileM = 1;
const short editM = 2;
const short fontM = 3;
const short sizeM = 4;

Boolean quit = false;
Boolean windowNotOpen = true;
char *progArg = NULL;
Str255 fontName;
int fontSizeIndex = 2;
int windowZoomed = 0;

/*
  gInBackground is maintained by our OSEvent handling routines. Any part of
  the program can check it to find out if it is currently in the background.
  It is maintained by Initialize and DoEvent
*/
Boolean gInBackground;

/*  kMaxDocWidth is an arbitrary number used to specify the width of the TERec's
  destination rectangle so that word wrap and horizontal scrolling can be
  demonstrated. */
#define kMaxDocWidth      576

/* kMinDocDim is used to limit the minimum dimension of a window when GrowWindow
  is called. */
//#define kMinDocDim        64

/*  kCrChar is used to match with a carriage return when calculating the
  number of lines in the TextEdit record. kDelChar is used to check for
  delete in keyDowns. */
//#define kCrChar         13
//#define kDelChar        8

/*  kControlInvisible is used to 'turn off' controls (i.e., cause the control not
  to be redrawn as a result of some Control Manager call such as SetCtlValue)
  by being put into the contrlVis field of the record. kControlVisible is used
  the same way to 'turn on' the control. */
//#define kControlInvisible   0
//#define kControlVisible     0xFF

/*  kScrollbarAdjust and kScrollbarWidth are used in calculating
  values for control positioning and sizing. */
//#define kScrollbarWidth     16
//#define kScrollbarAdjust    (kScrollbarWidth - 1)

/* kOSEvent is the event number of the suspend/resume and mouse-moved events sent
   by MultiFinder. Once we determine that an event is an OSEvent, we look at the
   high byte of the message sent to determine which kind it is. To differentiate
   suspend and resume events we check the resumeMask bit. */
#define kOSEvent        app4Evt /* event used by MultiFinder */
#define kSuspendResumeMessage 1   /* high byte of suspend/resume event message */
#define kResumeMask       1   /* bit of message field for resume vs. suspend */
#define kMouseMovedMessage    0xFA  /* high byte of mouse-moved event message */
#define kNoEvents       0   /* no events mask */

/*  kExtremeNeg and kExtremePos are used to set up wide open rectangles and regions. */
#define kExtremeNeg       -32768
#define kExtremePos       (32767 - 1) /* required to address an old region bug */

/*  kScrollTweek compensates for off-by-one requirements of the scrollbars
 to have borders coincide with the growbox. */
#define kScrollTweek      -2

/* 1.01 - changed constants to begin with 'k' for consistency, except for resource IDs */
/*  kTextMargin is the number of pixels we leave blank at the edge of the window. */
#define kTextMargin       2

/*  kButtonScroll is how many pixels to scroll horizontally when the button part
  of the horizontal scrollbar is pressed. */
#define kButtonScroll     4

WindowPtr mainWindowPtr = nil;

//  A reference to our assembly language routine that gets attached to the clikLoop
//  field of our TE record.
extern pascal void AsmClikLoop(void);

char *strdup(const char *s);

// ---------------------------------------------------------------------------
//      € stricmp
// ---------------------------------------------------------------------------
//  Compare two strings case insensitive.
//  Based on MSL version of strcmp, just added tolower() before comparing.

int stricmp(const char *str1, const char *str2) {
#if !__POWERPC__
  const unsigned char *p1 = (unsigned char *)str1;
  const unsigned char *p2 = (unsigned char *)str2;
  unsigned char c1, c2;

  while(tolower(c1 = *p1++) == tolower(c2 = *p2++)) {
    if(!c1) {
      return 0;
    }
  }
#else
  const unsigned char *p1 = (unsigned char *)str1-1;
  const unsigned char *p2 = (unsigned char *)str2-1;
  unsigned long c1, c2;

  while (tolower(c1 = *++p1) == tolower(c2 = *++p2)){
    if (!c1) {
      return(0);
    }
  }
#endif

  return(c1 - c2);
}

#undef putenv
int putenv(char* string) {
  char* key = strdup(string);
  char* value = (char *)strchr(key, (int)('='));
  
  if(value == NULL) {
    errno = ENOMEM;
  free(key);
  return -1;
  }
  
  value[0] = '\0';
  value++;
  
  setenv(key, value);
  
  free(key);
  return 0;
}


//Check to see if a window belongs to a desk accessory.
Boolean isDeskAccessory(WindowPtr window) {
  //DA windows have negative windowKinds
  return window == nil?false:(((WindowPeek)window)->windowKind < 0);
}

//Check to see if a window is an application one (???).
Boolean isApplicationWindow(WindowPtr window) {
  //application windows have windowKinds = userKind (8)
  return window == nil?false:(((WindowPeek)window)->windowKind == userKind);
}

Rect getWindowBounds(WindowPtr window) {
  Rect r;

#if TARGET_API_MAC_CARBON
  GetWindowPortBounds(window, &r);
#else
  r = qd.thePort->portRect;
#endif

  return r;
}

Rect getScreenBounds() {
  Rect r;

#if TARGET_API_MAC_CARBON
  BitMap theScreenBits;

  GetQDGlobalsScreenBits(&theScreenBits); //carbon accessor
  r = theScreenBits.bounds;
#else
  r = qd.screenBits.bounds;
#endif

  return r;
}

void alertUser(short error) {
  Str255  message;

  SetCursor(&qd.arrow);

  //type Str255 is an array in MPW 3
  GetIndString(message, kErrStrings, error);
  ParamText(message, (unsigned char *)"", (unsigned char *)"", (unsigned char *)"");
  Alert(rUserAlert, nil);
}

void alertUserNum(int value) {
  Str255 message;

  SetCursor(&qd.arrow);

  //type Str255 is an array in MPW 3
  sprintf((char*)&message, "test %d", value);
  c2pstr((char*)message);

  ParamText(message, (unsigned char *)"", (unsigned char *)"", (unsigned char *)"");
  Alert(rUserAlert, nil);
}

static pascal OSErr appleEventOpenApp(
    const AppleEvent *theAppleEvent,
    AppleEvent *reply,
    long handlerRefCon
) {
  return noErr;
}

static pascal OSErr appleEventPrintDoc(
    const AppleEvent *theAppleEvent,
    AppleEvent *reply,
    long handlerRefCon
) {
  return noErr;
}

CFURLRef baseFolder;
	SInt32			response;
  CFStringEncoding enc;

FILE *fopen_mac(const char *filename, const char *mode) {
  char* absolutePath = NULL;
  int retval;
  CFStringEncoding enc;
  CFIndex neededLen;
  CFIndex usedLen;
  CFRange range;

  CFStringRef text1 = CFStringCreateWithCStringNoCopy(
    NULL,
    filename,
    kCFStringEncodingUTF8,
    kCFAllocatorNull
  );

  if(text1 == NULL) {
    text1 = CFStringCreateWithCString(
      NULL,
      filename,
      kCFStringEncodingUTF8
    );

    if(text1 == NULL) {
      exit(EXIT_FAILURE);
    }
  }

  CFURLRef cfabsolute = CFURLCreateWithFileSystemPathRelativeToBase(
    kCFAllocatorDefault,
    text1,
    response < 0x01000 ? kCFURLHFSPathStyle : kCFURLPOSIXPathStyle,
    FALSE,
    baseFolder
  );

  CFStringRef text2 = CFURLCopyFileSystemPath(
    cfabsolute,
    response < 0x01000 ? kCFURLHFSPathStyle : kCFURLPOSIXPathStyle
  );

  if(absolutePath = CFStringGetCStringPtr(text2, enc)) {
    retval = fopen(absolutePath, mode);
  }
  else {
    neededLen = 0;
    usedLen = 0;
    range = CFRangeMake(0, CFStringGetLength(text2));

    CFStringGetBytes(
      text2,
      range,
      enc,
      '?',
      FALSE,
      NULL,
      0,
      &neededLen
    );

    reallocMsg(&absolutePath, neededLen + 1);

    CFStringGetBytes(
      text,
      range,
      enc,
      '?',
      FALSE,
      (UInt8 *)absolutePath,
      neededLen,
      &usedLen
    );

    absolutePath[usedLen] = 0;

    retval = fopen(absolutePath, mode);

    free(absolutePath);
  }

  CFRelease(text1);
  CFRelease(cfabsolute);
  CFRelease(text2);

  return retval;
}

static pascal OSErr appleEventOpenDoc(
    const AppleEvent *theAppleEvent,
    AppleEvent *reply,
    long handlerRefCon
) {
  AEDescList  docList;
  AEKeyword   keyword;
  DescType    returnedType;
  FSSpec      theFSSpec;
  FSRef       theFSRef;
  Size        actualSize;
  long        itemsInList;
  long        index;
  OSErr       result;
  Boolean     showMessage = false;
  char *text = NULL;
  char *text2 = NULL;
  char* fileName = NULL;

  if(!windowNotOpen) {
    alertUser(eFileOpen);
  }
  else {
    if(
        (result = AEGetParamDesc(
          theAppleEvent,
          keyDirectObject,
          typeAEList,
          &docList
        ) != 0) ||
        (result = AECountItems(&docList, &itemsInList)) != 0
    ) {
      return result;
    }

    if((result = AEGetNthPtr(
        &docList,
        1,
        typeFSRef,
        &keyword,
        &returnedType,
        (Ptr)&theFSRef,
        sizeof(FSRef),
        &actualSize
    )) == 0) {
      CFURLRef cfUrl = CFURLCreateFromFSRef(kCFAllocatorDefault, &theFSRef);
      
      if(cfUrl != NULL) {
        baseFolder = CFURLCreateCopyDeletingLastPathComponent(kCFAllocatorDefault, cfUrl);

        CFStringRef cfFilename = CFURLCopyLastPathComponent(cfUrl);

        if((absolutePath = CFStringGetCStringPtr(cfFilename, kCFStringEncodingUTF8)) == 0) {
          neededLen = 0;
          usedLen = 0;
          range = CFRangeMake(0, CFStringGetLength(cfFilename));
          mallocedFileName = TRUE;

          CFStringGetBytes(
            cfFilename,
            range,
            kCFStringEncodingUTF8,
            '?',
            FALSE,
            NULL,
            0,
            &neededLen
          );

          reallocMsg(&absolutePath, neededLen + 1);

          CFStringGetBytes(
            cfFilename,
            range,
            kCFStringEncodingUTF8,
            '?',
            FALSE,
            (UInt8 *)absolutePath,
            neededLen,
            &usedLen
          );

          absolutePath[usedLen] = 0;
        }

        CFRelease(cfFilename);
        CFRelease(cfUrl);

        progArg = absolutePath;

        windowNotOpen = false;
      }

      return noErr;
    }

    else if((result = AEGetNthPtr(
        &docList,
        1,
        typeFSS,
        &keyword,
        &returnedType,
        (Ptr)&theFSSpec,
        sizeof(FSSpec),
        &actualSize
    )) != 0) {
      return result;
    }

    result = HSetVol(0, theFSSpec.vRefNum, theFSSpec.parID);

    text = malloc(64); //SFReply.fName is a STR63, plus 1 for the null character

    if(text != NULL) {
      p2cstrcpy(text, theFSSpec.name);
      text2 = realloc(text, strlen(text)+1);

      if(text2 != NULL) {
        progArg = text2;

        windowNotOpen = false;
      }
      else {
        free(text);
      }
    }

    if(!windowNotOpen && itemsInList > 1) {
      alertUser(eFileOpen);
    }
  }

  return noErr;
}

static pascal OSErr appleEventQuit(
    const AppleEvent *appleEvt,
    AppleEvent* reply,
    long refcon
  ) {

  quit = true;
  return noErr;
}

void setupAppleEvents() {
  long result;

  if(
    Gestalt(gestaltAppleEventsAttr, &result) != noErr ||  //Problem calling Gestalt or
    (result & (1 << gestaltAppleEventsPresent)) == 0      //test the 0th bit of the result. If it is zero then Apple events is not available
  ) {
    return;
  }

  if(
    AEInstallEventHandler(
        kCoreEventClass,
        kAEOpenApplication,
        NewAEEventHandlerProc(appleEventOpenApp),
        0,
        false
      ) != noErr ||
    AEInstallEventHandler(
        kCoreEventClass,
        kAEOpenDocuments,
        NewAEEventHandlerProc(appleEventOpenDoc),
        0,
        false
      ) != noErr ||
    AEInstallEventHandler(
        kCoreEventClass,
        kAEPrintDocuments,
        NewAEEventHandlerProc(appleEventPrintDoc),
        0,
        false
      ) != noErr ||
    AEInstallEventHandler(
        kCoreEventClass,
        kAEQuitApplication,
        NewAEEventHandlerProc(appleEventQuit),
        0,
        false
      ) != noErr
  ) {
    raise(SIGABRT);
  }
}

void setupMenus() {
  MenuRef menu;
  MenuHandle myMenus[5];
  int i;

  myMenus[appleM] = GetMenu(mApple);

#if TARGET_API_MAC_TOOLBOX
  AddResMenu(myMenus[appleM],'DRVR'); // System-provided Desk Accessories menu
#endif

  myMenus[appleM] = GetMenu(mApple);
  myMenus[fileM]  = GetMenu(mFile);
  myMenus[editM]  = GetMenu(mEdit);
  myMenus[fontM]  = GetMenu(mFont);
  myMenus[sizeM]  = GetMenu(mSize);

  for(i = 0; i < 5; i++) {
    InsertMenu(myMenus[i], 0);
  }

#if TARGET_API_MAC_CARBON
  //In OS X, 'Quit' moves from File to the Application Menu
  if(
      Gestalt(gestaltMenuMgrAttr, &result) == noErr &&
      (result & gestaltMenuMgrAquaLayoutMask) != 0
    ) {
    menu = GetMenuHandle(mFile);
    DeleteMenuItem(menu, mFileQuit);
  }
#endif

  menu = GetMenuHandle(mFont);
  AppendResMenu(menu, 'FONT');

  DrawMenuBar();
}

void restoreSettings() {
  StringHandle strh;

  //whether the window is zoomed (in a bit of a roundabout way but I know this'll work).
  //The non zoomed window dimensions are loaded from & saved to the 'WIND' resource
  strh = GetString(rZoomPrefStr);
  if (strh != (StringHandle) nil) {
    memcpy(fontName, *strh, 256);
    p2cstr(fontName);
    windowZoomed = atoi((char *)fontName);
    ReleaseResource((Handle)strh);
  }

  //font size (in a bit of a roundabout way but I know this'll work)
  strh = GetString(rFontSizePrefStr);
  if (strh != (StringHandle) nil) {
    memcpy(fontName, *strh, 256);
    p2cstr(fontName);
    fontSizeIndex = atoi((char *)fontName);
    ReleaseResource((Handle)strh);
  }

  //font name
  strh = GetString(rFontPrefStr);
  if (strh != (StringHandle) nil) {
    memcpy(fontName, *strh, 256);
    ReleaseResource((Handle)strh);
  }
}

void saveSettings() {
  Str255 str;
  StringHandle strh;
  StringHandle strh2;

  //font name
  strh = GetString(rFontPrefStr);
  if (strh != (StringHandle) nil) {
    memcpy(*strh, fontName, 256);
    ChangedResource((Handle)strh);
    WriteResource((Handle)strh);
    ReleaseResource((Handle)strh);
  }

  //whether the window is zoomed (in a bit of a roundabout way but I know this'll work).
  //The non zoomed window dimensions are loaded from & saved to the 'WIND' resource
  strh2 = GetString(rZoomPrefStr);
  if (strh2 != (StringHandle) nil) {
    sprintf((char *)str, "%d", windowZoomed);
    c2pstr((char *)str);
    memcpy(*strh2, str, 256);
    ChangedResource((Handle)strh2);
    WriteResource((Handle)strh2);
    ReleaseResource((Handle)strh2);
  }

  //font size (in a bit of a roundabout way but I know this'll work)
  strh2 = GetString(rFontSizePrefStr);
  if (strh2 != (StringHandle) nil) {
    sprintf((char *)str, "%d", fontSizeIndex);
    c2pstr((char *)str);
    memcpy(*strh2, str, 256);
    ChangedResource((Handle)strh2);
    WriteResource((Handle)strh2);
    ReleaseResource((Handle)strh2);
  }
}

void initialize() {
#if TARGET_API_MAC_TOOLBOX
  MaxApplZone();
  InitGraf((Ptr)&qd.thePort);
  InitFonts();
  InitWindows();
  InitMenus();
  TEInit();
  InitDialogs(nil);
#endif

  InitCursor();

  FlushEvents(everyEvent, 0);

	/* get operating system version, used to tell whether os x is being used */
  Gestalt(gestaltSystemVersion, &response);

  /* get the system default encoding. used by the fopen wrapper */
  enc = CFStringGetSystemEncoding();

  setupAppleEvents();

  setupMenus();

  restoreSettings();

  gInBackground = false;
}

void adjustCursor(Point mouse, RgnHandle region) {
  WindowPtr   window;
  RgnHandle   arrowRgn;
  RgnHandle   iBeamRgn;
  Rect        iBeamRect;

  window = FrontWindow();   //we only adjust the cursor when we are in front

  if (!gInBackground && !isDeskAccessory(window)) {
    //calculate regions for different cursor shapes
    arrowRgn = NewRgn();
    iBeamRgn = NewRgn();

    //start arrowRgn wide open
    SetRectRgn(arrowRgn, kExtremeNeg, kExtremeNeg, kExtremePos, kExtremePos);

    //calculate iBeamRgn
    if (isApplicationWindow(window)) {
      /*iBeamRect = (*((DocumentPeek) window)->docTE)->viewRect;

      SetPort(window);        //make a global version of the viewRect
      LocalToGlobal(&TopLeft(iBeamRect));
      LocalToGlobal(&BotRight(iBeamRect));
      RectRgn(iBeamRgn, &iBeamRect);

      //we temporarily change the port's origin to 'globalfy' the visRgn
      SetOrigin(-window->portBits.bounds.left, -window->portBits.bounds.top);
      SectRgn(iBeamRgn, window->visRgn, iBeamRgn);
      SetOrigin(0, 0);
      */
    }

    //subtract other regions from arrowRgn
    DiffRgn(arrowRgn, iBeamRgn, arrowRgn);

    //change the cursor and the region parameter
    if (PtInRgn(mouse, iBeamRgn)) {
      SetCursor(*GetCursor(iBeamCursor));
      CopyRgn(iBeamRgn, region);
    }
    else {
      SetCursor(&qd.arrow);
      CopyRgn(arrowRgn, region);
    }

    DisposeRgn(arrowRgn);
    DisposeRgn(iBeamRgn);
  }
}

void adjustMenus() {
  MenuHandle menu;
  int i, len;
  Boolean found = false;
  Str255 currentFontName;
  TEHandle te;

  menu = GetMenuHandle(mFile);
  if(!windowNotOpen) {
    DisableItem(menu, mFileOpen);

    menu = GetMenuHandle(mEdit);
    EnableItem(menu, mEditSelectAll);

    te = ((DocumentPeek) mainWindowPtr)->docTE;

    if (!TXNIsSelectionEmpty( fMLTEObject )){//(*te)->selStart < (*te)->selEnd) {
      EnableItem(menu, mEditCopy);
    }
    else {
      DisableItem(menu, mEditCopy);
    }
  }

  menu = GetMenuHandle(mFont);
  for(i = 1, len = CountMItems(menu)+1; i < len; i++) {
    GetMenuItemText(menu, i, currentFontName);

    if(!found && EqualString(fontName, currentFontName, true, true)) {
      SetItemMark(menu, i, checkMark);
      found = true;
    }
    else {
      SetItemMark(menu, i, 0);
    }
  }

  if(!found) {
    GetMenuItemText(menu, 1, fontName);
    SetItemMark(menu, 1, checkMark);
  }

  found = false;
  menu = GetMenuHandle(mSize);
  for(i = 1, len = CountMItems(menu)+1; i < len; i++) {
    if(!found && fontSizeIndex == i) {
      SetItemMark(menu, i, checkMark);
      found = true;
    }
    else {
      SetItemMark(menu, i, 0);
    }
  }

  if(!found) {
    fontSizeIndex = 1;
    SetItemMark(menu, 1, checkMark);
  }
}

void adjustTE(WindowPtr window) {
  TEPtr te;

  te = *((DocumentPeek)window)->docTE;

  TEScroll(
    te->viewRect.left -
    te->destRect.left -
    GetControlValue(((DocumentPeek)window)->docHScroll),
    te->viewRect.top -
    te->destRect.top -
    (GetControlValue(((DocumentPeek)window)->docVScroll) * lineHeight2),
    ((DocumentPeek)window)->docTE
  );
}

void adjustViewRect(TEHandle docTE) {
  TEPtr te;

  te = *docTE;

  te->viewRect.bottom = (
    (
      (
        te->viewRect.bottom - te->viewRect.top
      ) / lineHeight2
    ) * lineHeight2
  ) +
  te->viewRect.top;
}

void adjustHV(Boolean isVert, ControlHandle control, TEHandle docTE, Boolean canRedraw) {
  short oldValue, oldMax, value, max, lines;
  TEPtr te = *docTE;

  oldValue = GetControlValue(control);
  oldMax = GetControlMaximum(control);

  if (isVert) {
    lines = te->nLines;

    if (*(*te->hText + te->teLength - 1) == kCrChar) {
      lines += 1;
    }

    max = lines - ((te->viewRect.bottom - te->viewRect.top) / lineHeight2);
  }
  else {
    max = kMaxDocWidth - (te->viewRect.right - te->viewRect.left);
  }

  if (max < 0) {
    max = 0;
  }

  SetControlMaximum(control, max);

  if (isVert) {
    value = (te->viewRect.top - te->destRect.top) / lineHeight2;
  }
  else {
    value = te->viewRect.left - te->destRect.left;
  }

  if (value < 0) {
    value = 0;
  }
  else if (value > max) {
    value = max;
  }

  SetControlValue(control, value);

  if (
      canRedraw ||
      max != oldMax ||
      value != oldValue
  ) {
    ShowControl(control);
  }
}

void adjustScrollbarValues(WindowPtr window, Boolean canRedraw) {
  DocumentPeek doc = (DocumentPeek)window;

  adjustHV(true,  doc->docVScroll, doc->docTE, canRedraw);
  adjustHV(false, doc->docHScroll, doc->docTE, canRedraw);
}

void getTERect(WindowPtr window, Rect *teRect) {
  *teRect = window->portRect;
  InsetRect(teRect, kTextMargin, kTextMargin);        // adjust for margin
  teRect->bottom = teRect->bottom - 15;                // and for the scrollbars
  teRect->right = teRect->right - 15;
}

void adjustScrollBars(WindowPtr window, Boolean needsResize) {
  DocumentPeek doc;
  Rect teRect;

  doc = (DocumentPeek) window;

  //First, turn visibility of scrollbars off so we won't get unwanted redrawing
  (*doc->docVScroll)->contrlVis = kControlInvisible;
  (*doc->docHScroll)->contrlVis = kControlInvisible;

  lineHeight2 = TEGetHeight(1, 1, doc->docTE);

  if (needsResize) {    //move & size as needed
    //start with TERect
    getTERect(window, &teRect);
    (*doc->docTE)->viewRect = teRect;

    //snap to nearest line
    adjustViewRect(doc->docTE);

    MoveControl(doc->docVScroll, window->portRect.right - kScrollbarAdjust, -1);
    SizeControl(
        doc->docVScroll,
        kScrollbarWidth,
        window->portRect.bottom - window->portRect.top - kScrollbarAdjust - kScrollTweek
      );

    MoveControl(doc->docHScroll, -1, window->portRect.bottom - kScrollbarAdjust);
    SizeControl(
        doc->docHScroll,
        window->portRect.right - window->portRect.left - kScrollbarAdjust - kScrollTweek,
        kScrollbarWidth
      );
  }

  //mess with max and current value
  adjustScrollbarValues(window, needsResize);

  //Now, restore visibility in case we never had to ShowControl during adjustment
  (*doc->docVScroll)->contrlVis = kControlVisible;
  (*doc->docHScroll)->contrlVis = kControlVisible;
}


void saveWindow(WindowPtr window) {
  Rect *rptr;
  Rect windRect;
  Handle wind;

  ZoomWindow(window, inZoomIn, window == FrontWindow());

  windRect = getWindowBounds(window);

  wind = (Handle)GetResource('WIND', rDocWindow);

  LocalToGlobal(&((Point)windRect));

  HLock(wind);
  rptr = (Rect *) *wind;

  rptr->top = windRect.top;
  rptr->left = windRect.left;
  rptr->bottom = windRect.top + windRect.bottom;
  rptr->right = windRect.left + windRect.right;

  ChangedResource(wind);
  HUnlock(wind);

  WriteResource(wind);
}

void closeWindow(WindowPtr window) {
  TEHandle te;

  if (isDeskAccessory(window)) {
    CloseDeskAcc(((WindowPeek) window)->windowKind);
  }
  else if (isApplicationWindow(window)) {
    te = ((DocumentPeek) window)->docTE;

    //if (te != nil) {
    //  // dispose the TEHandle if we got far enough to make one
    //  TEDispose(te);
    //}

    		// change for this do a TXNDeleteObject
			if ( fMLTEObject != nil )
				TXNDeleteObject(fMLTEObject);									// dispose the TEHandle if we got far enough to make one 


    //calling disposeWindow here would be technically incorrect,
    //even though we allocated storage for the window on the heap.
    //We instead call CloseWindow to have the structures taken
    //care of and then dispose of the storage ourselves.
    CloseWindow(window);
    DisposePtr((Ptr)window);

    //As we only only ever have 1 application window, if it's
    //closed then we quit the application
    quit = true;
  }
}

void getLocalUpdateRgn(WindowPtr window, RgnHandle localRgn) {
  // save old update region
  CopyRgn(((WindowPeek) window)->updateRgn, localRgn);

  OffsetRgn(localRgn, window->portBits.bounds.left, window->portBits.bounds.top);
}

void resizedWindow(WindowPtr window) {
  adjustScrollBars(window, true);
  adjustTE(window);
  InvalRect(&window->portRect);
}

void growWindow(WindowPtr window, EventRecord *event) {
  //change replace the guts of this function with a call to TXNGrowWindow
  TXNGrowWindow(fMLTEObject, event);
  /*
  long         growResult;
  Rect         tempRect;
  RgnHandle    tempRgn;
  DocumentPeek doc;

  //set up limiting values
  tempRect = getScreenBounds();

  tempRect.left = kMinDocDim;
  tempRect.top = kMinDocDim;

  growResult = GrowWindow(window, event->where, &tempRect);

  //see if it really changed size
  if (growResult != 0) {
    doc = (DocumentPeek) window;

    //save old text box
    tempRect = (*doc->docTE)->viewRect;
    tempRgn = NewRgn();

    //get localized update region
    getLocalUpdateRgn(window, tempRgn);
    SizeWindow(window, LoWord(growResult), HiWord(growResult), true);
    resizedWindow(window);

    //calculate & validate the region that hasn't changed so it won't get redrawn
    SectRect(&tempRect, &(*doc->docTE)->viewRect, &tempRect);

    //take it out of update
    ValidRect(&tempRect);

    //put back any prior update
    InvalRgn(tempRgn);

    DisposeRgn(tempRgn);
  }*/
}

void zoomWindow(WindowPtr window, short part) {
	//change replace the guts of this whole function with a call to TXNZoomWindow
  TXNZoomWindow( fMLTEObject,partCode);

  windowZoomed = (part == inZoomOut);
}

void openWindow() {
  Ptr storage;
  WindowPtr window;
  DocumentPeek doc;
  Rect viewRect, destRect;
  Boolean proceed;

  //Attempt to allocate some memory to bind the generic window to TextEdit functionality
  storage = NewPtr(sizeof(DocumentRecord));

  if (storage == nil) {
    //abort the program
    raise(SIGABRT);

    //The raising of the abort signal should mean we never get here, but just in case
    return;
  }

  //attempt to create the window that will contain program output
  window = GetNewWindow(rDocWindow, storage, (WindowPtr)-1);

  if (window == nil) {
    // get rid of the storage if it is never used
    DisposePtr(storage);

    // abort the program
    raise(SIGABRT);

    // The raising of the abort signal should mean we never get here, but just in case
    return;
  }

  //set the port (Mac OS boilerplate?)
  SetPort(GetWindowPort(window));

  //  cast the window instance into a DocumentPeek structure,
  //  so we can set up the TextEdit related fields
  doc = (DocumentPeek)window;

  /* TEXTEDIT STUFF begins here
  ******************************/
  proceed = false;

  //change 9 replace call to TENew with a call to TXNNewObject
  status = TXNNewObject(	
          NULL, /* can be NULL */
          GetWindowPort(window),
          NULL, /* can be NULL */
          kTXNShowWindowMask|kTXNWantHScrollBarMask|kTXNWantVScrollBarMask|kTXNSaveStylesAsSTYLResourceMask|kTXNDrawGrowIconMask,
          kTXNTextEditStyleFrameType, /* the only valid option */
          kTXNTextFile,
          kTXNUnicodeEncoding,
          &fMLTEObject,
          &fMLTEFrameID,
          NULL
         );



  // set up the textedit content size (not the viewport) rectangle
  //getTERect(window, &viewRect);
  //destRect = viewRect;
  //destRect.right = destRect.left + kMaxDocWidth;

  //attempt to create a TextEdit control and bind
  //it to our window/document structure
  //doc->docTE = TEStyleNew(&destRect, &viewRect);

  //only proceed if the TextEdit control was successfully created
  //proceed = doc->docTE != nil;

  /* if(proceed) {
    // fix up the TextEdit view?
    adjustViewRect(doc->docTE);
    TEAutoView(true, doc->docTE);   //TEAutoView controls automatic scrolling

    //backup the original click loop routine then substitute our own.
    //It seems our substitute routine has to be written in
    //assembly language as "registers need to be mucked with" (???)
    doc->docClik = (ProcPtr) (*doc->docTE)->clickLoop;
    (*doc->docTE)->clickLoop = (TEClickLoopUPP) AsmClikLoop;

    // Attempt to setup vertical scrollbar control
    doc->docVScroll = GetNewControl(rVScroll, window);
    proceed = (doc->docVScroll != nil);
  }

  if (proceed) {
    // Attempt to setup horizontal scrollbar control
    doc->docHScroll = GetNewControl(rHScroll, window);
    proceed = (doc->docHScroll != nil);
  }

  if (proceed) {
    //Adjust & draw the controls, then show the window.
    //False to adjustScrollValues means musn't redraw; technically, of course,
    //the window is hidden so it wouldn't matter whether we called ShowControl or not.
    adjustScrollBars(window, true);
    adjustScrollbarValues(window, false);
    ShowWindow(window);

    if(windowZoomed) {
      zoomWindow(window, inZoomOut);
    }

    mainWindowPtr = window;
  }
  else {*/
  		//change use of fDocTE to fMLTEObject and test the function result
		//also remove all the control calls and the call to TEAutoView
		if ( fMLTEObject == NULL || status != noErr )	{ // if TENew succeeded, we have a good document 

    //Something failed in the window creation process.
    //Clean up then tell the user what happened
    closeWindow(window);
    alertUser(eNoWindow);

    // abort the program
    raise(SIGABRT);
  }
}

void idleWindow() {
  WindowPtr window = FrontWindow();

  if(isApplicationWindow(window)) {
    //change: replace TEIdle with a call to TXNIdle
		TXNIdle(fMLTEObject);
  }
}

void repaintWindow(WindowPtr window) {
  if (isApplicationWindow(window)) {
    //change: replace all this with a call to TXNUpdate.  TXNUpdate calls BeginUpdate/EndUpdate
		//and handles drawing the scroll bars.
		TXNUpdate ( fMLTEObject );

    /*
    BeginUpdate(window);

    // draw if updating needs to be done
    if (!EmptyRgn(window->visRgn)) {
      SetPort(window);
      EraseRect(&window->portRect);
      DrawControls(window);
      DrawGrowIcon(window);
      TEUpdate(&window->portRect, ((DocumentPeek) window)->docTE);
    }

    EndUpdate(window);
    */
  }
}

void activateWindow(WindowPtr window, Boolean becomingActive) {
  RgnHandle     tempRgn, clipRgn;
  Rect          growRect;
  DocumentPeek  doc;

  if (isApplicationWindow(window)) {
    doc = (DocumentPeek)window;

		//change replace all this with a call to TXNFocus followed by a call to TXNActivate
		TXNFocus( fMLTEObject, becomingActive );
		TXNActivate( fMLTEObject, fMLTEFrameID, becomingActive);

    /*if (becomingActive) {
      //since we don't want TEActivate to draw a selection in an area where
      //we're going to erase and redraw, we'll clip out the update region
      //before calling it.
      tempRgn = NewRgn();
      clipRgn = NewRgn();

      // get localized update region
      getLocalUpdateRgn(window, tempRgn);
      GetClip(clipRgn);

      // subtract updateRgn from clipRgn
      DiffRgn(clipRgn, tempRgn, tempRgn);
      SetClip(tempRgn);
      TEActivate(doc->docTE);

      // restore the full-blown clipRgn
      SetClip(clipRgn);
      DisposeRgn(tempRgn);
      DisposeRgn(clipRgn);

      // the controls must be redrawn on activation:
      (*doc->docVScroll)->contrlVis = kControlVisible;
      (*doc->docHScroll)->contrlVis = kControlVisible;
      InvalRect(&(*doc->docVScroll)->contrlRect);
      InvalRect(&(*doc->docHScroll)->contrlRect);

      // the growbox needs to be redrawn on activation:
      growRect = window->portRect;

      // adjust for the scrollbars
      growRect.top = growRect.bottom - kScrollbarAdjust;
      growRect.left = growRect.right - kScrollbarAdjust;
      InvalRect(&growRect);
    }
    else {
      TEDeactivate(doc->docTE);
      // the controls must be hidden on deactivation:
      HideControl(doc->docVScroll);
      HideControl(doc->docHScroll);
      // the growbox should be changed immediately on deactivation:
      DrawGrowIcon(window);
    }*/
  }
}

//  Gets called from our assembly language routine, AsmClikLoop, which is in
//  turn called by the TEClick toolbox routine. Saves the windows clip region,
//  sets it to the portRect, adjusts the scrollbar values to match the TE scroll
//  amount, then restores the clip region.
pascal void PascalClikLoop(void) {
  WindowPtr window;
  RgnHandle region;

  window = FrontWindow();
  region = NewRgn();

  GetClip(region);
  ClipRect(&window->portRect);
  adjustScrollbarValues(window, true);
  SetClip(region);
  DisposeRgn(region);
}

void setFont(SInt16 menuItem) {
  TextStyle styleRec;

  GetMenuItemText(GetMenuHandle(mFont), menuItem, fontName);

  if(!mainWindowPtr) {
    return;
  }

  GetFNum(fontName, &(styleRec.tsFont));

  TESetSelect(0, 32767, ((DocumentPeek)mainWindowPtr)->docTE);
  TESetStyle(doFont, &styleRec, true, ((DocumentPeek)mainWindowPtr)->docTE);
  TESetSelect(32767, 32767, ((DocumentPeek)mainWindowPtr)->docTE);

  adjustScrollBars(mainWindowPtr, false);
}

short doGetSize(SInt16 menuItem) {
  switch(menuItem) {
    case 1:
      return 9;
    case 2:
      return 10;
    case 3:
      return 12;
    case 4:
      return 14;
    case 5:
      return 18;
    case 6:
      return 24;
    case 7:
      return 36;
  }

  return 10;
}

void setFontSize(SInt16 menuItem) {
  TextStyle styleRec;

  fontSizeIndex = menuItem;

  if(!mainWindowPtr) {
    return;
  }

  styleRec.tsSize = doGetSize(fontSizeIndex);

  TESetSelect(0, 32767, ((DocumentPeek)mainWindowPtr)->docTE);
  TESetStyle(doSize, &styleRec, true, ((DocumentPeek)mainWindowPtr)->docTE);
  TESetSelect(32767, 32767, ((DocumentPeek)mainWindowPtr)->docTE);

  adjustScrollBars(mainWindowPtr, false);
}

/*
  Gets called from our assembly language routine, AsmClikLoop, which is in
  turn called by the TEClick toolbox routine. It returns the address of the
  default clikLoop routine that was put into the TERec by TEAutoView to
  AsmClikLoop so that it can call it.
*/
pascal ProcPtr GetOldClikLoop(void) {
  return ((DocumentPeek)FrontWindow())->docClik;
}

/* Handles clicking on menus or keyboard shortcuts */
void menuSelect(long mResult) {
  short theItem;
  short theMenu;
  Str255 daName;

  theItem = LoWord(mResult);
  theMenu = HiWord(mResult);

  switch(theMenu) {
    case mApple: {
      if(theItem == mAppleAbout) {
        Alert(rAboutAlert, nil);
      }

#if TARGET_API_MAC_TOOLBOX
      else {
        /* all non-About items in this menu are Desk Accessories */
        /* type Str255 is an array in MPW 3 */
        GetMenuItemText(GetMenuHandle(mApple), theItem, daName);
        OpenDeskAcc(daName);
      }
#endif

    } break;

    case mFile: {
      switch(theItem) {
        case mFileOpen: {
          if(windowNotOpen) {
            openFileDialog();
          }
        } break;

        case mFileQuit: {
          quit = true;
        } break;
      }
    } break;

    case mEdit: {
      switch(theItem) {
        case mEditCopy: {
          //change: replace the guts of this with a call to TXNCopy
          //again TXNCopy returns an OSStatus which we are ignoring
          (void)TXNCopy( fMLTEObject );

          /*if (ZeroScrap() == noErr) {
            if(mainWindowPtr){
              TECopy(((DocumentPeek)mainWindowPtr)->docTE);
            }

            // after copying, export the TE scrap
            if (TEToScrap() != noErr) {
              //AlertUser(eNoCopy);
              ZeroScrap();
            }
          }*/

        } break;

        case mEditSelectAll: {
          TESetSelect(0, 32767, ((DocumentPeek)mainWindowPtr)->docTE);
        } break;
      }
    } break;

    case mFont: {
      setFont(theItem);
    } break;

    case mSize: {
      setFontSize(theItem);
    } break;
  }

  HiliteMenu(0);
}

void commonAction(ControlHandle control, short *amount) {
  short value, max;

  value = GetControlValue(control);        /* get current value */
  max = GetControlMaximum(control);        /* and maximum value */
  *amount = value - *amount;

  if (*amount < 0) {
    *amount = 0;
  }
  else if (*amount > max) {
    *amount = max;
  }

  SetControlValue(control, *amount);
  *amount = value - *amount;    /* calculate the real change */
}

pascal void VActionProc(ControlHandle control, short part) {
  short       amount;
  WindowPtr   window;
  TEPtr       te;

  /* if it was actually in the control */
  if (part != 0) {
    window = (*control)->contrlOwner;
    te = *((DocumentPeek) window)->docTE;

    switch (part) {
      case kControlUpButtonPart:        /* one line */
      case kControlDownButtonPart: {
        amount = 1;
      } break;

      case kControlPageUpPart:          /* one page */
      case kControlPageDownPart: {
        amount = (te->viewRect.bottom - te->viewRect.top) / lineHeight2;
      } break;
    }

    if (
        part == kControlDownButtonPart ||
        part == kControlPageDownPart
    ) {
      amount = -amount;                /* reverse direction for a downer */
    }

    commonAction(control, &amount);

    if (amount != 0) {
      TEScroll(0, amount * lineHeight2, ((DocumentPeek) window)->docTE);
    }
  }
}

/*
  Determines how much to change the value of the horizontal scrollbar by and how
  much to scroll the TE record.
*/
#pragma segment Main
pascal void HActionProc(ControlHandle control, short part) {
  short       amount;
  WindowPtr   window;
  TEPtr       te;

  if (part != 0) {
    window = (*control)->contrlOwner;
    te = *((DocumentPeek) window)->docTE;

    switch (part) {
      case kControlUpButtonPart:                /* a few pixels */
      case kControlDownButtonPart: {
        amount = kButtonScroll;
      } break;

      case kControlPageUpPart:                        /* a page */
      case kControlPageDownPart: {
        amount = te->viewRect.right - te->viewRect.left;
      } break;
    }

    if (
        part == kControlDownButtonPart ||
        part == kControlPageDownPart
    ) {
      amount = -amount;   /* reverse direction */
    }

    commonAction(control, &amount);

    if (amount != 0) {
      TEScroll(amount, 0, ((DocumentPeek) window)->docTE);
    }
  }
}

void contentClick(WindowPtr window, EventRecord *event) {
  Point           mouse;
  ControlHandle   control;
  short           part, value;
  Boolean         shiftDown;
  DocumentPeek    doc;
  Rect            teRect;

  if (isApplicationWindow(window)) {
    //change replace the guts with a call to TXNClick
		TXNClick(fMLTEObject, event);
    /* SetPort(window);

    /* get the click position * /
    mouse = event->where;
    GlobalToLocal(&mouse);

    doc = (DocumentPeek) window;

    /* see if we are in the viewRect. if so, we wonÕt check the controls * /
    getTERect(window, &teRect);

    if (PtInRect(mouse, &teRect)) {
      /* see if we need to extend the selection * /
      shiftDown = (event->modifiers & shiftKey) != 0;        /* extend if Shift is down * /
      TEClick(mouse, shiftDown, doc->docTE);
    }
    else {
      part = FindControl(mouse, window, &control);

      switch (part) {
        case 0: {
          /* do nothing for viewRect case * /
        } break;

        case kControlIndicatorPart: {
          value = GetControlValue(control);
          part = TrackControl(control, mouse, nil);

          if (part != 0) {
            value -= GetControlValue(control);

            /* value now has CHANGE in value; if value changed, scroll * /
            if (value != 0) {
              if (control == doc->docVScroll) {
                TEScroll(0, value * lineHeight2, doc->docTE);
              }
              else {
                TEScroll(value, 0, doc->docTE);
              }
            }
          }
        } break;

        default: {    /* they clicked in an arrow, so track & scroll * /
          if (control == doc->docVScroll) {
            value = TrackControl(control, mouse, (ControlActionUPP) VActionProc);
          }
          else {
            value = TrackControl(control, mouse, (ControlActionUPP) HActionProc);
          }
        } break;
      }
    } */
  }
}

/* Handles mouse down events */
void mouseClick(EventRecord *event) {
  Rect tempRect;
  WindowPtr window;
  short part = FindWindow(event->where, &window);

  switch(part) {
    case inContent: {
      if(window != FrontWindow()) {
        SelectWindow(window);
      }
      else {
        contentClick(window, event);
      }
    } break;

    case inMenuBar: {
      adjustMenus();
      menuSelect(MenuSelect(event->where));
    } break;

    case inDrag: {
      tempRect = getScreenBounds();
      DragWindow(window, event->where, &tempRect);
    } break;

    case inGoAway: {
      if(TrackGoAway(window, event->where)) {
        quit = true;
      }
    } break;

    case inGrow: {
      growWindow(window, event);
    } break;

    case inZoomIn:
    case inZoomOut: {
      if(TrackBox(window, event->where, part)) {
        zoomWindow(window, part);
      }
    } break;

#if TARGET_API_MAC_TOOLBOX
    case inSysWindow: { // Click happens in a Desk Accessory
      SystemClick(event, window);
    } break;
#endif
  }
}

void badMount(EventRecord *event) {
  Point pt = {70, 70};

  if((event->message & 0xFFFF0000) != noErr) {
    DILoad();
    DIBadMount(pt, event->message);
    DIUnload();
  }
}

void HandleEvent(EventRecord *pEvent) {
  switch(event->what) {
    case nullEvent: {
      idleWindow();
    } break;

    case kOSEvent: {
      switch ((event->message >> 24) & 0x0FF) {
        case kMouseMovedMessage: {
          idleWindow();
        } break;

        case kSuspendResumeMessage: {
          gInBackground = (event->message & kResumeMask) == 0;
          activateWindow(FrontWindow(), !gInBackground);
        } break;
      }
    } break;

    case updateEvt: {
      repaintWindow((WindowPtr)(event->message));
    } break;

    case mouseDown: {
      mouseClick(event);
    } break;

    case keyDown: {
      if(event->modifiers & cmdKey) {
        adjustMenus();
        menuSelect(MenuKey(event->message & charCodeMask));
      }
    } break;

    case activateEvt: {
      activateWindow((WindowPtr)(event->message), (event->modifiers & activeFlag) != 0);
    } break;

    case kHighLevelEvent: {
      AEProcessAppleEvent(event);
    } break;

    case diskEvt: {
      badMount(event);
    } break;
  }

  if(quit) {
    if(mainWindowPtr) {
      saveWindow(mainWindowPtr);
      closeWindow(mainWindowPtr);
    }

    saveSettings();
  }
}

void loopTick() {
  EventRecord event;

#if TARGET_API_MAC_TOOLBOX
  SystemTask();
#endif

  GetNextEvent(everyEvent, &event);

  HandleEvent(&event);
}

void macYield() {
  loopTick(); // get one event

  if(quit)  {
    ExitToShell(); // does not return to sioDemoRead...
  }
}

#if TARGET_API_MAC_TOOLBOX
#include <StandardFile.h>

OSStatus openFileDialog() {
  Point where;
  unsigned const char prompt = '\0';
  OSType typeList = 'TEXT';
  SFReply reply;
  OSErr result;
  char *text = NULL;
  char *text2 = NULL;

  where.h = where.v = 70;

  SFGetFile(where, &prompt, nil, 1, &typeList, nil, &reply);

  if(reply.good) {
    result = SetVol(NULL, reply.vRefNum);

    /* error check */
    if(result != 0) {

    }

    text = malloc(64); //SFReply.fName is a STR63, plus 1 for the null character

    if(text != NULL) {
      p2cstrcpy(text, reply.fName);
      text2 = realloc(text, strlen(text)+1);

      if(text2 != NULL) {
        progArg = text2;

        windowNotOpen = false;
      }
      else {
        free(text);
      }
    }
  }

  return noErr;
}
#endif

#if TARGET_API_MAC_CARBON
#include <Navigation.h>

static OSStatus SendOpenAE(AEDescList list) {
  OSStatus err;
  AEAddressDesc theAddress;
  AppleEvent dummyReply;
  AppleEvent theEvent;
  
  theAddress.descriptorType = typeNull;
  theAddress.dataHandle = NULL;

  dummyReply.descriptorType = typeNull;
  dummyReply.dataHandle = NULL;

  theEvent.descriptorType = typeNull;
  theEvent.dataHandle = NULL;

  for(;;) {
    ProcessSerialNumber psn;

    err = GetCurrentProcess(&psn);
    if(err != noErr) {
      break;
    }
    
    err = AECreateDesc(
      typeProcessSerialNumber,
      &psn,
      sizeof(ProcessSerialNumber),
      &theAddress
    );
    if(err != noErr) {
      break;
    }
      
    err = AECreateAppleEvent(
      kCoreEventClass,
      kAEOpenDocuments,
      &theAddress,
      kAutoGenerateReturnID,
      kAnyTransactionID,
      &theEvent
    );
    if(err != noErr) {
      break;
    }
    
    err = AEPutParamDesc(&theEvent, keyDirectObject, &list);
    if(err != noErr) {
      break;
    }
    
    err = AESend(
      &theEvent,
      &dummyReply,
      kAENoReply,
      kAENormalPriority,
      kAEDefaultTimeout,
      NULL,
      NULL
    );
    if(err != noErr) {
      break;
    }
  }
  
  if(theAddress.dataHandle != NULL) {
    AEDisposeDesc(&theAddress);
  }

  if(dummyReply.dataHandle != NULL) {
    AEDisposeDesc(&dummyReply);
  }

  if(theEvent.dataHandle != NULL) {
    AEDisposeDesc(&theEvent);
  }

  return err;
}


static NavDialogRef gOpenFileDialog = NULL;
static NavEventUPP gEventUPP = NULL;

static pascal void MyPrivateEventProc(
    NavEventCallbackMessage callbackSelector,
    NavCBRecPtr callbackParms,
    NavCallBackUserData callbackUD
  ) {

#pragma unused (callbackUD)

  switch (callbackSelector) {
    case kNavCBEvent: {
      switch(callbackParms->eventData.eventDataParms.event->what) {
        case updateEvt:
        case activateEvt: {
          HandleEvent(callbackParms->eventData.eventDataParms.event);
        } break;
      }
    } break;
    
    case kNavCBUserAction: {
      if(callbackParms->userAction == kNavUserActionOpen) {
        // This is an open files action, send an AppleEvent
        NavReplyRecord reply;
        OSStatus theErr = noErr;

        theErr = NavDialogGetReply(callbackParms->context, &reply);

        if(theErr == noErr) {
          SendOpenAE(reply.selection);

          NavDisposeReply(&reply);
        }
      }
    } break;

    case kNavCBTerminate: {
      if(callbackParms->context == gOpenFileDialog) {
        NavDialogDispose(gOpenFileDialog);
        gOpenFileDialog = NULL;
      }
    } break;
  }
}

void openFileDialog() {
  OSStatus theErr = noErr;
  NavDialogCreationOptions dialogOptions;
  NavTypeListHandle openList = NULL;
  
  if(gOpenFileDialog == NULL) {
    NavGetDefaultDialogCreationOptions(&dialogOptions);

    /*
      YUCK! NavTypeList works by using the 'struct hack'!
    */
    openList = (NavTypeListHandle)NewHandle(sizeof(NavTypeList) + sizeof(OSType));
    if (openList != NULL) {
      (*openList)->componentSignature = kNavGenericSignature;
      (*openList)->osTypeCount        = 2;
      (*openList)->osType[0]          = 'TEXT';
      (*openList)->osType[1]          = 'utxt';
    }

    if(gEventUPP == NULL) {
      gEventUPP = NewNavEventUPP(MyPrivateEventProc);
    }

    theErr = NavCreateGetFileDialog(
        &dialogOptions,
        openList,
        gEventUPP,
        NULL,
        NULL,
        NULL,
        &gOpenFileDialog
      );

    if (theErr == noErr) {
      theErr = NavDialogRun(gOpenFileDialog);
      
      if(theErr != noErr) {
        NavDialogDispose(gOpenFileDialog);
        gOpenFileDialog = NULL;
      }
    }

    if(openList != NULL) {
      DisposeHandle((Handle)openList);
    }
  }
  else if(NavDialogGetWindow(gOpenFileDialog) != NULL) {
    SelectWindow(NavDialogGetWindow(gOpenFileDialog));
  }
}
#endif


/*
  if you're looking for the main() function, this is it.
  the runtime libraries in MPW provide hooks that allow
  you to setup the mac boilerplate.
*/
pascal void sioDemoInit(int *mainArgc, char ***mainArgv) {
  static char progName[] = "querycsv";
  static char* argv[3];

  initialize();

  *mainArgc = 1;
  argv[0] = progName;
  argv[1] = NULL;
  argv[2] = NULL;
  *mainArgv = argv;

  while(!quit && windowNotOpen) {
    loopTick();
  }

  if(quit) {
    ExitToShell(); //raise(SIGABRT);
  }
  else if(!windowNotOpen) {
    openWindow();
    *mainArgc = 2;
    argv[1] = progArg;
  }
}

/* our program will be getting all its input from the file whose name is passed to main, so just return end of file */
pascal void sioDemoRead(
  char *buffer,
  long nCharsDesired,
  long *nCharsUsed,
  short *eofFlag
) {
  *eofFlag = true;
  *nCharsUsed = 0;
}

void output(char *buffer, SInt32 nChars, Boolean isBold) {
  char* startPoint = buffer;
  SInt32 lineChars = 0;
  SInt32 charsLeft = nChars;
  long temp;
  struct lineOffsets *temp2;
  TextStyle theStyle;
  TEHandle docTE;
  Boolean skipByte;

  if(!mainWindowPtr) {
    return;
  }

  docTE = ((DocumentPeek)mainWindowPtr)->docTE;

  //GetFNum(fontName, &(theStyle.tsFont));
  //theStyle.tsSize = doGetSize(fontSizeIndex);
  theStyle.tsFace = isBold?bold:normal;

  //first run initialization
  if(firstLine == NULL) {
    firstLine = (struct lineOffsets *)malloc(sizeof(struct lineOffsets));

    if(firstLine == NULL) {
      ExitToShell();
    }

    lastLine = firstLine;
    lastLine->lineLength = 0;
    lastLine->nextLine = NULL;
  }

  do {
    skipByte = false;
    
    //use funky for/switch construct to output/append until a newline or end of string
    for(;;) {
      if(charsLeft < 1) {
        break;
      }

      switch(startPoint[lineChars]) {
        case '\r':
          startPoint[lineChars] = '\n';

          if(startPoint[lineChars+1] == '\n') {
            skipByte = true;
          }
        case '\n':
          lineChars++;
        case '\0':
          charsLeft--;
          break;
        default:
          lineChars++;
          charsLeft--;
          continue;
      }

      break;
    }

    //While the line length plus the total length used is greater than 32767 and
    //there are lines to be removed (not the last line) then remove the first line
    while((temp = textUsed+lineChars) > 32767 && firstLine != lastLine) {
      TEAutoView(false, docTE);   //TEAutoView controls automatic scrolling
      TESetSelect(0, firstLine->lineLength, docTE);
      TEDelete(docTE);

      textUsed -= firstLine->lineLength;

      temp2 = firstLine;
      firstLine = firstLine->nextLine;
      free(temp2);
      temp2 = NULL;
      TEAutoView(true, docTE);   //TEAutoView controls automatic scrolling
    }

    //If the line length greater than 32767 then remove the last line of text.
    //Otherwise insert the text gathered.
    if((temp = lineChars+(lastLine->lineLength)) > 32767) {
      TEAutoView(false, docTE);   //TEAutoView controls automatic scrolling
      TESetSelect(0, lastLine->lineLength, docTE);
      TEDelete(docTE);
      lastLine->lineLength = 0;
      textUsed = 0;
      TEAutoView(true, docTE);   //TEAutoView controls automatic scrolling
    }
    else {
      TESetSelect(32767, 32767, docTE);
      TESetStyle(doFace, &theStyle, false, docTE);
      TEInsert(startPoint, lineChars, docTE);
      lastLine->lineLength = temp;
      textUsed += lineChars;
    }

    //allocate another line if one is needed
    if(startPoint[lineChars-1] == '\n' && lastLine->lineLength != 0) {
      lastLine->nextLine = (struct lineOffsets *)malloc(sizeof(struct lineOffsets));

      if(lastLine->nextLine == NULL) {
        ExitToShell();
      }

      lastLine = lastLine->nextLine;
      lastLine->lineLength = 0;
      lastLine->nextLine = NULL;
    }

    //update the starting point for the next line to be output
    startPoint = &(startPoint[lineChars]);

    //eat /n after /r (as in /r/n)
    if(skipByte) {
      startPoint++;
      charsLeft--;
    }
  } while (charsLeft > 0);   //any more characters to be output?

  adjustScrollBars(mainWindowPtr, false);
}


pascal void sioDemoWrite(SInt16 filenum, char *buffer, SInt32 nChars) {
  if(filenum == kSIOStdOutNum) {
    output(buffer, nChars, false);
  }
  else if(filenum == kSIOStdErrNum) {
    output(buffer, nChars, true);
  }

  loopTick(); // get one event

  if(quit)  {
    ExitToShell();  //raise(SIGABRT); // does not return to sioDemoRead...
  }
}


pascal void sioDemoExit(void) {
  free(progArg);

  while(!quit) {
    // loop until user quits.
    loopTick();
  };
}

pascal void (*__sioInit)(int *mainArgc, char ***mainArgv) = sioDemoInit;

pascal void (*__sioRead)(
    char    *buffer,
    SInt32  nCharsDesired,
    SInt32  *nCharsUsed,
    SInt16  *eofFlag
  ) = sioDemoRead;

pascal void (*__sioWrite)(SInt16 filenum, char *buffer, SInt32 nChars) = sioDemoWrite;
pascal void (*__sioExit)(void) = sioDemoExit;

/*int main(int argc, char **argv) {
  int size = 1024, pos;
  int c;
  char *buffer = (char *)malloc(size);

  FILE *f = fopen(argv[1], "rb");
  if(f) {
    do { // read all lines in file
      pos = 0;
      do{ // read one line
        c = fgetc(f);
        if(c != EOF) buffer[pos++] = (char)c;
        if(pos >= size - 1) { // increase buffer length - leave room for 0
          size *=2;
          buffer = (char*)realloc(buffer, size);
        }
      }while(c != EOF && c != '\n');
      buffer[pos] = 0;
      // line is now in buffer
      printf("%s", buffer);
    } while(c != EOF); 
    fclose(f);           
  }
  free(buffer);
  int i = 0;

  for(;;) {
    macYield();
    printf("%d\n", i++);
  }
  return 0;
}
*/
