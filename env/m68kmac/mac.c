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
void adjustMenus(void);
void menuSelect(long menuResult);
int openWindow(void);
void closeWindow(WindowPtr window);
void terminate(void);
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

#define TARGET_API_MAC_TOOLBOX (!TARGET_API_MAC_CARBON)
#if TARGET_API_MAC_TOOLBOX
#define GetWindowPort(w) w
QDGlobals qd;   /* qd is needed by the Macintosh runtime */
#endif

#include "mac.h"

#undef main
int realmain(int argc, char **argv);

/*
  A DocumentRecord contains the WindowRecord for one of our document windows,
  as well as the TEHandle for the text we are editing. Other document fields
  can be added to this record as needed. For a similar example, see how the
  Window Manager and Dialog Manager add fields after the GrafPort.
*/
#if TARGET_API_MAC_CARBON
typedef struct {
  TXNObject     docMLTEObject;
  TXNFrameID    docMLTEFrameID;
} DocumentRecord, *DocumentPeek;
#else
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
int lineHeight2 = 10;


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
  gMac is used to hold the result of a SysEnvirons call. This makes
  it convenient for any routine to check the environment. It is
  global information, anyway.
  It is set up by Initialize
*/
SysEnvRec gMac;

/*
  gHasWaitNextEvent is set at startup, and tells whether the WaitNextEvent
  trap is available. If it is false, we know that we must call GetNextEvent.
  It is set up by Initialize
*/
Boolean gHasWaitNextEvent;

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
#define kMinDocDim        64

/*  kCrChar is used to match with a carriage return when calculating the
  number of lines in the TextEdit record. kDelChar is used to check for
  delete in keyDowns. */
#define kCrChar         13
#define kDelChar        8

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
#define	kSysEnvironsVersion		1
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

/*
  gNumDocuments is used to keep track of how many open documents there are
  at any time. It is maintained by the routines that open and close documents.
  It is maintained by Initialize, DoNew, and DoCloseWindow
*/
short gNumDocuments;

WindowPtr mainWindowPtr = NULL;
DocumentRecord mainWindowDoc;

//  A reference to our assembly language routine that gets attached to the clikLoop
//  field of our TE record.
extern pascal void AsmClikLoop(void);

#define ENC_MAC 4
char *d_charsetEncode(char* s, int encoding, size_t *bytesStored);

void openFileDialog(void);

SInt32 macOSVersion;

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
  return (window == NULL) && (getWindowKind(window) < 0);
}

