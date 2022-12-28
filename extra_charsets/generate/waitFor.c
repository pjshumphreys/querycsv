#include <stdio.h>
#include <unistd.h>

int main(int argc, char**argv) {
  printf("Waiting until %s exists\n", argv[1]);

  while (!access(argv[1], F_OK) == 0) {
    // file doesn't exist
    sleep(1);
  }

  printf("Found %s!\n", argv[1]);

  return 0;
}
