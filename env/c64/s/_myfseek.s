.include "../code2.s"
.export _myfseek
.proc	_myfseek: near


;
; int myfseek(FILE *stream, long offset, int origin) {
;
	jsr     pushax
;
; for(current = 0; current<offset; current++) {
;
	jsr     decsp6
	ldx     #$00
	stx     sreg
	stx     sreg+1
	txa
	ldy     #$02
	jsr     steaxysp
L0BFC:	ldy     #$05
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$0F
	jsr     ldeaxysp
	jsr     toslteax
	beq     L0BFD
;
; c = fgetc(stream);
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     _fgetc
	jsr     stax0sp
;
; for(current = 0; current<offset; current++) {
;
	ldy     #$05
	jsr     ldeaxysp
	jsr     saveeax
	ldy     #$01
	jsr     inceaxy
	ldy     #$02
	jsr     steaxysp
	jsr     resteax
	jmp     L0BFC
;
; return 0;
;
L0BFD:	ldx     #$00
	txa
;
; }
;
	ldy     #$0E
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ tree_walkAndCleanup (__near__ struct qryData *, __near__ __near__ struct resultTree * *, __near__ __near__ function returning void *)
; ---------------------------------------------------------------


.endproc
