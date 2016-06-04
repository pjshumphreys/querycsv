/* Set terminal (tty) into "raw" mode: no line or other processing done
   Terminal handling documentation:
       curses(3X)  - screen handling library.
       tput(1)     - shell based terminal handling.
       terminfo(4) - SYS V terminal database.
       termcap     - BSD terminal database. Obsoleted by above.
       termio(7I)  - terminal interface (ioctl(2) - I/O control).
       termios(3)  - preferred terminal interface (tc* - terminal control).
*/

#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wcwidth2.c"

static struct termios orig_termios;  /* TERMinal I/O Structure */
int sigwinch = 0;
char* systemBuffer = NULL;  //stores any remaining characters for the next invocation of readLine

void reallocMsg(char *failureMessage, void** mem, size_t size) {
  void * temp = NULL;
  if((temp = realloc(*mem, size)) == NULL) {
    fputs(failureMessage, stderr);
    exit(EXIT_FAILURE);
  }

  *mem = temp;
}

/* reset tty - useful also for restoring the terminal when this process wishes
   to temporarily relinquish the tty */
int tty_reset() {
  /* flush and reset */
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) < 0) {
    return -1;
  }
  
  return 0;
}

/* exit handler for tty reset */
/* NOTE: If the program terminates due to a signal this code will not run. This
   is for exit()'s only.  For resetting the terminal after a signal, a signal
   handler which calls tty_reset is needed. */
void tty_atexit() {
  tty_reset();
}

void tty_sigint(int c) {
  tty_reset();
}

static void sigwinchHandler(int sig) {
  sigwinch = 1;
}

void fatal(char *message) {
  fprintf(stderr, "fatal error: %s\n", message);
  exit(EXIT_FAILURE);
}

/*
   code that differs between posix and win32
*/

/* put terminal in raw mode - see termio(7I) for modes */
void tty_raw() {
  struct termios raw;

  raw = orig_termios;  /* copy original and then modify below */

  /* input modes - clear indicated ones giving: no break, no CR to NL, 
     no parity check, no strip char, no start/stop output (sic) control */
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

  /* output modes - clear giving: no post processing such as NL to CR+NL */
  raw.c_oflag &= ~(OPOST);

  /* control modes - set 8 bit chars */
  raw.c_cflag |= (CS8);

  /* local modes - clear giving: echoing off, canonical off (no erase with 
     backspace, ^U,...),  no extended functions, no signal chars (^Z,^C) */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN);

  /* control chars - set return condition: min number of bytes and timer */
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 2; 

  /* put terminal in raw mode after flushing */
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
    fatal("can't set raw mode");
  }

  setvbuf(stdout, NULL, _IOFBF, 1024);

  //set xterm f key legacy mode, \033[?25lhide cursor, get current position
  fputs("\033[?1060h\033[6n", stdout);
  fflush(stdout);
}

int hashCompare(const void* a, const void* b) {
  return (*((long*)a) < *((long*)b)) ? -1 :
		(*((long*)a) != *((long*)b) ? 1 : 0);
}

#include "hash3.h"

int isCombining(long codepoint) {
  struct hash3Entry * result;

  if(
      codepoint < 0x300 ||
      (
        codepoint > 0x309A && (
          codepoint < 0xA66F || (
            codepoint < 0xFB1E &&
            codepoint > 0xABED
          )
        )
      )
  ) {
    return 0;
  }

  result = (struct hash3Entry*)bsearch(
      (const void*)&codepoint,
      (const void*)&hash3EntryMap,
      745,
      sizeof(struct hash3Entry),
      &hashCompare
    );

  return (result == NULL ? 0 : result->order);
}

long toUTF32(char*sequence, int sequenceLength) {
  switch(sequenceLength) {
    case 1: {
      return ((unsigned char)sequence[0]);
    } break;

    case 2: {
      return (((unsigned char)sequence[0])<<6) + (((unsigned char)sequence[1]))- 0x3080; 
        
    } break;

    case 3: {
      return (((unsigned char)sequence[0])<<12) + (((unsigned char)sequence[1])<<6) + (((unsigned char)sequence[2]))- 0xE2080;
    } break;
  }

  return (((unsigned char)sequence[0])<<18) + (((unsigned char)sequence[1])<<12) + (((unsigned char)sequence[2])<<6) + (((unsigned char)sequence[3])) - 0x3C82080;
}

#define BYTES_INITIAL 1
#define BYTES_ESCAPE 2
#define BYTES_ESCAPE_INTERMEDIATE 3
#define BYTES_CSI_ENTRY 4
#define BYTES_CSI_INTERMEDIATE 5
#define BYTES_CSI_PARAM 6
#define BYTES_CSI_IGNORE 7
#define BYTES_DCS_ENTRY 8
#define BYTES_DCS_INTERMEDIATE 9
#define BYTES_DCS_PARAM 10
#define BYTES_DCS_IGNORE 11
#define BYTES_ESCAPE_CHAR 12
#define BYTES_DCS_PASSTHROUGH 13
#define BYTES_OSC_STRING 14
#define BYTES_STRING 15

