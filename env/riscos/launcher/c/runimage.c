/*
  RunImage - a small program to create a Risc OS icon bar icon that files
  can be dragged onto.
  It will then launch the main program in a task window with the full
  pathname as the first parameter passed to the main function
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <kernel.h>
#include <swis.h>

#define TRUE 1
#define FALSE 0

const char applicationName[] = "QueryCSV";
 /*  This is the name displayed */
 /*  on the menu */
 /*  and in the Task Window */
 /*  It should be 12 characters */
 /*  or less */

const char spriteName[] = "!querycsv";
 /*  This is the name of the sprite */
 /*  to use (from !Sprites) */

int windowPosition = -1;
 /*  This should be -1 for the */
 /*  right side of the icon bar */
 /*  or -2 for the left */

/* handy utility function to allocate and write memory strings */
int d_sprintf(char **str, char *format, ...) {
  size_t newSize;
  char *newStr = NULL;
  va_list args;

  /* Check sanity of inputs */
  if(str == NULL || format == NULL) {
    return FALSE;
  }

  /* get the space needed for the new string */
  va_start(args, format);
  newSize = (size_t)(vsnprintf(NULL, 0, format, args)); /* plus '\0' */
  va_end(args);

  /* Create a new block of memory with the correct size rather than using realloc */
  /* as any old values could overlap with the format string. quit on failure */
  if((newStr = (char*)malloc(newSize+1)) == NULL) {
    return FALSE;
  }

  /* do the string formatting for real. */
  va_start(args, format);
  vsprintf(newStr, format, args);
  va_end(args);

  /* ensure null termination of the string */
  newStr[newSize] = '\0';

  /* free the old contents of the output if present */
  free(*str);

  /* set the output pointer to the new pointer location */
  *str = newStr;

  /* everything occurred successfully */
  return newSize;
}

void showMenu(int yOrigin, int *menuBlock) {
  _kernel_swi_regs regs;

  regs.r[1] = (int)(menuBlock);
  regs.r[2] = yOrigin - 68;
  regs.r[3] = 140;
  _kernel_swi(Wimp_CreateMenu, &regs, &regs);
}

void showMessage(void) {
  _kernel_swi_regs regs;
  char * text = NULL;

  d_sprintf(
      &text,
      "xxxxDrag a query file onto this icon to run it with QueryCSV"
    );

  /* overwrite text with reason code */
  ((int *)text)[0] = 1;

  regs.r[0] = (int)text;
  regs.r[1] = 1;
  regs.r[2] = (int)applicationName;
  _kernel_swi(Wimp_ReportError, &regs, &regs);

  free(text);
}

void openFile(char *pathName) {
  _kernel_swi_regs regs;
  char *text = NULL;

  d_sprintf(
      &text,
      "TaskWindow \"<QueryCSV$Dir>.querycsv %s\" -quit -name QueryCSV",
      pathName
    );

  regs.r[0] = (int)text;
  _kernel_swi(Wimp_StartTask, &regs, &regs);

  free(text);
}

/* Initialise the Wimp environment */
int wimpInitialise(void) {
  _kernel_swi_regs regs;

  regs.r[0] = 200;    /* Risc OS 2.0 and up */
  regs.r[1] = 0x4b534154; /* 'TASK' */
  regs.r[2] = (int)applicationName;
  _kernel_swi(Wimp_Initialise, &regs, &regs);

  return regs.r[1];
}

int * createMenu(void) {
  int * menuBlock = (int *)calloc(52, 1);
  int applicationNameLength = 9;

  /* create the menu (for usage later) */
  memcpy((void *)menuBlock, (void *)applicationName, applicationNameLength);
  ((char *)menuBlock)[11] = 0; /* null terminate the string */
  ((char *)menuBlock)[12] = 7;
  ((char *)menuBlock)[13] = 2;
  ((char *)menuBlock)[14] = 7;
  ((char *)menuBlock)[15] = 0;

  menuBlock[4] = 12+16*applicationNameLength; /*characters are each 16 pixels wide, plus a border */
  menuBlock[5] = 44; /* height of menu items */
  menuBlock[6] = 0;  /* vertical gap between menu items */

  /* first menu item */
  menuBlock[7] = 128;  /* menu item flags. bit 7 = last item in the menu */
  menuBlock[8] = -1; /* no sub menu */
  menuBlock[9] = 0x7009031; /*flags from the last menu item */

  memcpy((void *)(((char *)menuBlock)+40), (void *)"Quit", 5);

  return menuBlock;
}

void wimpCreateIcon(int *block) {
  _kernel_swi_regs regs;

  block[0] = windowPosition;
  block[1] = 0;
  block[2] = 0;
  block[3] = 68;
  block[4] = 68;
  block[5] = 0x301a;
  memcpy((void *)(((char *)block)+24), (void *)spriteName, 10);
  regs.r[1] = (int)(block);
  _kernel_swi(Wimp_CreateIcon, &regs, &regs);
}

void poll(int *block, int *menuBlock) {
  _kernel_swi_regs regs;
  int shouldQuit = FALSE;

  do {
    regs.r[0] = 0x1831; /* which messages to listen for */
    regs.r[1] = (int)(block);
    _kernel_swi(Wimp_Poll, &regs, &regs);

    switch (regs.r[0]) {
      case 6: { /* mouse click */
        switch(block[2]) {
          /* middle click? show a menu */
          case 2: {
            showMenu(block[0], menuBlock);
          } break;

          /* just clicking? show some help text */
          case 4: {
            showMessage();
          } break;
        }
      } break;

      case 9: { /* quit message */
        shouldQuit = TRUE;
      } break;

      case 17:
      case 18: { /* user message */
        switch(block[4]) {
          case 0: { /* Message_Quit */
            shouldQuit = TRUE;
          } break;

          case 3: { /* Message_DataLoad */
            openFile((char *)(block)+44);
          } break;
        }
      } break;
    }
  } while (shouldQuit == FALSE);
}

void wimpCloseDown(int taskHandle) {
  _kernel_swi_regs regs;

  regs.r[0] = taskHandle;
  regs.r[1] = 0x4b534154; /* 'TASK' */
  _kernel_swi(Wimp_CloseDown, &regs, &regs);
}

int main(void) {
  int taskHandle = wimpInitialise();

  int * menuBlock = createMenu();

  int * block = (int *)calloc(0x301, 1);

  wimpCreateIcon(block);

  poll(block, menuBlock);

  free(block);

  free(menuBlock);

  wimpCloseDown(taskHandle);

  return EXIT_SUCCESS;
}
