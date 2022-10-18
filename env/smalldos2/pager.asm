.8087
		PUBLIC main2_
		PUBLIC _pageNumber
		PUBLIC funcstart_
;  INCLUDE <Exports.inc>
		EXTRN	dosload_:BYTE
		EXTRN	b_:BYTE
		EXTRN	fputc_:BYTE
		EXTRN	sprintf_:BYTE
		EXTRN	open_:BYTE
		EXTRN	fputs_:BYTE
		EXTRN	exit_:BYTE
		EXTRN	read_:BYTE
		EXTRN	close_:BYTE
		EXTRN	atol_:BYTE
		EXTRN	abs_:BYTE
		EXTRN	strncpy_:BYTE
		EXTRN	strcmp_:BYTE
		EXTRN	stricmp_:BYTE
		EXTRN	strnicmp_:BYTE
		EXTRN	strstr_:BYTE
		EXTRN	strrchr_:BYTE
		EXTRN	vsnprintf_:BYTE
		EXTRN	memset_:BYTE
		EXTRN	strcat_:BYTE
		EXTRN	strncat_:BYTE
		EXTRN	memcpy_:BYTE
		EXTRN	memmove_:BYTE
		EXTRN	fwrite_:BYTE
		EXTRN	fseek_:BYTE
		EXTRN	fclose_:BYTE
		EXTRN	fread_:BYTE
		EXTRN	fgetc_:BYTE
		EXTRN	ungetc_:BYTE
		EXTRN	fflush_:BYTE
		EXTRN	atexit_:BYTE
DGROUP		GROUP	CONST,CONST2,_DATA,_BSS
_TEXT		SEGMENT	BYTE PUBLIC USE16 'CODE'
		ASSUME CS:_TEXT, DS:DGROUP, ES:_TEXT, SS:DGROUP

main2_:
   push ax
   mov Word Ptr ax,cs
   mov Word Ptr es,ax
   pop ax

funcstart_:  ; the array of call xxxx instructions and page numbers
;  INCLUDE <functions.inc>
	call Near Ptr farcall
	db 1
  call Near Ptr b_
  ret

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
  mov cx, funcstart_+3
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

;INCLUDE <serialLnBC.asm>

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
  call Near Ptr dosload_
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

overlayPlaceHolder:
	dw 0

_TEXT		ENDS
CONST		SEGMENT	WORD PUBLIC USE16 'DATA'
CONST		ENDS
CONST2		SEGMENT	WORD PUBLIC USE16 'DATA'
CONST2		ENDS
_DATA		SEGMENT	WORD PUBLIC USE16 'DATA'
;-----------------------------------------

pageStack:
  dw 0 ;WORD  50  DUP (0) ;must be a multiple of 4 bytes
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

lookupTable:
;  INCLUDE <lookupTable.inc>
  dw 0
lookupTableEnd:

datastart:
;  INCLUDE <data.inc>
	dw 0

_DATA		ENDS
_BSS		SEGMENT	WORD PUBLIC USE16 'BSS'
_BSS		ENDS

		END