void getBytes(char**buffer, int*bufferLength) {
  unsigned char c_in;
  int bytesread;

  int utf8remaining = 0; //utf-8 bytes we are still expecting
  int utf8i;
  unsigned char utf8Buffer[12];

  int state = BYTES_INITIAL;

  unsigned char* partialEscape = NULL;
  int partialEscapeLength = 0;

  if(systemBuffer) {  //if any characters remain to be used after the last time readLine was invoked
    (*buffer) = systemBuffer;
    (*bufferLength) = strlen(systemBuffer);
    systemBuffer = NULL;
    return;
  }

  if(sigwinch) { 
    strAppend(buffer, "\033[8~\x9C", bufferLength, 5);   // the terminal window changed size
    sigwinch = 0;
    return;
  }
    
  for( ; ; ) {
    //try to read 1 byte
    bytesread = read(STDIN_FILENO, &c_in, 1);

    //state insensitive action
    if(bytesread) {
      if(utf8remaining) {
        utf8remaining--;
        
        if((c_in & 0xC0) == 0x80) {
          utf8Buffer[utf8i++] = c_in;

          if(utf8remaining == 0) {
            if(
                (
                  utf8i == 2 ||
                  (
                    utf8i == 3 &&
                    (utf8Buffer[0] != 0xE0 || utf8Buffer[1] > 0x9F)
                  ) ||
                  (
                    utf8i == 4 &&
                    (utf8Buffer[0] != 0xF0 || utf8Buffer[1] > 0x8F) &&
                    (utf8Buffer[0] != 0xF4 || utf8Buffer[1] < 0x90)
                  )
                ) &&
                isValidUtf8(toUTF32(utf8Buffer, utf8i))
              ) {
              //output the bytes directly
              strAppend(buffer, utf8Buffer, bufferLength, utf8i);
              strAppend(buffer, "\x9C", bufferLength, 1);
            }
            else {
              //codepoint was overlong or invalid. output the individual bytes as utf-8
              for(utf8remaining = utf8i-1; utf8remaining > -1; utf8remaining--) {
                utf8Buffer[(utf8remaining*3)+2] = 0x9C;
                
                if(utf8Buffer[utf8remaining] > 0xBF) {
                  utf8Buffer[(utf8remaining*3)+1] = utf8Buffer[utf8remaining] - 0x40;
                  utf8Buffer[utf8remaining*3] = 0xC3;
                }
                else {
                  utf8Buffer[(utf8remaining*3)+1] = utf8Buffer[utf8remaining];
                  utf8Buffer[utf8remaining*3] = 0xC2;
                }
              }

              strAppend(buffer, utf8Buffer, bufferLength, utf8i*3);
            }
          }
          
          continue;
        }
        else {
          //output each byte up to now as a utf8 char
          for(utf8remaining = utf8i-1; utf8remaining > -1; utf8remaining--) {
            utf8Buffer[(utf8remaining*3)+2] = 0x9C;
            
            if(utf8Buffer[utf8remaining] > 0xBF) {
              utf8Buffer[(utf8remaining*3)+1] = utf8Buffer[utf8remaining] - 0x40;
              utf8Buffer[utf8remaining*3] = 0xC3;
            }
            else {
              utf8Buffer[(utf8remaining*3)+1] = utf8Buffer[utf8remaining];
              utf8Buffer[utf8remaining*3] = 0xC2;
            }
          }

          strAppend(buffer, utf8Buffer, bufferLength, utf8i*3);
          utf8remaining = 0;
        }
      }

      if(
          c_in < 0x20 ||
          (
            (c_in & 0x80) != 0 &&
            c_in < 0xA0
          )
        ) {

        switch(state) {
          case BYTES_OSC_STRING:
          case BYTES_DCS_PASSTHROUGH:
          case BYTES_STRING:
            if(c_in < 0x20 && c_in != 0x1B && c_in != 0x18 && c_in != 0x1A) {
              strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);
              continue;
            }
            else {
              strAppend(buffer, partialEscape, bufferLength, partialEscapeLength);
              strAppend(buffer, "\033\\\x9C", bufferLength, 3);
            }
          break;
        }

        partialEscapeLength = 0;

        switch(c_in) {
          case 0x1B: {
            state = BYTES_ESCAPE;
            continue;
          } break;

          case 0x9B: {
            state = BYTES_CSI_ENTRY;
            strAppend(&partialEscape, "\033[", &partialEscapeLength, 2);
            continue;
          } break;
          
          case 0x8F: {
            state = BYTES_ESCAPE_CHAR;
            strAppend(&partialEscape, "\033O", &partialEscapeLength, 2);
            continue;
          } break;

          case 0x18:
          case 0x1A:
          break;

          case 0x84: {
            strAppend(buffer, "\033D\x9C", bufferLength, 3);
          } break;

          case 0x85: {
            strAppend(buffer, "\033E\x9C", bufferLength, 3);
          } break;

          case 0x88: {
            strAppend(buffer, "\033H\x9C", bufferLength, 3);
          } break;          

          case 0x8D: {
            strAppend(buffer, "\033M\x9C", bufferLength, 3);
          } break;

          case 0x8E: {
            state = BYTES_ESCAPE_CHAR;
            strAppend(&partialEscape, "\033N", &partialEscapeLength, 2);
            continue;
          } break;
          
          case 0x90: {
            state = BYTES_DCS_ENTRY;
            strAppend(&partialEscape, "\033P", &partialEscapeLength, 2);
            continue;
          } break;

          case 0x96: {
            strAppend(buffer, "\033V\x9C", bufferLength, 3);
          } break;

          case 0x97: {
            strAppend(buffer, "\033W\x9C", bufferLength, 3);
          } break;

          case 0x98: {
            state = BYTES_STRING;
            strAppend(&partialEscape, "\033X", &partialEscapeLength, 2);
            continue;
          } break;
          
          case 0x9A: {
            strAppend(buffer, "\033Z\x9C", bufferLength, 3);
          } break;

          case 0x9D: {
            state = BYTES_OSC_STRING;
            strAppend(&partialEscape, "\033]", &partialEscapeLength, 2);
            continue;
          } break;

          case 0x9E: {
            state = BYTES_STRING;
            strAppend(&partialEscape, "\033^", &partialEscapeLength, 2);
            continue;
          } break;

          case 0x9F: {
            state = BYTES_STRING;
            strAppend(&partialEscape, "\033_", &partialEscapeLength, 2);
            continue;
          } break;

          default: {
            if(c_in < 0x20) {
              switch(state) {
                case BYTES_STRING:
                case BYTES_OSC_STRING:
                case BYTES_DCS_ENTRY:
                case BYTES_DCS_IGNORE:
                case BYTES_DCS_PARAM:
                case BYTES_DCS_INTERMEDIATE:
                case BYTES_DCS_PASSTHROUGH:
                  continue;
                break;
              }

              if(c_in > 0x1F || c_in == 0x0A || c_in == 0x09) {
                strAppend(buffer, &c_in, bufferLength, 1);
              }
              else if(c_in == 0x08) {
                strAppend(buffer, "\x7F", bufferLength, 1);
              }
            }
          } break;
        }

        state = BYTES_INITIAL;
        continue;
      }
      else if(c_in == 0x7F) {
        if(state == BYTES_INITIAL) {
          strAppend(buffer, &c_in, bufferLength, 1);
        }
        else if(state == BYTES_OSC_STRING) {
          strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);
        }
        
        continue;
      }
      else if(c_in == 0x00) {
        //skip null bytes
        continue;
      }
    }
        
    switch(state) {
      case BYTES_INITIAL: {
        if(bytesread == 0) {  //no input waiting
          free(partialEscape);
          return;
        }
        else if(c_in < 0x80) {  //ascii byte
          strAppend(buffer, &c_in, bufferLength, 1);
        }
        else if(c_in > 0xC1 && c_in < 0xF5) {   //probably utf-8 initial byte
          //first byte of utf-8 char
          utf8Buffer[0] = c_in;
          utf8i = 1;

          if (c_in < 0xE0) { 
            //read 2 bytes
            utf8remaining = 1;
          }
          else if (c_in < 0xF0) {
            //read 3 bytes
            utf8remaining = 2;
          }
          else {
            //read 4 bytes
            utf8remaining = 3;
          }
        }
        else {
          utf8Buffer[2] = 0x9C;
            
          if(c_in > 0xBF) {
            utf8Buffer[1] = c_in - 0x40;
            utf8Buffer[0] = 0xC3;
          }
          else {
            utf8Buffer[1] = c_in;
            utf8Buffer[0] = 0xC2;
          }

          strAppend(buffer, utf8Buffer, bufferLength, 3);
        }
      } break;

      case BYTES_ESCAPE: {
        if(bytesread == 0) {
          strAppend(buffer, "\033[7~\x9C", bufferLength, 5);   //escape key was pressed
          free(partialEscape);
          return;
        }
        else {
          switch(c_in) {
            case 0x5B: {
              state = BYTES_CSI_ENTRY;
              strAppend(&partialEscape, "\033[", &partialEscapeLength, 2);
            } break;

            case 0x5D: {
              state = BYTES_OSC_STRING;
              strAppend(&partialEscape, "\033]", &partialEscapeLength, 2);
            } break;

            case 0x50: {
              state = BYTES_DCS_ENTRY;
              strAppend(&partialEscape, "\033P", &partialEscapeLength, 2);
            } break;

            case 0x58: {
              state = BYTES_STRING;
              strAppend(&partialEscape, "\033X", &partialEscapeLength, 2);
            } break;
            
            case 0x5E: {
              state = BYTES_STRING;
              strAppend(&partialEscape, "\033^", &partialEscapeLength, 2);
            } break;
            
            case 0x5F: {
              state = BYTES_STRING;
              strAppend(&partialEscape, "\033_", &partialEscapeLength, 2);
            } break;

            case 0x5C: {
              //don't output an escape sequence at all here as one will have been generated already
              state = BYTES_INITIAL;
            } break;

            default: {
              if(c_in > 0x19 && c_in < 0x30) {
                state = BYTES_ESCAPE_INTERMEDIATE;
                strAppend(&partialEscape, "\033", &partialEscapeLength, 1);
                strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);
              }
              else {
                strAppend(buffer, "\033", bufferLength, 1);
                strAppend(buffer, &c_in, bufferLength, 1);
                strAppend(buffer, "\x9C", bufferLength, 1);
                state = BYTES_INITIAL;
              }
            } break;
          }
        }
      } break;

      case BYTES_ESCAPE_INTERMEDIATE: {
        if(bytesread) {
          strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);

          if (c_in > 0x2F) {
            strAppend(buffer, partialEscape, bufferLength, partialEscapeLength);
            strAppend(buffer, "\x9C", bufferLength, 1);
            state = BYTES_INITIAL;
          }
        }
      } break;

      case BYTES_CSI_ENTRY: {
        if(bytesread) {
          strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);
          
          if(c_in < 0x30) {
            state = BYTES_CSI_INTERMEDIATE;
          }
          else if (c_in > 0x3F) {
            if(c_in == 0x4E || c_in == 0x4F) {
              state = BYTES_ESCAPE_CHAR;
            }
            else {
              strAppend(buffer, partialEscape, bufferLength, partialEscapeLength);
              strAppend(buffer, "\x9C", bufferLength, 1);
              state = BYTES_INITIAL;
            }
          }
          else if (c_in == 0x3A) {
            state = BYTES_CSI_IGNORE;
          }
          else {
            state = BYTES_CSI_PARAM;
          }
        }
      } break;

      case BYTES_CSI_INTERMEDIATE: {
        if(bytesread) {
          strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);

          if(c_in > 0x2F) {
            if(c_in > 0x3F) {
              strAppend(buffer, partialEscape, bufferLength, partialEscapeLength);
              strAppend(buffer, "\x9C", bufferLength, 1);
              state = BYTES_INITIAL;
            }
            else {
              state = BYTES_CSI_IGNORE;
            }
          }
        }
      } break;

      case BYTES_CSI_PARAM: {
        if(bytesread) {
          strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);

          if(c_in < 0x30) {
            state = BYTES_CSI_INTERMEDIATE;
          }
          else if(c_in > 0x3F) {
            strAppend(buffer, partialEscape, bufferLength, partialEscapeLength);
            strAppend(buffer, "\x9C", bufferLength, 1);
            state = BYTES_INITIAL;
          }
          else if (c_in == 0x3A || (c_in > 0x3B && c_in < 0x40)) {
            state = BYTES_CSI_IGNORE;
          }
        }
      } break;

      case BYTES_CSI_IGNORE: {
        if(bytesread && c_in > 0x3F && c_in < 0x7F) {
          strAppend(buffer, partialEscape, bufferLength, partialEscapeLength);
          strAppend(buffer, &c_in, bufferLength, 1);
          strAppend(buffer, "\x9C", bufferLength, 1);
          state = BYTES_INITIAL;
        }
      } break;

      case BYTES_DCS_ENTRY: {
        if(bytesread) {
          strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);
          
          if(c_in < 0x30) {
            state = BYTES_DCS_INTERMEDIATE;
          }
          else if (c_in > 0x3F) {
            state = BYTES_DCS_PASSTHROUGH;
          }
          else if (c_in == 0x3A) {
            state = BYTES_DCS_IGNORE;
          }
          else {
            state = BYTES_DCS_PARAM;
          }
        }
      } break;

      case BYTES_DCS_INTERMEDIATE: {
        if(bytesread) {
          strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);

          if(c_in > 0x2F) {
            if(c_in > 0x3F) {
              state = BYTES_DCS_PASSTHROUGH;
            }
            else {
              state = BYTES_DCS_IGNORE;
            }
          }
        }
      } break;

      case BYTES_DCS_PARAM: {
        if(bytesread) {
          strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);

          if(c_in < 0x30) {
            state = BYTES_DCS_INTERMEDIATE;
          }
          else if (c_in == 0x3A || (c_in > 0x3B && c_in < 0x40)) {
            state = BYTES_DCS_IGNORE;
          }
          else {
            state = BYTES_DCS_PASSTHROUGH;
          }
        }
      } break;

      case BYTES_DCS_IGNORE:
      break;

      case BYTES_ESCAPE_CHAR: {
        if(bytesread) {
          strAppend(buffer, partialEscape, bufferLength, partialEscapeLength);
          strAppend(buffer, &c_in, bufferLength, 1);
          strAppend(buffer, "\x9C", bufferLength, 1);
          state = BYTES_INITIAL;
        }
      } break;

      case BYTES_DCS_PASSTHROUGH:
      case BYTES_OSC_STRING:
      case BYTES_STRING: {
        if(bytesread) {
          strAppend(&partialEscape, &c_in, &partialEscapeLength, 1);
        }
      } break;
    }
  }
}

