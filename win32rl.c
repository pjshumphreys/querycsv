#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>

#include "win32.h"

#define FALSE 0
#define TRUE 1

/*
BOOL WINAPI ReadConsoleInput(
  _In_  HANDLE        hConsoleInput,
  _Out_ PINPUT_RECORD lpBuffer,
  _In_  DWORD         nLength,
  _Out_ LPDWORD       lpNumberOfEventsRead
);

BOOL WINAPI SetConsoleCursorPosition(
  _In_ HANDLE hConsoleOutput,
  _In_ COORD  dwCursorPosition
);

BOOL WINAPI GetConsoleScreenBufferInfo(
  _In_  HANDLE                      hConsoleOutput,
  _Out_ PCONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo
);

BOOL WINAPI SetConsoleWindowInfo(
  _In_       HANDLE     hConsoleOutput,
  _In_       BOOL       bAbsolute,
  _In_ const SMALL_RECT *lpConsoleWindow
);
*/

void reallocMsg(char *failureMessage, void** mem, size_t size) {
  void * temp = NULL;
  if((temp = realloc(*mem, size)) == NULL) {
    fputs(failureMessage, stderr);
    exit(EXIT_FAILURE);
  }

  *mem = temp;
}

int strAppendUTF8(long codepoint, unsigned char ** nfdString, int nfdLength) {
  if (codepoint < 0x80) {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+1);

    (*nfdString)[nfdLength++] = codepoint;
  }
  else if (codepoint < 0x800) {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+2);

    (*nfdString)[nfdLength++] = (codepoint >> 6) + 0xC0;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else if (codepoint < 0x10000) {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+3);

    (*nfdString)[nfdLength++] = (codepoint >> 12) + 0xE0;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }
  else {
    reallocMsg("realloc failure\n", (void**)nfdString, nfdLength+4);

    (*nfdString)[nfdLength++] = (codepoint >> 18) + 0xF0;
    (*nfdString)[nfdLength++] = ((codepoint >> 12) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = ((codepoint >> 6) & 0x3F) + 0x80;
    (*nfdString)[nfdLength++] = (codepoint & 0x3F) + 0x80;
  }

  return nfdLength;
}

int strAppend(char c, char** value, size_t* strSize) {
  char* temp;

  //validate inputs
  //increase value length by 1 character

  //update the string pointer
  //increment strSize
  if(strSize != NULL) {
    if(value != NULL) {
      if((temp = realloc(*value, (*strSize)+1)) != NULL) {
      *value = temp;

      //store the additional character
      (*value)[*strSize] = c;
      }
      else {
      return FALSE;
      }
    }

    (*strSize)++;
  }

  return TRUE;
}


