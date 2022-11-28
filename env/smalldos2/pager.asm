.8087
		PUBLIC farcall
		PUBLIC farRet
		PUBLIC codeBlock
		PUBLIC main2_
		PUBLIC _buffer
		INCLUDE <exports.inc>
		EXTRN	b_:BYTE
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

  ;push the far return loader onto the stack so we'll return to it rather than the original caller
  pop dx
  push Word Ptr farRet

  mov Word Ptr [spBackup], sp
  mov sp, Word Ptr [internalSp]

  cmp sp, pageStack
  jnz skip
  jmp abort

skip:
  push dx	; push the original return address onto our secondary stack for safe keeping

  ;push the virtual page to return to onto the stack
  mov bx, Word Ptr [currentVirtualPage]
  push  bx

  push  cx
  pop bx
  mov dl, Byte Ptr [bx]
  ;ld c, (hl)
  ;ld b, 0
  ;call serialLnBC

  lahf
  xchg  al, ah
  push  ax
  xchg  al, ah

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
  mov cl, Byte Ptr [bx]
  lahf
  inc bx
  sahf
  mov ch, Byte Ptr [bx]
  ;call serialLnBC
  mov al, dl
  pop dx

  push  cx  ; store the address of the function to call on the stack for later

  ;change to the appropriate page
  push  dx

  mov dl, al
  mov al, Byte Ptr [currentVirtualPage]
  mov dh, al
  mov al, dl
  mov Byte Ptr [currentVirtualPage], al
  mov al, dh

  ; a = current, e = dl = desired
  call changePage
  pop ax
  xchg  al, ah
  sahf

  pop cx

  mov Word Ptr [internalSp], sp
  mov sp, Word Ptr [spBackup]

  push cx

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

  mov ah, 0x4c     ; "terminate program" sub-function
  int 0x21         ; call dos services

;------------------------------------------------------

changePage: ; is the virtual page currently in a ram page?
  cmp al, dl
  jnz L@0
  ret

L@0:
  cmp dl, 0
  jnz L@1
  ret

L@1:
  mov bh, 0
  mov bl, dl
  mov Word Ptr [_pageNumber], bx

  push ds
  mov ax,cs
  mov ds,ax
  call faq_masm
  pop ds
  ret

faq_masm:
  mov ax, 0x100 ; dosload_ address
  push ax
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

  mov Word Ptr [spBackup], sp
  mov sp, Word Ptr [internalSp]

  pop dx  ; get the virtual page number to return to from the stack

  lahf
  xchg al, ah
  push ax
  xchg al, ah

  mov al, Byte Ptr [currentVirtualPage]

  lahf
  xchg al, ah
  push ax
  xchg al, ah

  mov al, dl
  mov Byte Ptr [currentVirtualPage], al

  pop ax
  xchg al, ah
  sahf

  call changePage

  pop ax
  xchg al, ah
  sahf

  pop dx	; get the original return address from our secondary stack and put it back onto the main stack

  mov Word Ptr [internalSp], sp
  mov sp, Word Ptr [spBackup]

  push dx

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
  db 16401 DUP(0) ; 16384 + 16 + 1 so we can definately start the overlays code from cs:c000 somehow, the same bytes can be fudged to appear at ds:0000 (within a few bytes) somehow also
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

hlBackup:
  dw 0

deBackup:
  dw 0

bcBackup:
  dw 0

axBackup:
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





