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
#define OLDP2C 1

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>

#ifdef TARGET_API_MAC_CARBON

#ifdef __MACH__
#include <Carbon/Carbon.h>
#else
#include <Carbon.h>
#endif

#else

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

int main(void);
void setupMLTE(void);
void alertUser(short error);
void loopTick(void);
void handleEvent(EventRecord *event);
void adjustCursor(RgnHandle region);
void growWindow(WindowPtr window, EventRecord *event);
void zoomWindow(WindowPtr window, short part);
void repaintWindow(WindowPtr window);
void activateWindow(WindowPtr window, Boolean becomingActive);
void contentClick( WindowPtr window, EventRecord *event);
void idleWindow(void);
void setupMenus(void);
void adjustMenus(void);
void menuSelect(long menuResult);
OSStatus openWindow(void);
Boolean closeWindow(WindowPtr window);
void terminate(void);
void BigBadError(short error);
Boolean isApplicationWindow(WindowPtr window);
Boolean isDeskAccessory(WindowPtr window);
void setupAppleEvents(void);
pascal OSErr appleEventOpenApp(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
    long handlerRefCon);
pascal OSErr appleEventOpenDoc(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
    long handlerRefCon);
pascal OSErr appleEventPrintDoc(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
    long handlerRefCon);
pascal OSErr appleEventQuit(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
    long handlerRefCon);

#define TARGET_API_MAC_TOOLBOX (!TARGET_API_MAC_CARBON)
#ifdef TARGET_API_MAC_TOOLBOX
#define GetWindowPort(w) w
QDGlobals qd;   /* qd is needed by the Macintosh runtime */
#endif

#include "powermac.h"

#undef main
int realmain(int argc, char **argv);

/*
  A DocumentRecord contains the WindowRecord for one of our document windows,
  as well as the TEHandle for the text we are editing. Other document fields
  can be added to this record as needed. For a similar example, see how the
  Window Manager and Dialog Manager add fields after the GrafPort.
*/
typedef struct {
#ifdef TARGET_API_MAC_CARBON
  TXNObject     docMLTEObject;
  TXNFrameID    docMLTEFrameID;
#else
  TEHandle      docTE;
  ControlHandle docVScroll;
  ControlHandle docHScroll;
  ProcPtr       docClik;
#endif
} DocumentRecord, *DocumentPeek;

struct lineOffsets {
  int lineLength;
  struct lineOffsets *nextLine;
};

struct lineOffsets *firstLine = NULL;
struct lineOffsets *lastLine = NULL;
SInt32 textUsed = 0;
int lineHeight2 = 10;

TXNFontMenuObject gTXNFontMenuObject;

const short kStartHierMenuID = 160;

NavDialogRef gOpenFileDialog = NULL;
NavEventUPP gEventUPP = NULL;

#define TRUE 1
#define FALSE 0

/*
  Define TopLeft and BotRight macros for convenience. Notice the implicit
  dependency on the ordering of fields within a Rect
*/
#define TopLeft(aRect) (* (Point *) &(aRect).top)
#define BotRight(aRect) (* (Point *) &(aRect).bottom)

/* TODO: do something about these ugly global variables. What are they even for anyway? */
const short appleM = 0;
const short fileM = 1;
const short editM = 2;
const short fontM = 3;
const short sizeM = 4;

Boolean quit = FALSE;
Boolean windowNotOpen = TRUE;
char *progArg = NULL;
Str255 fontName;
int fontSizeIndex = 2;
int windowZoomed = 0;
extern char * devNull;

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
#define	kMinDocDim				64

/*  kCrChar is used to match with a carriage return when calculating the
  number of lines in the TextEdit record. kDelChar is used to check for
  delete in keyDowns. */
#define kCrChar					13
#define kDelChar				8

/*  kControlInvisible is used to 'turn off' controls (i.e., cause the control not
  to be redrawn as a result of some Control Manager call such as SetCtlValue)
  by being put into the contrlVis field of the record. kControlVisible is used
  the same way to 'turn on' the control. */
#define kControlInvisible		0
#define kControlVisible			0xFF

/*  kScrollbarAdjust and kScrollbarWidth are used in calculating
  values for control positioning and sizing. */
#define kScrollbarWidth			16
#define kScrollbarAdjust		(kScrollbarWidth - 1)

/* kOSEvent is the event number of the suspend/resume and mouse-moved events sent
   by MultiFinder. Once we determine that an event is an OSEvent, we look at the
   high byte of the message sent to determine which kind it is. To differentiate
   suspend and resume events we check the resumeMask bit. */
