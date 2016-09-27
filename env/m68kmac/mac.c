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
#endif

#define TARGET_API_MAC_TOOLBOX (!TARGET_API_MAC_CARBON)
#if TARGET_API_MAC_TOOLBOX
#define GetWindowPort(w) w
QDGlobals qd;   /* qd is needed by the Macintosh runtime */
#endif

#include "mac.h"

#define TRUE 1
#define FALSE 0

/* TODO: do something about these ugly global variables. What are they even for anyway? */
WindowPtr mainWindowPtr;
Boolean quit = 0;
Rect picBounds = { 0, 0, BITMAP_SIZE, BITMAP_SIZE };
char *picBitmap;
int MouseState = 0;
int HeldPixel = 0;
const short appleM = 0;
const short fileM = 1;
const short editM = 2;
const short fontM = 3;
const short sizeM = 4;
int lastTick = 0;
int gHasAppleEvents = FALSE;

#if TARGET_API_MAC_TOOLBOX
#include <StandardFile.h>

OSStatus openFile() {
  Point       where;
  unsigned const char prompt = '\0';
  OSType      typeList[] = {'TEXT','utxt'};
  SFReply     reply;
  OSErr       result;

  where.h = where.v = 70;

  SFGetFile(where, &prompt, nil, 2, &typeList[0], nil, &reply);

  if(reply.good) {
    result = SetVol(reply.fName, reply.vRefNum);

    if (result != 0) {
      /* error check */
    }

    Alert(rUserAlert, nil);
  }

  return noErr;
}
#endif

#if TARGET_API_MAC_CARBON

#endif

