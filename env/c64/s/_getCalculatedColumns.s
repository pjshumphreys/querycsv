.include "../code2.s"
.export _getCalculatedColumns
.proc	_getCalculatedColumns: near


;
; ) {
;
	jsr     pushax
;
; matchParams.ptr = match;
;
	ldy     #$0E
	jsr     subysp
	ldy     #$11
	jsr     ldaxysp
	ldy     #$02
	jsr     staxysp
;
; matchParams.params = query->params;
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     stax0sp
;
; for(i = 0; i < query->columnReferenceHashTable->size; i++) {
;
	ldx     #$00
	txa
L1552:	ldy     #$0C
	jsr     staxysp
	jsr     pushax
	ldy     #$15
	jsr     ldaxysp
	ldy     #$17
	jsr     ldaxidx
	jsr     ldaxi
	jsr     tosicmp
	jpl     L0478
;
; currentHashEntry = query->columnReferenceHashTable->table[i];
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$17
	jsr     ldaxidx
	ldy     #$03
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	ldy     #$0D
	jsr     ldaxysp
	jsr     aslax1
	clc
	adc     ptr1
	pha
	txa
	adc     ptr1+1
	tax
	pla
	ldy     #$01
;
; while(currentHashEntry != NULL) {
;
	jmp     L1550
;
; currentReference = currentHashEntry->content;
;
L154B:	jsr     ldaxysp
	ldy     #$03
;
; while(currentReference != NULL) {
;
	jmp     L1551
;
; currentReference->referenceType == REF_EXPRESSION &&
;
L154C:	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	jne     L048E
	cmp     #$02
	jne     L048E
;
; currentReference->reference.calculatedPtr.expressionPtr->containsAggregates == runAggregates &&
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$07
	jsr     ldaxidx
	ldy     #$0E
	cmp     (sp),y
	jne     L048E
	txa
	iny
	cmp     (sp),y
	jne     L048E
;
; (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	ldy     #$06
	jsr     staxysp
;
; ) {
;
	cpx     #$00
	bne     L048F
	cmp     #$00
	jeq     L048E
;
; j = currentResultColumn->resultColumnIndex;
;
L048F:	ldy     #$07
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$0A
	jsr     staxysp
;
; match[j].isQuoted = FALSE;
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$04
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; match[j].isNormalized = FALSE;
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$06
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; match[j].startOffset = 0;   /* ftell(query->scratchpad); */
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	jsr     pushax
	ldx     #$00
	stx     sreg
	stx     sreg+1
	txa
	tay
	jsr     steaxspidx
;
; getValue(currentReference->reference.calculatedPtr.expressionPtr, &matchParams);
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	lda     #$02
	jsr     leaa0sp
	jsr     _getValue
;
; if(currentReference->reference.calculatedPtr.expressionPtr->leftNull) {
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$08
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L04A4
;
; match[j].length = 0;
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0A
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; match[j].leftNull = TRUE;
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$08
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; match[j].value = strdup("");
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	jsr     pushax
	lda     #<(l04af)
	ldx     #>(l04af)
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; else {
;
	jmp     L04B1
;
; match[j].leftNull = FALSE;
;
L04A4:	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$08
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; match[j].length = strlen(currentReference->reference.calculatedPtr.expressionPtr->value);
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	jsr     pushax
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _strlen
	ldy     #$0A
	jsr     staxspidx
;
; match[j].value = currentReference->reference.calculatedPtr.expressionPtr->value;
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	sta     sreg
	stx     sreg+1
	ldy     #$09
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	ldy     #$0C
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; currentReference->reference.calculatedPtr.expressionPtr->value = NULL;
;
L04B1:	ldy     #$09
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; currentReference = currentReference->nextReferenceWithName;
;
L048E:	ldy     #$09
	jsr     ldaxysp
	ldy     #$09
L1551:	jsr     ldaxidx
	ldy     #$08
	jsr     staxysp
;
; while(currentReference != NULL) {
;
	ldy     #$08
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L154C
;
; currentHashEntry = currentHashEntry->nextReferenceInHash;
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$05
L1550:	jsr     ldaxidx
	ldy     #$04
	jsr     staxysp
;
; while(currentHashEntry != NULL) {
;
	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L154B
;
; for(i = 0; i < query->columnReferenceHashTable->size; i++) {
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     incax1
	jmp     L1552
;
; }
;
L0478:	ldy     #$14
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ __cdecl__ main (int, __near__ unsigned char * *)
; ---------------------------------------------------------------


.endproc
