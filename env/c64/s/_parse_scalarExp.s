.include "../code2.s"
.export _parse_scalarExp
.proc	_parse_scalarExp: near


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
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L15A3
	cmp     #$01
	beq     L067E
;
; return NULL;
;
L15A3:	ldx     #$00
	txa
	jmp     L067C
;
; reallocMsg((void**)(&expressionPtr), sizeof(struct expression));
;
L067E:	lda     sp
	ldx     sp+1
	jsr     pushax
	ldx     #$00
	lda     #$12
	jsr     _reallocMsg
;
; expressionPtr->type = operator;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
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
; if(rightPtr == NULL) {
;
	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L159D
;
; expressionPtr->unionPtrs.leaves.leftPtr = leftPtr;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$0E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr->unionPtrs.leaves.rightPtr = NULL;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$10
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; expressionPtr->minColumn = leftPtr->minColumn;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->minTable = leftPtr->minTable;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	ldy     #$02
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->containsAggregates = leftPtr->containsAggregates;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	ldy     #$06
	sta     (sreg),y
	iny
	txa
;
; else if(rightPtr->minTable < leftPtr->minTable ||
;
	jmp     L15A8
L159D:	jsr     ldaxysp
	ldy     #$03
	jsr     pushwidx
	ldy     #$09
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     tosicmp
	bmi     L15A1
;
; (rightPtr->minTable == leftPtr->minTable &&
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     sreg+1
	jne     L0698
	cmp     sreg
	jne     L0698
;
; rightPtr->minColumn < leftPtr->minColumn)
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$09
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     tosicmp
	jpl     L0698
;
; operator == EXP_PLUS ||
;
L15A1:	ldy     #$05
	lda     (sp),y
	bne     L15AA
	dey
	lda     (sp),y
	cmp     #$04
	beq     L15A2
;
; operator == EXP_MULTIPLY ||
;
	iny
L15AA:	lda     (sp),y
	bne     L15AB
	dey
	lda     (sp),y
	cmp     #$06
	beq     L15A2
;
; operator == EXP_AND ||
;
	iny
L15AB:	lda     (sp),y
	bne     L15AC
	dey
	lda     (sp),y
	cmp     #$0B
	beq     L15A2
;
; operator == EXP_OR
;
	iny
L15AC:	lda     (sp),y
;
; ) {
;
	bne     L069E
;
; operator == EXP_OR
;
	dey
	lda     (sp),y
;
; ) {
;
	cmp     #$0C
	bne     L069E
;
; expressionPtr->unionPtrs.leaves.leftPtr = rightPtr;
;
L15A2:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$0E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr->unionPtrs.leaves.rightPtr = leftPtr;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$07
;
; else {
;
	jmp     L15B4
;
; expressionPtr->unionPtrs.leaves.leftPtr = leftPtr;
;
L069E:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$0E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr->unionPtrs.leaves.rightPtr = rightPtr;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
L15B4:	jsr     ldaxysp
	ldy     #$10
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr->minColumn = leftPtr->minColumn;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->minTable = leftPtr->minTable;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
;
; else {
;
	jmp     L15D2
;
; expressionPtr->unionPtrs.leaves.leftPtr = leftPtr;
;
L0698:	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$0E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr->unionPtrs.leaves.rightPtr = rightPtr;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$10
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; expressionPtr->minColumn = rightPtr->minColumn;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->minTable = rightPtr->minTable;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$03
L15D2:	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	ldy     #$02
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->containsAggregates = (leftPtr->containsAggregates) || (rightPtr->containsAggregates);
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$06
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	bne     L06C1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$06
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L06C4
L06C1:	lda     #$01
L06C4:	dey
	sta     (sreg),y
	iny
	lda     #$00
L15A8:	sta     (sreg),y
;
; return expressionPtr;
;
	jsr     ldax0sp
;
; }
;
L067C:	ldy     #$0A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ cleanup_columnReferences (__near__ struct columnReferenceHash *)
; ---------------------------------------------------------------


.endproc
