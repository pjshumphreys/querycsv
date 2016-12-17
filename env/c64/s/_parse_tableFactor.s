.include "../code2.s"
.export _parse_tableFactor
.proc	_parse_tableFactor: near


;
; ) {
;
	jsr     pushax
;
; struct inputTable *newTable = NULL;
;
	jsr     decsp2
	jsr     push0
;
; struct columnReference *currentReference = NULL;
;
	jsr     push0
;
; char *columnText = NULL;
;
	jsr     decsp4
	jsr     push0
;
; char *columnText2 = NULL;
;
	jsr     push0
;
; size_t columnLength = 0;
;
	jsr     push0
;
; int recordContinues = TRUE;
;
	lda     #$01
	jsr     pusha0
;
; long headerByteLength = 0;
;
	jsr     pushl0
;
; if(queryData->parseMode != 0) {
;
	ldy     #$1D
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L14D1
	cmp     #$00
	beq     L01DE
;
; free(fileName);
;
L14D1:	ldy     #$19
	jsr     ldaxysp
	jsr     _free
;
; free(tableName);
;
	ldy     #$17
	jsr     ldaxysp
	jsr     _free
;
; return;
;
	jmp     L01D6
;
; csvFile = fopen(fileName, "rb");
;
L01DE:	ldy     #$1B
	jsr     pushwysp
	lda     #<(l01e8)
	ldx     #>(l01e8)
	jsr     _fopen
	ldy     #$14
	jsr     staxysp
;
; columnText = strdup(tableName);
;
	ldy     #$17
	jsr     ldaxysp
	jsr     _strdup
	ldy     #$0A
	jsr     staxysp
;
; columnText2 = strdup(fileName);
;
	ldy     #$19
	jsr     ldaxysp
	jsr     _strdup
	ldy     #$08
	jsr     staxysp
;
; if(csvFile == NULL || columnText == NULL || columnText2 == NULL) {
;
	ldy     #$14
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L01F1
	ldy     #$0A
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L01F1
	ldy     #$08
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L01F0
;
; fputs(TDB_COULDNT_OPEN_INPUT, stderr);
;
L01F1:	lda     #<(l01f7)
	ldx     #>(l01f7)
	jsr     pushax
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
; free(fileName);
;
L01F0:	ldy     #$19
	jsr     ldaxysp
	jsr     _free
;
; free(tableName);
;
	ldy     #$17
	jsr     ldaxysp
	jsr     _free
;
; fclose(csvFile);
;
	ldy     #$15
	jsr     ldaxysp
	jsr     _fclose
;
; csvFile = skipBom(columnText2, &headerByteLength);
;
	ldy     #$0B
	jsr     pushwysp
	lda     #$02
	jsr     leaa0sp
	jsr     _skipBom
	ldy     #$14
	jsr     staxysp
;
; tableName = strdup(columnText);
;
	ldy     #$0B
	jsr     ldaxysp
	jsr     _strdup
	ldy     #$16
	jsr     staxysp
;
; fileName = strdup(columnText2);
;
	ldy     #$09
	jsr     ldaxysp
	jsr     _strdup
	ldy     #$18
	jsr     staxysp
;
; free(columnText);
;
	ldy     #$0B
	jsr     ldaxysp
	jsr     _free
;
; free(columnText2);  /* free the filename string data as we don't need it any more */
;
	ldy     #$09
	jsr     ldaxysp
	jsr     _free
;
; columnText = NULL;
;
	ldx     #$00
	txa
	ldy     #$0A
	jsr     staxysp
;
; if(csvFile == NULL || tableName == NULL || fileName == NULL) {
;
	ldy     #$14
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L14CF
	iny
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L14CF
	iny
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0212
;
; fputs(TDB_COULDNT_OPEN_INPUT, stderr);
;
L14CF:	lda     #<(l0219)
	ldx     #>(l0219)
	jsr     pushax
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
; reallocMsg((void**)(&newTable), sizeof(struct inputTable));
;
L0212:	lda     #$12
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$14
	jsr     _reallocMsg
;
; newTable->queryTableName = tableName;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$17
	jsr     ldaxysp
	ldy     #$0C
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newTable->fileStream = csvFile;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$15
	jsr     ldaxysp
	ldy     #$0E
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newTable->firstInputColumn = NULL;  /* the table initially has no columns */
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$12
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; newTable->isLeftJoined = FALSE;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$04
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; newTable->noLeftRecord = TRUE;   /* set just for initialsation purposes */
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$06
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; if(queryData->firstInputTable == NULL) {
;
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$19
	jsr     ldaxidx
	cpx     #$00
	bne     L022C
	cmp     #$00
	bne     L022C
;
; newTable->fileIndex = 1;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; newTable->nextInputTable = newTable;
;
	ldy     #$13
	jsr     ldaxysp
;
; else {
;
	jmp     L150B
;
; newTable->fileIndex = 0; /*  we have to fill the file indexes in afterwards because of left join reordering (queryData->firstInputTable->fileIndex)+1; */
;
L022C:	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; if(isLeftJoin) {
;
	ldy     #$1B
	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L0238
;
; newTable->isLeftJoined = TRUE;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$04
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; if(queryData->secondaryInputTable == NULL) {
;
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$1B
	jsr     ldaxidx
	cpx     #$00
	bne     L023C
	cmp     #$00
	bne     L023C
;
; queryData->secondaryInputTable = newTable;
;
	ldy     #$1D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$1A
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newTable->nextInputTable = queryData->firstInputTable->nextInputTable;
;
	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$19
	jsr     ldaxidx
	ldy     #$11
	jsr     ldaxidx
	ldy     #$10
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; queryData->firstInputTable->nextInputTable = newTable;
;
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$19
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$10
;
; else {
;
	jmp     L150F
;
; newTable->nextInputTable = queryData->secondaryInputTable->nextInputTable;
;
L023C:	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$1B
	jsr     ldaxidx
	ldy     #$11
	jsr     ldaxidx
	ldy     #$10
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; queryData->secondaryInputTable->nextInputTable = newTable;
;
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$1B
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$10
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; queryData->secondaryInputTable = newTable;
;
	ldy     #$1D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$1A
;
; else {
;
	jmp     L150F
;
; newTable->nextInputTable = queryData->firstInputTable->nextInputTable;  /* this is a circularly linked list until we've finished adding records */
;
L0238:	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$19
	jsr     ldaxidx
	ldy     #$11
	jsr     ldaxidx
	ldy     #$10
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; queryData->firstInputTable->nextInputTable = newTable;
;
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$19
	jsr     ldaxidx
L150B:	sta     ptr1
	stx     ptr1+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$10
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; queryData->firstInputTable = newTable;
;
	ldy     #$1D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$18
L150F:	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newReference = NULL;
;
	ldx     #$00
L14D0:	txa
	ldy     #$0E
	jsr     staxysp
;
; newColumn = NULL;
;
	ldy     #$0C
	jsr     staxysp
;
; &(newTable->fileStream),
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0E
	jsr     incaxy
	jsr     pushax
;
; &columnText,
;
	lda     #$0C
	jsr     leaa0sp
	jsr     pushax
;
; &columnLength,
;
	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
;
; NULL,
;
	jsr     push0
;
; &headerByteLength,
;
	lda     #$08
	jsr     leaa0sp
	jsr     pushax
;
; (queryData->params & PRM_TRIM
;
	ldy     #$27
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
;
; ) == 0);
;
	and     #$01
	jsr     booleq
	jsr     _getCsvColumn
	ldy     #$04
	jsr     staxysp
;
; d_sprintf(&columnText, "_%s", columnText);
;
	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
	lda     #<(l0267)
	ldx     #>(l0267)
	jsr     pushax
	ldy     #$11
	jsr     pushwysp
	ldy     #$06
	jsr     _d_sprintf
;
; currentReference = hash_lookupString(queryData->columnReferenceHashTable, columnText);
;
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$17
	jsr     pushwidx
	ldy     #$0D
	jsr     ldaxysp
	jsr     _hash_lookupString
	ldy     #$10
	jsr     staxysp
;
; if(currentReference == NULL) {
;
	cpx     #$00
	bne     L026E
	cmp     #$00
	bne     L026E
;
; reallocMsg((void**)(&newReference), sizeof(struct columnReference));
;
	lda     #$0E
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$0A
	jsr     _reallocMsg
;
; newReference->referenceName = columnText;
;
	ldy     #$0F
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
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$08
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; hash_addString(queryData->columnReferenceHashTable, columnText, newReference);  
;
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$17
	jsr     pushwidx
	ldy     #$0F
	jsr     pushwysp
	ldy     #$13
	jsr     ldaxysp
	jsr     _hash_addString
;
; else {
;
	jmp     L028D
;
; free(columnText);
;
L026E:	ldy     #$0B
	jsr     ldaxysp
	jsr     _free
;
; reallocMsg((void**)(&newReference), sizeof(struct columnReference));
;
	lda     #$0E
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$0A
	jsr     _reallocMsg
;
; newReference->referenceName = currentReference->referenceName;
;
	ldy     #$0F
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$11
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$00
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; if(currentReference->nextReferenceWithName == NULL) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	cpx     #$00
	bne     L0286
	cmp     #$00
	bne     L0286
;
; newReference->nextReferenceWithName = newReference;
;
	ldy     #$0F
	jsr     ldaxysp
;
; else {
;
	jmp     L150C
;
; newReference->nextReferenceWithName = currentReference->nextReferenceWithName->nextReferenceWithName;
;
L0286:	ldy     #$0F
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$11
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	ldy     #$09
	jsr     ldaxidx
	ldy     #$08
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; currentReference->nextReferenceWithName->nextReferenceWithName = newReference;
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
L150C:	sta     ptr1
	stx     ptr1+1
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$08
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; currentReference->nextReferenceWithName = newReference;
;
	ldy     #$11
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$08
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newReference->referenceType = REF_COLUMN;
;
L028D:	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$02
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; reallocMsg((void**)(&newColumn), sizeof(struct inputColumn));
;
	lda     #$0C
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$0A
	jsr     _reallocMsg
;
; newReference->reference.columnPtr = newColumn;
;
	ldy     #$0F
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
; newColumn->fileColumnName = newReference->referenceName;
;
	ldy     #$0D
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$0F
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$02
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; newColumn->inputTablePtr = (void*)newTable;
;
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$04
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newColumn->firstResultColumn = NULL;
;
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$06
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; if(newTable->firstInputColumn == NULL) {
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$13
	jsr     ldaxidx
	cpx     #$00
	bne     L02A2
	cmp     #$00
	bne     L02A2
;
; newColumn->columnIndex = 1;
;
	ldy     #$0D
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; newColumn->nextColumnInTable = newColumn;
;
	ldy     #$0D
	jsr     ldaxysp
;
; else {
;
	jmp     L150D
;
; newColumn->columnIndex = (newTable->firstInputColumn->columnIndex)+1;
;
L02A2:	ldy     #$0D
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$13
	jsr     ldaxidx
	jsr     ldaxi
	jsr     incax1
	ldy     #$00
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; newColumn->nextColumnInTable = newTable->firstInputColumn->nextColumnInTable;  /* this is a circularly linked list until we've finished adding records */
;
	ldy     #$0D
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$13
	jsr     ldaxidx
	ldy     #$09
	jsr     ldaxidx
	ldy     #$08
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; newTable->firstInputColumn->nextColumnInTable = newColumn;
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$13
	jsr     ldaxidx
L150D:	sta     ptr1
	stx     ptr1+1
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$08
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newTable->firstInputColumn = newColumn;
;
	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$12
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; columnText = NULL;
;
	ldx     #$00
	txa
	ldy     #$0A
	jsr     staxysp
;
; columnLength = 0;
;
	ldy     #$06
	jsr     staxysp
;
; } while(recordContinues);
;
	ldy     #$05
	lda     (sp),y
	dey
	ora     (sp),y
	jne     L14D0
;
; newTable->firstRecordOffset = headerByteLength;
;
	ldy     #$15
	jsr     pushwysp
	ldy     #$05
	jsr     ldeaxysp
	ldy     #$08
	jsr     steaxspidx
;
; newTable->columnCount = newTable->firstInputColumn->columnIndex;
;
	ldy     #$13
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$13
	jsr     ldaxysp
	ldy     #$13
	jsr     ldaxidx
	jsr     ldaxi
	ldy     #$02
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; newColumn = newTable->firstInputColumn->nextColumnInTable;
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$13
	jsr     ldaxidx
	ldy     #$09
	jsr     ldaxidx
	ldy     #$0C
	jsr     staxysp
;
; newTable->firstInputColumn->nextColumnInTable = NULL;
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$13
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$08
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; newTable->firstInputColumn = newColumn;
;
	ldy     #$13
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$12
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; }
;
L01D6:	ldy     #$1E
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ readParams (__near__ unsigned char *, __near__ int *)
; ---------------------------------------------------------------


.endproc
