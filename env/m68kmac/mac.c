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
#include <setjmp.h>

#if TARGET_API_MAC_CARBON

#ifdef __MACH__
#include <Carbon/Carbon.h>
#else
#include <Carbon.h>
#endif

#include <CarbonStdCLib.h>
#include <Navigation.h>

#else

#include <Types.h>
#include <Quickdraw.h>
#include <Fonts.h>
#include <Controls.h>
#include <Menus.h>
#include <TextEdit.h>
#include <Dialogs.h>
#include <Scrap.h>
#include <ToolUtils.h>
#include <Memory.h>
#include <Files.h>
#include <DiskInit.h>
#include <Packages.h>
#include <Resources.h>
#include <Devices.h>
#include <Events.h>
#include <Gestalt.h>
#include <ControlDefinitions.h>
#include <StandardFile.h>
#include <Retrace.h>
#include <OSUtils.h>


#endif

#include <Traps.h>

void exit_mac(int dummy);
void restoreSettings(void);
void saveSettings(void);
void PsPStrCopy(StringPtr p1, StringPtr p2);
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
int isSelectionEmpty(void);

#define TARGET_API_MAC_TOOLBOX (!TARGET_API_MAC_CARBON)
#if TARGET_API_MAC_TOOLBOX
#ifndef RETRO68
#define GetWindowPort(w) w
#endif
QDGlobals qd;   /* qd is needed by the Macintosh runtime */
#endif

#undef main
int realmain(int argc, char **argv);

#define TRUE 1
#define FALSE 0

#define ENC_MAC 4
#define ENC_UTF16BE 10
char *d_charsetEncode(char* s, int encoding, size_t *bytesStored, struct qryData *query);
char* mystrdup(const char* s);
void reallocMsg(void **mem, size_t size);

extern char * devNull;

#include "mac.h"

/*
  A reference to our assembly language routine that gets attached to the clikLoop
  field of our TE record.
*/
extern pascal void ASMCLIKLOOP(void);
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

int lineHeight = 10;

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

jmp_buf env_buffer;

typedef struct VBLRec {
  VBLTask theVBLTask;
  long vblA5;
} VBLRec, *VBLRecPtr;

VBLRec taskrec;
int isWaiting = FALSE;
int isMouseDown = FALSE;
int currentWaitCursor = 0;
int nextWaitCursor = 0;

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

#if TARGET_API_MAC_TOOLBOX
short getWindowKind(WindowPtr window) {
  return ((WindowPeek) window)->windowKind;
}
  #define DisableMenuItem DisableItem
  #define EnableMenuItem EnableItem
  #define CountMenuItems CountMItems
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

Rect windOffsets;

Rect getWindowBounds(WindowPtr window) {
  Rect r;

#if TARGET_API_MAC_CARBON
  GetWindowBounds(window, kWindowGlobalPortRgn, &r);
#else
  #pragma unused (window)
  r = qd.thePort->portRect;
  r.bottom -= r.top;
  r.right -= r.left;
  LocalToGlobal(&((Point *) &(r))[0]);
  r.bottom += r.top;
  r.right += r.left;
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
  Str255      str;

  if(
      (result = AECountItems(docList, &itemsInList)) != noErr ||
      itemsInList == 0
  ) {
    return result;
  }

  if(
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
    memcpy((char*)&str, ((char *)&(theFSSpec.name)), sizeof(Str255));
    p2cstr(str);
    progArg = mystrdup((char*)str);

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
    menu = GetMHandle(mFile);
    DeleteMenuItem(menu, mFileQuit);
    macOSVersion = 0x1000;
    devNull = "/dev/null";  /* needed as the carbon build can run on OS X */
  }
#endif

  menu = GetMHandle(mFont);
  AppendResMenu(menu, 'FONT');

  DrawMenuBar();
}

