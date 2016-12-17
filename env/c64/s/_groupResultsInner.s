.include "../code2.s"
.export _groupResultsInner
.proc	_groupResultsInner: near


;
; ) {
;
	jsr     pushax
;
; if(i) {
;
	jsr     decsp2
	ldy     #$03
	lda     (sp),y
	dey
	ora     (sp),y
	jeq     L12C9
;
; previousMatch = query->match;
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$27
	jsr     ldaxidx
	jsr     stax0sp
;
; query->match = columns;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$26
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; (query->groupByClause != NULL &&   /* if no group by clause then every record is part of one group */
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$23
	jsr     ldaxidx
	cpx     #$00
	bne     L1711
	cmp     #$00
	beq     L170E
;
; (void *)previousMatch,
;
L1711:	jsr     pushw0sp
;
; (void *)query->match,
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$27
	jsr     pushwidx
;
; ) != 0) ||
;
	ldy     #$0B
	jsr     ldaxysp
	jsr     _recordCompare
	cpx     #$00
	bne     L1710
	cmp     #$00
	bne     L1710
;
; i == query->recordCount
;
L170E:	ldy     #$03
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
;
; ) {
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	cpx     sreg+1
	bne     L12A9
	cmp     sreg
	bne     L12A9
;
; getGroupedColumns(query);
;
L1710:	ldy     #$07
	jsr     ldaxysp
	jsr     _getGroupedColumns
;
; getCalculatedColumns(query, previousMatch, TRUE);
;
	ldy     #$09
	jsr     pushwysp
	ldy     #$05
	jsr     pushwysp
	ldx     #$00
	lda     #$01
	jsr     _getCalculatedColumns
;
; query->useGroupBy = FALSE;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; tree_insert(query, previousMatch, &(query->resultSet));
;
	ldy     #$09
	jsr     pushwysp
	ldy     #$05
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$24
	jsr     incaxy
	jsr     _tree_insert
;
; query->useGroupBy = TRUE;
;
	ldy     #$07
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$0C
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; else {
;
	jmp     L12C3
;
; cleanup_matchValues(query, &previousMatch);
;
L12A9:	ldy     #$09
	jsr     pushwysp
	lda     #$02
	jsr     leaa0sp
	jsr     _cleanup_matchValues
;
; free(previousMatch);
;
	jsr     ldax0sp
	jsr     _free
;
; if(i < query->recordCount) {
;
L12C3:	ldy     #$05
	jsr     pushwysp
	ldy     #$09
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	jsr     tosicmp
	bpl     L12C9
;
; updateRunningCounts(query, query->match);
;
	ldy     #$09
	jsr     pushwysp
	ldy     #$09
	jsr     ldaxysp
	ldy     #$27
	jsr     ldaxidx
	jsr     _updateRunningCounts
;
; }
;
L12C9:	jsr     incsp8
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct _FILE * __near__ skipBom (__near__ const unsigned char *, __near__ long *)
; ---------------------------------------------------------------


.endproc
