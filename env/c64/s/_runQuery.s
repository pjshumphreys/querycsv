.include "../code2.s"
.export _runQuery
.proc	_runQuery: near


;
; int runQuery(char *queryFileName) {
;
	jsr     pushax
;
; struct resultColumnValue* match = NULL;
;
	ldy     #$28
	jsr     subysp
	jsr     push0
;
; readQuery(queryFileName, &query);
;
	ldy     #$2D
	jsr     pushwysp
	lda     #$04
	jsr     leaa0sp
	jsr     _readQuery
;
; (void**)&match,
;
	lda     sp
	ldx     sp+1
	jsr     pushax
;
; (query.columnCount)*sizeof(struct resultColumnValue)
;
	ldy     #$0B
;
; );
;
	jsr     pushwysp
	lda     #$0E
	jsr     tosumula0
	jsr     _reallocMsg
;
; query.orderByClause == NULL &&
;
	ldy     #$22
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0C4B
;
; query.intoFileName == NULL &&
;
	ldy     #$12
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0C4B
;
; query.groupByClause == NULL
;
	ldy     #$24
	lda     (sp),y
	iny
	ora     (sp),y
;
; ) {
;
	bne     L0C4B
;
; outputHeader(&query); 
;
	lda     #$02
	jsr     leaa0sp
	jsr     _outputHeader
;
; while(getMatchingRecord(&query, match)) {
;
	jmp     L0C56
;
; outputResult(&query, match, 0);
;
L0C54:	lda     #$02
	jsr     leaa0sp
	jsr     pushax
	ldy     #$05
	jsr     pushwysp
	ldx     #$00
	txa
	jsr     _outputResult
;
; match = NULL;
;
	ldx     #$00
	txa
	jsr     stax0sp
;
; (void**)&match,
;
	lda     sp
	ldx     sp+1
	jsr     pushax
;
; (query.columnCount)*sizeof(struct resultColumnValue)
;
	ldy     #$0B
;
; );
;
	jsr     pushwysp
	lda     #$0E
	jsr     tosumula0
	jsr     _reallocMsg
;
; while(getMatchingRecord(&query, match)) {
;
L0C56:	lda     #$02
	jsr     leaa0sp
	jsr     pushax
	ldy     #$03
	jsr     ldaxysp
	jsr     _getMatchingRecord
	stx     tmp1
	ora     tmp1
	bne     L0C54
;
; free(match);
;
	jsr     ldax0sp
	jsr     _free
;
; match = NULL;
;
	ldx     #$00
	txa
	jsr     stax0sp
;
; else {
;
	jmp     L0C91
;
; if(query.groupByClause != NULL) {
;
L0C4B:	ldy     #$24
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0C70
;
; query.useGroupBy = TRUE;
;
	ldx     #$00
	lda     #$01
	ldy     #$0E
	jsr     staxysp
;
; while(getMatchingRecord(&query, match)) {
;
	jmp     L0C70
;
; tree_insert(&query, match, &(query.resultSet));
;
L0C6E:	lda     #$02
	jsr     leaa0sp
	jsr     pushax
	ldy     #$05
	jsr     pushwysp
	lda     #$2A
	jsr     leaa0sp
	jsr     _tree_insert
;
; match = NULL;
;
	ldx     #$00
	txa
	jsr     stax0sp
;
; (void**)&match,
;
	lda     sp
	ldx     sp+1
	jsr     pushax
;
; (query.columnCount)*sizeof(struct resultColumnValue)
;
	ldy     #$0B
;
; );
;
	jsr     pushwysp
	lda     #$0E
	jsr     tosumula0
	jsr     _reallocMsg
;
; while(getMatchingRecord(&query, match)) {
;
L0C70:	lda     #$02
	jsr     leaa0sp
	jsr     pushax
	ldy     #$03
	jsr     ldaxysp
	jsr     _getMatchingRecord
	stx     tmp1
	ora     tmp1
	bne     L0C6E
;
; free(match);
;
	jsr     ldax0sp
	jsr     _free
;
; match = NULL;
;
	ldx     #$00
	txa
	jsr     stax0sp
;
; if(query.groupByClause != NULL) {
;
	ldy     #$24
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0C83
;
; groupResults(&query);
;
	lda     #$02
	jsr     leaa0sp
	jsr     _groupResults
;
; query.useGroupBy = FALSE;
;
	ldx     #$00
	txa
	ldy     #$0E
	jsr     staxysp
;
; outputHeader(&query);
;
L0C83:	lda     #$02
	jsr     leaa0sp
	jsr     _outputHeader
;
; &query,
;
	lda     #$02
	jsr     leaa0sp
	jsr     pushax
;
; &(query.resultSet),
;
	lda     #$28
	jsr     leaa0sp
	jsr     pushax
;
; );
;
	lda     #<(_outputResult)
	ldx     #>(_outputResult)
	jsr     _tree_walkAndCleanup
;
; if(query.intoFileName) {
;
	ldy     #$13
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L0C91
;
; fclose(query.outputFile);
;
	ldy     #$17
	jsr     ldaxysp
	jsr     _fclose
;
; fprintf(stdout, "%d", query.recordCount);
;
	lda     _stdout
	ldx     _stdout+1
	jsr     pushax
	lda     #<(l0c97)
	ldx     #>(l0c97)
	jsr     pushax
	ldy     #$11
	jsr     pushwysp
	ldy     #$06
	jsr     _fprintf
;
; fflush(stdout);
;
	lda     _stdout
	ldx     _stdout+1
	jsr     _fflush
;
; cleanup_query(&query);
;
L0C91:	lda     #$02
	jsr     leaa0sp
	jsr     _cleanup_query
;
; return 0;
;
	ldx     #$00
	txa
;
; }
;
	ldy     #$2C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; unsigned int __near__ hash_compare (__near__ struct columnReferenceHash *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
