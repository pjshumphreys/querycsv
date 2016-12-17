.include "../code2.s"
.export _exp_divide
.proc	_exp_divide: near


;
; void exp_divide(char **value, double leftVal, double rightVal) {
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
; if(fcmp(rightVal, temp)) {
;
	ldy     #$07
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$07
	jsr     ldeaxysp
	jsr     __fcmp
	tax
	beq     L071D
;
; temp = fdiv(leftVal, rightVal);
;
	ldy     #$0B
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$0B
	jsr     ldeaxysp
	jsr     __fdiv
	jsr     steax0sp
;
; ftostr(value, temp);
;
	ldy     #$0F
	jsr     pushwysp
	ldx     #$00
	lda     #$21
	jsr     _reallocMsg
	ldy     #$0D
	jsr     ldaxysp
	jsr     pushw
	ldy     #$05
	jsr     ldeaxysp
	jsr     __ftostr
	ldy     #$0F
	jsr     pushwysp
	ldy     #$0F
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _strlen
	jsr     incax1
	jsr     _reallocMsg
;
; else {
;
	jmp     L0735
;
; *value = strdup("Infinity");
;
L071D:	ldy     #$0F
	jsr     pushwysp
	lda     #<(l0738)
	ldx     #>(l0738)
	jsr     _strdup
	ldy     #$00
	jsr     staxspidx
;
; }
;
L0735:	ldy     #$0E
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ strCompare (__near__ __near__ unsigned char * *, __near__ __near__ unsigned char * *, int, __near__ __near__ function returning void *, __near__ __near__ function returning void *)
; ---------------------------------------------------------------


.endproc