#define	kOSEvent				app4Evt	/* event used by MultiFinder */
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

/*
  gNumDocuments is used to keep track of how many open documents there are
  at any time. It is maintained by the routines that open and close documents.
  It is maintained by Initialize, DoNew, and DoCloseWindow
*/
short gNumDocuments;

WindowPtr mainWindowPtr = NULL;
DocumentRecord mainWindowDoc;

#define ENC_UTF16BE 9
char *d_charsetEncode(char* s, int encoding, size_t *bytesStored);
OSStatus openFileDialog(void);

/* how to treat filenames that are fopened */
CFURLRef baseFolder;
SInt32 macOSVersion;
CFStringEncoding enc;
int mallocedFileName;

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

//Check to see if a window belongs to a desk accessory.
Boolean isDeskAccessory(WindowPtr window) {
  //DA windows have negative windowKinds
  return (window == NULL) && (GetWindowKind(window) < 0);
}

//Check to see if a window is an application one (???).
Boolean isApplicationWindow(WindowPtr window) {
  //application windows have windowKinds = userKind (8)
  return (window != NULL) && (GetWindowKind(window) == userKind);
}

#if TARGET_API_MAC_CARBON
TXNObject *getTXNObject(WindowPtr window, TXNObject *object) {
  GetWindowProperty(window, 'GRIT', 'tObj', sizeof(TXNObject), NULL, object);

  return object;
}
#else
TEHandle getTEHandle(WindowPtr window) {
  return ((DocumentPeek)window)->docTE;
}
#endif

Rect getWindowBounds(WindowPtr window) {
  Rect r;

#if TARGET_API_MAC_CARBON
  GetWindowPortBounds(window, &r);
#else
  r = qd.thePort->portRect;
#endif

  return r;
}

Rect getScreenBounds(void) {
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
  Str255 message;

#if TARGET_API_MAC_CARBON
  Cursor theArrow;
  GetQDGlobalsArrow(&theArrow);
  SetCursor(&theArrow);
#else
  SetCursor(&qd.arrow);
#endif

  //type Str255 is an array in MPW 3
  GetIndString(message, kErrStrings, error);
  ParamText(message, (unsigned char *)"", (unsigned char *)"", (unsigned char *)"");
  Alert(rUserAlert, NULL);
}

void alertUserNum(int value) {
  Str255 message;

#if TARGET_API_MAC_CARBON
  Cursor theArrow;
  GetQDGlobalsArrow(&theArrow);
  SetCursor(&theArrow);
#else
  SetCursor(&qd.arrow);
#endif

  //type Str255 is an array in MPW 3
  sprintf((char*)&message, "test %d", value);
  c2pstr((char*)message);

  ParamText(message, (unsigned char *)"", (unsigned char *)"", (unsigned char *)"");
  Alert(rUserAlert, NULL);
}

void BigBadError(short error) {
  alertUser(error);
  ExitToShell();
}

pascal OSErr appleEventOpenApp(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
    long handlerRefCon
) {
#pragma unused (theAppleEvent, reply, handlerRefCon)
  return noErr;
}

pascal OSErr appleEventPrintDoc(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
    long handlerRefCon
) {
#pragma unused (theAppleEvent, reply, handlerRefCon)
  return errAEEventNotHandled;
}

