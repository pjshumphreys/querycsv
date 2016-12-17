.include "../code2.s"
.export _cleanup_expression
.proc	_cleanup_expression: near


;
; void cleanup_expression(struct expression *currentExpression) {
;
	jsr     pushax
;
; if(currentExpression != NULL) {
;
	ldy     #$00
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L09FF
;
; switch(currentExpression->type) {
;
	jsr     ldax0sp
	jsr     ldaxi
;
; }
;
	cpx     #$00
	bne     L0A1D
	cmp     #$01
	beq     L0A04
	cmp     #$02
	beq     L0A12
	cmp     #$03
	beq     L0A04
	cmp     #$09
	beq     L0A18
	cmp     #$0A
	beq     L0A18
	cmp     #$14
	beq     L0A06
	cmp     #$15
	beq     L0A06
	cmp     #$16
	beq     L0A04
	jmp     L0A1D
;
; cleanup_expression(currentExpression->unionPtrs.inLeaves.leftPtr);
;
L0A06:	jsr     ldax0sp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _cleanup_expression
;
; cleanup_atomList(currentExpression->unionPtrs.inLeaves.lastEntryPtr);
;
	jsr     ldax0sp
	ldy     #$11
	jsr     ldaxidx
	jsr     _cleanup_atomList
;
; break;
;
	jmp     L0A04
;
; free(currentExpression->unionPtrs.voidPtr);
;
L0A12:	jsr     ldax0sp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _free
;
; break;
;
	jmp     L0A04
;
; cleanup_expression(currentExpression->unionPtrs.leaves.leftPtr);
;
L0A18:	jsr     ldax0sp
	ldy     #$0F
;
; break;
;
	jmp     L1600
;
; cleanup_expression(currentExpression->unionPtrs.leaves.leftPtr);
;
L0A1D:	jsr     ldax0sp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _cleanup_expression
;
; cleanup_expression(currentExpression->unionPtrs.leaves.rightPtr);
;
	jsr     ldax0sp
	ldy     #$11
L1600:	jsr     ldaxidx
	jsr     _cleanup_expression
;
; free(currentExpression);
;
L0A04:	jsr     ldax0sp
	jsr     _free
;
; }
;
L09FF:	jsr     incsp2
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct atomEntry * __near__ parse_atomCommaList (__near__ struct qryData *, __near__ struct atomEntry *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