void clearScreenEndAndGotoXY(long x, long y) {
  printf("\033[0J\033[%ld;%ldH", y, x);
}

void gotoXY(long x, long y) {
  printf("\033[%ld;%ldH", y, x);
}

void getXY() {
  fputs("\033[6n", stdout);
}

void moveDownAndGetXY() { //move down then get position
  fputs("\033[1B\r\033[6n", stdout);
}

void scrollDownAndGetXY() {
  fputs("\033[7h\r\012\033[7l\033[6n", stdout);
}

void moveLeftAndGetXY() {
  fputs("\r\033[6n", stdout);
}

void clearScreenEnd() {
  fputs("\033[0J", stdout);
}

void carrageReturn() {
  fputs("\r", stdout);
}

void outputCodepoint(unsigned long c) {
  char utf8[5];

  //convert codepoint to utf 8
  if (c<0x80) {
    utf8[0]=c;
    utf8[1]='\0';    
  }
  else if (c<0x800) {
    utf8[0]=192+c/64;
    utf8[1]=128+c%64;
    utf8[2]='\0'; 
  }
  else if (c<0x10000) {
    utf8[0]=224+c/4096;
    utf8[1]=128+c/64%64;
    utf8[2]=128+c%64;
    utf8[3]='\0';
  }
  else {
    utf8[0]=240+c/262144;
    utf8[1]=128+c/4096%64;
    utf8[2]=128+c/64%64;
    utf8[3]=128+c%64;
    utf8[4]='\0';
  }

  printf("%s", utf8);
}

