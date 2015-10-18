/* getchar example : typewriter */
#include <stdio.h>

int main ()
{
  FILE* temp;
  int c;
  //freopen ("NUL","w",stdin);
  puts ("Enter text. Include a dot ('.') in a sentence to exit:");
setvbuf ( stdin , NULL , _IONBF , 1024 );

  do {
    c=getchar();
    //putchar (c);
  } while (c != '.');
  return 0;
}
