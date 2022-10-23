/*
  .com file within the final .exe that can load from a file into the code segment
*/

#include <dos.h>
#include <fcntl.h>

unsigned int success = 0;
char pageName[] = "qrycsv01.ovl"; 
char buffer[16384] = { 0 };
int handle = 0;

void main(void) {
  if(_dos_open(pageName, O_RDONLY, &handle) != 0) {
    success = 0;
    return;
  }
  
  _dos_read(handle, buffer, 16384, &success);
  _dos_close(handle);
  success = 1;
}
