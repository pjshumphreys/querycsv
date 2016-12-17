.include "../code2.s"
.export _getGroupedColumns
.proc	_getGroupedColumns: near


;
; void getGroupedColumns(struct qryData *query) {
;
	jsr     pushax
;
; for(i = 0; i < query->columnReferenceHashTable->size; i++) {
;
	jsr     decsp8
	ldx     #$00
	txa
L1787:	jsr     stax0sp
	jsr     pushax
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$17
	jsr     ldaxidx
	jsr     ldaxi
	jsr     tosicmp
	jpl     L1423
;
; currentHashEntry = query->columnReferenceHashTable->table[i];
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$17
	jsr     ldaxidx
	ldy     #$03
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
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
	jmp     L1784
;
; currentReference = currentHashEntry->content;
;
L177D:	jsr     ldaxysp
	ldy     #$03
;
; while(currentReference != NULL) {
;
	jmp     L1785
;
; currentReference->referenceType == REF_EXPRESSION &&
;
L177E:	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	jne     L1444
	cmp     #$02
	jne     L1444
;
; (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	ldy     #$02
	jsr     staxysp
	cpx     #$00
	bne     L177F
	cmp     #$00
	jeq     L1444
;
; currentResultColumn->groupType != GRP_NONE
;
L177F:	ldy     #$03
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
;
; ) {
;
	cpx     #$00
	bne     L143A
	cmp     #$00
	jeq     L1444
;
; switch(currentResultColumn->groupType) {
;
L143A:	ldy     #$03
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
;
; }
;
	cpx     #$00
	jne     L1444
	cmp     #$01
	beq     L1446
	cmp     #$04
	beq     L1450
	cmp     #$05
	jeq     L1464
	cmp     #$07
	jeq     L146E
	cmp     #$08
	beq     L1446
	cmp     #$0B
	beq     L1450
	cmp     #$0C
	beq     L1464
	jmp     L1444
;
; currentResultColumn->groupCount);
;
L1446:	ldy     #$05
	jsr     pushwysp
	ldy     #$05
	jsr     ldaxysp
	ldy     #$0D
	jsr     ldeaxidx
	jsr     pusheax
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     axlong
	jsr     __fdiv
	ldy     #$0A
	jsr     steaxspidx
;
; ftostr(&(currentResultColumn->groupText),currentResultColumn->groupNum);
;
L1450:	ldy     #$03
	jsr     ldaxysp
	jsr     incax8
	jsr     pushax
	ldx     #$00
	lda     #$21
	jsr     _reallocMsg
	ldy     #$03
	jsr     ldaxysp
	jsr     incax8
	jsr     pushw
	ldy     #$05
	jsr     ldaxysp
	ldy     #$0D
	jsr     ldeaxidx
	jsr     __ftostr
	ldy     #$03
	jsr     ldaxysp
	jsr     incax8
	jsr     pushax
	ldy     #$05
	jsr     ldaxysp
	jsr     incax8
	jsr     ldaxi
	jsr     _strlen
	jsr     incax1
	jsr     _reallocMsg
;
; break;
;
	jmp     L1444
;
; d_sprintf(&(currentResultColumn->groupText), "%d", currentResultColumn->groupCount);
;
L1464:	ldy     #$03
	jsr     ldaxysp
	jsr     incax8
	jsr     pushax
	lda     #<(l146a)
	ldx     #>(l146a)
	jsr     pushax
	ldy     #$07
	jsr     ldaxysp
	ldy     #$0F
;
; break;
;
	jmp     L1788
;
; d_sprintf(&(currentResultColumn->groupText), "%d", query->groupCount);
;
L146E:	ldy     #$03
	jsr     ldaxysp
	jsr     incax8
	jsr     pushax
	lda     #<(l1472)
	ldx     #>(l1472)
	jsr     pushax
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$0B
L1788:	jsr     pushwidx
	ldy     #$06
	jsr     _d_sprintf
;
; currentReference = currentReference->nextReferenceWithName;
;
L1444:	ldy     #$05
	jsr     ldaxysp
	ldy     #$09
L1785:	jsr     ldaxidx
	ldy     #$04
	jsr     staxysp
;
; while(currentReference != NULL) {
;
	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L177E
;
; currentHashEntry = currentHashEntry->nextReferenceInHash;
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$05
L1784:	jsr     ldaxidx
	ldy     #$06
	jsr     staxysp
;
; while(currentHashEntry != NULL) {
;
	ldy     #$06
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L177D
;
; for(i = 0; i < query->columnReferenceHashTable->size; i++) {
;
	jsr     ldax0sp
	sta     regsave
	stx     regsave+1
	jsr     incax1
	jmp     L1787
;
; }
;
L1423:	ldy     #$0A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ yyerror2 (long, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
