.include "../code2.s"
.export _parse_expCommaList
.proc	_parse_expCommaList: near


;
; ) {
;
	jsr     pushax
;
; struct columnReference *currentReference = NULL;
;
	jsr     push0
;
; struct columnReference *newReference = NULL;
;
	jsr     push0
;
; struct columnReference *newReference2 = NULL;
;
	jsr     push0
;
; struct resultColumn *newResultColumn = NULL;
;
	jsr     push0
;
; if(queryData->parseMode != 1) {
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L162C
	cmp     #$01
	beq     L0B31
;
; return NULL;
;
L162C:	ldx     #$00
	txa
	jmp     L0BDB
;
; if(aggregationType == GRP_NONE) {
;
L0B31:	ldy     #$08
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L0B35
;
; queryData->columnCount++;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     pushax
	ldy     #$05
	jsr     ldaxidx
	jsr     incax1
	ldy     #$04
	jsr     staxspidx
;
; if(resultColumnName == NULL) {
;
	ldy     #$0A
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L0B5F
;
; if(expressionPtr->type == EXP_COLUMN) {
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L0B3C
	cmp     #$01
	bne     L0B3C
;
; if((resultColumnName = strdup(((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->fileColumnName)) == NULL) {
;
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$03
	jsr     ldaxidx
	jsr     _strdup
	ldy     #$0A
	jsr     staxysp
	cpx     #$00
	jne     L0B5F
	cmp     #$00
	jne     L0B5F
;
; fputs(TDB_MALLOC_FAILED, stderr);
;
	lda     #<(l0b48)
	ldx     #>(l0b48)
;
; else if(d_sprintf(&resultColumnName, TDB_UNTITLED_COLUMN) == FALSE) {
;
	jmp     L1639
L0B3C:	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
	lda     #<(l0b51)
	ldx     #>(l0b51)
	jsr     pushax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$05
	jsr     pushwidx
	ldy     #$06
	jsr     _d_sprintf
	cpx     #$00
	bne     L0B5F
	cmp     #$00
	bne     L0B5F
;
; fputs(TDB_MALLOC_FAILED, stderr);
;
	lda     #<(l0b56)
	ldx     #>(l0b56)
;
; else {
;
	jmp     L1639
;
; free(resultColumnName);
;
L0B35:	ldy     #$0B
	jsr     ldaxysp
	jsr     _free
;
; queryData->hiddenColumnCount++;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     pushax
	ldy     #$07
	jsr     ldaxidx
	jsr     incax1
	ldy     #$06
	jsr     staxspidx
;
; if(d_sprintf(&resultColumnName, "%d", queryData->hiddenColumnCount) == FALSE) {
;
	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
	lda     #<(l0b62)
	ldx     #>(l0b62)
	jsr     pushax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$07
	jsr     pushwidx
	ldy     #$06
	jsr     _d_sprintf
	cpx     #$00
	bne     L0B5F
	cmp     #$00
	bne     L0B5F
;
; fputs(TDB_MALLOC_FAILED, stderr);
;
	lda     #<(l0b67)
	ldx     #>(l0b67)
L1639:	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; exit(EXIT_FAILURE);
;
	ldx     #$00
	lda     #$01
	jsr     _exit
;
; currentReference = hash_lookupString(queryData->columnReferenceHashTable, resultColumnName);
;
L0B5F:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$17
	jsr     pushwidx
	ldy     #$0D
	jsr     ldaxysp
	jsr     _hash_lookupString
	ldy     #$06
	jsr     staxysp
;
; if(currentReference == NULL) {
;
	cpx     #$00
	bne     L0B70
	cmp     #$00
	bne     L0B70
;
; reallocMsg((void**)(&newReference), sizeof(struct columnReference));
;
	lda     #$04
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$0A
	jsr     _reallocMsg
;
; newReference->referenceName = resultColumnName;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$00
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newReference->nextReferenceWithName = NULL;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$08
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; hash_addString(queryData->columnReferenceHashTable, resultColumnName, newReference);
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$17
	jsr     pushwidx
	ldy     #$0F
	jsr     pushwysp
	ldy     #$09
	jsr     ldaxysp
	jsr     _hash_addString
;
; else {
;
	jmp     L0B9D
;
; free(resultColumnName);
;
L0B70:	ldy     #$0B
	jsr     ldaxysp
	jsr     _free
;
; reallocMsg((void**)(&newReference), sizeof(struct columnReference));
;
	lda     #$04
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$0A
	jsr     _reallocMsg
;
; if(currentReference->referenceType == REF_COLUMN) {
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	jne     L0BA0
	cmp     #$01
	jne     L0BA0
;
; reallocMsg((void**)(&newReference2), sizeof(struct columnReference));
;
	lda     #$02
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$0A
	jsr     _reallocMsg
;
; memcpy(newReference, currentReference, sizeof(struct columnReference));
;
	ldy     #$07
	jsr     pushwysp
	ldy     #$0B
	jsr     pushwysp
	ldx     #$00
	lda     #$0A
	jsr     _memcpy
;
; memcpy(currentReference, newReference2, sizeof(struct columnReference));
;
	ldy     #$09
	jsr     pushwysp
	ldy     #$07
	jsr     pushwysp
	ldx     #$00
	lda     #$0A
	jsr     _memcpy
;
; free(newReference2);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _free
;
; currentReference->nextReferenceWithName = newReference;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$08
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; currentReference->referenceName = newReference->referenceName;
;
	ldy     #$07
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$00
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; newReference = currentReference;
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$04
	jsr     staxysp
;
; else {
;
	jmp     L0B9D
;
; currentReference = currentReference->nextReferenceWithName;
;
L0B9E:	ldy     #$07
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	ldy     #$06
	jsr     staxysp
;
; currentReference->nextReferenceWithName != NULL &&
;
L0BA0:	ldy     #$07
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	cpx     #$00
	bne     L162D
	cmp     #$00
	beq     L1628
;
; currentReference->nextReferenceWithName->referenceType != REF_COLUMN
;
L162D:	ldy     #$07
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	ldy     #$03
	jsr     ldaxidx
;
; ) {
;
	cpx     #$00
	bne     L0B9E
	cmp     #$01
	bne     L0B9E
;
; newReference->nextReferenceWithName = currentReference->nextReferenceWithName;
;
L1628:	ldy     #$05
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	ldy     #$08
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; currentReference->nextReferenceWithName = newReference;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$08
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newReference->referenceName = currentReference->referenceName;
;
	ldy     #$05
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$07
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$00
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; newReference->referenceType = REF_EXPRESSION;
;
L0B9D:	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$02
	tay
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; newReference->reference.calculatedPtr.expressionPtr = expressionPtr;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$04
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; if(expressionPtr->type == EXP_COLUMN && aggregationType == GRP_NONE) {
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	jne     L0BB2
	cmp     #$01
	jne     L0BB2
	ldy     #$08
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L0BB2
;
; newResultColumn = ((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->firstResultColumn;
;
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$07
	jsr     ldaxidx
	jsr     stax0sp
;
; newReference->reference.calculatedPtr.firstResultColumn = NULL;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$06
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; while (newResultColumn != NULL) {
;
	jmp     L162F
;
; if(queryData->firstResultColumn == newResultColumn) {
;
L0BBE:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$1D
	jsr     ldaxidx
	ldy     #$00
	cmp     (sp),y
	bne     L1630
	txa
	iny
	cmp     (sp),y
	bne     L1630
;
; queryData->firstResultColumn->isHidden = FALSE;
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$1D
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$02
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; queryData->firstResultColumn->resultColumnName = strdup(newReference->referenceName);
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$1D
	jsr     pushwidx
	ldy     #$07
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _strdup
	ldy     #$12
	jsr     staxspidx
;
; return queryData->firstResultColumn;
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$1D
	jmp     L162A
;
; if(newResultColumn->nextColumnInstance == NULL) {
;
L1630:	jsr     ldax0sp
	ldy     #$15
	jsr     ldaxidx
	cpx     #$00
	bne     L0BCB
	cmp     #$00
	bne     L0BCB
;
; newResultColumn->nextColumnInstance = parse_newOutputColumn(
;
	jsr     pushw0sp
;
; queryData,
;
	ldy     #$13
	jsr     pushwysp
;
; /*isHidden = */FALSE,
;
	jsr     push0
;
; /*isCalculated = */FALSE,
;
	jsr     push0
;
; /*resultColumnName = */strdup(newReference->referenceName),
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _strdup
	jsr     pushax
;
; );
;
	ldx     #$00
	txa
	jsr     _parse_newOutputColumn
	ldy     #$14
	jsr     staxspidx
;
; return newResultColumn->nextColumnInstance;
;
	jsr     ldax0sp
	ldy     #$15
	jmp     L162A
;
; newResultColumn = newResultColumn->nextColumnInstance;
;
L0BCB:	jsr     ldax0sp
	ldy     #$15
	jsr     ldaxidx
	jsr     stax0sp
;
; while (newResultColumn != NULL) {
;
L162F:	ldy     #$00
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L0BBE
;
; return NULL;
;
	tax
	jmp     L0BDB
;
; newReference->reference.calculatedPtr.firstResultColumn = parse_newOutputColumn(
;
L0BB2:	ldy     #$07
	jsr     pushwysp
;
; queryData,
;
	ldy     #$13
	jsr     pushwysp
;
; /*isHidden = */aggregationType != GRP_NONE,
;
	ldy     #$0D
	jsr     ldaxysp
	cpx     #$00
	bne     L0BE0
	cmp     #$00
L0BE0:	jsr     boolne
	jsr     pusha0
;
; /*isCalculated = */TRUE,
;
	lda     #$01
	jsr     pusha0
;
; /*resultColumnName = */strdup(newReference->referenceName),
;
	ldy     #$0D
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _strdup
	jsr     pushax
;
; );
;
	ldy     #$13
	jsr     ldaxysp
	jsr     _parse_newOutputColumn
	ldy     #$06
	jsr     staxspidx
;
; return newReference->reference.calculatedPtr.firstResultColumn;
;
	dey
	jsr     ldaxysp
	ldy     #$07
L162A:	jsr     ldaxidx
;
; }
;
L0BDB:	ldy     #$10
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct expression * __near__ parse_functionRefStar (__near__ struct qryData *, long)
; ---------------------------------------------------------------


.endproc
