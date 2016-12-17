.include "../code2.s"
.export _strAppend
.proc	_strAppend: near


;
; int strAppend(char c, char **value, size_t *strSize) {
;
	jsr     pushax
;
; if(strSize != NULL) {
;
	jsr     decsp2
	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L13BB
;
; if(value != NULL) {
;
	iny
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L13CF
;
; if((temp = realloc(*value, (*strSize)+1)) != NULL) {
;
	jsr     ldaxysp
	jsr     pushw
	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	jsr     incax1
	jsr     _realloc
	jsr     stax0sp
	cpx     #$00
	bne     L1773
	cmp     #$00
	beq     L13BA
;
; *value = temp;
;
L1773:	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; (*value)[*strSize] = c;
;
	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	sta     sreg
	stx     sreg+1
	ldy     #$03
	jsr     ldaxysp
	jsr     ldaxi
	clc
	adc     sreg
	sta     ptr1
	txa
	adc     sreg+1
	sta     ptr1+1
	ldy     #$06
	lda     (sp),y
	ldy     #$00
	sta     (ptr1),y
;
; (*strSize)++;
;
L13CF:	ldy     #$03
	jsr     ldaxysp
	jsr     pushax
	jsr     ldaxi
	jsr     incax1
	ldy     #$00
	jsr     staxspidx
;
; return TRUE;
;
L13BB:	ldx     #$00
	lda     #$01
;
; }
;
L13BA:	jsr     incsp7
	jmp     farret


; ---------------------------------------------------------------
; int __near__ d_sprintf (__near__ __near__ unsigned char * *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
