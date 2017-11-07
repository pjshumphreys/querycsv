#include <stdlib.h>

#include <Quickdraw.h>
#include <MacWindows.h>
#include <Dialogs.h>
#include <Menus.h>
#include <ToolUtils.h>
#include <Resources.h>
#include <TextUtils.h>

QDGlobals qd;   /* qd is needed by the Macintosh runtime */

#include "powermac.h"

/* just show an informative error message */
int main(void) {
  Str255 message;

  MaxApplZone();
  InitGraf((Ptr)&qd.thePort);
  InitFonts();
  InitWindows();
  InitMenus();
  TEInit();
  InitDialogs(NULL);
  InitCursor();

  SetCursor(&qd.arrow);

  //type Str255 is an array in MPW 3
  GetIndString(message, kErrStrings, eWrongSystem);
  ParamText(message, (unsigned char *)"", (unsigned char *)"", (unsigned char *)"");
  Alert(rUserAlert, NULL);

  return EXIT_FAILURE;
}
