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

#endif

#define TARGET_API_MAC_TOOLBOX (!TARGET_API_MAC_CARBON)
#if TARGET_API_MAC_TOOLBOX
#define GetWindowPort(w) w
QDGlobals qd;   /* qd is needed by the Macintosh runtime */
#endif

#include "macos.h"
#include "ThreadLibraryManager.h"

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
const short windowM = 3;
int lastTick = 0;
int gHasAppleEvents = FALSE;

#if TARGET_API_MAC_TOOLBOX
#include <StandardFile.h>

OSStatus openFile() {
  Point       where;
  char        prompt = '\0';
  short       numTypes = 1;
  SFTypeList  typeList = 'TEXT';
  SFReply     reply;
  OSErr       result;

  where.h = where.v = 70;

  SFGetFile(where, &prompt, nil, 1, &typeList, nil, &reply);

  if(theReply.sfGood) {
    result = SetVol(reply.fName, reply.vRefNum);

    if (result != 0){
        /* error check */
    }
  }

  return noErr;
}
#endif

#if TARGET_API_MAC_CARBON
#include <Navigation.h>

static NavDialogRef gOpenFileDialog = NULL;
static NavEventUPP gEventUPP = NULL;

void TerminateOpenFileDialog() {
	if(gOpenFileDialog != NULL) {
		TerminateDialog(gOpenFileDialog);
    gOpenFileDialog = NULL;
	}
}

