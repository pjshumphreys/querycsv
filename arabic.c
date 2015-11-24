#include <stdio.h>

int main(int argc, char*argv[]) {

  //printf("\033[?25l‎(يَجِبُ عَلَى الإنْ\033[7mسَ\033[0mانِ)‎");
  printf("\033[40m\033[8mab\033[0m‎");
  fflush(stdout);
  //while(1){};

  return 0;
}
