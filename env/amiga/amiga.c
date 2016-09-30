/* Hello World in C for Intution (Amiga GUI) */

#include <intuition/intuition.h>

struct IntuitionBase *IntuitionBase = NULL;

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

int main(int argc, char **argv) {
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 0);
  AutoRequest(NULL, &hello_text, NULL, &ok_text, NULL, NULL, 358, 85);
  CloseLibrary(IntuitionBase);
}