void restoreSettings(void) {
  StringHandle strh;

  //whether the window is zoomed (in a bit of a roundabout way but I know this'll work).
  //The non zoomed window dimensions are loaded from & saved to the 'WIND' resource
  strh = GetString(rZoomPrefStr);
  if(strh != (StringHandle) NULL) {
    HLock((Handle)strh);
    memcpy((void *)&fontName, (void *)*strh, 256);
    HUnlock((Handle)strh);
    p2cstr((unsigned char *)&fontName);
    windowZoomed = atoi((char*)&fontName);
    ReleaseResource((Handle)strh);
  }

  //font size (in a bit of a roundabout way but I know this'll work)
  strh = GetString(rFontSizePrefStr);
  if(strh != (StringHandle) NULL) {
    HLock((Handle)strh);
    memcpy((void *)&fontName, (void *)*strh, 256);
    HUnlock((Handle)strh);
    p2cstr((unsigned char *)&fontName);
    fontSizeIndex = atoi((char *)&fontName);
    ReleaseResource((Handle)strh);
  }

  //font name
  strh = GetString(rFontPrefStr);
  if(strh != (StringHandle) NULL) {
    HLock((Handle)strh);
    PsPStrCopy(*strh, (StringPtr)&fontName);
    HUnlock((Handle)strh);
    ReleaseResource((Handle)strh);
  }
}

#ifdef RETRO68
void fsetfileinfo_absolute(
    const char *filename,
    unsigned long newcreator,
    unsigned long newtype
  ) {
  OSErr err;
  Str255 pFile;
  FInfo fndrInfo;

  strncpy(&pFile[1], filename, 255);
  pFile[0] = strlen(filename);

  err = HGetFInfo(0, 0, pFile, &fndrInfo);

  if(err == noErr) {
    fndrInfo.fdType = (OSType)newtype;
    fndrInfo.fdCreator = (OSType)newcreator;

    HSetFInfo(0, 0, pFile, &fndrInfo);
  }
}
#endif

void PsPStrCopy(StringPtr p1, StringPtr p2) {
  register short len;

  len = *p2++ = *p1++;
  while( --len >= 0 ) {
    *p2++ = *p1++;
  }
}

void saveSettings(void) {
  StringHandle strh;
  char str[256];

  //font name
  strh = GetString(rFontPrefStr);
  if(strh != (StringHandle) NULL) {
    HLock((Handle)strh);
    PsPStrCopy((StringPtr)&fontName, *strh);
    HUnlock((Handle)strh);
    ChangedResource((Handle)strh);
    WriteResource((Handle)strh);
    ReleaseResource((Handle)strh);
  }

  //whether the window is zoomed (in a bit of a roundabout way but I know this'll work).
  //The non zoomed window dimensions are loaded from & saved to the 'WIND' resource
  strh = GetString(rZoomPrefStr);
  if(strh != (StringHandle) NULL) {
    sprintf((char *)&str, "%d", windowZoomed);
    c2pstr((char *)&str);
    HLock((Handle)strh);
    PsPStrCopy((StringPtr)&str, *strh);
    HUnlock((Handle)strh);
    ChangedResource((Handle)strh);
    WriteResource((Handle)strh);
    ReleaseResource((Handle)strh);
  }

  //font size (in a bit of a roundabout way but I know this'll work)
  strh = GetString(rFontSizePrefStr);
  if(strh != (void *)NULL) {
    sprintf((char *)&str, "%d", fontSizeIndex);
    c2pstr((char *)&str);
    HLock((Handle)strh);
    PsPStrCopy((StringPtr)&str, *strh);
    HUnlock((Handle)strh);
    ChangedResource((Handle)strh);
    WriteResource((Handle)strh);
    ReleaseResource((Handle)strh);
  }
}

void adjustMenus(int setStyles) {
  MenuHandle menu;
  int len;
  SInt16 i;
  Boolean found = FALSE;
  Str255 currentFontName;

  menu = GetMHandle(mFile);
  if(mainWindowPtr) {
    DisableMenuItem(menu, mFileOpen);

    menu = GetMHandle(mEdit);
    EnableMenuItem(menu, mEditSelectAll);

    if(isSelectionEmpty()) {
      DisableMenuItem(menu, mEditCopy);
    }
    else {
      EnableMenuItem(menu, mEditCopy);
    }
  }

  menu = GetMHandle(mFont);
  for(i = 1, len = CountMenuItems(menu)+1; i < len; i++) {
    GetItem(menu, i, currentFontName);

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
    GetItem(menu, 1, fontName);
    SetItemMark(menu, 1, checkMark);

    if(setStyles) {
      setFont(1);
    }
  }

  found = FALSE;
  menu = GetMHandle(mSize);

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

  if(windowZoomed) {
    ZoomWindow(window, inZoomIn, window == FrontWindow());
  }

  windRect = getWindowBounds(window);

  wind = (Handle)GetResource('WIND', rDocWindow);

  HLock(wind);
  rptr = (Rect *) *wind;

  rptr->top = windRect.top + windOffsets.top;
  rptr->left = windRect.left - windOffsets.left;
  rptr->bottom = windRect.bottom + windOffsets.bottom;
  rptr->right = windRect.right + windOffsets.right;

  ChangedResource(wind);
  HUnlock(wind);

  WriteResource(wind);
}

