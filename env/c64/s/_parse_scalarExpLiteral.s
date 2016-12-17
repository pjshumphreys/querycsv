.include "../code2.s"
.export _parse_scalarExpLiteral
.proc	_parse_scalarExpLiteral: near


;
; ) {
;
	jsr     pushax
;
; struct expression *expressionPtr = NULL;
;
	jsr     push0
;
; if(queryData->parseMode != 1) {
;
	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L1679
	cmp     #$01
	beq     L0EDC
;
; return NULL;
;
L1679:	ldx     #$00
	txa
	jmp     L0EDA
;
; reallocMsg((void**)(&expressionPtr), sizeof(struct expression));
;
L0EDC:	lda     sp
	ldx     sp+1
	jsr     pushax
	ldx     #$00
	lda     #$12
	jsr     _reallocMsg
;
; expressionPtr->type = EXP_LITERAL;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$02
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; expressionPtr->value = NULL;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; expressionPtr->unionPtrs.voidPtr = strdup(literal);
;
	jsr     pushw0sp
	ldy     #$05
	jsr     ldaxysp
	jsr     _strdup
	ldy     #$0E
	jsr     staxspidx
;
; expressionPtr->minColumn = 0;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$04
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; expressionPtr->minTable = 0;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$02
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; expressionPtr->containsAggregates = FALSE;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$06
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; return expressionPtr;
;
	jsr     ldax0sp
;
; }
;
L0EDA:	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; int __near__ strAppendUTF8 (long, __near__ __near__ unsigned char * *, int)
; ---------------------------------------------------------------


.endproc