//Check to see if a given trap is available on the system
Boolean trapAvailable(short tNumber) {
#if TARGET_API_MAC_CARBON
  return TRUE;
#else
  TrapType tType = tNumber & 0x800 ? ToolTrap : OSTrap;

  if (
      tType == (unsigned char)ToolTrap &&
      gMac.machineType > envMachUnknown &&
      gMac.machineType < envMacII
  ) {
    /* it's a 512KE, Plus, or SE. As a consequence, the tool traps only go to 0x01FF */
    tNumber = tNumber & 0x03FF;

    if (tNumber > 0x01FF) {
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
  return (((DocumentPeek) window)->docTE);
}
#endif

//  Calculate sleep value for WaitNextEvent.
static UInt32 getSleep() {
#if TARGET_API_MAC_CARBON
  WindowPtr window;
  TXNObject object = NULL;

  window = FrontWindow();

  if (isApplicationWindow(window)) {
    return TXNGetSleepTicks(*getTXNObject(window, &object));
  }
#endif

  return GetCaretTime();
}


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

pascal OSErr appleEventOpenDoc(
    const AppleEvent *theAppleEvent,
    AEDescList *reply,
    long handlerRefCon
) {
#pragma unused (reply, handlerRefCon)
  AEDescList  docList;
  AEKeyword   keyword;
  DescType    returnedType;
  FSSpec      theFSSpec;
  Size        actualSize;
  long        itemsInList;
  OSErr       result;
  Boolean     showMessage = FALSE;
  char *text = NULL;
  char *text2 = NULL;

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

void adjustCursor(Point mouse, RgnHandle region) {
  WindowPtr window = FrontWindow();
  RgnHandle arrowRgn;
  RgnHandle iBeamRgn;
  Rect      iBeamRect;

  if(!gInBackground && !isDeskAccessory(window)) {
    //calculate regions for different cursor shapes
    arrowRgn = NewRgn();
    iBeamRgn = NewRgn();

    //start arrowRgn wide open
    SetRectRgn(arrowRgn, kExtremeNeg, kExtremeNeg, kExtremePos, kExtremePos);

    //calculate iBeamRgn
    if(isApplicationWindow(window)) {
      iBeamRect = (*getTEHandle(window))->viewRect;

      SetPort(window);        //make a global version of the viewRect
      LocalToGlobal(&TopLeft(iBeamRect));
      LocalToGlobal(&BotRight(iBeamRect));
      RectRgn(iBeamRgn, &iBeamRect);

      //we temporarily change the port's origin to 'globalfy' the visRgn
      SetOrigin(-window->portBits.bounds.left, -window->portBits.bounds.top);
      SectRgn(iBeamRgn, window->visRgn, iBeamRgn);
      SetOrigin(0, 0);
    }

    //subtract other regions from arrowRgn
    DiffRgn(arrowRgn, iBeamRgn, arrowRgn);

    //change the cursor and the region parameter
    if(PtInRgn(mouse, iBeamRgn)) {
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

void adjustMenus(void) {
  MenuHandle menu;
  int i, len;
  Boolean found = FALSE;
  Str255 currentFontName;
  TEHandle te;

  menu = GetMenuHandle(mFile);
  if(!windowNotOpen) {
    DisableMenuItem(menu, mFileOpen);

    menu = GetMenuHandle(mEdit);
    EnableMenuItem(menu, mEditSelectAll);

    if(mainWindowPtr) {
      te = getTEHandle(mainWindowPtr);

      if((*te)->selStart < (*te)->selEnd) {
        EnableMenuItem(menu, mEditCopy);
      }
      else {
        DisableMenuItem(menu, mEditCopy);
      }
    }
  }

  menu = GetMenuHandle(mFont);
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
}

/* resync diffs */
void adjustTE(WindowPtr window) {
  TEPtr te;

  te = *getTEHandle(window);

  TEScroll(
    te->viewRect.left -
    te->destRect.left -
    GetControlValue(((DocumentPeek) window)->docHScroll),
    te->viewRect.top -
    te->destRect.top -
    (GetControlValue(((DocumentPeek) window)->docVScroll) * lineHeight2),
    getTEHandle(window)
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

  if(isVert) {
    lines = te->nLines;

    if(*(*te->hText + te->teLength - 1) == kCrChar) {
      lines += 1;
    }

    max = lines - ((te->viewRect.bottom - te->viewRect.top) / lineHeight2);
  }
  else {
    max = kMaxDocWidth - (te->viewRect.right - te->viewRect.left);
  }

  if(max < 0) {
    max = 0;
  }

  SetControlMaximum(control, max);

  if(isVert) {
    value = (te->viewRect.top - te->destRect.top) / lineHeight2;
  }
  else {
    value = te->viewRect.left - te->destRect.left;
  }

  if(value < 0) {
    value = 0;
  }
  else if(value > max) {
    value = max;
  }

  SetControlValue(control, value);

  if(
      canRedraw ||
      max != oldMax ||
      value != oldValue
  ) {
    ShowControl(control);
  }
}

void adjustScrollbarValues(WindowPtr window, Boolean canRedraw) {
  DocumentPeek doc = (DocumentPeek)window;

  adjustHV(TRUE,  doc->docVScroll, getTEHandle(window), canRedraw);
  adjustHV(FALSE, doc->docHScroll, getTEHandle(window), canRedraw);
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

  lineHeight2 = TEGetHeight(1, 1, getTEHandle(window));

  if(needsResize) {    //move & size as needed
    //start with TERect
    getTERect(window, &teRect);
    (*getTEHandle(window))->viewRect = teRect;

    //snap to nearest line
    adjustViewRect(getTEHandle(window));

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

void getLocalUpdateRgn(WindowPtr window, RgnHandle localRgn) {
  // save old update region
  CopyRgn(((WindowPeek) window)->updateRgn, localRgn);

  OffsetRgn(localRgn, window->portBits.bounds.left, window->portBits.bounds.top);
}

void resizedWindow(WindowPtr window) {
  adjustScrollBars(window, TRUE);
  adjustTE(window);
  InvalRect(&window->portRect);
}

//  Gets called from our assembly language routine, AsmClikLoop, which is in
//  turn called by the TEClick toolbox routine. Saves the windows clip region,
//  sets it to the portRect, adjusts the scrollbar values to match the TE scroll
//  amount, then restores the clip region.
pascal void PascalClikLoop(void) {
  WindowPtr window = FrontWindow();
  RgnHandle region = NewRgn();

  GetClip(region);
  ClipRect(&window->portRect);

  adjustScrollbarValues(window, TRUE);

  SetClip(region);
  DisposeRgn(region);
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

void commonAction(ControlHandle control, short *amount) {
  short value, max;

  value = GetControlValue(control);        /* get current value */
  max = GetControlMaximum(control);        /* and maximum value */
  *amount = value - *amount;

  if(*amount < 0) {
    *amount = 0;
  }
  else if(*amount > max) {
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
  if(part != 0) {
    window = (*control)->contrlOwner;
    te = *getTEHandle(window);

    switch(part) {
      case kControlUpButtonPart:        /* one line */
      case kControlDownButtonPart: {
        amount = 1;
      } break;

      case kControlPageUpPart:          /* one page */
      case kControlPageDownPart: {
        amount = (te->viewRect.bottom - te->viewRect.top) / lineHeight2;
      } break;
    }

    if(
        part == kControlDownButtonPart ||
        part == kControlPageDownPart
    ) {
      amount = -amount;                /* reverse direction for a downer */
    }

    commonAction(control, &amount);

    if(amount != 0) {
      TEScroll(0, amount * lineHeight2, getTEHandle(window));
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

  if(part != 0) {
    window = (*control)->contrlOwner;
    te = *getTEHandle(window);

    switch(part) {
      case kControlUpButtonPart:                /* a few pixels */
      case kControlDownButtonPart: {
        amount = kButtonScroll;
      } break;

      case kControlPageUpPart:                        /* a page */
      case kControlPageDownPart: {
        amount = te->viewRect.right - te->viewRect.left;
      } break;
    }

    if(
        part == kControlDownButtonPart ||
        part == kControlPageDownPart
    ) {
      amount = -amount;   /* reverse direction */
    }

    commonAction(control, &amount);

    if(amount != 0) {
      TEScroll(amount, 0, getTEHandle(window));
    }
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

void closeWindow(WindowPtr window) {
  TEHandle te;

  if(isDeskAccessory(window)) {
    CloseDeskAcc(getWindowKind(window));
  }
  else if(isApplicationWindow(window)) {
    te = getTEHandle(window);

    if(te != NULL) {
      // dispose the TEHandle if we got far enough to make one
      TEDispose(te);
    }

    //TODO: ensure allocated memory associated to the window is freed
    DisposeWindow(window);
    gNumDocuments -= 1;
  }
}

void growWindow(WindowPtr window, EventRecord *event) {
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
  if(growResult != 0) {
    doc = (DocumentPeek) window;

    //save old text box
    tempRect = (*getTEHandle(window))->viewRect;
    tempRgn = NewRgn();

    //get localized update region
    getLocalUpdateRgn(window, tempRgn);
    SizeWindow(window, LoWord(growResult), HiWord(growResult), TRUE);
    resizedWindow(window);

    //calculate & validate the region that hasn't changed so it won't get redrawn
    SectRect(&tempRect, &(*getTEHandle(window))->viewRect, &tempRect);

    //take it out of update
    ValidRect(&tempRect);

    //put back any prior update
    InvalRgn(tempRgn);

    DisposeRgn(tempRgn);
  }
}

/* resync */
void zoomWindow(WindowPtr window, short part) {
  EraseRect(&window->portRect);
  ZoomWindow(window, part, window == FrontWindow());
  resizedWindow(window);
  windowZoomed = (part == inZoomOut);
}

int openWindow(void) {
  WindowPtr window;
  DocumentPeek doc;
  Rect viewRect, destRect;
  Boolean proceed;
  OSStatus status = noErr;
  Ptr storage;

  //Attempt to allocate some memory to bind the generic window to TextEdit functionality
  storage = NewPtr(sizeof(DocumentRecord));

  if(storage == NULL) {
    //The raising of the abort signal should mean we never get here, but just in case
    return FALSE;
  }

  //attempt to create the window that will contain program output
  window = GetNewWindow(rDocWindow, storage, (WindowPtr)-1);

  if(window == NULL) {
    alertUser(eNoWindow);

    // get rid of the storage if it is never used
    DisposePtr(storage);

    return FALSE;
  }

  //set the port (Mac OS boilerplate?)
  SetPort(GetWindowPort(window));

  //  cast the window instance into a DocumentPeek structure,
  //  so we can set up the TextEdit related fields
  doc = (DocumentPeek)window;

  /* TEXTEDIT STUFF begins here
  ******************************/

  // set up the textedit content size (not the viewport) rectangle
  getTERect(window, &viewRect);
  destRect = viewRect;
  destRect.right = destRect.left + kMaxDocWidth;

  //attempt to create a TextEdit control and bind
  //it to our window/document structure
  doc->docTE = TEStyleNew(&destRect, &viewRect);

  //only proceed if the TextEdit control was successfully created
  proceed = doc->docTE != NULL;

  if(proceed) {
    // fix up the TextEdit view?
    adjustViewRect(doc->docTE);
    TEAutoView(TRUE, doc->docTE);   //TEAutoView controls automatic scrolling

    //backup the original click loop routine then substitute our own.
    //It seems our substitute routine has to be written in
    //assembly language as "registers need to be mucked with" (???)
    doc->docClik = (ProcPtr) (*doc->docTE)->clickLoop;
    (*doc->docTE)->clickLoop = (TEClickLoopUPP) AsmClikLoop;

    // Attempt to setup vertical scrollbar control
    doc->docVScroll = GetNewControl(rVScroll, window);
    proceed = (doc->docVScroll != NULL);
  }

  if(proceed) {
    // Attempt to setup horizontal scrollbar control
    doc->docHScroll = GetNewControl(rHScroll, window);
    proceed = (doc->docHScroll != NULL);
  }

  if(proceed) {
    //Adjust & draw the controls, then show the window.
    //False to adjustScrollValues means musn't redraw; technically, of course,
    //the window is hidden so it wouldn't matter whether we called ShowControl or not.
    adjustScrollBars(window, TRUE);
    adjustScrollbarValues(window, FALSE);
    ShowWindow(window);

    if(windowZoomed) {
      zoomWindow(window, inZoomOut);
    }

    adjustMenus();
    gNumDocuments++;
    mainWindowPtr = window;

    return TRUE;
  }

  //Something failed in the window creation process.
  //Clean up then tell the user what happened
  closeWindow(window);
  alertUser(eNoWindow);

  return FALSE;
}

void idleWindow(void) {
  WindowPtr window = FrontWindow();

  if(isApplicationWindow(window)) {
    TEIdle(getTEHandle(window));
  }
}

void repaintWindow(WindowPtr window) {
  GrafPtr savePort;

  GetPort(&savePort);

  if(isApplicationWindow(window)) {
    BeginUpdate(window);

    // draw if updating needs to be done
    if(!EmptyRgn(window->visRgn)) {
      SetPort(window);
      EraseRect(&window->portRect);
      DrawControls(window);
      DrawGrowIcon(window);
      TEUpdate(&window->portRect, getTEHandle(window));
    }

    EndUpdate(window);
  }

  SetPort(savePort);
}

void activateWindow(WindowPtr window, Boolean becomingActive) {
  RgnHandle     tempRgn, clipRgn;
  Rect          growRect;
  DocumentPeek  doc;

  if(isApplicationWindow(window)) {
    doc = (DocumentPeek)window;

    if(becomingActive) {
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
      TEActivate(getTEHandle(window));

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
      TEDeactivate(getTEHandle(window));
      // the controls must be hidden on deactivation:
      HideControl(doc->docVScroll);
      HideControl(doc->docHScroll);
      // the growbox should be changed immediately on deactivation:
      DrawGrowIcon(window);
    }
  }
}

void setFont(SInt16 menuItem) {
  TextStyle styleRec;
  OSStatus status = noErr;
  short res;

  if(!mainWindowPtr) {
    return;
  }

  GetMenuItemText(GetMenuHandle(mFont), menuItem, fontName);

  GetFNum(fontName, &res);

  styleRec.tsFont = res;

  TESetSelect(0, 32767, getTEHandle(mainWindowPtr));
  TESetStyle(doFont, &styleRec, TRUE, getTEHandle(mainWindowPtr));
  TESetSelect(32767, 32767, getTEHandle(mainWindowPtr));

  adjustScrollBars(mainWindowPtr, FALSE);
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
  TextStyle styleRec;

  if(!mainWindowPtr) {
    return;
  }

  fontSizeIndex = menuItem;

  styleRec.tsSize = doGetSize(fontSizeIndex);

  TESetSelect(0, 32767, getTEHandle(mainWindowPtr));
  TESetStyle(doSize, &styleRec, TRUE, getTEHandle(mainWindowPtr));
  TESetSelect(32767, 32767, getTEHandle(mainWindowPtr));

  adjustScrollBars(mainWindowPtr, FALSE);
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
      if(mainWindowPtr) {
        switch(theItem) {
          case mEditCopy: {
            if(ZeroScrap() == noErr) {
              TECopy(getTEHandle(mainWindowPtr));

              // after copying, export the TE scrap
              if(TEToScrap() != noErr) {
                alertUser(eNoCopy);
                ZeroScrap();
              }
            }
          } break;

          case mEditSelectAll: {
            TESetSelect(0, 32767, getTEHandle(mainWindowPtr));
          } break;
        }
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
  adjustMenus();
}

/*
  comment to re sync the diffs
*/
void contentClick(WindowPtr window, EventRecord *event) {
  Point           mouse;
  ControlHandle   control;
  short           part, value;
  Boolean         shiftDown;
  DocumentPeek    doc;
  Rect            teRect;

  if(isApplicationWindow(window)) {
    SetPort(window);

    /* get the click position */
    mouse = event->where;
    GlobalToLocal(&mouse);

    /* see if we are in the viewRect. if so, we wonÕt check the controls */
    getTERect(window, &teRect);

    if(PtInRect(mouse, &teRect)) {
      /* see if we need to extend the selection */
      shiftDown = (event->modifiers & shiftKey) != 0;        /* extend if Shift is down */
      TEClick(mouse, shiftDown, getTEHandle(window));
    }
    else {
      doc = (DocumentPeek)window;
      part = FindControl(mouse, window, &control);

      switch(part) {
        case 0: {
          /* do nothing for viewRect case */
        } break;

        case kControlIndicatorPart: {
          value = GetControlValue(control);
          part = TrackControl(control, mouse, NULL);

          if(part != 0) {
            value -= GetControlValue(control);

            /* value now has CHANGE in value; if value changed, scroll */
            if(value != 0) {
              if(control == doc->docVScroll) {
                TEScroll(0, value * lineHeight2, getTEHandle(window));
              }
              else {
                TEScroll(value, 0, getTEHandle(window));
              }
            }
          }
        } break;

        default: {    /* they clicked in an arrow, so track & scroll */
          if(control == doc->docVScroll) {
            value = TrackControl(control, mouse, (ControlActionUPP) VActionProc);
          }
          else {
            value = TrackControl(control, mouse, (ControlActionUPP) HActionProc);
          }
        } break;
      }
    }
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
      badMount(event);
    } break;
    #endif
  }

  if(quit) {
    if(mainWindowPtr) {
      saveWindow(mainWindowPtr);
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
    terminate();
  }
}

#if TARGET_API_MAC_TOOLBOX
#include <StandardFile.h>

void openFileDialog(void) {
  Point where;
  unsigned const char prompt = '\0';
  OSType typeList = 'TEXT';
  SFReply reply;
  OSErr result;
  char *text = NULL;
  char *text2 = NULL;

  where.h = where.v = 70;

  SFGetFile(where, &prompt, NULL, 1, &typeList, NULL, &reply);

  if(reply.good) {
    result = SetVol(NULL, reply.vRefNum);

    /* error check */
    if(result != 0) {

    }

    text = malloc(256); //SFReply.fName is a STR63, plus 1 for the null character

    if(text != NULL) {
      p2cstrcpy(text, reply.fName);
      text2 = realloc(text, strlen(text)+1);

      if(text2 != NULL) {
        progArg = text2;

        windowNotOpen = FALSE;
      }
      else {
        free(text);
      }
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
  TextStyle theStyle;
  TEHandle docTE;
  int skipByte;
  char *encoded = NULL;

  if(!mainWindowPtr) {
    return;
  }

  docTE = getTEHandle(mainWindowPtr);

  theStyle.tsFace = isBold?bold:normal;

  encoded = d_charsetEncode((char *)buffer, ENC_MAC, &charsLeft);
  startPoint = encoded;

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
      TEAutoView(FALSE, docTE);   //TEAutoView controls automatic scrolling
      TESetSelect(0, firstLine->lineLength, docTE);
      TEDelete(docTE);

      textUsed -= firstLine->lineLength;

      temp2 = firstLine;
      firstLine = firstLine->nextLine;
      free(temp2);
      temp2 = NULL;
      TEAutoView(TRUE, docTE);   //TEAutoView controls automatic scrolling
    }

    //If the line length greater than 32767 then remove the last line of text.
    //Otherwise insert the text gathered.
    if((temp = lineChars+(lastLine->lineLength)) > 32767) {
      TEAutoView(FALSE, docTE);   //TEAutoView controls automatic scrolling
      TESetSelect(0, lastLine->lineLength, docTE);
      TEDelete(docTE);
      lastLine->lineLength = 0;
      textUsed = 0;
      TEAutoView(TRUE, docTE);   //TEAutoView controls automatic scrolling
    }
    else {
      TESetSelect(32767, 32767, docTE);
      TESetStyle(doFace, &theStyle, FALSE, docTE);
      TEInsert(startPoint, lineChars, docTE);
      lastLine->lineLength = temp;
      textUsed += lineChars;
    }

    //allocate another line if one is needed
    if(startPoint[lineChars-1] == '\r' && lastLine->lineLength != 0) {
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

  free(encoded);

  adjustScrollBars(mainWindowPtr, FALSE);
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

  loopTick();

  if(quit) {
    terminate();
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
      terminate();
    }

    return newSize-1;
  }

  va_start(args, format);
  retval = vfprintf(stream, format, args);
  va_end(args);

  loopTick(); // get one event

  if(quit) {
    terminate();
  }

  return retval;
}

void terminate() {
  WindowPtr window = FrontWindow();

  while(window) {
    closeWindow(window);

    window = FrontWindow();
  }

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
  InitCursor();

  SysEnvirons(kSysEnvironsVersion, &gMac);

  if (gMac.machineType < 0) {
    BigBadError(eWrongSystem);
  }

  gHasWaitNextEvent = trapAvailable(_WaitNextEvent);
#else
  InitCursor();
#endif

  setupAppleEvents();

  setupMenus();

  restoreSettings();

  gInBackground = FALSE;
  gNumDocuments = 0;

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

  terminate();

  return 0; //macs don't do anything with the return value
}
