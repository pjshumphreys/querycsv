#include <stdio.h>
#include <conio.h>
 
int main( void )
{
   int x, y;
   x = 42;
   y = 42;
   _clrscr();
   _gotoxy(x, y);
 
   printf("gotoxy jumps to cursor position x42 y42.");
 
   getch();
   return 0;
}