int isValidUtf8(unsigned long c) {
  return (
      c < 0xD800 ||
      (c > 0xDFFF && c < 0xFDD0) ||
      (c > 0xFDEF && c < 0x110000 && (c & 0xFFFE ) != 0xFFFE)
    );
}

#include "bidi.h"

int compareCodepoints (const void * a, const void * b) {
  return (*((unsigned long*)a) < ((struct bidi_entry*)b)->codepoint)?-1:
  ((*((unsigned long*)a) == ((struct bidi_entry*)b)->codepoint)?0:1);
}

unsigned char getBidiClass(unsigned long c) {
  struct bidi_entry *retval;
  
  if (c < 256) {
    return ansiBidiLookup[c];
  }

  retval = (struct bidi_entry *)bsearch(
      &c,
      &unicodeBidiLookup,
      9193,
      sizeof(struct bidi_entry),
      compareCodepoints
    );

  return retval?retval->bidiClass:BIDI_L;
}

/*
   code that's the same for both posix and win32
*/

void getSequence(unsigned char*buffer, int*length) {  
  int i;

  if((*buffer) == 0) {
    (*length) = 0;
    return;
  }
  else if((*buffer) == 0x9C) {
    (*length) = 1;
    return;
  }
  else if((*buffer) == '\033' || ((*buffer) & 0x80)) {   //multibyte escape sequence. getBytes will put a \x9C character after them to simplify the code here
    i = 1;
    
    while (buffer[i] != 0 && buffer[i] != 0x9C) {
      i++;
    }
    
    (*length) = i;

    return;
  }

  //ascii character. just return it
  (*length) = 1;
  return;
}