static pascal void MyPrivateEventProc(
    NavEventCallbackMessage callbackSelector,
    NavCBRecPtr callbackParms,
    NavCallBackUserData callbackUD
  ) {

#pragma unused (callbackUD)

  switch (callbackSelector) {
    case kNavCBUserAction: {
      if(callbackParms->userAction == kNavUserActionOpen) {
        // This is an open files action, send an AppleEvent
        NavReplyRecord  reply;
        OSStatus        status;

        status = NavDialogGetReply(callbackParms->context, &reply);

        if(status == noErr) {
          //SendOpenAE(reply.selection);

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

OSStatus openFile() {
  OSStatus theErr = noErr;
  
  if(gOpenFileDialog == NULL) {
    NavDialogCreationOptions  dialogOptions;
    NavTypeListHandle         openList = NULL;

    NavGetDefaultDialogCreationOptions(&dialogOptions);
    dialogOptions.modality = kWindowModalityNone;

    /*
      YUCK! NavTypeList works by using the 'struct hack'!
    */
    openList = (NavTypeListHandle)NewHandle(sizeof(NavTypeList));
    if (openList != NULL) {
      (*openList)->componentSignature = 'ttxt';
      (*openList)->osTypeCount		    = 1;
      (*openList)->osType[0]          = 'TEXT';
    }

    if(gEventUPP == NULL) {
      gEventUPP = NewNavEventUPP(MyPrivateEventProc);
    }

    theErr = NavCreateGetFileDialog(
        &dialogOptions,
        openList,
        privateEventUPP,
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
  else {
    if(NavDialogGetWindow(gOpenFileDialog) != NULL) {
      SelectWindow(NavDialogGetWindow(gOpenFileDialog));
    }
  }

  return theErr;
}
#endif

int split(char *str, char delim, char ***array, int *length) {
  char *p;
  char **res;
  int count = 0;
  int k = 0;

  p = str;

  // Count occurance of delim in string
  while((p = strchr(p, delim)) != NULL) {
    *p = 0; // Null terminate the deliminator.
    p++; // Skip past our new null
    count++;
  }

  // allocate dynamic array
  res = calloc(1, count * sizeof(char *));
  if(!res) {
    return -1;
  }

  p = str;

  for(k=0; k<count; k++) {
    if(*p) {
      res[k] = p;  // Copy start of string
    }

    p = strchr(p, 0);    // Look for next null
    p++; // Start of next string
  }

  *array = res;
  *length = count;

  return 0;
}

#define ROOTID 2 /* Root directory ID */
fopen_mac(const char *filename, const char *mode) {
  FILE *retval = NULL;

  unsigned char* filenameCopy= NULL;
  char** pathArray = NULL;
  int pathParts = 0;
  int currentPart = 0;
  int fileNameLen = 0;
  OSErr result;

  short vRefNum;
  long dirID;
  
  short vRefNumOrig;
  long dirIDOrig;

  //if not colons then the file should be in the current volume and folder
  if(strchr(filename, ':') == NULL) {    
    return fopen(filename, mode);
  }

  //allocate some temporary space for HGetVol to output into
  if((filenameCopy = (char *)malloc(256)) == NULL) {
    return NULL;
  }

  //backup original volume and directory ids
  if(result = HGetVol(filenameCopy, &vRefNum, &dirID)) {
    free(filenameCopy);
    errno = EFAULT;
    return NULL;
  };

  vRefNumOrig = vRefNum;
  dirIDOrig = dirID;

  fileNameLen = strlen(filename)+1;

  if((filenameCopy = (unsigned char *)realloc(strlen(fileNameLen))) == NULL) {
    return NULL;
  }

  //copy the string (as split modifies it)
  strncpy(filenameCopy, filename,fileNameLen);

  //split by colon
  if(split(filenameCopy, ':', &pathArray, &pathParts)) {
    free(filenameCopy);
    return NULL;
  }

  // if more than 0 delimiers then the part changes folder
  for(currentPart = 0; currentPart<pathParts-1; currentPart++) {
    //if path item length is zero then go to the parent folder
    if(pathArray[currentPart] == NULL) {

    }

    //if first character in path is not a colon then search begining with volume name
    else if(currentPart == 0) {
      //lookup volume by name
      if(HSetVol(pathArray[0], vRefNum, ROOTID) != noErr) {
        errno = ENOENT;
        free(filenameCopy);
        free(pathArray);
        return NULL;
      }
    }
    
    //if volume name/directory name is not found then set perror and return NULL
    else {
      errno = ENOENT;
      HSetVol(NULL, vRefNumOrig, dirIDOrig);
      free(filenameCopy);
      free(pathArray);
      return NULL;
    }
  }
 
  //if no colons then file should exist in the current folder
  retval = fopen(pathArray[partParts], mode);

  //restore original volume and directory ids
  HSetVol(NULL, vRefNumOrig, dirIDOrig);

  //clean up allocated memory
  free(filenameCopy);
  free(pathArray);

  return retval;
}

fputs_mac() {

}

fprintf_mac() {

}

getch_mac() {

}


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

    case mWindow: {

    } break;

    default: {
    } break;
  }

  HiliteMenu(0);
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
        case mouseUp: {
          MouseState = 0;
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
              windRect = BPGetScreenBounds();
              DragWindow(whichWindow, theEvent.where, &windRect);
            } break;

            case inContent: {
              if(whichWindow != FrontWindow()) {
                SelectWindow(whichWindow);
              }
              else {
                MouseState = 1;
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
          if ((event->message & 0xFFFF0000) != noErr) {
            Point pt = {70, 70};

            DILoad();
            DIBadMount(pt, event->message);
            DIUnload();
          }
        } break;

        case activateEvt: {
        } break;

        case updateEvt: {
          Rect pr = BPGetBounds();

          BeginUpdate((WindowPtr)theEvent.message);
          EraseRect(&pr);
          BPDrawWindow();
          EndUpdate((WindowPtr)theEvent.message);
        } break;

        case kHighLevelEvent: {
          if(gHasAppleEvents) {
            AEProcessAppleEvent(&theEvent);
          }
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

    if (MouseState != 0) {
      GlobalToLocal(&theEvent.where);

      BPTogglePixel((theEvent.where.h/20), (theEvent.where.v/20));

      if ((TickCount()-lastTick) > 0) {
        BPDrawWindow();
        lastTick = TickCount();
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
      result = AEGetParamDesc(
        theAppleEvent,
        keyDirectObject,
        typeAEList,
        &docList
      ) ||
      result = AECountItems(&docList, &itemsInList)
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
  myMenus[windowM] = GetMenu(mWindow);

  for(i = 0; i < 4; i++) {
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