void adjustTE(WindowPtr window) {
  TEHandle handle = getTEHandle(window);
  TEPtr te = *handle;

  TEScroll(
    te->viewRect.left -
    te->destRect.left -
    GetCtlValue(((DocumentPeek) window)->docHScroll),
    te->viewRect.top -
    te->destRect.top -
    (GetCtlValue(((DocumentPeek) window)->docVScroll) * lineHeight),
    handle
  );
}

void adjustViewRect(TEHandle docTE) {
  TEPtr te;

  te = *docTE;

  te->viewRect.bottom = (
    (
      (
        te->viewRect.bottom - te->viewRect.top
      ) / lineHeight
    ) * lineHeight
  ) +
  te->viewRect.top;
}

void adjustHV(Boolean isVert, ControlHandle control, TEHandle docTE, Boolean canRedraw) {
  short oldValue, oldMax, value, max, lines;
  TEPtr te = *docTE;

  oldValue = GetCtlValue(control);
  oldMax = GetCtlMax(control);

  if(isVert) {
    lines = te->nLines;

    if(*(*te->hText + te->teLength - 1) == kCrChar) {
      lines += 1;
    }

    max = lines - ((te->viewRect.bottom - te->viewRect.top) / lineHeight);
  }
  else {
    max = kMaxDocWidth - (te->viewRect.right - te->viewRect.left);
  }

  if(max < 0) {
    max = 0;
  }

  SetCtlMax(control, max);

  if(isVert) {
    value = (te->viewRect.top - te->destRect.top) / lineHeight;
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

  SetCtlValue(control, value);

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
  TEHandle handle = getTEHandle(window);

  adjustHV(TRUE,  doc->docVScroll, handle, canRedraw);
  adjustHV(FALSE, doc->docHScroll, handle, canRedraw);
}

void getTERect(WindowPtr window, Rect *teRect) {
  *teRect = window->portRect;
  InsetRect(teRect, kTextMargin, kTextMargin);        // adjust for margin
  teRect->bottom = teRect->bottom - 15;                // and for the scrollbars
  teRect->right = teRect->right - 15;
}

void adjustScrollBars(WindowPtr window, Boolean needsResize) {
  DocumentPeek doc = (DocumentPeek)window;
  TEHandle handle = getTEHandle(window);
  Rect teRect;

  //First, turn visibility of scrollbars off so we won't get unwanted redrawing
  (*doc->docVScroll)->contrlVis = kControlInvisible;
  (*doc->docHScroll)->contrlVis = kControlInvisible;

  lineHeight = TEGetHeight(1, 1, handle);

  if(needsResize) {    //move & size as needed
    //start with TERect
    getTERect(window, &teRect);
    (*handle)->viewRect = teRect;

    //snap to nearest line
    adjustViewRect(handle);

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

  value = GetCtlValue(control);        /* get current value */
  max = GetCtlMax(control);        /* and maximum value */
  *amount = value - *amount;

  if(*amount < 0) {
    *amount = 0;
  }
  else if(*amount > max) {
    *amount = max;
  }

  SetCtlValue(control, *amount);
  *amount = value - *amount;    /* calculate the real change */
}

pascal void VActionProc(ControlHandle control, short part) {
  short       amount;
  WindowPtr   window;
  TEHandle    handle;
  TEPtr       te;

  /* if it was actually in the control */
  if(part != 0) {
    window = (*control)->contrlOwner;
    handle = getTEHandle(window);
    te = *handle;

    switch(part) {
      case inUpButton:        /* one line */
      case inDownButton: {
        amount = 1;
      } break;

      case inPageUp:          /* one page */
      case inPageDown: {
        amount = (te->viewRect.bottom - te->viewRect.top) / lineHeight;
      } break;
    }

    if(part == inDownButton || part == inPageDown) {
      amount = -amount;                /* reverse direction for a downer */
    }

    commonAction(control, &amount);

    if(amount != 0) {
      TEScroll(0, amount * lineHeight, handle);
    }
  }
}

/*
  Determines how much to change the value of the horizontal scrollbar by and how
  much to scroll the TE record.
*/
pascal void HActionProc(ControlHandle control, short part) {
  short       amount;
  WindowPtr   window;
  TEHandle handle;
  TEPtr       te;

  if(part != 0) {
    window = (*control)->contrlOwner;
    handle = getTEHandle(window);
    te = *handle;

    switch(part) {
      case inUpButton:                /* a few pixels */
      case inDownButton: {
        amount = kButtonScroll;
      } break;

      case inPageUp:                        /* a page */
      case inPageDown: {
        amount = te->viewRect.right - te->viewRect.left;
      } break;
    }

    if(part == inDownButton || part == inPageDown) {
      amount = -amount;   /* reverse direction */
    }

    commonAction(control, &amount);

    if(amount != 0) {
      TEScroll(amount, 0, handle);
    }
  }
}


void adjustCursor(Point mouse, RgnHandle region) {
  WindowPtr window = FrontWindow();
  RgnHandle arrowRgn;
  RgnHandle iBeamRgn;
  Rect      iBeamRect;

  if(isApplicationWindow(window)) {
    //calculate regions for different cursor shapes
    arrowRgn = NewRgn();
    iBeamRgn = NewRgn();

    //start arrowRgn wide open
    SetRectRgn(arrowRgn, kExtremeNeg, kExtremeNeg, kExtremePos, kExtremePos);

    //calculate iBeamRgn
    if(isApplicationWindow(window)) {
      iBeamRect = (*getTEHandle(window))->viewRect;

      SetPort(window);        //make a global version of the viewRect

      LocalToGlobal(&(*(Point *)&(iBeamRect).top));
      LocalToGlobal(&(*(Point *)&(iBeamRect).bottom));
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
      if(isWaiting) {
        if(nextWaitCursor != currentWaitCursor) {
          currentWaitCursor = nextWaitCursor;
        }

        SetCursor(*GetCursor(128 + currentWaitCursor));
      }
      else {
        SetCursor(*GetCursor(iBeamCursor));
      }

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

int isSelectionEmpty(void) {
  TEHandle te = getTEHandle(mainWindowPtr);

  return (*te)->selStart >= (*te)->selEnd;
}

void contentClick(WindowPtr window, EventRecord *event) {
  Point           mouse;
  ControlHandle   control;
  short           part, value;
  Boolean         shiftDown;
  DocumentPeek    doc;
  Rect            teRect;

  if(isApplicationWindow(window)) {
    SetPort(window);

    isMouseDown = FALSE;

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

        case inThumb: {
          value = GetCtlValue(control);
          part = TrackControl(control, mouse, NULL);

          if(part != 0) {
            value -= GetCtlValue(control);

            /* value now has CHANGE in value; if value changed, scroll */
            if(value != 0) {
              if(control == doc->docVScroll) {
                TEScroll(0, value * lineHeight, getTEHandle(window));
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

void closeWindow(WindowPtr window) {
  TEHandle te;

  if(isDeskAccessory(window)) {
    CloseDeskAcc(getWindowKind(window));
    return;
  }

  if(isApplicationWindow(window)) {
    te = getTEHandle(window);

    if(te != NULL) {
      // dispose the TEHandle if we got far enough to make one
      TEDispose(te);
    }

    //calling disposeWindow here would be technically incorrect,
    //even though we allocated storage for the window on the heap.
    //We instead call CloseWindow to have the structures taken
    //care of and then dispose of the storage ourselves.
    CloseWindow(window);
    DisposePtr((Ptr)window);
  }
}

void growWindow(WindowPtr window, EventRecord *event) {
  long         growResult;
  Rect         tempRect;
  RgnHandle    tempRgn;
  DocumentPeek doc;
  TEHandle     handle;

  windowZoomed = 0;

  //set up limiting values
  tempRect = getScreenBounds();

  tempRect.left = kMinDocDim;
  tempRect.top = kMinDocDim;

  growResult = GrowWindow(window, event->where, &tempRect);

  //see if it really changed size
  if(growResult != 0) {
    doc = (DocumentPeek)window;
    handle = getTEHandle(window);

    //save old text box
    tempRect = (*handle)->viewRect;
    tempRgn = NewRgn();

    //get localized update region
    getLocalUpdateRgn(window, tempRgn);
    SizeWindow(window, LoWord(growResult), HiWord(growResult), TRUE);
    resizedWindow(window);

    //calculate & validate the region that hasn't changed so it won't get redrawn
    SectRect(&tempRect, &(*handle)->viewRect, &tempRect);

    //take it out of update
    ValidRect(&tempRect);

    //put back any prior update
    InvalRgn(tempRgn);

    DisposeRgn(tempRgn);
  }
}

void zoomWindow(WindowPtr window, short part) {
  if(isApplicationWindow(window)) {
    EraseRect(&window->portRect);
    ZoomWindow(window, part, window == FrontWindow());
    resizedWindow(window);
  }

  windowZoomed = (part == inZoomOut);
}

int openWindow(void) {
  WindowPtr window;
  Handle wind;
  Rect *rptr;
  Rect viewRect, destRect;
  Ptr storage;
  DocumentPeek doc;
  Boolean proceed;

  //Attempt to allocate some memory to bind the generic window to TextEdit functionality
  if((storage = NewPtr(sizeof(DocumentRecord))) == NULL) {
    alertUser(eNoWindow);

    return FALSE;
  }

  //attempt to create the window that will contain program output
  window = GetNewWindow(rDocWindow, storage, (WindowPtr)-1);

  if(window == NULL) {
    alertUser(eNoWindow);

    // get rid of the storage if it is never used
    DisposPtr(storage);

    return FALSE;
  }

  //set the port (Mac OS boilerplate?)
  SetPort((GrafPtr)GetWindowPort(window));

  windOffsets = getWindowBounds(window);

  wind = (Handle)GetResource('WIND', rDocWindow);

  HLock(wind);
  rptr = (Rect *) *wind;

  windOffsets.top = windOffsets.top - rptr->top;
  windOffsets.left = windOffsets.left - rptr->left;
  windOffsets.bottom = rptr->bottom - windOffsets.bottom;
  windOffsets.right = rptr->right - windOffsets.right;

  HUnlock(wind);

  //  cast the window instance into a DocumentPeek structure,
  //  so we can set up the TextEdit related fields
  doc = (DocumentPeek)window;

  /* TEXTEDIT STUFF begins here
  ******************************/

  getTERect(window, &viewRect);
  destRect = viewRect;
  destRect.right = destRect.left + kMaxDocWidth;
  doc->docTE = TEStylNew(&destRect, &viewRect);
  proceed = doc->docTE != NULL;

  if(proceed) {
    // fix up the TextEdit view?
    adjustViewRect(doc->docTE);
    TEAutoView(TRUE, doc->docTE);   //TEAutoView controls automatic scrolling

    //backup the original click loop routine then substitute our own.
    //It seems our substitute routine has to be written in
    //assembly language as "registers need to be mucked with" (???)
    doc->docClik = (ProcPtr) (*doc->docTE)->clickLoop;
    (*doc->docTE)->clickLoop = (TEClickLoopUPP) ASMCLIKLOOP;

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

    mainWindowPtr = window;
    adjustMenus(TRUE);

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

void editCopy(void) {
  if(mainWindowPtr && ZeroScrap() == noErr) {
    TECopy(getTEHandle(mainWindowPtr));

    // after copying, export the TE scrap
    if(TEToScrap() != noErr) {
      alertUser(eNoCopy);
      ZeroScrap();
    }
  }
}

void editSelectAll(void) {
  if(mainWindowPtr) {
    TESetSelect(0, 32767, getTEHandle(mainWindowPtr));
  }
}

void setFont(SInt16 menuItem) {
  TextStyle styleRec;
  TEHandle handle;
  short res;

  GetItem(GetMHandle(mFont), menuItem, fontName);

  if(mainWindowPtr) {
    handle = getTEHandle(mainWindowPtr);

    GetFNum(fontName, &res);

    styleRec.tsFont = res;

    TESetSelect(0, 32767, handle);
    TESetStyle(doFont, &styleRec, TRUE, handle);
    TESetSelect(32767, 32767, handle);

    adjustScrollBars(mainWindowPtr, FALSE);
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
  TextStyle styleRec;
  TEHandle handle;

  fontSizeIndex = menuItem;

  if(mainWindowPtr) {
    handle = getTEHandle(mainWindowPtr);

    styleRec.tsSize = doGetSize(fontSizeIndex);

    TESetSelect(0, 32767, handle);
    TESetStyle(doSize, &styleRec, TRUE, handle);
    TESetSelect(32767, 32767, handle);

    adjustScrollBars(mainWindowPtr, FALSE);
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
        GetItem(GetMHandle(mApple), theItem, daName);
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

  if(event->what != mouseDown) {
    isMouseDown = FALSE;
  }

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
      isMouseDown = TRUE;
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
    saveSettings();

    if(mainWindowPtr) {
      saveWindow(mainWindowPtr);
      closeWindow(mainWindowPtr);

      #if TARGET_API_MAC_CARBON
        TXNTerminateTextension();
      #endif
    }
  }
}

RgnHandle cursorRgn;

void loopTick(void) {
  EventRecord event;

#if TARGET_API_MAC_TOOLBOX
  Point mouse;

  getGlobalMouse(&mouse);
  adjustCursor(mouse, cursorRgn);

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
#endif

  if(WaitNextEvent(everyEvent, &event, 0, cursorRgn)) {
    #if TARGET_API_MAC_CARBON
      adjustCursor(event.where, cursorRgn);
    #endif

    handleEvent(&event);
  }
  else {
    idleWindow();
  }
}

void macYield(void) {
  EventRecord event;

  if(nextWaitCursor == currentWaitCursor) {
    nextWaitCursor = (nextWaitCursor + 1) % 4;
  }

  if(EventAvail(-1, &event)) {
    loopTick(); // get one event

    if(quit) {
      exit_mac(EXIT_SUCCESS);
    }
  }
}

#if TARGET_API_MAC_TOOLBOX
void openFileDialog(void) {
  Point where;
  unsigned const char prompt = '\0';
  OSType typeList = 'TEXT';
  SFReply reply;
  Str255 str;

  where.h = where.v = 70;

  SFGetFile(where, &prompt, NULL, 1, &typeList, NULL, &reply);

  if(reply.good && SetVol(NULL, reply.vRefNum) == noErr) {
    //SFReply.fName is a STR63, plus 1 for the null character
    memcpy(&str, reply.fName, sizeof(Str255));
    p2cstr(str);
    progArg = mystrdup((char*)&str);

    windowNotOpen = FALSE;
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
  TextStyle theStyle;
  TEHandle handle;
  char *encoded = NULL;
  short res;
  #ifdef RETRO68
    const char nl = '\r';
  #else
    const char nl = '\n';
  #endif

  if(!mainWindowPtr) {
    return;
  }

  handle = getTEHandle(mainWindowPtr);

  GetFNum(fontName, &res);
  theStyle.tsFont = res;
  theStyle.tsFace = isBold?bold:normal;
  theStyle.tsSize = doGetSize(fontSizeIndex);

  encoded = d_charsetEncode((char *)buffer, ENC_MAC, &charsLeft, NULL);
  startPoint = encoded;

  //first run initialization
  if(firstLine == NULL) {
    reallocMsg((void**)&firstLine, sizeof(struct lineOffsets));

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
        #ifdef RETRO68
          case '\n': {
            startPoint[lineChars] = '\r';

            lineChars++;
            charsLeft--;
          } break;

          case '\r': {
            if(startPoint[lineChars+1] == '\n') {
              skipByte = TRUE;
            }

            lineChars++;
            charsLeft--;
          } break;
        #else
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
        #endif

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

    while(lineChars > 32767) {
      startPoint = &(startPoint[32767]);
      lineChars -= 32767;
    }

    //While the line length plus the total length used is greater than 32767 and
    //there are lines to be removed (not the last line) then remove the first line
    while((temp = textUsed + lineChars) > 32767) {
      TEAutoView(FALSE, handle);   //TEAutoView controls automatic scrolling
      TESetSelect(0, firstLine->lineLength, handle);
      TEDelete(handle);
      TEAutoView(TRUE, handle);   //TEAutoView controls automatic scrolling

      textUsed -= firstLine->lineLength;

      if(firstLine == lastLine) {
        lastLine->lineLength = 0;
      }
      else {
        temp2 = firstLine;
        firstLine = firstLine->nextLine;
        free(temp2);
        temp2 = NULL;
      }
    }

    //Insert the text gathered.
    TESetSelect(32767, 32767, handle);
    TESetStyle(doFont + doFace + doSize, &theStyle, FALSE, handle);
    TEInsert(startPoint, lineChars, handle);
    lastLine->lineLength += lineChars;
    textUsed += lineChars;

    //allocate another line if one is needed
    if(startPoint[lineChars-1] == nl && lastLine->lineLength != 0) {
      reallocMsg((void**)&(lastLine->nextLine), sizeof(struct lineOffsets));

      lastLine = lastLine->nextLine;
      lastLine->lineLength = 0;
      lastLine->nextLine = NULL;
    }

    //update the starting point for the next line to be output
    startPoint = &(startPoint[lineChars]);
    lineChars = 0;

    //eat /n after /r (as in /r/n)
    if(skipByte) {
      startPoint++;
      charsLeft--;
    }
  } while(charsLeft > 0);   //any more characters to be output?

  free(encoded);

  adjustScrollBars(mainWindowPtr, FALSE);
}

size_t fwrite_mac(const void * str, size_t size, size_t count, FILE * stream) {
  size_t len = size * count;

  if(stream != stdout && stream != stderr) {
    return fwrite(str, 1, len, stream);
  }

  output((char *)str, len, stream == stderr);

  return len;
}

int fputs_mac(const char *str, FILE *stream) {
  return fwrite_mac(str, 1, strlen(str), stream);
}

int fprintf_mac(FILE *stream, const char *format, ...) {
  va_list args;
  int retval;
  size_t newSize;
  char* newStr = NULL;

  #ifndef RETRO68
    FILE * pFile;
  #endif

  if(stream != stdout && stream != stderr) {
    va_start(args, format);
    retval = vfprintf(stream, format, args);
    va_end(args);

    return retval;
  }

  #ifdef RETRO68
    va_start(args, format);
    newSize = (size_t)(vsnprintf(NULL, 0, format, args)); /* plus '\0' */
    va_end(args);
  #else
    if(format == NULL || (pFile = fopen(devNull, "wb")) == NULL) {
      return FALSE;
    }

    //get the space needed for the new string
    va_start(args, format);
    newSize = (size_t)(vfprintf(pFile, format, args)); //plus L'\0'
    va_end(args);

    //close the file. We don't need to look at the return code as we were writing to /dev/null
    fclose(pFile);
  #endif

  //Create a new block of memory with the correct size rather than using realloc
  //as any old values could overlap with the format string. quit on failure
  reallocMsg((void**)&newStr, newSize);

  //do the string formatting for real
  va_start(args, format);
  vsprintf(newStr, format, args);
  va_end(args);

  output(newStr, newSize, stream == stderr);

  free(newStr);

  return newSize;
}

void exit_mac(int dummy) {
  longjmp(env_buffer, 1);
}

pascal void spinCursor() {
  Point mouse;
  long oldA5 = (long)SetCurrentA5();

  SetA5(taskrec.vblA5);

  if(isWaiting) {
    taskrec.theVBLTask.vblCount = 20;

    if(isMouseDown == FALSE) {
      getGlobalMouse(&mouse);
      adjustCursor(mouse, cursorRgn);
    }
  }

  SetA5(oldA5);
}

int main(void) {
  char progName[] = "querycsv";
  char* argv[3];
  int val;

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
  InitCarbonStdCLib();

  /* get the system default encoding. used by the fopen wrapper */
  enc = CFStringGetSystemEncoding();

  setupMLTE();
#endif
  setupMenus();

  restoreSettings();

  cursorRgn = NewRgn();

  setupAppleEvents();

  while(!quit && windowNotOpen) {
    loopTick();
  }

  if(!quit && openWindow()) {
    argv[1] = progArg;

    val = setjmp(env_buffer);

    if(val == 0) {
      isWaiting = TRUE;
      isMouseDown = FALSE;
      taskrec.theVBLTask.qType = vType;
      taskrec.theVBLTask.vblAddr = (VBLUPP)spinCursor;
      taskrec.theVBLTask.vblPhase = 0;
      taskrec.theVBLTask.vblCount = 20;
      taskrec.vblA5 = (long)SetCurrentA5();
      VInstall((QElemPtr)(&taskrec));

      realmain(2, argv);
    }

    isWaiting = FALSE;

    //VRemove((QElemPtr)(&taskrec));  // not needed as we can just let the VBL task not reinitialise itself instead.

    free(progArg);

    while(!quit) {
      loopTick();
    }
  }

  return EXIT_SUCCESS; //macs don't do anything with the return value
}
