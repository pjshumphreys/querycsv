.8087
		PUBLIC farcall
		PUBLIC farRet
		PUBLIC codeBlock
		PUBLIC main2_
		PUBLIC _buffer
		PUBLIC _pageNumber
		PUBLIC _success
		PUBLIC funcstart
		INCLUDE <exports.inc>
		EXTRN	dosload_:BYTE
		EXTRN	b_:BYTE
		EXTRN	__I4D:BYTE
		EXTRN	__EDA:BYTE
		EXTRN	__FDA:BYTE
		EXTRN	__FDC:BYTE
		EXTRN	__FDD:BYTE
		EXTRN	__FDM:BYTE
		EXTRN	__FDN:BYTE
		EXTRN	__EDC:BYTE
		EXTRN	__FDS:BYTE
		EXTRN	__I4FD:BYTE
		EXTRN	strcmp_:BYTE
		EXTRN	strlen_:BYTE
		EXTRN	strcat_:BYTE
		EXTRN	strcpy_:BYTE
		EXTRN	memset_:BYTE
		EXTRN	abs_:BYTE
DGROUP		GROUP	CONST,CONST2,_DATA,_BSS
_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:_TEXT, DS:DGROUP, ES:_TEXT, SS:DGROUP

funcstart:  ; the array of call xxxx instructions and page numbers
main2_:
  include <functions.inc>
	call Near Ptr farcall
	db 1
  call Near Ptr b_
  ret

lookupTable:
  include <lookupTable.inc>
lookupTableEnd:

;-----------------------------------------

farcall:
  ; backup registers
  push dx
  push cx
  push ax
  mov Word Ptr [hlBackup], bx
  pop bx
  mov Word Ptr [axBackup], bx
  pop bx
  mov Word Ptr [bcBackup], bx
  pop bx
  mov Word Ptr [deBackup], bx

  pop cx  ; (bc) contains virtual page number to use

  ;replace the return address far return loader onto the stack so we'll return to it rather than the original caller
  pop dx  ; dx - original return address

  mov ax, farRet
  push ax

  ;push af
  lahf
  xchg  al, ah
  push  ax
  xchg  al, ah

  ; swap to our hidden stack
  mov Word Ptr [ssBackup], ss
  mov ax, ds
  mov ss, ax
  mov Word Ptr [spBackup], sp
  mov sp, Word Ptr [internalSp]

  ; check for stack overflow
  cmp sp, pageStack
  jnz skip
  jmp abort

skip:
  push dx	; push the original return address onto our secondary stack for safe keeping

  ;push the virtual page to return to onto the stack
  mov bx, Word Ptr [currentVirtualPage]
  push  bx

  push cx
  pop bx  ; copy address of page number to use from cx to bx

  ; get number of virtual page to use into dx
  mov dl, Byte Ptr [cs:bx]
  ;ld c, (hl)
  ;ld b, 0
  ;call serialLnBC

  ;calculate which value in the jump table to use
  or  al, al  ; clear carry bit
  mov cx, funcstart+3
  lahf
  sub bx, cx
  rcr si, 1
  sahf
  rcl si, 1

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  xor al, al
  mov al, bh
  rcr al, 1
  mov bh, al
  mov al, bl
  rcr al, 1
  mov bl, al

  mov cx, lookupTable
  lahf
  add bx, cx
  rcr si, 1
  sahf
  rcl si, 1
  mov cl, Byte Ptr [cs:bx]
  lahf
  inc bx
  sahf
  mov ch, Byte Ptr [cs:bx]
  ;call serialLnBC
  push cx

  ;change to the appropriate page
  mov al, Byte Ptr [currentVirtualPage]
  mov Byte Ptr [currentVirtualPage], dl

  ; a = al = current, e = dl = desired
  call changePage
  pop cx

  ;swap back to the regular stack
  mov ss, Word Ptr [ssBackup]
  mov Word Ptr [internalSp], sp
  mov sp, Word Ptr [spBackup]

  ;pop af
  pop ax
  xchg  al, ah
  sahf

  push cx ; new function address to jump to

  ;restore all registers and jump to the function we want via ret
  mov bx, Word Ptr [deBackup]
  push  bx
  mov bx, Word Ptr [bcBackup]
  push  bx
  mov bx, Word Ptr [axBackup]
  push  bx
  mov bx, Word Ptr [hlBackup]
  pop ax
  pop cx
  pop dx
  ret

