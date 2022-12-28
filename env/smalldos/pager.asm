.8086
		PUBLIC farcall
		PUBLIC farRet
		PUBLIC main2_
		PUBLIC _buffer
		PUBLIC _currentVirtualPage
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
DGROUP		GROUP	_BEFORECONST, CONST,CONST2,_DATA,_BSS
_TEXT		SEGMENT	PARA PUBLIC USE16 'CODE'
		ASSUME CS:_TEXT, DS:DGROUP, ES:_TEXT, SS:DGROUP

main2_:
  mov Word Ptr [cs:axBackup], ax
  mov ax,ds
  mov Word Ptr [cs:dsInternal], ax
  mov ax, Word Ptr [cs:axBackup]

funcstart:  ; the array of call xxxx instructions and page numbers
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
  mov Word Ptr [cs:axBackup], ax
  mov ax,ds
  mov Word Ptr [cs:dsBackup], ax
  mov ax, Word Ptr [cs:dsInternal]
  mov ds,ax
  mov Word Ptr [hlBackup], bx
  mov Word Ptr [bcBackup], cx
  mov Word Ptr [deBackup], dx

  pop bx  ; bx contains the address of the virtual page number to use. bx is also used to calculate the jump table offset

  ;replace the return address far return loader onto the stack so we'll return to it rather than the original caller
  pop cx  ; cx - original return address

  mov ax, farRet
  push ax

  ;push af
  lahf
  push  ax

  ; swap to our hidden stack
  mov Word Ptr [ssBackup], ss
  mov ax, ds
  mov ss, ax
  mov Word Ptr [spBackup], sp
  mov sp, Word Ptr [internalSp]

  ; check for stack overflow
  cmp sp, pageStack
  jg skip
  jmp abort

skip:
  ; get number of virtual page to use into dx
  mov dl, Byte Ptr [cs:bx]

  push cx	; push the original return address onto our secondary stack for safe keeping

  ;calculate which value in the jump table to use
  mov cx, funcstart+3
  clc  ; clear carry bit
  sub bx, cx

  ;shift right to divide by 2 (for groups of 2 rather than 4)
  xor al, al
  mov al, bh
  rcr al, 1
  mov bh, al
  mov al, bl
  rcr al, 1
  mov bl, al

  mov cx, lookupTable
  clc
  add bx, cx
  mov cl, Byte Ptr [cs:bx]
  inc bx
  mov ch, Byte Ptr [cs:bx]

  ;change to the appropriate page
  mov ax, Word Ptr [_currentVirtualPage]
  push ax

  push cx ; - cx is now the resultant address we should jump to

  cmp dl, 0
  jz skip2
  mov dh, 0
  mov Word Ptr [_currentVirtualPage], dx

  ; a = al = current, e = dl = desired
  call changePage
skip2:
  pop cx

  ;swap back to the regular stack
  mov ss, Word Ptr [ssBackup]
  mov Word Ptr [internalSp], sp
  mov sp, Word Ptr [spBackup]

  ;pop af
  pop ax
  sahf

  push cx ; new function address to jump to

  ;restore all registers and jump to the function we want via ret
  mov dx, Word Ptr [deBackup]
  mov cx, Word Ptr [bcBackup]
  mov bx, Word Ptr [hlBackup]
  mov ax, Word Ptr [cs:dsBackup]
  mov ds,ax
  mov ax,Word Ptr [cs:axBackup]
  ret

abort:
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
  call near Ptr dosload_
  mov ax, Word Ptr [_success]
  cmp al, 0
  jz abort2
  ret

;---------------------------------------------------

farRet:
; backup registers
  mov Word Ptr [cs:axBackup], ax
  mov ax,ds
  mov Word Ptr [cs:dsBackup], ax
  mov ax,Word Ptr [cs:dsInternal]
  mov ds,ax
  mov Word Ptr [hlBackup], bx
  mov Word Ptr [bcBackup], cx
  mov Word Ptr [deBackup], dx

  ;push af
  lahf
  push  ax

  ; swap to our hidden stack
  mov Word Ptr [ssBackup], ss
  mov ax, ds
  mov ss, ax
  mov Word Ptr [spBackup], sp
  mov sp, Word Ptr [internalSp]

  pop dx  ; get the virtual page number to return to from the stack

  mov ax, Word Ptr [_currentVirtualPage]
  mov Word Ptr [_currentVirtualPage], dx

  ; a = al = current, e = dl = desired
  call changePage

  pop dx	; get the original return address from our secondary stack

  ;swap back to the regular stack
  mov ss, Word Ptr [ssBackup]
  mov Word Ptr [internalSp], sp
  mov sp, Word Ptr [spBackup]

  ;pop af
  pop ax
  sahf

  push dx ;put the original return address back onto the stack

  mov dx, Word Ptr [deBackup]
  mov cx, Word Ptr [bcBackup]
  mov bx, Word Ptr [hlBackup]
  mov ax, Word Ptr [cs:dsBackup]
  mov ds,ax
  mov ax,Word Ptr [cs:axBackup]
  ret

axBackup:
	dw 0

dsbackup:
  dw 0

dsInternal:
  dw 0

_TEXT		ENDS
_NULL		SEGMENT	WORD PUBLIC USE16 'BEGDATA'
_NULL ENDS
_AFTERNULL		SEGMENT	WORD PUBLIC USE16 'BEGDATA'
_AFTERNULL ENDS
_BEFORECONST		SEGMENT	PARA PUBLIC USE16 'DATA'
_buffer:

  db 16384 DUP(0)
_BEFORECONST		ENDS
CONST		SEGMENT	WORD PUBLIC USE16 'DATA'
CONST		ENDS
CONST2		SEGMENT	WORD PUBLIC USE16 'DATA'
CONST2		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'

overflowMsg:
  db "Stack overflow!", 24h

_success:
  dw 0

hlBackup:
  dw 0

deBackup:
  dw 0

bcBackup:
  dw 0

ssBackup:
  dw 0

spBackup:
  dw 0

internalSp:
  dw pageStackEnd

_currentVirtualPage: ; which virtual page currently is loaded into the memory at 0xc000-0xffff
  dw 0

pageStack:
  dw 1000 DUP (0) ;must be a multiple of 4 bytes
pageStackEnd:

;----------------------------------------------

datastart:
  INCLUDE <data.inc>
	dw 0

_DATA		ENDS
_BSS		SEGMENT	WORD PUBLIC USE16 'BSS'
_BSS		ENDS

		END





