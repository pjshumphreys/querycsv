.include "../code2.s"
.export _cleanup_columnReferences
.proc	_cleanup_columnReferences: near


;
; void cleanup_columnReferences(struct columnReferenceHash *table) {
;
	jsr     pushax
;
; for(i=0; i<table->size; i++) {
;
	ldy     #$0A
	jsr     subysp
	ldx     #$00
	txa
L15D8:	ldy     #$08
	jsr     staxysp
	jsr     pushax
	ldy     #$0D
	jsr     ldaxysp
	jsr     ldaxi
	jsr     tosicmp
	jpl     L06C8
;
; currentHashEntry = table->table[i];
;
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	ldy     #$09
	jsr     ldaxysp
	jsr     aslax1
	clc
	adc     ptr1
	pha
	txa
	adc     ptr1+1
	tax
	pla
	jsr     ldaxi
;
; while(currentHashEntry != NULL) {
;
	jmp     L15D6
;
; nextHashEntry = currentHashEntry->nextReferenceInHash;
;
L15D4:	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	jsr     staxysp
;
; free(currentHashEntry->referenceName);
;
	ldy     #$07
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _free
;
; currentReference = currentHashEntry->content;
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	ldy     #$02
	jsr     staxysp
;
; free(currentHashEntry);
;
	ldy     #$07
	jsr     ldaxysp
	jsr     _free
;
; while(currentReference != NULL) {
;
	jmp     L06E1
;
; nextReference = currentReference->nextReferenceWithName;
;
L15D5:	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	jsr     stax0sp
;
; if(currentReference->referenceType == REF_COLUMN) {
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	bne     L06E6
	cmp     #$01
	bne     L06E6
;
; free(currentReference->reference.columnPtr);
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     _free
;
; if(currentReference->referenceType == REF_EXPRESSION) {
;
L06E6:	ldy     #$03
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	bne     L06EB
	cmp     #$02
	bne     L06EB
;
; cleanup_expression(currentReference->reference.calculatedPtr.expressionPtr);
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     _cleanup_expression
;
; free(currentReference);
;
L06EB:	ldy     #$03
	jsr     ldaxysp
	jsr     _free
;
; currentReference = nextReference;
;
	jsr     ldax0sp
	ldy     #$02
	jsr     staxysp
;
; while(currentReference != NULL) {
;
L06E1:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L15D5
;
; currentHashEntry = nextHashEntry;
;
	ldy     #$05
	jsr     ldaxysp
L15D6:	ldy     #$06
	jsr     staxysp
;
; while(currentHashEntry != NULL) {
;
	ldy     #$06
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L15D4
;
; for(i=0; i<table->size; i++) {
;
	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	jmp     L15D8
;
; free(table->table);
;
L06C8:	ldy     #$0B
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     _free
;
; free(table);
;
	ldy     #$0B
	jsr     ldaxysp
	jsr     _free
;
; }
;
	ldy     #$0C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct expression * __near__ parse_inPredicate (__near__ struct qryData *, __near__ struct expression *, int, __near__ struct atomEntry *)
; ---------------------------------------------------------------


.endproc