abort:
  mov Word Ptr [spBackup], sp
  mov sp, Word Ptr [internalSp]

	mov dx, overflowMsg
  mov ah, 0x09
  int 0x21

abort2:
  mov ah, 0x4c     ; "terminate program" sub-function
  int 0x21         ; call dos services

;------------------------------------------------------

changePage: ; is the virtual page currently in a ram page?
  cmp al, dl
  jnz L@1
  ret

L@1:
  mov bh, 0
  mov bl, dl
  mov Word Ptr [_pageNumber], bx
  call near Ptr dosload_
  mov ax, Word Ptr [_success]
  cmp al, 0
  jz abort2
  ret

;---------------------------------------------------

farRet:
; backup registers
  push dx
  push cx
  push ax
  mov Word Ptr [hlBackup], bx
  pop bx
  mov Word Ptr [axBackup], bx
  pop bx
  mov Word Ptr [bcBackup], bx
  pop bx
  mov Word Ptr [deBackup], bx

  ;push af
  lahf
  xchg  al, ah
  push  ax
  xchg  al, ah

  ; swap to our hidden stack
  mov Word Ptr [ssBackup], ss
  mov ax, ds
  mov ss, ax
  mov Word Ptr [spBackup], sp
  mov sp, Word Ptr [internalSp]

  pop dx  ; get the virtual page number to return to from the stack

  mov al, Byte Ptr [currentVirtualPage]
  mov Byte Ptr [currentVirtualPage], dl

  ; a = al = current, e = dl = desired
  call changePage

  pop dx	; get the original return address from our secondary stack

  ;swap back to the regular stack
  mov ss, Word Ptr [ssBackup]
  mov Word Ptr [internalSp], sp
  mov sp, Word Ptr [spBackup]

  ;pop af
  pop ax
  xchg  al, ah
  sahf

  push dx ;put the original return address back onto the stack

  mov bx, Word Ptr [deBackup]
  push  bx
  mov bx, Word Ptr [bcBackup]
  push  bx
  mov bx, Word Ptr [axBackup]
  push  bx
  mov bx, Word Ptr [hlBackup]
  pop ax
  pop cx
  pop dx
  ret
  

_TEXT		ENDS
_NULL		SEGMENT	WORD PUBLIC USE16 'BEGDATA'
_buffer:
  db 16384 DUP(0)
_NULL ENDS
_AFTERNULL		SEGMENT	WORD PUBLIC USE16 'BEGDATA'
_AFTERNULL ENDS
CONST		SEGMENT	WORD PUBLIC USE16 'DATA'
CONST		ENDS
CONST2		SEGMENT	WORD PUBLIC USE16 'DATA'
CONST2		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
;-----------------------------------------

pageStack:
  dw 52 DUP (0) ;must be a multiple of 4 bytes
pageStackEnd:

;-----------------------------------------

overflowMsg:
  db "Stack overflow!", 0Dh, 0Ah, 24h

_pageNumber:
	dw 0

_success:
  dw 0

hlBackup:
  dw 0

deBackup:
  dw 0

bcBackup:
  dw 0

axBackup:
	dw 0

ssBackup:
  dw 0

spBackup:
  dw 0

internalSp:
  dw pageStackEnd

currentVirtualPage: ; which virtual page currently is loaded into the memory at 0xc000-0xffff
  db 0

;----------------------------------------------

datastart:
  INCLUDE <data.inc>
	dw 0

_DATA		ENDS
_BSS		SEGMENT	WORD PUBLIC USE16 'BSS'
_BSS		ENDS

		END





