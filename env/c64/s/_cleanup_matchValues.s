.include "../code2.s"
.export _cleanup_matchValues
.proc	_cleanup_matchValues: near


;
; ) {
;
	jsr     pushax
;
; for(i = 0, len = query->columnCount; i < len; i++) {
;
	jsr     decsp4
	ldx     #$00
	txa
	ldy     #$02
	jsr     staxysp
	ldy     #$07
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$00
L178B:	jsr     staxysp
	ldy     #$03
	jsr     ldaxysp
	ldy     #$00
	cmp     (sp),y
	txa
	iny
	sbc     (sp),y
	bvc     L178A
	eor     #$80
L178A:	bpl     L1490
;
; free((*match)[i].value);
;
	ldy     #$05
	jsr     ldaxysp
	jsr     pushw
	ldy     #$07
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
;
; for(i = 0, len = query->columnCount; i < len; i++) {
;
	ldy     #$03
	jsr     ldaxysp
	jsr     incax1
	ldy     #$02
	jmp     L178B
;
; free(*match);
;
L1490:	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _free
;
; *match = NULL;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; }
;
	jsr     incsp8
	jmp     farret


.endproc