#define KEY_LEFT 1
#define KEY_RIGHT 2
#define KEY_DELETE 3
#define KEY_BACKSPACE 4
#define KEY_TAB 5
#define KEY_SHIFT_TAB 6
#define KEY_ENTER 7
#define KEY_HOME 8
#define KEY_END 9
#define KEY_CHAR 10
#define KEY_OTHER 11
#define KEY_RESIZE 12

int getKey(unsigned char*sequence, int sequenceLength) {
  if((*sequence) == '\033') {
    if(sequence[1] == '[') {
      sequenceLength-=2;
      sequence+=2;

      if(sequence[sequenceLength-1] == 'D') {  //left
        return KEY_LEFT;
      }
      else if(sequence[sequenceLength-1] == 'C') {  //right
        return KEY_RIGHT;
      }
      else if(strncmp("3~", sequence, sequenceLength) == 0) {   //delete
        return KEY_DELETE;
      }
      else if(sequence[sequenceLength-1] == 'Z') {  //shift + tab
        return KEY_SHIFT_TAB;
      }
      else if(strncmp("8~", sequence, sequenceLength) == 0) {   //window resize
        return KEY_RESIZE;
      }
      else {
        sequenceLength+=1;
        sequence-=1;
      }
    }
    else {
      sequenceLength-=1;
      sequence+=1;
    }

    if(
        strncmp("[1~", sequence, sequenceLength) == 0 ||
        strncmp("OH", sequence, sequenceLength) == 0
      ) {   //home
      return KEY_HOME;
    }
    else if(
        strncmp("[4~", sequence, sequenceLength) == 0 ||
        strncmp("OF", sequence, sequenceLength) == 0
      ) {   //end
      return KEY_END;
    }
  }
  else if((*sequence) == '\177') { //backspace
    return KEY_BACKSPACE;
  }
  else if ((*sequence) == '\010') {   //tab
    return KEY_TAB;
  }
  else if ((*sequence) == '\012') {   //enter
    return KEY_ENTER;
  }
  else if ((*sequence) > 0x1F && (*sequence) != 0x9C) {
    return KEY_CHAR;
  }

  return KEY_OTHER;
}

struct codePointEntry {
  long codepoint;
  int wcwidth;
};

int measureFullChar(
    int *i,
    struct codePointEntry *lineHead,
    int lineHeadLength,
    struct codePointEntry *reversedLineTail,
    int lineTailLength
  ) {

  int currentPosition, currentCombiner;
  struct codePointEntry * currentEntry;
          
  for(currentPosition = *i; currentPosition <= lineHeadLength+lineTailLength -1; currentPosition++) {
    if(currentPosition < lineHeadLength) {
      currentEntry = &(lineHead[currentPosition]);
    }
    else {
      currentEntry = &(reversedLineTail[lineTailLength+lineHeadLength-currentPosition-1]);
    }

    if(currentEntry->wcwidth == -1) {
      fputs("\xE2\x80\x8E", stdout);

      outputCodepoint(currentEntry->codepoint);

      *i = currentPosition;

      if(!isCombining(currentEntry->codepoint)) {
        for(
            currentCombiner = currentPosition + 1;
            currentCombiner <= lineHeadLength+lineTailLength -1;
            currentCombiner++
          ) {

          if(currentCombiner < lineHeadLength) {
            currentEntry = &(lineHead[currentCombiner]);
          }
          else {
            currentEntry = &(reversedLineTail[lineTailLength+lineHeadLength-currentCombiner-1]);
          }

          if(isCombining(currentEntry->codepoint)) {
            outputCodepoint(currentEntry->codepoint);
            currentEntry->wcwidth = 0;
          }
          else {
            break;
          }
        }
      }

      fputs("\xE2\x80\x8E\033[6n", stdout);
      return 0;
    }
  }

  return 1;
}

