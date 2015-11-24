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

static struct termios orig_termios;  /* TERMinal I/O Structure */

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

void fatal(char *message) {
  fprintf(stderr, "fatal error: %s\n", message);
  exit(EXIT_FAILURE);
}

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

  /* after 5 bytes or .8 seconds after first byte seen */
  //raw.c_cc[VMIN] = 5;
  //raw.c_cc[VTIME] = 8; 

  /* immediate - anything */
  //raw.c_cc[VMIN] = 0;
  //raw.c_cc[VTIME] = 0; 

  /* after two bytes, no timer  */
  //raw.c_cc[VMIN] = 2;
  //raw.c_cc[VTIME] = 0;

  /* after a byte or .8 seconds */
  //raw.c_cc[VMIN] = 0;
  //raw.c_cc[VTIME] = 8;

  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 2; 

  /* put terminal in raw mode after flushing */
  if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) < 0) {
    fatal("can't set raw mode");
  }

  write(STDOUT_FILENO, "\033[\?1060h", 8);
}

/* Read and write from tty - this is just toy code!! 
   Prints T on timeout, quits on q input, prints Z if z input, goes up
   if u input, prints * for any other input character */
void screenio() {
  int bytesread;
  char c_in;
  int esc = 0;
  
  for( ; ; ) {
    bytesread = read(STDIN_FILENO, &c_in, 1 /* read 1 byte */);

    if(bytesread < 1) {
      if(esc) {
        write(STDOUT_FILENO, "^[", 2);
        esc = 0;
      }
    }
    else {
      switch(c_in) { /* 1 byte inputed */
        case '\033':
          esc = 1;
        break;

        case 'q': /* quit - no other way to quit - no EOF */
          return;
        break;
        
        default:
          if(esc) {
            write(STDOUT_FILENO, "^[", 2);
            esc = 0;
          }
          
          write(STDOUT_FILENO, &c_in, 1);
        break;
      }
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