pascal OSErr appleEventOpenDoc(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
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
  Boolean     showMessage = FALSE;
  char *text = NULL;
  char *text2 = NULL;
  char* fileName = NULL;
  CFURLRef cfUrl;
  CFStringRef cfFilename;
  CFIndex neededLen;
  CFIndex usedLen;
  CFRange range;

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
      cfUrl = CFURLCreateFromFSRef(kCFAllocatorDefault, &theFSRef);

      if(cfUrl != NULL) {
        baseFolder = CFURLCreateCopyDeletingLastPathComponent(kCFAllocatorDefault, cfUrl);

        cfFilename = CFURLCopyLastPathComponent(cfUrl);

        if((fileName = (char *)CFStringGetCStringPtr(cfFilename, kCFStringEncodingUTF8)) == NULL) {
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

          reallocMsg((void**)&fileName, neededLen + 1);

          CFStringGetBytes(
            cfFilename,
            range,
            kCFStringEncodingUTF8,
            '?',
            FALSE,
            (UInt8 *)fileName,
            neededLen,
            &usedLen
          );

          fileName[usedLen] = 0;
        }

        CFRelease(cfFilename);
        CFRelease(cfUrl);

        progArg = fileName;

        windowNotOpen = FALSE;
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

        windowNotOpen = FALSE;
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

pascal OSErr appleEventQuit(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
    long handlerRefCon
) {
#pragma unused (theAppleEvent, reply, handlerRefCon)
  quit = TRUE;
  return noErr;
}

void setupAppleEvents(void) {
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
        NewAEEventHandlerUPP(appleEventOpenApp),
        0,
        FALSE
      ) != noErr ||
    AEInstallEventHandler(
        kCoreEventClass,
        kAEOpenDocuments,
        NewAEEventHandlerUPP(appleEventOpenDoc),
        0,
        FALSE
      ) != noErr ||
    AEInstallEventHandler(
        kCoreEventClass,
        kAEPrintDocuments,
        NewAEEventHandlerUPP(appleEventPrintDoc),
        0,
        FALSE
      ) != noErr ||
    AEInstallEventHandler(
      kCoreEventClass,
      kAEQuitApplication,
      NewAEEventHandlerUPP(appleEventQuit),
      0,
      FALSE
    ) != noErr
  ) {
    ExitToShell();
  }
}

void setupMLTE(void) {
  OSStatus status;
  TXNMacOSPreferredFontDescription defaults;
  TXNInitOptions options;

  if(TXNVersionInformation == (void*)kUnresolvedCFragSymbolAddress) {
    BigBadError(eWrongSystem);
  }

  defaults.fontID = kTXNDefaultFontName;
  defaults.pointSize = kTXNDefaultFontSize;

  defaults.encoding = CreateTextEncoding(
    kTextEncodingMacRoman,
    kTextEncodingDefaultVariant,
    kTextEncodingDefaultFormat
  );

  defaults.fontStyle  = kTXNDefaultFontStyle;

  options = kTXNWantMoviesMask | kTXNWantSoundMask | kTXNWantGraphicsMask;

  status = TXNInitTextension(&defaults, 1, options);

  if(status != noErr) {
    BigBadError(eNoMLTE);
  }
}

void setupMenus(void) {
  MenuRef menu;
  MenuHandle myMenus[5];
  int i;
  long result;
  OSStatus err;

  myMenus[appleM] = GetMenu(mApple);

#if TARGET_API_MAC_TOOLBOX
  AddResMenu(myMenus[appleM], 'DRVR'); // System-provided Desk Accessories menu
#endif

  myMenus[fileM]  = GetMenu(mFile);
  myMenus[editM]  = GetMenu(mEdit);
  myMenus[fontM]  = GetMenu(mFont);
  myMenus[sizeM]  = GetMenu(mSize);

  for(i = 0; i < 5; i++) {
    InsertMenu(myMenus[i], 0);
  }

  macOSVersion = 0x0860;
  devNull = "Dev:Null";   /* null filename on MacOS Classic (i.e. pre OS X) */

#if TARGET_API_MAC_CARBON
  //In OS X, 'Quit' moves from File to the Application Menu
  if(
      Gestalt(gestaltMenuMgrAttr, &result) == noErr &&
      (result & gestaltMenuMgrAquaLayoutMask) != 0
    ) {
    menu = GetMenuHandle(mFile);
    DeleteMenuItem(menu, mFileQuit);
    macOSVersion = 0x1000;
    devNull = "/dev/null";  /* needed as the carbon build can run on OS X */
  }
#endif

  err = TXNNewFontMenuObject(
    GetMenuHandle(mFont),
    mFont,
    kStartHierMenuID,
    &gTXNFontMenuObject
  );

  if(err != noErr) {
    BigBadError(eNoMemory);
  }

  DrawMenuBar();
}

void restoreSettings(void) {
  StringHandle strh;

  //whether the window is zoomed (in a bit of a roundabout way but I know this'll work).
  //The non zoomed window dimensions are loaded from & saved to the 'WIND' resource
  strh = GetString(rZoomPrefStr);
  if (strh != (StringHandle) NULL) {
    memcpy((void *)fontName, (void *)*strh, 256);
    p2cstr(fontName);
    windowZoomed = atoi((char *)fontName);
    ReleaseResource((Handle)strh);
  }

  //font size (in a bit of a roundabout way but I know this'll work)
  strh = GetString(rFontSizePrefStr);
  if (strh != (StringHandle) NULL) {
    memcpy((void *)fontName, (void *)*strh, 256);
    p2cstr(fontName);
    fontSizeIndex = atoi((char *)fontName);
    ReleaseResource((Handle)strh);
  }

  //font name
  strh = GetString(rFontPrefStr);
  if (strh != (StringHandle) NULL) {
    memcpy((void *)fontName, (void *)*strh, 256);
    ReleaseResource((Handle)strh);
  }
}

