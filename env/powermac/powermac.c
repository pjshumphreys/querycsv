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

#if TARGET_API_MAC_CARBON

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

#include <Traps.h>

int main(void);
void setupMLTE(void);
void alertUser(short error);
void loopTick(void);
void handleEvent(EventRecord *event);
void adjustCursor(Point mouse, RgnHandle region);
void growWindow(WindowPtr window, EventRecord *event);
void zoomWindow(WindowPtr window, short part);
void repaintWindow(WindowPtr window);
void activateWindow(WindowPtr window, Boolean becomingActive);
void contentClick( WindowPtr window, EventRecord *event);
void idleWindow(void);
void setupMenus(void);
void adjustMenus(int setStyles);
void menuSelect(long menuResult);
int openWindow(void);
void closeWindow(WindowPtr window);
void BigBadError(short error);
int isApplicationWindow(WindowPtr window);
int isDeskAccessory(WindowPtr window);
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
void openFileDialog(void);
void setFont(SInt16 menuItem);
void setFontSize(SInt16 menuItem);

#define TARGET_API_MAC_TOOLBOX (!TARGET_API_MAC_CARBON)
#if TARGET_API_MAC_TOOLBOX
#define GetWindowPort(w) w
QDGlobals qd;   /* qd is needed by the Macintosh runtime */
#endif

#undef main
int realmain(int argc, char **argv);

#define TRUE 1
#define FALSE 0

#define ENC_MAC 4
#define ENC_UTF16BE 9
char *d_charsetEncode(char* s, int encoding, size_t *bytesStored);
char* mystrdup(const char* s);
void reallocMsg(void **mem, size_t size);

extern char * devNull;

#include "powermac.h"

/* how to treat filenames that are fopened */
CFURLRef baseFolder;
CFStringEncoding enc;

NavDialogRef gOpenFileDialog = NULL;
NavEventUPP gEventProc = NULL;

/*
  A DocumentRecord contains the WindowRecord for one of our document windows,
  as well as the TEHandle for the text we are editing. Other document fields
  can be added to this record as needed. For a similar example, see how the
  Window Manager and Dialog Manager add fields after the GrafPort.
*/
#if TARGET_API_MAC_TOOLBOX
typedef struct {
  WindowRecord  docWindow;
  TEHandle      docTE;
  ControlHandle docVScroll;
  ControlHandle docHScroll;
  ProcPtr       docClik;
} DocumentRecord, *DocumentPeek;
#endif

struct lineOffsets {
  int lineLength;
  struct lineOffsets *nextLine;
};

struct lineOffsets *firstLine = NULL;
struct lineOffsets *lastLine = NULL;
SInt32 textUsed = 0;

Boolean quit = FALSE;
Boolean windowNotOpen = TRUE;
int windowZoomed = 0;
char *progArg = NULL;

Str255 fontName;
int fontSizeIndex = 2;

WindowPtr mainWindowPtr = NULL;
SInt32 macOSVersion;

int const appleM = 0;
int const fileM  = 1;
int const editM  = 2;
int const fontM  = 3;
int const sizeM  = 4;

/*  kMaxDocWidth is an arbitrary number used to specify the width of the TERec's
  destination rectangle so that word wrap and horizontal scrolling can be
  demonstrated. */
#define kMaxDocWidth      576

/* kMinDocDim is used to limit the minimum dimension of a window when GrowWindow
  is called. */
#define kMinDocDim        64

/*  kCrChar is used to match with a carriage return when calculating the
  number of lines in the TextEdit record. */
#define kCrChar         13

/*  kControlInvisible is used to 'turn off' controls (i.e., cause the control not
  to be redrawn as a result of some Control Manager call such as SetCtlValue)
  by being put into the contrlVis field of the record. kControlVisible is used
  the same way to 'turn on' the control. */
#define kControlInvisible   0
#define kControlVisible     0xFF

/*  kScrollbarAdjust and kScrollbarWidth are used in calculating
  values for control positioning and sizing. */
#define kScrollbarWidth     16
#define kScrollbarAdjust    (kScrollbarWidth - 1)

/* kSysEnvironsVersion is passed to SysEnvirons to tell it which version of the
   SysEnvRec we understand. */
#define kSysEnvironsVersion   1

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

  while(tolower(c1 = *++p1) == tolower(c2 = *++p2)){
    if(!c1) {
      return(0);
    }
  }