void renderText(
    struct codePointEntry *lineHead,
    int lineHeadLength,
    struct codePointEntry *reversedLineTail,
    int lineTailLength,
    long startX,
    long startY,
    long screenX,
    long screenY
  ) {

  int currentPosition;
  int currentWidth;
  int availableWidth;
  struct codePointEntry * currentEntry;
  
  printf("\033[%ld;%ldH\033[0m\033[J\xE2\x80\x8E", startY, startX);

  for(
      currentPosition = 0, currentWidth = 0, availableWidth = screenX - startX +1;
      currentPosition <= lineHeadLength+lineTailLength -1;
      currentPosition++
    ) {

    if(currentPosition < lineHeadLength) {
      currentEntry = &(lineHead[currentPosition]);
    }
    else {
      currentEntry = &(reversedLineTail[lineTailLength+lineHeadLength-currentPosition-1]);
    }

    if(currentPosition == lineHeadLength) {
      fputs("\033[30;42m", stdout);
    }

    if(currentWidth + currentEntry->wcwidth <= availableWidth) {
      currentWidth += currentEntry->wcwidth;
    }
    else {
      currentWidth = currentEntry->wcwidth;
      availableWidth = screenX;
      fputs("\r\012\xE2\x80\x8E", stdout);
    }

    if(currentPosition > lineHeadLength && (!isCombining(currentEntry->codepoint)) ) {
      fputs("\033[0m", stdout);
    }

    outputCodepoint(currentEntry->codepoint);
  }

  //if(lineTailLength == 0) {
  //  fputs("\033[30;42m \033[0m\033[J", stdout);
  //}
  //else {
    fputs("\033[0m\033[J", stdout);
  //}
}


int strAppend(char** buffer, char *addString, int* bufferLength, int addStringLength) {
  char * temp;
  int size;
  
  if(buffer && bufferLength) {
    size = (*bufferLength);
    
    if( (temp = (char*)realloc( (*buffer), size + addStringLength + 1 )) == NULL ) {
      return 0;
    }

    memcpy(&(temp[size]), addString, addStringLength);

    size += addStringLength;
    
    temp[size] = '\0';

    (*buffer) = temp;
    (*bufferLength) = size;
    
    return 1;
  }

  return 0;
}

int strAppendCodePoint(char** buffer, long c, int* bufferlength) {
  char utf8[4];

  //convert codepoint to utf 8
  if (c<0x80) {
    utf8[0]=c;
    strAppend(buffer, utf8, bufferlength, 1);    
  }
  else if (c<0x800) {
    utf8[0]=192+c/64;
    utf8[1]=128+c%64;
    strAppend(buffer, utf8, bufferlength, 2);    
  }
  else if (c<0x10000) {
    utf8[0]=224+c/4096;
    utf8[1]=128+c/64%64;
    utf8[2]=128+c%64;
    strAppend(buffer, utf8, bufferlength, 3);
  }
  else {
    utf8[0]=240+c/262144;
    utf8[1]=128+c/4096%64;
    utf8[2]=128+c/64%64;
    utf8[3]=128+c%64;
    strAppend(buffer, utf8, bufferlength, 4);
  }
}

#define STATE_ENTRY 1
#define STATE_SIZE 2
#define STATE_WAIT 3
#define STATE_FIRST 4
#define STATE_WRAP 5
#define STATE_NEXT 6
#define STATE_MEASURE 7
#define STATE_RESIZE 8
#define STATE_START_MEASURE 9
#define STATE_GET_POS 10


