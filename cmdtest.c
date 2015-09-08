#include <stdio.h>
#include "win32.h"

int main(int argc, char** argv){
  int argc2;
  char ** argv2;
  int i;
  FILE* test;
  
  argc2 = argc;
  argv2 = argv;

  setupWin32(&argc2, &argv2);

  for(i = 0; i < argc2; i++) {
    fprintf(stdout, "%s\n", argv2[i]);
  }
  
  test = fopen("日本.txt", "w");
  
  if(test != NULL) {
    for(i = 0; i < argc2; i++) {
      fprintf(test, "%s\n", argv2[i]);
    }

    fclose(test);
  }

  return 0;
}