#endif

  return(c1 - c2);
}

#if TARGET_API_MAC_TOOLBOX
short getWindowKind(WindowPtr window) {
  return ((WindowPeek) window)->windowKind;
}
  #define DisableMenuItem DisableItem
  #define EnableMenuItem EnableItem
#else
  #define getWindowKind GetWindowKind
#endif

//Check to see if a window is an application one (???).
int isApplicationWindow(WindowPtr window) {
  //application windows have windowKinds = userKind (8)
  return (window != NULL) && (getWindowKind(window) == userKind);
}

//Check to see if a window belongs to a desk accessory.
int isDeskAccessory(WindowPtr window) {
  //DA windows have negative windowKinds
  return (window != NULL) && (getWindowKind(window) < 0);
}

//Check to see if a given trap is available on the system
Boolean trapAvailable(short tNumber) {
#if TARGET_API_MAC_CARBON
  return TRUE;
#else
  TrapType tType = tNumber & 0x800 ? ToolTrap : OSTrap;

  if(
      tType == (unsigned char)ToolTrap &&
      gMac.machineType > envMachUnknown &&
      gMac.machineType < envMacII
  ) {
    /* it's a 512KE, Plus, or SE. As a consequence, the tool traps only go to 0x01FF */
    tNumber = tNumber & 0x03FF;

    if(tNumber > 0x01FF) {
      tNumber = _Unimplemented;
    }
  }

  return NGetTrapAddress(tNumber, tType) != NGetTrapAddress(_Unimplemented, ToolTrap);
#endif
}

