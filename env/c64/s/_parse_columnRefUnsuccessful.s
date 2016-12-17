.include "../code2.s"
.export _parse_columnRefUnsuccessful
.proc	_parse_columnRefUnsuccessful: near


;
; ) {
;
	jsr     pushax
;
; struct columnReference *currentReference = NULL;
;
	jsr     push0
;
; if(queryData->parseMode != 1) {
;
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L14C4
	cmp     #$01
	beq     L0174
;
; free(tableName);
;
L14C4:	ldy     #$05
	jsr     ldaxysp
	jsr     _free
;
; free(columnName);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
;
; *result = NULL;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; return FALSE; /* successful, but only because we don't really care about getting column references when in parse mode 0 */
;
	tax
	jmp     L0172
;
; currentReference = hash_lookupString(queryData->columnReferenceHashTable, columnName);
;
L0174:	ldy     #$09
	jsr     ldaxysp
	ldy     #$17
	jsr     pushwidx
	ldy     #$05
	jsr     ldaxysp
	jsr     _hash_lookupString
	jsr     stax0sp
;
; if(tableName != NULL) {
;
	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0182
;
; while (currentReference != NULL) {
;
	jmp     L0187
;
; currentReference->referenceType == 1 &&
;
L14C3:	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	bne     L018A
	cmp     #$01
	bne     L018A
;
; strcmp(tableName, ((struct inputTable*)(currentReference->reference.columnPtr->inputTablePtr))->queryTableName) == 0
;
	ldy     #$07
	jsr     pushwysp
	ldy     #$03
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$05
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _strcmp
;
; ) {
;
	cpx     #$00
	bne     L018A
	cmp     #$00
	beq     L0197
;
; currentReference = currentReference->nextReferenceWithName;
;
L018A:	jsr     ldax0sp
	ldy     #$09
	jsr     ldaxidx
	jsr     stax0sp
;
; while (currentReference != NULL) {
;
L0187:	ldy     #$00
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L14C3
;
; else if(
;
	jmp     L0197
;
; currentReference != NULL &&
;
L0182:	tay
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0197
;
; currentReference->nextReferenceWithName != NULL &&
;
	jsr     ldax0sp
	ldy     #$09
	jsr     ldaxidx
	cpx     #$00
	bne     L14C5
	cmp     #$00
	beq     L0197
;
; currentReference->nextReferenceWithName->referenceType == currentReference->referenceType
;
L14C5:	jsr     ldax0sp
	ldy     #$09
	jsr     ldaxidx
	ldy     #$03
	jsr     ldaxidx
	sta     sreg
	stx     sreg+1
;
; ) {
;
	jsr     ldax0sp
	ldy     #$03
	jsr     ldaxidx
	cpx     sreg+1
	bne     L0197
	cmp     sreg
	bne     L0197
;
; currentReference = NULL;
;
	ldx     #$00
	txa
	jsr     stax0sp
;
; free(tableName);
;
L0197:	ldy     #$05
	jsr     ldaxysp
	jsr     _free
;
; free(columnName);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
;
; currentReference != NULL &&
;
	ldy     #$00
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L01A3
;
; currentReference->referenceType == REF_COLUMN &&
;
	jsr     ldax0sp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	bne     L01A3
	cmp     #$01
	bne     L01A3
;
; currentReference->reference.columnPtr->firstResultColumn == NULL
;
	jsr     ldax0sp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$07
	jsr     ldaxidx
;
; ) {
;
	cpx     #$00
	bne     L01A3
	cmp     #$00
	bne     L01A3
;
; currentReference->reference.columnPtr->firstResultColumn = parse_newOutputColumn(
;
	jsr     ldax0sp
	ldy     #$05
	jsr     pushwidx
;
; queryData,
;
	ldy     #$0D
	jsr     pushwysp
;
; /*isHidden = */TRUE,
;
	lda     #$01
	jsr     pusha0
;
; /*isCalculated = */FALSE,
;
	jsr     push0
;
; /*resultColumnName = */NULL,
;
	jsr     push0
;
; );
;
	jsr     _parse_newOutputColumn
	ldy     #$06
	jsr     staxspidx
;
; *result = currentReference;
;
L01A3:	ldy     #$07
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
; return currentReference == NULL;
;
	jsr     ldax0sp
	cpx     #$00
	bne     L01B4
	cmp     #$00
L01B4:	jsr     booleq
;
; }
;
L0172:	ldy     #$0A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct columnReferenceHash * __near__ hash_createTable (int)
; ---------------------------------------------------------------


.endproc