void saveSettings(void) {
  Str255 str;
  StringHandle strh;
  StringHandle strh2;

  //font name
  strh = GetString(rFontPrefStr);
  if (strh != (StringHandle) NULL) {
    memcpy((void *)*strh, (void *)fontName, 256);
    ChangedResource((Handle)strh);
    WriteResource((Handle)strh);
    ReleaseResource((Handle)strh);
  }

  //whether the window is zoomed (in a bit of a roundabout way but I know this'll work).
  //The non zoomed window dimensions are loaded from & saved to the 'WIND' resource
  strh2 = GetString(rZoomPrefStr);
  if (strh2 != (StringHandle) NULL) {
    sprintf((char *)str, "%d", windowZoomed);
    c2pstr((char *)str);
    memcpy((void *)*strh2, (void *)str, 256);
    ChangedResource((Handle)strh2);
    WriteResource((Handle)strh2);
    ReleaseResource((Handle)strh2);
  }

  //font size (in a bit of a roundabout way but I know this'll work)
  strh2 = GetString(rFontSizePrefStr);
  if (strh2 != (StringHandle) NULL) {
    sprintf((char *)str, "%d", fontSizeIndex);
    c2pstr((char *)str);
    memcpy((void *)*strh2, (void *)str, 256);
    ChangedResource((Handle)strh2);
    WriteResource((Handle)strh2);
    ReleaseResource((Handle)strh2);
  }
}

void adjustCursor(RgnHandle region) {
  WindowPtr window = FrontWindow();
  TXNObject object = NULL;

  if(isApplicationWindow(window)) {
    TXNAdjustCursor(*getTXNObject(window, &object), region);
  }
}