void getGlobalMouse(Point *mouse) {
  EventRecord event;

  OSEventAvail(kNoEvents, &event);    /* we aren't interested in any events */
  *mouse = event.where;               /* just the mouse position */
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
  #pragma unused (window)
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

pascal OSErr openItems(AEDescList * docList) {
  OSErr       result;
  long        itemsInList;

  AEKeyword   keyword;
  DescType    returnedType;

  FSSpec      theFSSpec;
  Size        actualSize;

  FSRef       theFSRef;
  char* fileName = NULL;
  CFURLRef cfUrl;
  CFStringRef cfFilename;
  CFIndex neededLen;
  CFIndex usedLen;
  CFRange range;

  if(
      (result = AECountItems(docList, &itemsInList)) != noErr ||
      itemsInList == 0
  ) {
    return result;
  }

  if((result = AEGetNthPtr(
      docList,
      1,
      typeFSRef,
      &keyword,
      &returnedType,
      (Ptr)&theFSRef,
      sizeof(FSRef),
      &actualSize
  )) == noErr) {
    if((cfUrl = CFURLCreateFromFSRef(kCFAllocatorDefault, &theFSRef)) != NULL) {
      baseFolder = CFURLCreateCopyDeletingLastPathComponent(kCFAllocatorDefault, cfUrl);
      cfFilename = CFURLCopyLastPathComponent(cfUrl);

      if(fileName = (char *)CFStringGetCStringPtr(cfFilename, kCFStringEncodingUTF8)) {
        progArg = mystrdup(fileName);
      }
      else {
        neededLen = 0;
        usedLen = 0;
        range = CFRangeMake(0, CFStringGetLength(cfFilename));

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

        reallocMsg((void**)&progArg, neededLen + 1);

        CFStringGetBytes(
          cfFilename,
          range,
          kCFStringEncodingUTF8,
          '?',
          FALSE,
          (UInt8 *)progArg,
          neededLen,
          &usedLen
        );

        progArg[usedLen] = 0;
      }

      CFRelease(cfFilename);
      CFRelease(cfUrl);

      windowNotOpen = FALSE;
    }
  }

  else if(
    (result = AEGetNthPtr(
      docList,
      1,
      typeFSS,
      &keyword,
      &returnedType,
      (Ptr)&theFSSpec,
      sizeof(FSSpec),
      &actualSize
    )) == noErr &&
    (result = HSetVol(0, theFSSpec.vRefNum, theFSSpec.parID)) == noErr
  ) {
    reallocMsg((void**)&progArg, 64);  //SFReply.fName is a STR63, plus 1 for the null character
    p2cstrcpy(progArg, theFSSpec.name);
    reallocMsg((void**)&progArg, strlen(progArg)+1);

    windowNotOpen = FALSE;
  }

  if(!windowNotOpen && itemsInList > 1) {
    alertUser(eFileOpen);
  }

  return result;
}

pascal OSErr appleEventOpenDoc(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
    long handlerRefCon
) {
#pragma unused (reply, handlerRefCon)
  AEDescList  docList;
  OSErr       result;

  if(!windowNotOpen) {
    alertUser(eFileOpen);

    return noErr;
  }

  if((result = AEGetParamDesc(
    theAppleEvent,
    keyDirectObject,
    typeAEList,
    &docList
  )) != noErr) {
    return result;
  }

  return openItems(&docList);
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
    !trapAvailable(_Gestalt) ||
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

void setupMenus(void) {
  MenuRef menu;
  MenuHandle myMenus[5];
  int i;
  long result;

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

  menu = GetMenuHandle(mFont);
  AppendResMenu(menu, 'FONT');

  DrawMenuBar();
}

void restoreSettings(void) {
  StringHandle strh;

  //whether the window is zoomed (in a bit of a roundabout way but I know this'll work).
  //The non zoomed window dimensions are loaded from & saved to the 'WIND' resource
  strh = GetString(rZoomPrefStr);
  if(strh != (StringHandle) NULL) {
    memcpy((void *)fontName, (void *)*strh, 256);
    p2cstr(fontName);
    windowZoomed = atoi((char *)fontName);
    ReleaseResource((Handle)strh);
  }

  //font size (in a bit of a roundabout way but I know this'll work)
  strh = GetString(rFontSizePrefStr);
  if(strh != (StringHandle) NULL) {
    memcpy((void *)fontName, (void *)*strh, 256);
    p2cstr(fontName);
    fontSizeIndex = atoi((char *)fontName);
    ReleaseResource((Handle)strh);
  }

  //font name
  strh = GetString(rFontPrefStr);
  if(strh != (StringHandle) NULL) {
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
  if(strh != (StringHandle) NULL) {
    memcpy((void *)*strh, (void *)fontName, 256);
    ChangedResource((Handle)strh);
    WriteResource((Handle)strh);
    ReleaseResource((Handle)strh);
  }

  //whether the window is zoomed (in a bit of a roundabout way but I know this'll work).
  //The non zoomed window dimensions are loaded from & saved to the 'WIND' resource
  strh2 = GetString(rZoomPrefStr);
  if(strh2 != (StringHandle) NULL) {
    sprintf((char *)str, "%d", windowZoomed);
    c2pstr((char *)str);
    memcpy((void *)*strh2, (void *)str, 256);
    ChangedResource((Handle)strh2);
    WriteResource((Handle)strh2);
    ReleaseResource((Handle)strh2);
  }

  //font size (in a bit of a roundabout way but I know this'll work)
  strh2 = GetString(rFontSizePrefStr);
  if(strh2 != (StringHandle) NULL) {
    sprintf((char *)str, "%d", fontSizeIndex);
    c2pstr((char *)str);
    memcpy((void *)*strh2, (void *)str, 256);
    ChangedResource((Handle)strh2);
    WriteResource((Handle)strh2);
    ReleaseResource((Handle)strh2);
  }
}

void adjustMenus(int setStyles) {
  MenuHandle menu;
  int len;
  SInt16 i;
  Boolean found = FALSE;
  Str255 currentFontName;

  menu = GetMenuHandle(mFile);
  if(mainWindowPtr) {
    DisableMenuItem(menu, mFileOpen);

    menu = GetMenuHandle(mEdit);
    EnableMenuItem(menu, mEditSelectAll);

    if(isSelectionEmpty()) {
      DisableMenuItem(menu, mEditCopy);
    }
    else {
      EnableMenuItem(menu, mEditCopy);
    }
  }

  menu = GetMenuHandle(mFont);
  for(i = 1, len = CountMenuItems(menu)+1; i < len; i++) {
    GetMenuItemText(menu, i, currentFontName);

    if(!found && EqualString(fontName, currentFontName, TRUE, TRUE)) {
      SetItemMark(menu, i, checkMark);

      if(setStyles) {
        setFont(i);
      }

      found = TRUE;
    }
    else {
      SetItemMark(menu, i, 0);
    }
  }

  if(!found) {
    GetMenuItemText(menu, 1, fontName);
    SetItemMark(menu, 1, checkMark);

    if(setStyles) {
      setFont(1);
    }
  }

  found = FALSE;
  menu = GetMenuHandle(mSize);

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

  if(setStyles) {
    setFontSize(fontSizeIndex);
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
void setupMLTE(void) {
  OSStatus status;
  TXNMacOSPreferredFontDescription defaults;

  if(TXNVersionInformation == (void*)kUnresolvedCFragSymbolAddress) {
    BigBadError(eWrongSystem);
  }

  defaults.fontID = kTXNDefaultFontName;
  defaults.pointSize = kTXNDefaultFontSize;

  defaults.encoding = CreateTextEncoding(
    kTextEncodingUnicodeV3_2,
    kTextEncodingDefaultVariant,
    kTextEncodingDefaultFormat
  );

  defaults.fontStyle = kTXNDefaultFontStyle;

  status = TXNInitTextension(&defaults, 1, 0L);

  if(status != noErr) {
    BigBadError(eNoMLTE);
  }
}

void adjustCursor(Point mouse, RgnHandle region) {
  WindowPtr window = FrontWindow();
  TXNObject object;

  if(isApplicationWindow(window)) {
    TXNAdjustCursor(*getTXNObject(window, &object), region);
  }
}

int isSelectionEmpty(void) {
  TXNObject object;

  return TXNIsSelectionEmpty(*getTXNObject(mainWindowPtr, &object));
}

void contentClick(WindowPtr window, EventRecord *event) {
  TXNObject object;

  if(isApplicationWindow(window)) {
    TXNClick(*getTXNObject(window, &object), event);
  }
}

void closeWindow(WindowPtr window) {
  TXNObject object;

  if(isApplicationWindow(window)) {
    TXNDeleteObject(*getTXNObject(window, &object));

    DisposeWindow(window);
  }
}

void growWindow(WindowPtr window, EventRecord *event) {
  TXNObject object;

  if(isApplicationWindow(window)) {
    TXNGrowWindow(*getTXNObject(window, &object), event);
  }
}

void zoomWindow(WindowPtr window, short part) {
  TXNObject object;

  if(isApplicationWindow(window)) {
    TXNZoomWindow(*getTXNObject(window, &object), part);
  }

  windowZoomed = (part == inZoomOut);
}

int openWindow(void) {
  WindowPtr window;
  Rect frame;
  TXNObject object = NULL;
  TXNFrameID frameID = 0;
  OSStatus status = noErr;

  window = GetNewCWindow(rDocWindow, NULL, (WindowPtr)-1L);

  if(window == NULL) {
    alertUser(eNoWindow);

    return FALSE;
  }

  /* TEXTEDIT STUFF begins here
  ******************************/

  GetWindowPortBounds(window, &frame);
  status = TXNNewObject(
    NULL, /* can be NULL */
    window,
    &frame, /* can be NULL */
    kTXNShowWindowMask|
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

  if(object == NULL) {
    status = notEnoughMemoryErr;
  }

  if(status == noErr) {
    status = TXNAttachObjectToWindow(object, (GWorldPtr)window, TRUE);
  }

  if(status != noErr) {
    alertUser(eNoAttachObjectToWindow);
  }

  if(status == noErr) {
    status = TXNActivate(object, frameID, kScrollBarsAlwaysActive);

    if(status != noErr) {
      alertUser(eNoActivate);
    }
  }

  if(status == noErr) {
    status = SetWindowProperty(window, 'GRIT', 'tFrm', sizeof(TXNFrameID), &frameID);
    status = SetWindowProperty(window, 'GRIT', 'tObj', sizeof(TXNObject), &object);

    if(status != noErr || !TXNIsObjectAttachedToWindow(object)) {
      alertUser(eObjectNotAttachedToWindow);
    }
  }

  if(status == noErr) {
    if(windowZoomed) {
      zoomWindow(window, inZoomOut);
    }

    mainWindowPtr = window;
    adjustMenus(TRUE);

    return TRUE;
  }

  //Something failed in the window creation process.
  //Clean up then tell the user what happened
  DisposeWindow(window);

  if(object) {
    TXNDeleteObject(object);
  }

  alertUser(eNoWindow);

  return FALSE;
}

void idleWindow(void) {
  WindowPtr window = FrontWindow();
  TXNObject object;

  if(isApplicationWindow(window)) {
    TXNIdle(*getTXNObject(window, &object));
  }
}

void repaintWindow(WindowPtr window) {
  GrafPtr savePort;
  TXNObject object;

  GetPort(&savePort);

  if(isApplicationWindow(window)) {
    TXNUpdate(*getTXNObject(window, &object));
  }

  SetPort(savePort);
}

void activateWindow(WindowPtr window, Boolean becomingActive) {
  TXNFrameID frameID = 0;
  TXNObject object;

  if(isApplicationWindow(window)) {
    GetWindowProperty(window, 'GRIT', 'tFrm', sizeof(TXNFrameID), NULL, &frameID);

    if(becomingActive) {
      TXNActivate(*getTXNObject(window, &object), frameID, kScrollBarsAlwaysActive);
      adjustMenus(FALSE);
    }
    else {
      TXNActivate(*getTXNObject(window, &object), frameID, kScrollBarsSyncWithFocus);
    }

    TXNFocus(object, becomingActive);
  }
}

void editCopy(void) {
  TXNObject object;

  if(mainWindowPtr && TXNCopy(*getTXNObject(mainWindowPtr, &object)) != noErr) {
    alertUser(eNoCopy);
  }
}

void editSelectAll(void) {
  TXNObject object;

  if(mainWindowPtr) {
    TXNSelectAll(*getTXNObject(mainWindowPtr, &object));
  }
}

void setFont(SInt16 menuItem) {
  TXNTypeAttributes typeAttr[1];
  TXNObject object;
  short res;

  GetMenuItemText(GetMenuHandle(mFont), menuItem, fontName);

  if(mainWindowPtr) {
    getTXNObject(mainWindowPtr, &object);

    GetFNum(fontName, &res);

    typeAttr[0].tag = kTXNQDFontFamilyIDAttribute;
    typeAttr[0].size = kTXNQDFontFamilyIDAttributeSize;
    typeAttr[0].data.dataValue = res;

    TXNSetTypeAttributes(
      object,
      1,
      typeAttr,
      kTXNStartOffset,
      kTXNEndOffset
    );
  }
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
  TXNTypeAttributes typeAttr[1];
  TXNObject object;

  fontSizeIndex = menuItem;

  if(mainWindowPtr) {
    getTXNObject(mainWindowPtr, &object);

    typeAttr[0].tag = kTXNQDFontSizeAttribute;
    typeAttr[0].size = kTXNQDFontSizeAttributeSize;
    typeAttr[0].data.dataValue = doGetSize(fontSizeIndex) << 16;

    TXNSetTypeAttributes(
      object,
      1,
      typeAttr,
      kTXNStartOffset,
      kTXNEndOffset
    );
  }
}

/* Handles clicking on menus or keyboard shortcuts */
void menuSelect(long mResult) {
  short theItem;
  short theMenu;
  Str255 daName;
  short itemHit;

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
      switch(theItem) {
        case mEditCopy: {
          editCopy();
        } break;

        case mEditSelectAll: {
          editSelectAll();
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
  adjustMenus(FALSE);
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
        adjustMenus(FALSE);
      }
    } break;

    case inMenuBar: {
      adjustMenus(FALSE);
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

#if TARGET_API_MAC_TOOLBOX
void badMount(EventRecord *event) {
  Point pt = {70, 70};

  if((event->message & 0xFFFF0000) != noErr) {
    DILoad();
    DIBadMount(pt, event->message);
    DIUnload();
  }
}
#endif

void handleEvent(EventRecord *event) {
#if TARGET_API_MAC_CARBON
  if(IsDialogEvent(event)) {
    DialogPtr theDialog = NULL;
    short itemHit;

    DialogSelect(event, &theDialog, &itemHit);
  }
#endif

  switch(event->what) {
    case nullEvent: {
      idleWindow();
    } break;

    case kOSEvent: {
      switch((event->message >> 24) & 0x0FF) {
        case kMouseMovedMessage: {
          idleWindow();
        } break;

        case kSuspendResumeMessage: {
          activateWindow(FrontWindow(), (event->message & kResumeMask) != 0);
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
        adjustMenus(FALSE);
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
      badMount(event);
    } break;
    #endif
  }

  if(quit) {
    if(mainWindowPtr) {
      saveWindow(mainWindowPtr);
      closeWindow(mainWindowPtr);

      #if TARGET_API_MAC_CARBON
        TXNTerminateTextension();
      #endif
    }

    saveSettings();
  }
}

RgnHandle cursorRgn;

void loopTick(void) {
  EventRecord event;

#if TARGET_API_MAC_TOOLBOX
  Point mouse;

  if(!gHasWaitNextEvent) {
    SystemTask();

    if(GetNextEvent(everyEvent, &event)) {
      handleEvent(&event);
    }
    else {
      idleWindow();
    }

    return;
  }

  getGlobalMouse(&mouse);
  adjustCursor(mouse, cursorRgn);
#endif

  if(WaitNextEvent(everyEvent, &event, 0, cursorRgn)) {
    adjustCursor(event.where, cursorRgn);
    handleEvent(&event);
  }
  else {
    idleWindow();
  }
}

void macYield(void) {
  loopTick(); // get one event

  if(quit) {
    ExitToShell();
  }
}

#if TARGET_API_MAC_CARBON
#include <Navigation.h>

pascal void eventProc(
    NavEventCallbackMessage callBackSelector,
    NavCBRecPtr callBackParms,
    void* callBackUD
) {
#pragma unused(callBackUD)
  OSStatus err = noErr;
  NavReplyRecord reply;

  switch(callBackSelector) {
    case kNavCBUserAction: {
      if((err = NavDialogGetReply(callBackParms->context, &reply)) == noErr) {
        if(
            gOpenFileDialog != NULL &&
            NavDialogGetUserAction(callBackParms->context) ==
              kNavUserActionOpen
        ) {
          if(windowNotOpen) {
            openItems(&reply.selection);
          }
          else {
            alertUser(eFileOpen);
          }
        }

        err = NavDisposeReply(&reply);
      }
    } break;

    case kNavCBTerminate: {
      if(gOpenFileDialog != NULL) {
        NavDialogDispose(gOpenFileDialog);
      }

      gOpenFileDialog = NULL;
    }
  }
}

void openFileDialog(void) {
  OSStatus theErr = noErr;
  NavDialogCreationOptions dialogOptions;

  if(
      gOpenFileDialog != NULL &&
      NavDialogGetWindow(gOpenFileDialog) != NULL
  ) {
    SelectWindow(NavDialogGetWindow(gOpenFileDialog));
    return;
  }

  if((theErr = NavGetDefaultDialogCreationOptions(&dialogOptions)) == noErr) {
    dialogOptions.clientName = CFStringCreateWithCStringNoCopy(
        NULL,
        "QueryCSV",
        kCFStringEncodingUTF8,
        kCFAllocatorNull
      );
    dialogOptions.modality = kWindowModalityNone;

    if(gEventProc == NULL) {
      gEventProc = NewNavEventUPP(eventProc);
    }

    if((theErr = NavCreateGetFileDialog(
        &dialogOptions,
        NULL,
        gEventProc,
        NULL,
        NULL,
        NULL,
        &gOpenFileDialog
      )) == noErr) {

      if((theErr = NavDialogRun(gOpenFileDialog)) != noErr) {
        if(gOpenFileDialog != NULL) {
          NavDialogDispose(gOpenFileDialog);
        }

        gOpenFileDialog = NULL;
      }
    }

    if(dialogOptions.clientName != NULL) {
      CFRelease(dialogOptions.clientName);
    }
  }
}
#endif

void output(char *buffer, size_t nChars, Boolean isBold) {
  char* startPoint;
  size_t lineChars = 0;
  size_t charsLeft = nChars;
  long temp;
  struct lineOffsets *temp2;
  int skipByte;
  TXNTypeAttributes iAttributes[3];
  TXNObject object;  // our text
  size_t len;
  short res;
  wchar_t *wide = NULL;

  if(!mainWindowPtr) {
    return;
  }

  getTXNObject(mainWindowPtr, &object);

  GetFNum(fontName, &res);
  iAttributes[0].tag = kTXNQDFontFamilyIDAttribute;
  iAttributes[0].size = kTXNQDFontFamilyIDAttributeSize;
  iAttributes[0].data.dataValue = res;

  iAttributes[1].tag=kTXNQDFontStyleAttribute;
  iAttributes[1].size=kTXNQDFontStyleAttributeSize;
  iAttributes[1].data.dataValue=isBold?bold:normal;

  iAttributes[2].tag = kTXNQDFontSizeAttribute;
  iAttributes[2].size = kTXNQDFontSizeAttributeSize;
  iAttributes[2].data.dataValue = doGetSize(fontSizeIndex) << 16;

  startPoint = buffer;

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
          startPoint[lineChars] = '\n';

          if(startPoint[lineChars+1] == '\n') {
            skipByte = TRUE;
          }

          lineChars++;
          charsLeft--;
        } break;

        case '\n': {
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
      TXNSetSelection(object, 0, firstLine->lineLength);
      TXNClear(object);

      textUsed -= firstLine->lineLength;

      temp2 = firstLine;
      firstLine = firstLine->nextLine;
      free(temp2);
      temp2 = NULL;
    }

    //If the line length greater than 32767 then remove the last line of text.
    //Otherwise insert the text gathered.
    if((temp = lineChars+(lastLine->lineLength)) > 32767) {
      TXNSetSelection(object, 0, lastLine->lineLength);
      TXNClear(object);
      lastLine->lineLength = 0;
      textUsed = 0;
    }
    else {
      TXNSetSelection(object, kTXNEndOffset, kTXNEndOffset);
      TXNSetTypeAttributes(
        object,
        3,
        iAttributes,
        kTXNUseCurrentSelection,
        kTXNUseCurrentSelection
      );

      len = lineChars;
      wide = (wchar_t *)d_charsetEncode(startPoint, ENC_UTF16BE, &len);

      TXNSetData(
        object,
        kTXNUnicodeTextData,
        (void *)wide,
        len,
        kTXNUseCurrentSelection,
        kTXNUseCurrentSelection
      );

      free(wide);
      wide = NULL;

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
  } while(charsLeft > 0);   //any more characters to be output?
}

int fputs_mac(const char *str, FILE *stream) {
  size_t len;

  if(stream == stdout || stream == stderr) {
    len = strlen(str);

    output((char *)str, len, stream == stderr);
  }
  else {
    len = fputs(str, stream);
  }

  macYield();

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
    newSize = (size_t)(vfprintf(pFile, format, args)); //plus L'\0'
    va_end(args);

    //close the file. We don't need to look at the return code as we were writing to /dev/null
    fclose(pFile);

    //Create a new block of memory with the correct size rather than using realloc
    //as any old values could overlap with the format string. quit on failure
    if((newStr = (char*)malloc(newSize+1)) == NULL) {
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

    retval = newSize-1;
  }
  else {
    va_start(args, format);
    retval = vfprintf(stream, format, args);
    va_end(args);
  }

  macYield();

  return retval;
}

char* makeAbsolute(const char *filename) {
  char* absolutePath = NULL;
  char* retval = NULL;
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

  if(text1 == NULL) {
    text1 = CFStringCreateWithCString(
      NULL,
      filename,
      kCFStringEncodingUTF8
    );

    if(text1 == NULL) {
      return NULL;
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
    retval = mystrdup(absolutePath);
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

    reallocMsg((void**)&retval, neededLen + 1);

    CFStringGetBytes(
      text2,
      range,
      enc,
      '?',
      FALSE,
      (UInt8 *)retval,
      neededLen,
      &usedLen
    );

    retval[usedLen] = 0;
  }

  CFRelease(text2);
  CFRelease(cfabsolute);
  CFRelease(text1);

  return retval;
}


FILE *fopen_mac(const char *filename, const char *mode) {
  char* temp = makeAbsolute(filename);
  FILE * retval = fopen(temp, mode);

  free(temp);

  return retval;
}

void fsetfileinfo_absolute(
  const char *filename,
  unsigned long newcreator,
  unsigned long newtype
) {
  char* temp = makeAbsolute(filename);
  fsetfileinfo(temp, newcreator, newtype);
  free(temp);
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
  InitCursor();

  SysEnvirons(kSysEnvironsVersion, &gMac);

  if(gMac.machineType < 0) {
    BigBadError(eWrongSystem);
  }

  gHasWaitNextEvent = trapAvailable(_WaitNextEvent);
#else
  InitCursor();

  /* get the system default encoding. used by the fopen wrapper */
  enc = CFStringGetSystemEncoding();

  setupMLTE();
#endif

  setupAppleEvents();

  setupMenus();

  restoreSettings();

  cursorRgn = NewRgn();

  while(!quit && windowNotOpen) {
    loopTick();
  }

  if(!quit && openWindow()) {
    argv[1] = progArg;

    realmain(2, argv);

    free(progArg);

    while(!quit) {
      loopTick();
    }
  }

  return EXIT_SUCCESS; //macs don't do anything with the return value
}
