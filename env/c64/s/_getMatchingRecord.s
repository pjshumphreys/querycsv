.include "../code2.s"
.export _getMatchingRecord
.proc	_getMatchingRecord: near


;
; {
;
	jsr     pushax
;
; int doLeftRecord = FALSE;
;
	ldy     #$1A
	jsr     subysp
	jsr     push0
;
; long templong = 0;
;
	jsr     pushl0
;
; matchParams.ptr = match;
;
	ldy     #$21
	jsr     ldaxysp
	ldy     #$0A
	jsr     staxysp
;
; matchParams.params = query->params;
;
	ldy     #$23
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$08
	jsr     staxysp
;
; if(query->secondaryInputTable == NULL) {
;
	ldy     #$23
	jsr     ldaxysp
	ldy     #$1B
	jsr     ldaxidx
	cpx     #$00
	bne     L0D5A
	cmp     #$00
	bne     L0D5A
;
; query->secondaryInputTable = query->firstInputTable;
;
	ldy     #$23
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$23
	jsr     ldaxysp
	ldy     #$19
	jsr     ldaxidx
	ldy     #$1A
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; currentInputTable = query->secondaryInputTable;
;
L0D5A:	ldy     #$23
	jsr     ldaxysp
	ldy     #$1B
	jsr     ldaxidx
	ldy     #$1E
;
; while(
;
	jmp     L1656
;
; recordHasColumn = TRUE;
;
L165D:	ldy     #$06
	jsr     staxysp
;
; currentInputColumn = currentInputTable->firstInputColumn;
;
	ldy     #$1F
	jsr     ldaxysp
	ldy     #$13
L166A:	jsr     ldaxidx
	ldy     #$1C
	jsr     staxysp
;
; currentInputColumn != NULL;
;
	ldy     #$1C
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L0D72
;
; if(recordHasColumn == TRUE && !doLeftRecord) {
;
	ldy     #$07
	lda     (sp),y
	bne     L0D7B
	dey
	lda     (sp),y
	cmp     #$01
	bne     L0D7B
	dey
	lda     (sp),y
	dey
	ora     (sp),y
	bne     L0D7B
;
; columnOffsetData.value = NULL;
;
	tax
	ldy     #$18
	jsr     staxysp
;
; templong = columnOffsetData.startOffset;
;
	ldy     #$0F
	jsr     ldeaxysp
	jsr     steax0sp
;
; &(currentInputTable->fileStream),
;
	ldy     #$1F
	jsr     ldaxysp
	ldy     #$0E
	jsr     incaxy
	jsr     pushax
;
; &(columnOffsetData.value),
;
	lda     #$1A
	jsr     leaa0sp
	jsr     pushax
;
; &(columnOffsetData.length),
;
	lda     #$1A
	jsr     leaa0sp
	jsr     pushax
;
; &(columnOffsetData.isQuoted),
;
	lda     #$16
	jsr     leaa0sp
	jsr     pushax
;
; &(templong),
;
	lda     #$08
	jsr     leaa0sp
	jsr     pushax
;
; (query->params & PRM_TRIM) == 0
;
	ldy     #$2D
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	and     #$01
;
; );
;
	jsr     booleq
	jsr     _getCsvColumn
	ldy     #$06
	jsr     staxysp
;
; columnOffsetData.isNormalized = FALSE;
;
	ldx     #$00
	txa
	ldy     #$12
	jsr     staxysp
;
; columnOffsetData.leftNull = FALSE;
;
	ldy     #$14
;
; else {
;
	jmp     L1657
;
; columnOffsetData.leftNull = doLeftRecord;
;
L0D7B:	ldy     #$05
	jsr     ldaxysp
	ldy     #$14
	jsr     staxysp
;
; columnOffsetData.startOffset = 0;
;
	ldx     #$00
	stx     sreg
	stx     sreg+1
	txa
	ldy     #$0C
	jsr     steaxysp
;
; columnOffsetData.length = 0;
;
	ldy     #$16
	jsr     staxysp
;
; columnOffsetData.isQuoted = FALSE;
;
	ldy     #$10
	jsr     staxysp
;
; columnOffsetData.isNormalized = TRUE; /* an empty string needs no unicode normalization */
;
	lda     #$01
	ldy     #$12
	jsr     staxysp
;
; columnOffsetData.value = strdup("");
;
	lda     #<(l0da4)
	ldx     #>(l0da4)
	jsr     _strdup
	ldy     #$18
L1657:	jsr     staxysp
;
; currentResultColumn = currentInputColumn->firstResultColumn;
;
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$07
L1669:	jsr     ldaxidx
	ldy     #$1A
	jsr     staxysp
;
; currentResultColumn != NULL;
;
	ldy     #$1A
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0D73
;
; &(match[currentResultColumn->resultColumnIndex]),
;
	ldy     #$23
	jsr     pushwysp
	ldy     #$1D
	jsr     ldaxysp
	jsr     pushw
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	jsr     pushax
;
; &columnOffsetData,
;
	lda     #$0E
	jsr     leaa0sp
	jsr     pushax
;
; );
;
	ldx     #$00
	lda     #$0E
	jsr     _memcpy
;
; ) {
;
	ldy     #$1B
	jsr     ldaxysp
	ldy     #$15
	jmp     L1669
;
; ) {  /* columns */
;
L0D73:	ldy     #$1D
	jsr     ldaxysp
	ldy     #$09
	jmp     L166A
;
; if(recordHasColumn == TRUE && !doLeftRecord) {
;
L0D72:	ldy     #$07
	lda     (sp),y
	bne     L0DBC
	dey
	lda     (sp),y
	cmp     #$01
	bne     L0DBC
	dey
	lda     (sp),y
	dey
	ora     (sp),y
	bne     L0DBC
;
; &(currentInputTable->fileStream),
;
L0DBD:	ldy     #$1F
	jsr     ldaxysp
	ldy     #$0E
	jsr     incaxy
	jsr     pushax
;
; NULL,
;
	jsr     push0
;
; NULL,
;
	jsr     push0
;
; NULL,
;
	jsr     push0
;
; NULL,
;
	jsr     push0
;
; (query->params & PRM_TRIM) == 0
;
	ldy     #$2D
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	and     #$01
;
; )) {
;
	jsr     booleq
	jsr     _getCsvColumn
	stx     tmp1
	ora     tmp1
	bne     L0DBD
;
; currentInputTable->fileIndex,
;
L0DBC:	ldy     #$1F
	jsr     ldaxysp
	jsr     pushw
;
; query->joinsAndWhereClause,
;
	ldy     #$25
	jsr     ldaxysp
	ldy     #$1F
	jsr     pushwidx
;
; )) {
;
	lda     #$0C
	jsr     leaa0sp
	jsr     _walkRejectRecord
	stx     tmp1
	ora     tmp1
	beq     L0DC8
;
; if(doLeftRecord) {
;
	ldy     #$05
	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L0D66
;
; doLeftRecord = FALSE;
;
	ldx     #$00
	txa
	jsr     staxysp
;
; break;
;
	jmp     L0D65
;
; else if(currentInputTable->nextInputTable == NULL) {
;
L0DC8:	ldy     #$1F
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	cpx     #$00
	jne     L0DD3
	cmp     #$00
	jne     L0DD3
;
; currentInputTable->noLeftRecord = FALSE;
;
	ldy     #$1F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$06
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; currentInputTable = query->firstInputTable;
;
	ldy     #$23
	jsr     ldaxysp
	ldy     #$19
;
; while((currentInputTable) != (query->secondaryInputTable)) {
;
	jmp     L166B
;
; currentInputTable->noLeftRecord = FALSE;
;
L0DDA:	ldy     #$1F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$06
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; currentInputTable = currentInputTable->nextInputTable;
;
	ldy     #$1F
	jsr     ldaxysp
	ldy     #$11
L166B:	jsr     ldaxidx
	ldy     #$1E
	jsr     staxysp
;
; while((currentInputTable) != (query->secondaryInputTable)) {
;
	sta     sreg
	stx     sreg+1
	ldy     #$23
	jsr     ldaxysp
	ldy     #$1B
	jsr     ldaxidx
	cpx     sreg+1
	bne     L0DDA
	cmp     sreg
	bne     L0DDA
;
; currentInputTable = query->secondaryInputTable;
;
	ldy     #$23
	jsr     ldaxysp
	ldy     #$1B
	jsr     ldaxidx
	ldy     #$1E
	jsr     staxysp
;
; getCalculatedColumns(query, match, FALSE);
;
	ldy     #$25
	jsr     pushwysp
	ldy     #$25
	jsr     pushwysp
	ldx     #$00
	txa
	jsr     _getCalculatedColumns
;
; currentInputTable->fileIndex+1, /* +1 means all tables and *CALCULATED* columns */
;
	ldy     #$1F
	jsr     ldaxysp
	jsr     ldaxi
	jsr     incax1
	jsr     pushax
;
; query->joinsAndWhereClause,
;
	ldy     #$25
	jsr     ldaxysp
	ldy     #$1F
	jsr     pushwidx
;
; )) {
;
	lda     #$0C
	jsr     leaa0sp
	jsr     _walkRejectRecord
	stx     tmp1
	ora     tmp1
	bne     L0DF0
;
; return TRUE;
;
	tax
	lda     #$01
	jmp     L0D53
;
; currentInputTable = query->secondaryInputTable = currentInputTable->nextInputTable;
;
L0DD3:	ldy     #$25
	jsr     pushwysp
	ldy     #$21
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$1A
	jsr     staxspidx
	ldy     #$1E
	jsr     staxysp
;
; doLeftRecord = FALSE;
;
L0DF0:	ldx     #$00
	txa
	ldy     #$04
L1656:	jsr     staxysp
;
; endOfFile(currentInputTable->fileStream) ||
;
L0D66:	ldy     #$1F
L165E:	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _endOfFile
	stx     tmp1
	ora     tmp1
	bne     L0D67
;
; currentInputTable->isLeftJoined &&
;
	ldy     #$1F
	jsr     ldaxysp
	ldy     #$04
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0D65
;
; currentInputTable->noLeftRecord &&
;
	ldy     #$1F
	jsr     ldaxysp
	ldy     #$06
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0D65
;
; (doLeftRecord = TRUE)
;
	ldx     #$00
	lda     #$01
	ldy     #$04
	jsr     staxysp
;
; )
;
	jmp     L165D
;
; ) {   /* records */
;
L0D67:	ldx     #$00
	lda     #$01
	jmp     L165D
;
; myfseek(currentInputTable->fileStream, currentInputTable->firstRecordOffset, SEEK_SET);
;
L0D65:	ldy     #$1F
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
	ldy     #$21
	jsr     ldaxysp
	ldy     #$0B
	jsr     ldeaxidx
	jsr     pusheax
	ldx     #$00
	lda     #$02
	jsr     _myfseek
;
; currentInputTable->noLeftRecord = TRUE;
;
	ldy     #$1F
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
; if(currentInputTable->fileIndex == 1) {
;
	ldy     #$1F
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L0DFC
	cmp     #$01
	bne     L0DFC
;
; currentInputTable = NULL;
;
	txa
	ldy     #$1E
	jsr     staxysp
;
; else {
;
	jmp     L0E01
;
; currentInputTable = query->firstInputTable;
;
L0DFC:	ldy     #$23
	jsr     ldaxysp
	ldy     #$19
;
; while(currentInputTable->nextInputTable != query->secondaryInputTable) {
;
	jmp     L166C
;
; currentInputTable = currentInputTable->nextInputTable;
;
L0E04:	ldy     #$1F
	jsr     ldaxysp
	ldy     #$11
L166C:	jsr     ldaxidx
	ldy     #$1E
	jsr     staxysp
;
; while(currentInputTable->nextInputTable != query->secondaryInputTable) {
;
	ldy     #$11
	jsr     ldaxidx
	sta     sreg
	stx     sreg+1
	ldy     #$23
	jsr     ldaxysp
	ldy     #$1B
	jsr     ldaxidx
	cpx     sreg+1
	bne     L0E04
	cmp     sreg
	bne     L0E04
;
; query->secondaryInputTable = currentInputTable;
;
	ldy     #$23
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$1F
	jsr     ldaxysp
	ldy     #$1A
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; } while (currentInputTable != NULL);
;
L0E01:	ldy     #$1E
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L165E
;
; return FALSE;
;
	tax
;
; }
;
L0D53:	ldy     #$24
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ stringGet (__near__ __near__ unsigned char * *, __near__ struct resultColumnValue *, int)
; ---------------------------------------------------------------


.endproc
