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

  //set xterm f key legacy mode, disable line wrapping, get current position
  write(STDOUT_FILENO, "\033[?1060h\033[7l\033[6n", 16);
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
            
            return;
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

        free(partialEscape);
        return;
      }
      else if(c_in == 0x7F) {
        if(state == BYTES_INITIAL) {
          strAppend(buffer, &c_in, bufferLength, 1);
          return;
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
          return;
        }
        else if(c_in < 0x80) {  //ascii byte
          strAppend(buffer, &c_in, bufferLength, 1);
          return;
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
          
          return;
        }
      } break;

      case BYTES_ESCAPE: {
        if(bytesread == 0) {
          strAppend(buffer, "\033[7~\x9C", bufferLength, 5);   //escape key was pressed
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
              free(partialEscape);
              //don't output an escape sequence at all here as one will have been generated already
              return;
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
                free(partialEscape);
                return;
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
            free(partialEscape);
            return;
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
              free(partialEscape);
              return;
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
              free(partialEscape);
              return;
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
            free(partialEscape);
            return;
          }
          else if (c_in == 0x3A || (c_in > 0x3B && c_in < 0x40)) {
            state = BYTES_CSI_IGNORE;
          }
        }
      } break;

      case BYTES_CSI_IGNORE: {
        if(bytesread) {

          if(c_in > 0x3F && c_in < 0x7F) {
            strAppend(buffer, partialEscape, bufferLength, partialEscapeLength);
            strAppend(buffer, &c_in, bufferLength, 1);
            strAppend(buffer, "\x9C", bufferLength, 1);
            free(partialEscape);
            return;
          }
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
          free(partialEscape);
          return;
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
  fflush(stdout);
}

void gotoXY(long x, long y) {
  printf("\033[%ld;%ldH", y, x);
  fflush(stdout);
}

void getXY() {
  write(STDOUT_FILENO, "\033[6n", 4);
}

void moveDownAndGetXY() { //move down then get position
  write(STDOUT_FILENO, "\033[1B\r\033[6n", 9);
}

void scrollDownAndGetXY() {
  write(STDOUT_FILENO, "\033[7h\r\012\033[7l\033[6n", 14);
}

void moveLeftAndGetXY() {
  write(STDOUT_FILENO, "\r\033[6n", 5);
}

void clearScreenEnd() {
  write(STDOUT_FILENO, "\033[0J", 4);
}

void carrageReturn() {
  write(STDOUT_FILENO, "\r", 1);
}

void GotoXYAndOutputCodepoint(long x, long y, long c) {
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

  if( x == -1 && y == -1) {
    printf("%s\033[6n", utf8);
  }
  else {
    printf("\033[%ld;%ldH%s\033[6n", y, x, utf8);
  }
  
  fflush(stdout);
}

int isValidUtf8(unsigned long c) {
  return (
      c < 0xD800 ||
      (c > 0xDFFF && c < 0xFDD0) ||
      (c > 0xFDEF && c < 0x110000 && (c & 0xFFFE ) != 0xFFFE)
    );
}

/*
   code that's the same for both posix and win32
*/

void getSequence(char*buffer, int*length) {  
  int i;

  if((*buffer) == '\0') {
    (*length) = 0;
    return;
  }
  else if((*buffer) == '\x9C') {
    (*length) = 1;
    return;
  }
  else if((*buffer) == '\033' || (((unsigned char)(*buffer)) & 0x80)) {   //multibyte escape sequence. getBytes will put a \x9C character after them to simplify the code here
    i = 1;
    
    while (buffer[i] != 0 && buffer[i] != 0x9C) {
      i++;
    }
    
    (*length) = i-1;

    return;
  }

  //ascii character. just return it
  (*length) = 1;
  return;
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

int strAppendCodePoint(char** buffer, int c, int* bufferlength) {
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
#define STATE_WAIT 2
#define STATE_FIRST 3
#define STATE_WRAP 4
#define STATE_NEXT 5

struct codePointEntry {
  long codepoint;
  int wcwidth;
  int afterX;
}

void screenio() {
  long currentX;
  long currentY;

  long startX;
  long startY;
  int outputLine;

  long screenX;
  long screenY;  

  long cursorX;
  long cursorY;
  
  long oldX;
  long oldY;

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

  long * lineHead = NULL;
  int lineHeadLength = 0;

  long * reversedLineTail = NULL;
  int lineTailLength = 0;

  int cursorPosition = 0;
  int currentPosition = 0;
  int oldCursorPosition = 0;

  int enterPressed = 0;

  int reRender = 0;

  int dumb = 0;
  int i;
  
  for( ; ; ) {
    getBytes(&stdinBuffer, &stdinBufferLength); //Will return a number of whole
    //utf-8 chars or complete escape sequences. Internally this may be
    //converting keyboard scan codes or the results of function calls to
    //escape sequences or other char sets to utf-8. Will not return invalid or
    //overlong utf-8 codepoints.
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

              cursorX = currentX;
              cursorY = currentY;
              
              state = STATE_WAIT;  // wait for a character to be typed
            } break;

            case STATE_WAIT: {  //should never happen, but just in case
              fputs("bug in the state machine", stderr);
            } break;

            case STATE_FIRST: { //check whether the screen has changed size since we last rendered
              if(reRender || screenX != currentX || screenY != currentY) {
                screenX = currentX;
                screenY = currentY;
                currentPosition = 0;
                outputLine = startY;
                //output the first character then call getxy
                GotoXYAndOutputCodepoint(startX, startY, (*lineHead));
              }
              else {
                currentPosition = oldCursorPosition;
                outputLine = cursorY;
                GotoXYAndOutputCodepoint(cursorX, cursorY, lineHead[currentPosition]);
              }
              
              state = STATE_NEXT;
            } break;

            case STATE_WRAP: {  //test whether we need to scroll down
              if(currentY == oldY) {  //we've hit the bottom of the screen as well. 
                //if the characters remaining extend off the bottom of the screen then stop outputting them and goto the current position.
                if(
                    currentPosition > cursorPosition &&
                    currentPosition < lineHeadLength + lineTailLength &&
                    startY < 1
                  ) {

                  gotoXY(cursorX, cursorY);
                  state = STATE_WAIT;
                  break;
                }
                //otherwise, scroll down 1 line
                else { 
                  startX--;
                  cursorX--;

                  scrollDownAndGetXY();
                  state = STATE_NEXT;
                  break;
                }
              }
              
              state = STATE_NEXT;
            } //fall through to case 5

            case STATE_NEXT: {  //measure the current cursor position, then decide what to do next
              currentPosition++;
              if(currentX != oldX) {
                //everything's ok. output the next character and re-measure
                oldX = currentX;
                oldY = currentY;

                //if we are in the right position, store the current cursor position for later
                if(currentPosition == cursorPosition) {
                  cursorX = currentX;
                  cursorY = currentY;
                }

                //output the character
                if(currentPosition < lineHeadLength) {
                  GotoXYAndOutputCodepoint(-1, -1, lineHead[currentPosition]);
                }
                else if (currentPosition < lineHeadLength + lineTailLength) {
                  GotoXYAndOutputCodepoint(-1, -1, reversedLineTail[lineTailLength-(currentPosition-lineHeadLength)]);
                }

                //if we are at the end of the string to display, clear the rest of the screen and
                //go to the cursor position we've stored previously
                else if(currentPosition == lineHeadLength + lineTailLength) {
                  if(enterPressed) {
                    clearScreenEnd();

                    //put the characters remaining in stdinBuffer into filteredBuffer (without getxy responses)
                    sequence += sequenceLength;
                    getSequence(sequence, &sequenceLength);

                    while(sequenceLength != 0) {
                      if(
                          sequenceLength < 6 ||
                          (*sequence) != '\033' ||
                          sequence[1] != '[' ||
                          sequence[sequenceLength-1] != 'R'
                        ) {
                        strAppend(&filteredBuffer, sequence, &filteredBufferLength, sequenceLength);
                      }

                      sequence += sequenceLength;
                      getSequence(sequence, &sequenceLength);
                    }
                    
                    //put back the remaining characters into the input stream
                    systemBuffer = filteredBuffer;

                    //free the buffers
                    free(stdinBuffer);

                    //filteredBuffer is reused to store the return text
                    filteredBuffer = NULL;
                    filteredBufferLength = 0;

                    //convert the codepoints back into a utf-8 string. it will need to be freed by the caller
                    for(i = 0; i < lineHeadLength; i++) {
                      strAppendCodePoint(&filteredBuffer, lineHead[i], &filteredBufferLength);
                    }

                    for(i = lineTailLength; i > -1; i--){
                      strAppendCodePoint(&filteredBuffer, reversedLineTail[i], &filteredBufferLength);
                    }

                    free(lineHead);
                    free(reversedLineTail);

                    retval = filteredBuffer;
                    return;
                  }
                  else {
                    //move the cursor if we need to
                    if(lineTailLength) {
                      clearScreenEndAndGotoXY(cursorX, cursorY);
                    }
                    else {
                      clearScreenEnd();
                    }
                  
                    //wait for more characters to be typed
                    state = STATE_WAIT; 
                  }
                }
              }
              else {  //we went off the right hand edge of the screen.
                outputLine++;
                
                //if the virtual currentY is still negative and is less than the cursor position, don't move down
                if(outputLine > 1) {
                  oldX = currentX;
                  oldY = currentY;

                  moveDownAndGetXY();
                  state = STATE_WRAP;  //test whether we need to scroll down
                }
                else {
                  moveLeftAndGetXY();   //just go back to the start of the first line
                }
              }
            } break;
          }
        }
        else if(!dumb) {
          //add the character or escape sequence to the buffer
          strAppend(&filteredBuffer, sequence, &filteredBufferLength, sequenceLength);
        }
        else {  //if a dumb terminal, output directly
          switch(*sequence) {
            case '\177': {  //backspace
              fputs("^H", stdout);
            } break;

            case '\010': {   //tab
              //output a number of backspaces (maybe 0), then the new characters to add
            } break;

            case '\012': { //enter
              //put the remaining characters in the systemBuffer, then return the string we obtained
            } break;

            case '\x9C': //string terminator
            break;

            default: {
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
        oldCursorPosition = cursorPosition;
        //swap the character buffers
        filteredBufferCopy = filteredBuffer;
        sequence = filteredBuffer;
        filteredBuffer = NULL;

        //get the next utf-8 character or escape sequence.
        getSequence(sequence, &sequenceLength); 

        while(sequenceLength != 0) {
          if((*sequence) == '\033') {
            if(sequence[1] == '[') {
              sequenceLength-=2;
              sequence+=2;

              if(sequence[sequenceLength-1] == 'D') {  //left
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

                if(cursorPosition > 0) {
                  cursorPosition--;
                }
              }
              else if(sequence[sequenceLength-1] == 'C') {  //right
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
                if(cursorPosition <= lineHeadLength+lineTailLength) {
                  cursorPosition++;
                }
              }
              else if(strncmp("3~", sequence, sequenceLength) == 0) {   //delete
                if(lineTailLength > 2) {
                  lineTailLength -= 1;
                  reallocMsg("realloc failure", &reversedLineTail, lineTailLength*sizeof(long));
                }
                else {
                  lineTailLength = 0;
                  free(reversedLineTail);
                  reversedLineTail = NULL;
                }
              }
              else if(sequence[sequenceLength-1] == 'Z') {  //shift + tab

              }
              else if(strncmp("8~", sequence, sequenceLength) == 0) {   //window resize
                reRender = 1;
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

            }
            else if(
                strncmp("[4~", sequence, sequenceLength) == 0 ||
                strncmp("OF", sequence, sequenceLength) == 0
              ) {   //end

            }
          }
          else if((*sequence) == '\177') { //backspace
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
            if(cursorPosition > 0) {
              cursorPosition--;
              reRender = 1;
            }
          }
          else if ((*sequence) == '\010') {   //tab

          }
          else if ((*sequence) == '\012') {   //enter
            //display the string 1 more time, then return the text we obtained
            filteredBufferLength -= (sequence+1-filteredBufferCopy);
            filteredBuffer = (char*)malloc(filteredBufferLength+1);

            if(filteredBuffer) {
              memcpy(filteredBuffer, &(sequence[1]), filteredBufferLength+1);
            }

            enterPressed = 1;
            break;
          }
          else if (((unsigned char)(*sequence)) > 0x1F && (*sequence) != '\x9C') {  //anything else. just update the codepoint buffer
            //if not currently at the end of the string, enable the re-render flag
            if(lineTailLength) {
              reRender = 1;
            }

            //convert whole utf-8 sequence to a long and stick it in the codepoint buffer
            lineHeadLength += 1;
            reallocMsg("realloc failure", &lineHead, lineHeadLength*sizeof(long));

            lineHead[lineHeadLength-1] = toUTF32(sequence, sequenceLength);

            cursorPosition++;
          }
          
          sequence += sequenceLength;
          getSequence(sequence, &sequenceLength);  //get the next escape sequence or character
        }

        if(filteredBuffer == NULL) {  //if enter was pressed, the buffer will have data put back into it 
          filteredBufferLength = 0;
        }

        free(filteredBufferCopy);
        filteredBufferCopy = NULL;

        state = STATE_FIRST;  
        gotoXY(999, 999);
        getXY();
      }

      stdinBufferLength = 0;
    }
  }
}

int main(int argc, char * argv[]) {
  struct sigaction act;

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

  /* register the tty reset with the exit handler */
  if(atexit(tty_atexit) != 0) {
    fatal("atexit: can't register tty reset");
  }

  tty_raw();  /* put tty in raw mode */
  screenio(); /* run application code */

  return EXIT_SUCCESS;  /* tty_atexit will restore terminal */
}