int main(int argc, char* argv[]){
  INPUT_RECORD buffer[512];
  wchar_t buffer2;
  char* retval = NULL;
  size_t retLength = 0;
  HANDLE std_in = GetStdHandle(STD_INPUT_HANDLE);
  HANDLE std_out = GetStdHandle(STD_OUTPUT_HANDLE);
  CONSOLE_SCREEN_BUFFER_INFO screen;
  CONSOLE_SCREEN_BUFFER_INFO cursor;
  CONSOLE_SCREEN_BUFFER_INFO space;
  long * lineHead = NULL;
  int lineHeadLength = 0;
  long * reversedLineTail = NULL;
  int lineTailLength = 0;
  int cursorPosition = 0;
  int useEnter = FALSE;
  COORD position;
  unsigned long eventsRead;
  unsigned long currentEvent;
  WORD wVirtualKeyCode;
  wchar_t UnicodeChar;

  int argc2;
  char ** argv2;
  setupWin32(&argc2, &argv2);

  SetConsoleMode(std_in, 0);
  //SetConsoleMode(std_out, 0);

  fputs_w32("\033[?7l", stdout);

  //store the current position in the console buffer
  GetConsoleScreenBufferInfo(std_out, &screen);

  for (;;) {
    //read console events buffer
    ReadConsoleInputW(std_in, &buffer, 512, &eventsRead);
    for(currentEvent = 0; currentEvent < eventsRead; currentEvent++) {
      if(
          buffer[currentEvent].EventType == KEY_EVENT &&
          buffer[currentEvent].Event.KeyEvent.bKeyDown == 0
        ) {

        wVirtualKeyCode = buffer[currentEvent].Event.KeyEvent.wVirtualKeyCode;

        switch(wVirtualKeyCode) {
          case VK_BACK: //backspace
            //fputs("BACKSPACE\n", stdout);
            if(lineHeadLength > 0) {
              lineHeadLength -= 1;

              if(lineHeadLength > 0) {
                reallocMsg("realloc failure", &lineHead, lineHeadLength*sizeof(long));
              }
              else {
                free(lineHead);
                lineHead = NULL;
              }
            }
          break;

          case VK_DELETE: //delete
            //fputs("DELETE\n", stdout);
            if(lineTailLength > 2) {
              lineTailLength -= 1;
              reallocMsg("realloc failure", &reversedLineTail, lineTailLength*sizeof(long));
            }
            else {
              lineTailLength = 0;
              free(reversedLineTail);
              reversedLineTail = NULL;
            }
          break;

          case VK_RETURN: //enter
            if(useEnter) {
              //fputs("ENTER\n", stdout);
              retLength = 0;
              for(cursorPosition = 0; cursorPosition < lineHeadLength; cursorPosition++) {
                retLength = strAppendUTF8(lineHead[cursorPosition], &retval, retLength);
              }

              for(cursorPosition = lineTailLength-1; cursorPosition > -1;cursorPosition--) {
                retLength = strAppendUTF8(reversedLineTail[cursorPosition], &retval, retLength);
              }

              strAppend('\0', &retval, &retLength);
              fputs_w32("\n", stdout);
              fputs_w32(retval, stdout);
              return 0;
            }
          break;

          case VK_HOME: //home
            fputs("HOME\n", stdout);
          break;

          case VK_END: //end
            fputs("END\n", stdout);
          break;

          case VK_LEFT: //left
            //fputs("LEFT\n", stdout);
            if(lineHeadLength > 0) {
              lineTailLength += 1;
              reallocMsg("realloc failure", &reversedLineTail, lineTailLength*sizeof(long));
              reversedLineTail[lineTailLength-1] = lineHead[lineHeadLength-1];

              lineHeadLength -= 1;
              
              if(lineHeadLength > 0) {
                reallocMsg("realloc failure", &lineHead, lineHeadLength*sizeof(long));
              }
              else {
                free(lineHead);
                lineHead = NULL;
              }
            }            
          break;

          case VK_RIGHT: //right
            //fputs("RIGHT\n", stdout);
            if(lineTailLength > 0) {
              lineHeadLength += 1;
              reallocMsg("realloc failure", &lineHead, lineHeadLength*sizeof(long));
              lineHead[lineHeadLength-1] = reversedLineTail[lineTailLength-1];

              lineTailLength -= 1;
              
              if(lineTailLength > 0) {
                reallocMsg("realloc failure", &reversedLineTail, lineTailLength*sizeof(long));
              }
              else {
                free(reversedLineTail);
                reversedLineTail = NULL;
              }
            }
          break;

          default:
            UnicodeChar = buffer[currentEvent].Event.KeyEvent.uChar.UnicodeChar;
            
            if(UnicodeChar != 0) {
              lineHeadLength += 1;
              reallocMsg("realloc failure", &lineHead, lineHeadLength*sizeof(long));

              //TODO: non BMP unicode codepoint handling
              lineHead[lineHeadLength-1] = (long)UnicodeChar;
            }
          break;
        }
      }
    }

    //display the updated line buffer
    SetConsoleCursorPosition(std_out, screen.dwCursorPosition);
    retLength = 0;
    
    for(cursorPosition = 0; cursorPosition < lineHeadLength; cursorPosition++) {
      retLength = strAppendUTF8(lineHead[cursorPosition], &retval, retLength);
    }

    strAppend('\0', &retval, &retLength);
    fputs_w32(retval, stdout);

    GetConsoleScreenBufferInfo(std_out, &cursor);
    retLength = 0;

    for(cursorPosition = lineTailLength-1; cursorPosition > -1;cursorPosition--) {
      retLength = strAppendUTF8(reversedLineTail[cursorPosition], &retval, retLength);
    }

    strAppend(' ', &retval, &retLength);
    strAppend(' ', &retval, &retLength);
    strAppend('\0', &retval, &retLength);
    fputs_w32(retval, stdout);
    //fputs_w32("\033[J", stdout);

    SetConsoleCursorPosition(std_out, cursor.dwCursorPosition); 

    useEnter = TRUE;
  }

  //free the malloced piece chain data 

  return 0;
} 
