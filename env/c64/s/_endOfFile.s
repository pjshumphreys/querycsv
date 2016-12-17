.include "../code2.s"
.export _endOfFile
.proc	_endOfFile: near


;
; int endOfFile(FILE* stream) {
;
	jsr     pushax
;
; c = fgetc(stream);
;
	jsr     decsp2
	ldy     #$03
	jsr     ldaxysp
	jsr     _fgetc
	jsr     stax0sp
;
; ungetc(c, stream);
;
	jsr     pushax
	ldy     #$05
	jsr     ldaxysp
	jsr     _ungetc
;
; return c != EOF;
;
	jsr     ldax0sp
	cpx     #$FF
	bne     L02F7
	cmp     #$FF
L02F7:	jsr     boolne
;
; }
;
	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; int __near__ strNumberCompare (__near__ unsigned char *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
