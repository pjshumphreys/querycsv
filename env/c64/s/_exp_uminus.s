.include "../code2.s"
.export _exp_uminus
.proc	_exp_uminus: near


;
; void exp_uminus(char **value, double leftVal) {
;
	jsr     pusheax
;
; temp = ctof(0);
;
	jsr     decsp4
	lda     #$00
	jsr     __ctof
	jsr     steax0sp
;
; if(fcmp(leftVal, temp)) {
;
	ldy     #$07
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$07
	jsr     ldeaxysp
	jsr     __fcmp
	tax
	beq     L0642
;
; temp = fneg(leftVal);
;
	ldy     #$07
	jsr     ldeaxysp
	jsr     __fneg
	jsr     steax0sp
;
; ftostr(value, temp);
;
	ldy     #$0B
	jsr     pushwysp
	ldx     #$00
	lda     #$21
	jsr     _reallocMsg
	ldy     #$09
	jsr     ldaxysp
	jsr     pushw
	ldy     #$05
	jsr     ldeaxysp
	jsr     __ftostr
	ldy     #$0B
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _strlen
	jsr     incax1
	jsr     _reallocMsg
;
; else {
;
	jmp     L0657
;
; *value = strdup("0");
;
L0642:	ldy     #$0B
	jsr     pushwysp
	lda     #<(l065a)
	ldx     #>(l065a)
	jsr     _strdup
	ldy     #$00
	jsr     staxspidx
;
; }
;
L0657:	ldy     #$0A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ hash_freeTable (__near__ struct columnReferenceHash *)
; ---------------------------------------------------------------


.endproc
