org 49152

CRT_ENABLE_STDIO = 1
__CRT_KEY_CAPS_LOCK = 6
__CRT_KEY_DEL = 12

CRT_ORG_BSS = 24576
DEFINED_CRT_ORG_BSS = 1

defc  __crt_org_bss = CRT_ORG_BSS

INCLUDE "crt/classic/crt_section.asm"
INCLUDE "./residos128/pager.map"

GLOBAL heap
GLOBAL fputc_cons
GLOBAL dodos

GLOBAL __CRT_KEY_CAPS_LOCK
GLOBAL __CRT_KEY_DEL
