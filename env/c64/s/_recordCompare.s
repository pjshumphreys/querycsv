.include "../code2.s"
.export _recordCompare
.proc	_recordCompare: near


;
; ) {
;
	jsr     pushax
;
; matchParams.params = ((struct qryData*)c)->params;
;
	ldy     #$10
	jsr     subysp
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     stax0sp
;
; orderByClause = ((struct qryData*)c)->useGroupBy?
;
	ldy     #$11
	jsr     ldaxysp
;
; ((struct qryData*)c)->groupByClause:
;
	ldy     #$0C
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0EA2
	ldy     #$11
	jsr     ldaxysp
	ldy     #$23
;
; ((struct qryData*)c)->orderByClause;
;
	jmp     L1676
L0EA2:	ldy     #$11
	jsr     ldaxysp
	ldy     #$21
L1676:	jsr     ldaxidx
	ldy     #$0E
	jsr     staxysp
;
; orderByClause != NULL;
;
	ldy     #$0E
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L0E9C
;
; matchParams.ptr = (struct resultColumnValue*)a;
;
	ldy     #$15
	jsr     ldaxysp
	ldy     #$02
	jsr     staxysp
;
; orderByClause->expressionPtr,
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     pushw
;
; );
;
	lda     #$02
	jsr     leaa0sp
	jsr     _getValue
;
; string1 = output1 = orderByClause->expressionPtr->value;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$0D
	jsr     ldaxidx
	ldy     #$08
	jsr     staxysp
	ldy     #$0C
	jsr     staxysp
;
; orderByClause->expressionPtr->value = NULL;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     ldaxi
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; matchParams.ptr = (struct resultColumnValue*)b;
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$02
	jsr     staxysp
;
; orderByClause->expressionPtr,
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     pushw
;
; );
;
	lda     #$02
	jsr     leaa0sp
	jsr     _getValue
;
; string2 = output2 = orderByClause->expressionPtr->value;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$0D
	jsr     ldaxidx
	ldy     #$06
	jsr     staxysp
	ldy     #$0A
	jsr     staxysp
;
; orderByClause->expressionPtr->value = NULL;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     ldaxi
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; (unsigned char **)(&output1),
;
	lda     #$08
	jsr     leaa0sp
	jsr     pushax
;
; (unsigned char **)(&output2),
;
	lda     #$08
	jsr     leaa0sp
	jsr     pushax
;
; 2,    /* orderByClause->expressionPtr->caseSensitive, */
;
	lda     #$02
	jsr     pusha0
;
; (void (*)())getUnicodeChar,
;
	lda     #<(_getUnicodeChar)
	ldx     #>(_getUnicodeChar)
	jsr     pushax
;
; );
;
	lda     #<(_getUnicodeChar)
	ldx     #>(_getUnicodeChar)
	jsr     _strCompare
	ldy     #$04
	jsr     staxysp
;
; freeAndZero(output1);
;
	ldy     #$09
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$08
	jsr     staxysp
;
; freeAndZero(output2);
;
	ldy     #$07
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$06
	jsr     staxysp
;
; if(compare != 0) {
;
	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0E9D
;
; return orderByClause->isDescending?compare:-compare;
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$02
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0ED5
	ldy     #$05
	jsr     ldaxysp
	jmp     L0E97
L0ED5:	ldy     #$05
	jsr     ldaxysp
	jsr     negax
	jmp     L0E97
;
; ) {
;
L0E9D:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$05
	jmp     L1676
;
; return 0;
;
L0E9C:	tax
;
; }
;
L0E97:	ldy     #$16
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct expression * __near__ parse_scalarExpLiteral (__near__ struct qryData *, __near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