void adjustMenus(void) {
  MenuRef menu;
  int i, len;
  Boolean found = FALSE;
  Str255 currentFontName;
  WindowPtr window;
  TXNObject object = NULL;

  menu = GetMenuRef(mFile);
  if(!windowNotOpen) {
    DisableMenuItem(menu, mFileOpen);

    menu = GetMenuRef(mEdit);
    EnableMenuItem(menu, mEditSelectAll);

    window = FrontWindow();
    if(isApplicationWindow(window)) {
      if(!TXNIsSelectionEmpty(*getTXNObject(window, &object))) {
        EnableMenuItem(menu, mEditCopy);
      }
      else {
        DisableMenuItem(menu, mEditCopy);
      }
    }
  }

  menu = GetMenuRef(mFont);
  for(i = 1, len = CountMenuItems(menu)+1; i < len; i++) {
    GetMenuItemText(menu, i, currentFontName);

    if(!found && EqualString(fontName, currentFontName, TRUE, TRUE)) {
      SetItemMark(menu, i, checkMark);
      found = TRUE;
    }
    else {
      SetItemMark(menu, i, 0);
    }
  }

  if(!found) {
    GetMenuItemText(menu, 1, fontName);
    SetItemMark(menu, 1, checkMark);
  }

  found = FALSE;
  menu = GetMenuRef(mSize);
  for(i = 1, len = CountMenuItems(menu)+1; i < len; i++) {
    if(!found && fontSizeIndex == i) {
      SetItemMark(menu, i, checkMark);
      found = TRUE;
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

Boolean closeWindow(WindowPtr window) {
  TXNObject object = NULL;

  if(isApplicationWindow(window)) {
    TXNDeleteObject(*getTXNObject(window, &object));
    DisposeWindow(window);
    gNumDocuments -= 1;

    //As we only only ever have 1 application window, if it's
    //closed then we quit the application
    quit = TRUE;
  }

  adjustMenus();
  return TRUE;
}

void growWindow(WindowPtr window, EventRecord *event) {
  TXNObject object = NULL;

  if(isApplicationWindow(window)) {
    TXNGrowWindow(*getTXNObject(window, &object), event);
  }
}

void zoomWindow(WindowPtr window, short part) {
  TXNObject object = NULL;

  if(isApplicationWindow(window)) {
    //change replace the guts of this whole function with a call to TXNZoomWindow
    TXNZoomWindow(*getTXNObject(window, &object), part);
    windowZoomed = (part == inZoomOut);
  }
}

OSStatus openWindow(void) {
  WindowPtr window;
  DocumentPeek doc;
  TXNObject object;
  TXNFrameID frameID;
  Rect frame;
  OSStatus status = noErr;    //change add an OSStatus

  window = GetNewCWindow(rDocWindow, NULL, (WindowPtr)-1L);

  if(window == NULL) {
    alertUser(eNoWindow);

    return status;
  }

  object = NULL;
  frameID = 0;

  GetWindowPortBounds(window, &frame);

  /* TEXTEDIT STUFF begins here
  ******************************/

  //change 9 replace call to TENew with a call to TXNNewObject
  status = TXNNewObject(
    NULL, /* can be NULL */
    window,
    &frame, /* can be NULL */
    kTXNShowWindowMask|kTXNReadOnlyMask|
    kTXNWantHScrollBarMask|kTXNWantVScrollBarMask|
    kOutputTextInUnicodeEncodingMask|
    kTXNSaveStylesAsSTYLResourceMask|kTXNDrawGrowIconMask,
    kTXNTextEditStyleFrameType, /* the only valid option */
    kTXNUnicodeTextFile,
    kTXNUnicodeEncoding,
    &object,
    &frameID,
    NULL
  );

  if(status == noErr) {
    status = TXNAttachObjectToWindow(object, (GWorldPtr)window, TRUE);

    if(status != noErr) {
      alertUser(eNoAttachObjectToWindow);
    }
  }

  if(status == noErr) {
    if(object != NULL) {
      Boolean isAttached;

      status = TXNActivate(object, frameID, kScrollBarsAlwaysActive);

      if(status != noErr) {
        alertUser(eNoActivate);
      }

      status = SetWindowProperty(window, 'GRIT', 'tFrm', sizeof(TXNFrameID), &frameID);
      status = SetWindowProperty(window, 'GRIT', 'tObj', sizeof(TXNObject), &object);

      isAttached = TXNIsObjectAttachedToWindow(object);

      if (!isAttached) {
        alertUser(eObjectNotAttachedToWindow);
      }
    }

    adjustMenus();
    gNumDocuments++;
  }

  mainWindowPtr = window;
  return status;
}

void idleWindow(void) {
  WindowPtr window = FrontWindow();
  TXNObject object = NULL;

  if(isApplicationWindow(window)) {
    //change: replace TEIdle with a call to TXNIdle
    TXNIdle(*getTXNObject(window, &object));
  }
}

void repaintWindow(WindowPtr window) {
  GrafPtr savePort;
  TXNObject object = NULL;

  GetPort(&savePort);

  if(isApplicationWindow(window)) {
    //change: replace all this with a call to TXNUpdate.  TXNUpdate calls BeginUpdate/EndUpdate
    //and handles drawing the scroll bars.
    TXNUpdate(*getTXNObject(window, &object));
  }

  SetPort(savePort);
}

void activateWindow(WindowPtr window, Boolean becomingActive) {
  TXNObject object = NULL;
  TXNFrameID frameID = 0;

  if(isApplicationWindow(window)) {
    getTXNObject(window, &object);

    GetWindowProperty(window, 'GRIT', 'tFrm', sizeof(TXNFrameID), NULL, &frameID);

    if (becomingActive) {
      TXNActivate(object, frameID, kScrollBarsAlwaysActive);
      adjustMenus();
    }
    else {
      TXNActivate(object, frameID, kScrollBarsSyncWithFocus);
    }

    TXNFocus(object, becomingActive);
  }
}

void setFont(SInt16 menuItem) {
  TXNObject object = NULL;
  TXNTypeAttributes typeAttr[1];
  OSStatus status = noErr;
  short res;

  if(!mainWindowPtr) {
    return;
  }

  GetMenuItemText(GetMenuHandle(mFont), menuItem, fontName);

  GetFNum(fontName, &res);

  typeAttr[0].tag = kTXNQDFontFamilyIDAttribute;
  typeAttr[0].size = kTXNQDFontFamilyIDAttributeSize;
  typeAttr[0].data.dataValue = res;

  TXNSetTypeAttributes(
    *getTXNObject(mainWindowPtr, &object),
    1,
    typeAttr,
    kTXNStartOffset,
    kTXNEndOffset
  );
}

UInt32 doGetSize(SInt16 menuItem) {
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
  TXNObject object = NULL;
  TXNTypeAttributes typeAttr[1];

  if(!mainWindowPtr) {
    return;
  }

  fontSizeIndex = menuItem;

  typeAttr[0].tag = kTXNQDFontSizeAttribute;
  typeAttr[0].size = kTXNQDFontSizeAttributeSize;
  typeAttr[0].data.dataValue = doGetSize(fontSizeIndex) << 16;

  TXNSetTypeAttributes(
    *getTXNObject(mainWindowPtr, &object),
    1,
    typeAttr,
    kTXNStartOffset,
    kTXNEndOffset
  );
}

/* Handles clicking on menus or keyboard shortcuts */
void menuSelect(long mResult) {
  short theItem;
  short theMenu;
  Str255 daName;
  short itemHit;
  WindowPtr window;
  OSStatus status = noErr;
  TXNObject object = NULL;
  TXNTypeAttributes typeAttr;

  window = FrontWindow();
  theItem = LoWord(mResult);
  theMenu = HiWord(mResult);

  switch(theMenu) {
    case mApple: {
      if(theItem == mAppleAbout) {
        itemHit = Alert(rAboutAlert, NULL);
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
          quit = TRUE;
        } break;
      }
    } break;

    case mEdit: {
      if(isApplicationWindow(window)) {
        switch(theItem) {
          case mEditCopy: {
            //change: replace the guts of this with a call to TXNCopy
            //again TXNCopy returns an OSStatus which we are ignoring
            if(TXNCopy(*getTXNObject(window, &object)) != noErr) {
              alertUser(eNoCopy);
            }
          } break;

          case mEditSelectAll: {
            TXNSelectAll(*getTXNObject(window, &object));
          } break;
        }
      }
    } break;

    case mFont: {
      if(
          isApplicationWindow(window) &&
          gTXNFontMenuObject != NULL &&
          TXNDoFontMenuSelection(
            *getTXNObject(window, &object),
            gTXNFontMenuObject,
            theMenu,
            theItem
          ) != noErr
      ) {
        alertUser(eNoFontName);
      }
    } break;

    case mSize: {
      if(isApplicationWindow(window)) {
        static short aFontSizeList[] = {9, 10, 12, 14, 18, 24, 36};
        short shortValue = aFontSizeList[theItem - 1];

        typeAttr.tag = kTXNQDFontSizeAttribute;
        typeAttr.size = kTXNFontSizeAttributeSize;
        typeAttr.data.dataValue = shortValue << 16;

        status = TXNSetTypeAttributes(
          *getTXNObject(window, &object),
          1,
          &typeAttr,
          kTXNUseCurrentSelection,
          kTXNUseCurrentSelection
        );

        if(status != noErr) {
          alertUser(eNoFontSize);
        }
      }
    } break;

    default: {
      if(
          theMenu >= kStartHierMenuID &&
          isApplicationWindow(window) &&
          gTXNFontMenuObject != NULL &&
          TXNDoFontMenuSelection(
            *getTXNObject(window, &object),
            gTXNFontMenuObject,
            theMenu,
            theItem
          ) != noErr
      ) {
        alertUser(eNoFontName);
      }
    } break;
  }

  HiliteMenu(0);
  adjustMenus();
}

/*
  comment to re sync the diffs
*/
void contentClick(WindowPtr window, EventRecord *event) {
  TXNObject object = NULL;

  if(isApplicationWindow(window)) {
    TXNClick(*getTXNObject(window, &object), event);
  }
}

/* Handles mouse down events */
void mouseClick(EventRecord *event) {
  Rect tempRect;
  WindowPtr window;
  short part = FindWindow(event->where, &window);

  switch (part) {
    case inContent: {
      if(window != FrontWindow()) {
        SelectWindow(window);
      }
      else {
        contentClick(window, event);
        adjustMenus();
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
        quit = TRUE;
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


void handleEvent(EventRecord *event) {
#if TARGET_API_MAC_CARBON
  if(IsDialogEvent(event)) {
    DialogPtr theDialog = NULL;
    short itemHit;

    DialogSelect(event, &theDialog, &itemHit);
  }
#endif

  switch (event->what) {
    case nullEvent: {
      idleWindow();
    } break;

    case kOSEvent: {
      switch((event->message >> 24) & 0x0FF) {
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

    #if TARGET_API_MAC_TOOLBOX
    case diskEvt: {
      if((event->message & 0xFFFF0000) != noErr) {
        DILoad();
        DIBadMount({70, 70}, event->message);
        DIUnload();
      }
    } break;
    #endif
  }

  if(quit) {
    if(mainWindowPtr) {
      saveWindow(mainWindowPtr);
      closeWindow(mainWindowPtr);
    }

    saveSettings();
  }
}

RgnHandle cursorRgn;

void loopTick(void) {
  EventRecord event;

#if TARGET_API_MAC_TOOLBOX
  SystemTask();
#endif

  if(GetNextEvent(everyEvent, &event)) {
    adjustCursor(cursorRgn);

    handleEvent(&event);
  }
  else {
    idleWindow();
  }
}

void macYield(void) {
  loopTick(); // get one event

  if(quit) {
    terminate();
  }
}

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
          handleEvent(callbackParms->eventData.eventDataParms.event);
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

void openFileDialog(void) {
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

int output(char *buffer, SInt32 nChars, Boolean isBold) {
  char* startPoint = buffer;
  SInt32 lineChars = 0;
  SInt32 charsLeft = nChars;
  long temp;
  struct lineOffsets *temp2;
  TXNTypeAttributes iAttributes[1];
  TXNObject fMLTEObject = NULL;  // our text
  Boolean skipByte;
  size_t len = 0;
  wchar_t *wide = NULL;

  if(!mainWindowPtr) {
    return;
  }

  getTXNObject(mainWindowPtr, &fMLTEObject);

  iAttributes[0].tag=kTXNQDFontStyleAttribute;
  iAttributes[0].size=kTXNQDFontStyleAttributeSize;
  iAttributes[0].data.dataValue=isBold?bold:normal;

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
    skipByte = FALSE;

    //use funky for/switch construct to output/append until a newline or end of string
    for(;;) {
      if(charsLeft < 1) {
        break;
      }

      switch(startPoint[lineChars]) {
        case '\r': {
          if(startPoint[lineChars+1] == '\n') {
            skipByte = TRUE;
          }

          lineChars++;
          charsLeft--;
        } break;

        case '\n': {
          startPoint[lineChars] = '\r';

          lineChars++;
          charsLeft--;
        } break;

        case '\0': {
          charsLeft--;
        } break;

        default: {
          lineChars++;
          charsLeft--;
        } continue;
      }

      break;
    }

    //While the line length plus the total length used is greater than 32767 and
    //there are lines to be removed (not the last line) then remove the first line
    while((temp = textUsed+lineChars) > 32767 && firstLine != lastLine) {
      TXNSetSelection(fMLTEObject, 0, firstLine->lineLength);
      TXNClear(fMLTEObject);

      textUsed -= firstLine->lineLength;

      temp2 = firstLine;
      firstLine = firstLine->nextLine;
      free(temp2);
      temp2 = NULL;
    }

    //If the line length greater than 32767 then remove the last line of text.
    //Otherwise insert the text gathered.
    if((temp = lineChars+(lastLine->lineLength)) > 32767) {
      TXNSetSelection(fMLTEObject, 0, lastLine->lineLength);
      TXNClear(fMLTEObject);
      lastLine->lineLength = 0;
      textUsed = 0;
    }
    else {
      TXNSetSelection(fMLTEObject, kTXNEndOffset, kTXNEndOffset);
      TXNSetTypeAttributes(
        fMLTEObject,
        1,
        iAttributes,
        kTXNUseCurrentSelection,
        kTXNUseCurrentSelection
      );

      wide = (wchar_t *)d_charsetEncode((char *)startPoint, ENC_UTF16BE, &len);

      TXNSetData(
        fMLTEObject,
        kTXNUnicodeTextData,
        (void *)wide,
        len,
        kTXNUseCurrentSelection,
        kTXNUseCurrentSelection
      );

      free(wide);
      wide = NULL;
      len = 0;

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
}

int fputs_mac(const char *str, FILE *stream) {
  int len;

  if(stream == stdout || stream == stderr) {
    len = strlen(str);

    output((char *)str, len, stream == stderr);
  }
  else {
    len = fputs(str, stream);
  }

  loopTick(); // get one event

  if(quit) {
    ExitToShell();  //raise(SIGABRT); // does not return to sioDemoRead...
  }

  return len;
}

int fprintf_mac(FILE *stream, const char *format, ...) {
  va_list args;
  int retval;
  size_t newSize;
  char* newStr = NULL;
  FILE * pFile;

  if(stream == stdout || stream == stderr) {
    if(format == NULL || (pFile = fopen(devNull, "wb")) == NULL) {
      return FALSE;
    }

    //get the space needed for the new string
    va_start(args, format);
    newSize = (size_t)(vfprintf(pFile, format, args)+1); //plus L'\0'
    va_end(args);

    //close the file. We don't need to look at the return code as we were writing to /dev/null
    fclose(pFile);

    //Create a new block of memory with the correct size rather than using realloc
    //as any old values could overlap with the format string. quit on failure
    if((newStr = (char*)malloc(newSize*sizeof(char))) == NULL) {
      return FALSE;
    }

    //do the string formatting for real
    va_start(args, format);
    vsprintf(newStr, format, args);
    va_end(args);

    //ensure null termination of the string
    newStr[newSize] = '\0';

    output(newStr, newSize-1, stream == stderr);

    free(newStr);

    loopTick(); // get one event

    if(quit) {
      ExitToShell();  //raise(SIGABRT); // does not return to sioDemoRead...
    }

    return newSize-1;
  }

  va_start(args, format);
  retval = vfprintf(stream, format, args);
  va_end(args);

  loopTick(); // get one event

  if(quit) {
    ExitToShell();  //raise(SIGABRT); // does not return to sioDemoRead...
  }

  return retval;
}

FILE *fopen_mac(const char *filename, const char *mode) {
  char* absolutePath = NULL;
  FILE * retval;
  CFIndex neededLen;
  CFIndex usedLen;
  CFRange range;
  CFStringRef text1;
  CFURLRef cfabsolute;
  CFStringRef text2;

  text1 = CFStringCreateWithCStringNoCopy(
    NULL,
    filename,
    kCFStringEncodingUTF8,
    kCFAllocatorNull
  );

/*
* the function "fsetfileinfo" can be used to change the creator and type code for a file
*/
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

  cfabsolute = CFURLCreateWithFileSystemPathRelativeToBase(
    kCFAllocatorDefault,
    text1,
    macOSVersion < 0x01000 ? kCFURLHFSPathStyle : kCFURLPOSIXPathStyle,
    FALSE,
    baseFolder
  );

  text2 = CFURLCopyFileSystemPath(
    cfabsolute,
    macOSVersion < 0x01000 ? kCFURLHFSPathStyle : kCFURLPOSIXPathStyle
  );

  if(absolutePath = (char *)CFStringGetCStringPtr(text2, enc)) {
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

    reallocMsg((void**)&absolutePath, neededLen + 1);

    CFStringGetBytes(
      text2,
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

  CFRelease(text2);
  CFRelease(cfabsolute);
  CFRelease(text1);

  return retval;
}

void terminate() {
  WindowPtr aWindow;
  Boolean closed;
  closed = TRUE;

  do {
    aWindow = FrontWindow();

    if(aWindow != NULL) {
      closed = closeWindow(aWindow);
    }
  } while (closed && (aWindow != NULL));

  if(closed) {
    if(gTXNFontMenuObject != NULL) {
      OSStatus status;

      status = TXNDisposeFontMenuObject(gTXNFontMenuObject);

      if (status != noErr) {
        alertUser(eNoDisposeFontMenuObject);
      }

      gTXNFontMenuObject = NULL;
    }
  }

  TXNTerminateTextension();
  ExitToShell();
}

int main(void) {
  char progName[] = "querycsv";
  char* argv[3];

  argv[0] = progName;
  argv[1] = NULL;
  argv[2] = NULL;

#if TARGET_API_MAC_TOOLBOX
  MaxApplZone();
  InitGraf((Ptr)&qd.thePort);
  InitFonts();
  InitWindows();
  InitMenus();
  TEInit();
  InitDialogs(NULL);
#endif

  InitCursor();

  setupAppleEvents();

  /* get the system default encoding. used by the fopen wrapper */
  enc = CFStringGetSystemEncoding();

  setupMLTE();

  setupMenus();

  restoreSettings();

  gInBackground = FALSE;
  gNumDocuments = 0;

  cursorRgn = NewRgn();

  while(!quit && windowNotOpen) {
    loopTick();
  }

  if(quit) {
    ExitToShell();
  }
  else if(!windowNotOpen) {
    argv[1] = progArg;

    openWindow();

    realmain(2, argv);

    free(progArg);

    while(!quit) {
      loopTick();
    }

    terminate();
  }

  return 0; //macs don't do anything with the return value
}
