org 49152

CRT_ENABLE_STDIO = 1
__CRT_KEY_CAPS_LOCK = 6
__CRT_KEY_DEL = 12

INCLUDE "crt/classic/crt_section.asm"
INCLUDE "./residos128/pager.map"

GLOBAL heap
GLOBAL fputc_cons
GLOBAL dodos

GLOBAL __CRT_KEY_CAPS_LOCK
GLOBAL __CRT_KEY_DEL
