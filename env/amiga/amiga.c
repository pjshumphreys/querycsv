#include "querycsv.h"
#undef main
int realmain(int argc, char **argv);
#include <stdio.h>
#include <errno.h>
#include <intuition/intuition.h>
#include <workbench/startup.h>

struct IntuitionBase *IntuitionBase = NULL;
struct Library *UtilityBase = NULL;

struct IntuiText hello_text4 = {
    AUTOFRONTPEN,
    AUTOBACKPEN,
    AUTODRAWMODE,
    8,
    40,
    AUTOITEXTFONT,
    "3. Double click the script file's icon",
    NULL
  };

struct IntuiText hello_text3 = {
    AUTOFRONTPEN,
    AUTOBACKPEN,
    AUTODRAWMODE,
    8,
    30,
    AUTOITEXTFONT,
    "2. Hold down the Shift key",
    &hello_text4
  };

struct IntuiText hello_text2 = {
    AUTOFRONTPEN,
    AUTOBACKPEN,
    AUTODRAWMODE,
    8,
    20,
    AUTOITEXTFONT,
    "1. Select the QueryCSV icon",
    &hello_text3
  };

struct IntuiText hello_text = {
    AUTOFRONTPEN,
    AUTOBACKPEN,
    AUTODRAWMODE,
    8,
    5,
    AUTOITEXTFONT,
    "To run a script file from the Workbench:",
    &hello_text2
  };

struct IntuiText ok_text = {
    AUTOFRONTPEN,
    AUTOBACKPEN,
    AUTODRAWMODE,
    AUTOLEFTEDGE,
    AUTOTOPEDGE,
    AUTOITEXTFONT,
    "Ok",
    AUTONEXTTEXT
  };

BPTR olddir = (BPTR)-1;
char **myargv = NULL;

void onShutdown(void) {
  free(myargv);

  if(olddir != (BPTR)-1) {
    CurrentDir(olddir);
    olddir = (BPTR)-1;
  }

  fputs("\nPRESS ENTER TO QUIT\n", stdout);
  setvbuf(stdout, NULL, _IONBF, 0);
  while(fgetc(stdout) != 10) { /* Do nothing */}
  fclose(stdout);
}

/* We need to define our own main function as VBCC seems to be doing something automagical with the main function specifically in regard to WBStartup */
int main(int argc, char** argv) {
  struct WBStartup *argmsg;
  struct WBArg *wbarg;
  int idx;
  FILE* fp;

  devNull = "Nil:";  /* null filename on Amiga OS */

  if(argc != 0) {
    return realmain(argc, argv);
  }

  argmsg = (struct WBStartup *)argv;

  if(argmsg->sm_NumArgs < 2) {
    IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
    AutoRequest(NULL, &hello_text, NULL, &ok_text, NULL, NULL, 358, 85);
    CloseLibrary(IntuitionBase);
    return EXIT_FAILURE;
  }

  /* open console for reading and writing so that we can prevent the console window
  from closing until enter has been pressed */
  if((fp = freopen("CON:30/30/510/175/QueryCSV", "a+", stdout)) == NULL) {
    return EXIT_FAILURE;
  }

  if((myargv = malloc(3*sizeof(char*))) == NULL) {
    return EXIT_FAILURE;
  }

  atexit(onShutdown);

  for(idx = 0, wbarg = argmsg->sm_ArgList; idx < argmsg->sm_NumArgs; idx++, wbarg++) {
    if(olddir != (BPTR)-1) {
      CurrentDir(olddir);
      olddir = (BPTR)-1;
    }

    if(wbarg->wa_Lock != (BPTR)0) {
      olddir = CurrentDir(wbarg->wa_Lock);
    }

    myargv[idx] = wbarg->wa_Name;
  }

  myargv[idx] = (char *)0;

  return realmain(idx, myargv);
}
