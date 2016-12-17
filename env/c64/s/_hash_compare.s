.include "../code2.s"
.export _hash_compare
.proc	_hash_compare: near


;
; ) {
;
	jsr     pushax
;
; hashval = 0;
;
	jsr     decsp2
	ldx     #$00
	txa
	tay
L1648:	jsr     staxysp
;
; for(; *str != '\0'; str++) {
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	beq     L0CA3
;
; hashval = *str + (hashval << 5) - hashval;
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	sta     ptr1
	jsr     ldax0sp
	jsr     shlax4
	jsr     shlax1
	clc
	adc     ptr1
	bcc     L1647
	inx
L1647:	sec
	ldy     #$00
	sbc     (sp),y
	pha
	txa
	iny
	sbc     (sp),y
	tax
	pla
	jsr     stax0sp
;
; for(; *str != '\0'; str++) {
;
	ldy     #$03
	jsr     ldaxysp
	jsr     incax1
	ldy     #$02
	jmp     L1648
;
; return hashval % hashtable->size;
;
L0CA3:	jsr     pushw0sp
	ldy     #$07
	jsr     ldaxysp
	jsr     ldaxi
	jsr     tosumodax
;
; }
;
	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct columnReference * __near__ hash_lookupString (__near__ struct columnReferenceHash *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