void screenio() {
  long currentX;
  long currentY;

  long startX;
  long startY;
  int outputLine;

  long screenX;
  long screenY;
  
  long oldX;
  long oldY;

  int availableWidth;
  int currentWidth;

  int state = STATE_ENTRY; 

  char* stdinBuffer = NULL;
  int stdinBufferLength = 0;

  char* sequence = NULL;
  int sequenceLength = 0;

  char* filteredBuffer = NULL;
  int filteredBufferLength = 0;

  char* filteredBufferCopy = NULL;

  char * retval = NULL;

  char* end;

  struct codePointEntry * lineHead = NULL;
  int lineHeadLength = 0;

  struct codePointEntry * reversedLineTail = NULL;
  int lineTailLength = 0;

  int cursorPosition = 0;
  int currentPosition = 0;

  int enterPressed = 0;

  int reRender = 0;

  int dumb = 0;
  int i;
  int key;

  for( ; ; ) {
    getBytes(&stdinBuffer, &stdinBufferLength); //Will return a number of whole
    //utf-8 chars or complete escape sequences, separated by \x9C bytes if the
    //sequence takes multiple bytes. Internally this may be converting keyboard
    //scan codes or the results of function calls to escape sequences or other
    //char sets to utf-8. Will not return invalid or overlong utf-8 codepoints.
    //Will not return ascii zero (other than at the end of the string ;) )
    //for posix, will convert 8 bit control chars to their 7 bit equivalent.
    //for win32, will treat them as windows 1252 characters and convert them to
    //their utf-8 equivalent

    if(stdinBufferLength) {
      sequence = stdinBuffer; //stdinBuffer address will be incremented
      getSequence(sequence, &sequenceLength); //get the next utf-8 character or escape sequence.

      while(sequenceLength != 0) {
        //if report cursor position sequence
        if(
            (*sequence) == '\033' &&
            sequence[1] == '[' &&
            sequence[sequenceLength-1] == 'R'
          ) {

          //parse x and y co ordinates
          currentY = strtol(&(sequence[2]), &end, 10);
          end++;
          currentX = strtol(end, NULL, 10);
          sequenceLength++; //skip the \x9C byte that will be coming afterwards

          //the state machine code
          switch(state) {
            case STATE_ENTRY: {   //initial entry to the readline method.
              startX = currentX;
              startY = currentY;

              state = STATE_SIZE;
              fputs("\033[999;999H\033[6n", stdout);
            } break;

            case STATE_SIZE:
              state = STATE_WAIT;  // wait for a character to be typed
            //fall through to STATE_RESIZE

            case STATE_RESIZE: {
              screenX = currentX;
              screenY = currentY;

              printf("\033[%ld;%ldH", startY, startX);

              if(state != STATE_WAIT) {
                //rerender the text
              }
            } break;

            case STATE_WAIT: {  //should never happen, but just in case
              fputs("bug in the state machine", stderr);
            } break;

            case STATE_GET_POS: {
              oldX = currentX;
              oldY = currentY;

              state = STATE_MEASURE;
              currentPosition = 0;

              //measure the width of the first full char we don't know the width of
              //(i.e. together with its combining characters)            
              measureFullChar(
                  &currentPosition,
                  lineHead,
                  lineHeadLength,
                  reversedLineTail,
                  lineTailLength
                );
            } break;

            case STATE_MEASURE: {
              //if the lines don't match then measure again at the start of the next line
              if(currentY != oldY) {
                state = STATE_START_MEASURE;
                
                //move cursor, clear end of line, move to next line, left to right mark, get position
                printf("\033[%ld;%ldH\033[K\r\012\xE2\x80\x8E\033[6n", oldY, oldX);
              }
              else if(currentX < oldX) {
                //the cursor was on the last line of the screen
                startY -=1;
                oldY -=1;

                state = STATE_START_MEASURE;

                //move cursor, clear end of line, move to next line, left to right mark, get position
                printf("\033[%ld;%ldH\033[K\r\012\xE2\x80\x8E\033[6n", oldY, oldX);
              }
              else {
                if(currentPosition < lineHeadLength) {
                  lineHead[currentPosition].wcwidth = currentX - oldX;
                }
                else {
                  reversedLineTail[lineTailLength+lineHeadLength-currentPosition-1].wcwidth = currentX - oldX;
                }

                gotoXY(oldX, oldY);

                if(measureFullChar(     //measure next character
                      &currentPosition,
                      lineHead,
                      lineHeadLength,
                      reversedLineTail,
                      lineTailLength
                    )
                  ) {
                  renderText(
                      lineHead,
                      lineHeadLength,
                      reversedLineTail,
                      lineTailLength,
                      startX,
                      startY,
                      screenX,
                      screenY
                    );

                   state = STATE_WAIT;
                }
              }

              fflush(stdout);
            } break;

            case STATE_START_MEASURE: {
              oldX = currentX;
              oldY = currentY;

              state = STATE_MEASURE;
              
              //measure next character
              measureFullChar(     
                  &currentPosition,
                  lineHead,
                  lineHeadLength,
                  reversedLineTail,
                  lineTailLength
                );

              fflush(stdout);
            } break;
          }

          fflush(stdout);
        }
        else if(!dumb) {
          //add the character or escape sequence to the buffer
          strAppend(&filteredBuffer, sequence, &filteredBufferLength, sequenceLength);
        }
        else {  //if a dumb terminal, output directly
          key = getKey(sequence, sequenceLength);
          
          switch(key) {
            case KEY_BACKSPACE: {  //backspace
              fputs("^H", stdout);
            } break;

            case KEY_TAB: {   //tab
              //output a number of backspaces (maybe 0), then the new characters to add
            } break;

            case KEY_ENTER: { //enter
              //put the remaining characters in the systemBuffer, then return the string we obtained
            } break;

            case KEY_CHAR: {
              if(((unsigned char)(*sequence)) > 0x1F) {  //any other printable character
                //just output it
              }
            } break;
          }
        }

        sequence += sequenceLength;
        getSequence(sequence, &sequenceLength);  //get the next escape sequence or character
      }

      //if there are characters that have not yet been output
      if(state == STATE_WAIT && filteredBufferLength != 0) {
        //swap the character buffers
        filteredBufferCopy = filteredBuffer;
        sequence = filteredBuffer;
        filteredBuffer = NULL;

        //get the next utf-8 character or escape sequence.
        getSequence(sequence, &sequenceLength); 

        while(sequenceLength != 0) {
          key = getKey(sequence, sequenceLength);

          switch(key) {
            case KEY_LEFT: {  //left
              if(lineHeadLength > 0) {
                lineTailLength += 1;

                reallocMsg(
                    "realloc failure",
                    &reversedLineTail,
                    lineTailLength*sizeof(struct codePointEntry)
                  );

                reversedLineTail[lineTailLength-1] = lineHead[lineHeadLength-1];

                lineHeadLength -= 1;
                
                if(lineHeadLength > 0) {
                  reallocMsg(
                      "realloc failure",
                      &lineHead,
                      lineHeadLength*sizeof(struct codePointEntry)
                    );
                }
                else {
                  free(lineHead);
                  lineHead = NULL;
                }
              }

              if(cursorPosition > 0) {
                cursorPosition--;
              }
            } break;
            
            case KEY_RIGHT: {  //right
              if(lineTailLength > 0) {
                lineHeadLength += 1;

                reallocMsg(
                    "realloc failure",
                    &lineHead,
                    lineHeadLength*sizeof(struct codePointEntry)
                  );

                lineHead[lineHeadLength-1] = reversedLineTail[lineTailLength-1];

                lineTailLength -= 1;
                
                if(lineTailLength > 0) {
                  reallocMsg(
                      "realloc failure",
                      &reversedLineTail,
                      lineTailLength*sizeof(struct codePointEntry)
                    );
                }
                else {
                  free(reversedLineTail);
                  reversedLineTail = NULL;
                }
              }
              if(cursorPosition <= lineHeadLength+lineTailLength) {
                cursorPosition++;
              }
            } break;
            
            case KEY_HOME: {   //home

            } break;
            
            case KEY_END: {   //end

            } break;
            
            case KEY_TAB: {   //tab

            } break;
            
            case KEY_SHIFT_TAB: {  //shift + tab

            } break;
            
            case KEY_BACKSPACE: { //backspace
              if(lineHeadLength > 0) {
                lineHeadLength -= 1;

                if(lineHeadLength > 0) {
                  reallocMsg(
                      "realloc failure",
                      &lineHead,
                      lineHeadLength*sizeof(struct codePointEntry)
                    );
                }
                else {
                  free(lineHead);
                  lineHead = NULL;
                }
              }
              if(cursorPosition > 0) {
                cursorPosition--;
                reRender = 1;
              }
            } break;

            case KEY_DELETE: {   //delete
              if(lineTailLength > 2) {
                lineTailLength -= 1;

                reallocMsg(
                    "realloc failure",
                    &reversedLineTail,
                    lineTailLength*sizeof(struct codePointEntry)
                  );
              }
              else {
                lineTailLength = 0;
                free(reversedLineTail);
                reversedLineTail = NULL;
              }
            } break;

            case KEY_ENTER: {   //enter
              //display the string 1 more time, then return the text we obtained
              filteredBufferLength -= (sequence+1-filteredBufferCopy);
              filteredBuffer = (char*)malloc(filteredBufferLength+1);

              if(filteredBuffer) {
                memcpy(filteredBuffer, &(sequence[1]), filteredBufferLength+1);
              }

              enterPressed = 1;
              break;  //don't scan for any more characters
            } break;

            case KEY_CHAR: {  //anything else. just update the codepoint buffer
              //if not currently at the end of the string, enable the re-render flag
              if(lineTailLength) {
                reRender = 1;
              }

              //convert whole utf-8 sequence and stick it in the codepoint buffer
              lineHeadLength += 1;
              reallocMsg("realloc failure", &lineHead, lineHeadLength*sizeof(struct codePointEntry));

              currentPosition = lineHeadLength-1;
              
              lineHead[currentPosition].codepoint = toUTF32(sequence, sequenceLength);
              lineHead[currentPosition].wcwidth = mk_wcwidth(lineHead[currentPosition].codepoint);
              /*
              lineHead[currentPosition].wcwidth = -1;   //not yet measured

              //if the codepoint is a combining character re measure its base character also
              while(currentPosition > 0 && isCombining(lineHead[currentPosition].codepoint)) {
                currentPosition--;
                lineHead[currentPosition].wcwidth = -1;
              }

              currentPosition = lineTailLength -1;
              
              while(currentPosition > -1 && isCombining(lineHead[currentPosition].codepoint)) {
                lineHead[currentPosition].wcwidth = -1;
                currentPosition--;
              }
              
              if(state == STATE_WAIT) {
                state = STATE_GET_POS;
              }*/

              cursorPosition++;
            } break;

            case KEY_RESIZE: {   //window resize
              //state = STATE_RESIZE;
            } break;
          }
          
          sequence += sequenceLength;
          getSequence(sequence, &sequenceLength);  //get the next escape sequence or character
        }

        if(filteredBuffer == NULL) {  //if enter was pressed, the buffer will have data put back into it 
          filteredBufferLength = 0;
        }

        free(filteredBufferCopy);
        filteredBufferCopy = NULL;

        oldX = startX;
        oldY = startY;

        switch(state) {
          case STATE_RESIZE: {
            gotoXY(999, 999);
            getXY();
          } break;
          
          case STATE_GET_POS: {
            getXY();
          } break;

          case STATE_WAIT: {
            renderText(
                lineHead,
                lineHeadLength,
                reversedLineTail,
                lineTailLength,
                startX,
                startY,
                screenX,
                screenY
              );
          } break;
        }

        fflush(stdout);
      }

      stdinBufferLength = 0;
    }
  }
}

int main(int argc, char * argv[]) {
  struct sigaction act;
  struct sigaction act2;

  /* check that input is from a tty */
  if(!isatty(STDIN_FILENO)) {
    fatal("not on a tty");
  }

  /* store current tty settings in orig_termios */
  if(tcgetattr(STDIN_FILENO, &orig_termios) < 0) {
    fatal("can't get tty settings");
  }

  act.sa_handler = tty_sigint;
  sigaction(SIGINT, &act, NULL);

  act2.sa_handler = sigwinchHandler;
  sigaction(SIGWINCH, &act2, NULL);

  /* register the tty reset with the exit handler */
  if(atexit(tty_atexit) != 0) {
    fatal("atexit: can't register tty reset");
  }

  tty_raw();  /* put tty in raw mode */
  screenio(); /* run application code */

  return EXIT_SUCCESS;  /* tty_atexit will restore terminal */
}