/* Handles clicking on menus or keyboard shortcuts */
void doCommand(long mResult) {
  short theItem;
  short theMenu;
  Str255 daName;

  theItem = LoWord(mResult);
  theMenu = HiWord(mResult);

  switch(theMenu) {
    case mApple: {
      if(theItem == mAppleAbout) {
        Alert(rUserAlert, nil);
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
      switch (theItem) {
        case mFileOpen: {
          openFile();
        } break;

        case mFileQuit: {
          quit = 1;
        } break;

        default: {
        } break;
      }
    } break;

    case mEdit: {
      switch (theItem) {
        case mEditCopy: {
        } break;

        case mEditSelectAll: {
        } break;

        default: {
        } break;
      }
    } break;

    case mFont: {
      switch (theItem) {
        default: {
        } break;
      }
    } break;

    case mSize: {
      switch (theItem) {
        default: {
        } break;
      }
    } break;

    default: {
    } break;
  }

  HiliteMenu(0);
}

void badMount(EventRecord *theEvent) {
  Point pt = {70, 70};

  if ((theEvent->message & 0xFFFF0000) != noErr) {
    DILoad();
    DIBadMount(pt, theEvent->message);
    DIUnload();
  }
}

Rect getBounds() {
	Rect r;
	
#if TARGET_API_MAC_CARBON
	GetWindowPortBounds(mainWindowPtr, &r);
#else
	r = qd.thePort->portRect;
#endif
	
	return r;
}

Rect getScreenBounds() {
	Rect r;
	BitMap theScreenBits;
	
#if TARGET_API_MAC_CARBON
	GetQDGlobalsScreenBits(&theScreenBits); /* carbon accessor */
	r = theScreenBits.bounds;
#else
	r = qd.screenBits.bounds;
#endif
	
	return r;
}

void eventLoop() {
  EventRecord theEvent;
  WindowPtr whichWindow;
  Rect windRect;

  while(!quit) {

#if TARGET_API_MAC_TOOLBOX
    SystemTask();
#endif

    if(GetNextEvent(everyEvent, &theEvent)) {
      switch(theEvent.what) {
        case kHighLevelEvent: {
          if(gHasAppleEvents) {
            AEProcessAppleEvent(&theEvent);
          }
        } break;

        case mouseDown: {
          switch(FindWindow(theEvent.where, &whichWindow)) {

      #if TARGET_API_MAC_TOOLBOX
            case inSysWindow: { // Click happens in a Desk Accessory
              SystemClick(&theEvent, whichWindow);
            } break;
      #endif

            case inMenuBar: {
              doCommand(MenuSelect(theEvent.where));
            } break;

            case inDrag: {
              windRect = getScreenBounds();
              DragWindow(whichWindow, theEvent.where, &windRect);
            } break;

            case inContent: {
              if(whichWindow != FrontWindow()) {
                SelectWindow(whichWindow);
              }
            } break;

            default: {
            } break;
          }
        } break;

        case autoKey:
        case keyDown: {
          if(theEvent.modifiers & cmdKey) {
            doCommand(MenuKey(theEvent.message & charCodeMask));
          }
        } break;

        case diskEvt: {
          badMount(&theEvent);
        } break;

        case activateEvt: {
        } break;

        case updateEvt: {
          windRect = getBounds();

          BeginUpdate((WindowPtr)theEvent.message);
          EraseRect(&windRect);
          //do stuff here
          EndUpdate((WindowPtr)theEvent.message);
        } break;

        case nullEvent: {
          if(FrontWindow()) {
            IdleControls(FrontWindow());
          }
        } break;

        default: {
        } break;
      }
    }
  }
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

static pascal OSErr appleEventOpenDoc(
    const AppleEvent *theAppleEvent,
    AppleEvent *reply,
    long handlerRefCon
) {
  AEDescList  docList;
  AEKeyword   keyword;
  DescType    returnedType;
  FSSpec      theFSSpec;
  Size        actualSize;
  long        itemsInList;
  long        index;
  OSErr       result;

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

  for(index = 1; index <= itemsInList; index++) {
    if(result = AEGetNthPtr(
        &docList,
        index,
        typeFSS,
        &keyword,
        &returnedType,
        (Ptr)&theFSSpec,
        sizeof(FSSpec),
        &actualSize
    )) {
      return result;
    }

    //Spin up a thread. set the volume and folder for that thread also
    Alert(rUserAlert, nil);
  }

  return noErr;
}

static pascal OSErr appleEventQuit(
    const AppleEvent *appleEvt,
    AppleEvent* reply,
    long refcon
  ) {

  quit = 1;
  return noErr;
}

void setupAppleEvents() {
  long result;

  if (
    Gestalt(gestaltAppleEventsAttr, &result) != noErr ||  /* Problem calling Gestalt or */
    (result & (1 << gestaltAppleEventsPresent)) == 0      /* test the 0th bit of the result. If it is zero then Apple events is not available */
  ) {
    return;
  }

  gHasAppleEvents = TRUE;

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
    ExitToShell();
  }
}

void setupMenus() {
  long result;
  MenuRef menu;
  MenuHandle myMenus[5];
  int i;

  myMenus[appleM] = GetMenu(mApple);

#if TARGET_API_MAC_TOOLBOX
  AddResMenu(myMenus[appleM],'DRVR'); // System-provided Desk Accessories menu
#endif

  myMenus[appleM] = GetMenu(mApple);
  myMenus[fileM] = GetMenu(mFile);
  myMenus[editM] = GetMenu(mEdit);
  myMenus[fontM] = GetMenu(mFont);
  myMenus[sizeM] = GetMenu(mSize);

  for(i = 0; i < 5; i++) {
    InsertMenu(myMenus[i], 0);
  }

#if TARGET_API_MAC_CARBON
  /* In OS X, 'Quit' moves from File to the Application Menu */
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

int main(int argc, char **argv) {
  Rect windowRect;

#if TARGET_API_MAC_TOOLBOX
  InitGraf((Ptr)&qd.thePort);
  InitFonts();
  InitWindows();
  InitMenus();
  TEInit();
  InitDialogs(nil);
#endif

  InitCursor();

  FlushEvents(everyEvent, 0);

  setupAppleEvents();

  setupMenus();

  //create a window
  SetRect(&windowRect, 50, 50, 50+SCREEN_WIDTH, 50+SCREEN_HEIGHT);

  mainWindowPtr = NewWindow(
      nil,
      &windowRect,
      APP_NAME_STRING,
      true,
      noGrowDocProc,
      (WindowPtr)-1L,
      true,
      (long)nil
    );

  SetPort(GetWindowPort(mainWindowPtr));
  

  eventLoop();

  return 0;
}
