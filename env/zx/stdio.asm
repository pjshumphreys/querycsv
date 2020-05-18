;-----------------------------------
; stdio file blocks

CRT_ENABLE_STDIO = 1
__CLIB_DISABLE_FGETS_CURSOR = 0
__CRT_KEY_CAPS_LOCK = 6
__CRT_KEY_DEL = 12

PUBLIC  __sgoioblk
PUBLIC  __sgoioblk_end
PUBLIC  __FOPEN_MAX
PUBLIC _stkend
DEFC    CLIB_FOPEN_MAX = 10
defc    __FOPEN_MAX = CLIB_FOPEN_MAX

GLOBAL __CRT_KEY_CAPS_LOCK
GLOBAL __CRT_KEY_DEL
GLOBAL __CLIB_DISABLE_FGETS_CURSOR

__sgoioblk:
  defs CLIB_FOPEN_MAX * 10      ;stdio control block
__sgoioblk_end:        ;end of stdio control block

_stkend equ 0x5c65
