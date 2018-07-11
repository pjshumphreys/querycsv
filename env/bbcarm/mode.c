#include <stdio.h>
#include <stdlib.h>
#include <kernel.h>

int main(int argc, char* argv[]) {
  int mode = 0;

  if(argc == 2) {
    mode = atoi(argv[1]);

    if(mode < 0 || mode > 7) {
      mode = 0;
    }
  }

  _kernel_oswrch(22);
  _kernel_oswrch(mode);

  return 0;
}
