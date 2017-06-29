/* temporary 'program' with which to test whether the webworker to host page
output interface is working correctly*/

#include <stdio.h>
#include <unistd.h>

/*the real entry point for the html5 version of querycsv. we can't use main as it's treated specially on emscripten and so can only be called once (on page load which we don't want) */
int realmain(int argc, char **argv);
/*
int realmain(int argc, char **argv) {
  int i;

  printf("Hello world!\nProgram arguments: %d\n", argc);

  for(i = 0; i < argc; i++) {
    printf("%d: %s\n", i, argv[i]);
  }

  //I believe this type coercion is valid in emscripten.
  //It should always print 0
  printf("%d\n", (int)argv[argc]);

  return 0;
}
//*/

int wrapmain(char *path, char* filename) {
  int retval;
  static char *argv2[3];

  chdir(path);

  argv2[0] = "querycsv";
  argv2[1] = filename;
  argv2[2] = NULL;

  retval = realmain(2, (char**)argv2);

  fputs("\n", stdout);

  return retval;
}
