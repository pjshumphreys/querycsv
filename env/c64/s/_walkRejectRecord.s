.include "../code2.s"
.export _walkRejectRecord
.proc	_walkRejectRecord: near


;
; ) {
;
	jsr     pushax
;
; if(expressionPtr == NULL || currentTable < expressionPtr->minTable) {
;
	jsr     decsp6
	ldy     #$08
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L0AD7
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     tosicmp
	jmi     L0AD7
;
; if(expressionPtr->type >= EXP_EQ && expressionPtr->type <= EXP_GTE){
;
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cmp     #$0E
	txa
	sbc     #$00
	bvs     L0A70
	eor     #$80
L0A70:	jpl     L0A6E
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cmp     #$14
	txa
	sbc     #$00
	bvc     L0A72
	eor     #$80
L0A72:	jpl     L0A6E
;
; getValue(expressionPtr->unionPtrs.leaves.leftPtr, match);
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
	ldy     #$09
	jsr     ldaxysp
	jsr     _getValue
;
; getValue(expressionPtr->unionPtrs.leaves.rightPtr, match);
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$11
	jsr     pushwidx
	ldy     #$09
	jsr     ldaxysp
	jsr     _getValue
;
; expressionPtr->unionPtrs.leaves.leftPtr->leftNull ||
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$08
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	bne     L0A7B
;
; expressionPtr->unionPtrs.leaves.rightPtr->leftNull
;
	jsr     ldaxysp
;
; ) {
;
	ldy     #$11
	jsr     ldaxidx
	ldy     #$08
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L161E
;
; freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
;
L0A7B:	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; freeAndZero(expressionPtr->unionPtrs.leaves.rightPtr->value);
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$09
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; return FALSE;
;
	tax
	jmp     L0A68
;
; (unsigned char **)(&(expressionPtr->unionPtrs.leaves.leftPtr->value)),
;
L161E:	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
;
; (unsigned char **)(&(expressionPtr->unionPtrs.leaves.rightPtr->value)),
;
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
;
; expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive,
;
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0B
	jsr     pushwidx
;
; (void (*)())&getUnicodeChar,
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
; freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; freeAndZero(expressionPtr->unionPtrs.leaves.rightPtr->value);
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$09
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; switch(expressionPtr->type) {
;
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
;
; }
;
	cpx     #$00
	jne     L0AD7
	cmp     #$0E
	beq     L0A9D
	cmp     #$0F
	beq     L0AA1
	cmp     #$10
	beq     L0AA5
	cmp     #$11
	beq     L0AAD
	cmp     #$12
	beq     L0AA9
	cmp     #$13
	beq     L0AB1
	jmp     L1619
;
; return retval != 0;
;
L0A9D:	ldy     #$05
	jsr     ldaxysp
	cpx     #$00
	bne     L0A9F
	cmp     #$00
L0A9F:	jsr     boolne
	jmp     L0A68
;
; return retval == 0;
;
L0AA1:	ldy     #$05
	jsr     ldaxysp
	cpx     #$00
	bne     L0AA3
	cmp     #$00
L0AA3:	jsr     booleq
	jmp     L0A68
;
; return retval != -1;
;
L0AA5:	ldy     #$05
	jsr     ldaxysp
	cpx     #$FF
	bne     L0AA7
	cmp     #$FF
L0AA7:	jsr     boolne
	jmp     L0A68
;
; return retval == 1;
;
L0AA9:	ldy     #$05
	jsr     ldaxysp
	cpx     #$00
	bne     L0AAB
	cmp     #$01
L0AAB:	jsr     booleq
	jmp     L0A68
;
; return retval != 1;
;
L0AAD:	ldy     #$05
	jsr     ldaxysp
	cpx     #$00
	bne     L0AAF
	cmp     #$01
L0AAF:	jsr     boolne
	jmp     L0A68
;
; return retval == -1;
;
L0AB1:	ldy     #$05
	jsr     ldaxysp
	cpx     #$FF
	bne     L0AB3
	cmp     #$FF
L0AB3:	jsr     booleq
	jmp     L0A68
;
; else if(expressionPtr->type == EXP_AND) {
;
L0A6E:	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L0AB5
	cmp     #$0B
	bne     L0AB5
;
; walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) ||
;
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
	ldy     #$0B
	jsr     ldaxysp
	jsr     _walkRejectRecord
	stx     tmp1
	ora     tmp1
	bne     L0AB8
;
; walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
;
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$11
	jsr     pushwidx
	ldy     #$0B
	jsr     ldaxysp
	jsr     _walkRejectRecord
	stx     tmp1
	ora     tmp1
	bne     L0AB8
	tax
	jmp     L0A68
L0AB8:	ldx     #$00
	lda     #$01
	jmp     L0A68
;
; else if(expressionPtr->type == EXP_OR) {
;
L0AB5:	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L0AC1
	cmp     #$0C
	bne     L0AC1
;
; walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) &&
;
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
	ldy     #$0B
	jsr     ldaxysp
	jsr     _walkRejectRecord
	stx     tmp1
	ora     tmp1
	beq     L0AC8
;
; walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.rightPtr, match);
;
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$11
	jsr     pushwidx
	ldy     #$0B
	jsr     ldaxysp
	jsr     _walkRejectRecord
	stx     tmp1
	ora     tmp1
	bne     L0AC4
L0AC8:	tax
	jmp     L0A68
L0AC4:	ldx     #$00
	lda     #$01
	jmp     L0A68
;
; else if(expressionPtr->type <= EXP_NOT) {
;
L0AC1:	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cmp     #$0E
	txa
	sbc     #$00
	bvc     L0AD0
	eor     #$80
L0AD0:	bpl     L0ACE
;
; return walkRejectRecord(currentTable, expressionPtr->unionPtrs.leaves.leftPtr, match) == 0;
;
	ldy     #$0D
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
	ldy     #$0B
	jsr     ldaxysp
	jsr     _walkRejectRecord
	cpx     #$00
	bne     L0AD5
	cmp     #$00
L0AD5:	jsr     booleq
	jmp     L0A68
;
; else if(expressionPtr->type == EXP_IN || expressionPtr->type == EXP_NOTIN) {
;
L0ACE:	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L1620
	cmp     #$14
	beq     L0AD8
L1620:	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	jne     L0AD7
	cmp     #$15
	jne     L1619
;
; getValue(expressionPtr->unionPtrs.leaves.leftPtr, match);
;
L0AD8:	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
	ldy     #$09
	jsr     ldaxysp
	jsr     _getValue
;
; if(expressionPtr->unionPtrs.leaves.leftPtr->leftNull) {
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$08
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L161F
;
; freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
;
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; return FALSE;
;
	tax
	jmp     L0A68
;
; i = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->index,
;
L161F:	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	jsr     ldaxi
	ldy     #$02
	jsr     staxysp
;
; currentAtom = expressionPtr->unionPtrs.inLeaves.lastEntryPtr->nextInList;
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
L1625:	ldy     #$05
	jsr     ldaxidx
	jsr     stax0sp
;
; i != currentAtom->index;
;
	ldy     #$03
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	jsr     ldax0sp
	jsr     ldaxi
	cpx     sreg+1
	bne     L1622
	cmp     sreg
	jeq     L0AE7
;
; (unsigned char **)(&(expressionPtr->unionPtrs.leaves.leftPtr->value)),
;
L1622:	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
;
; (unsigned char **)(&(currentAtom->content)),
;
	ldy     #$03
	jsr     ldaxysp
	jsr     incax2
	jsr     pushax
;
; expressionPtr->unionPtrs.leaves.leftPtr->caseSensitive,
;
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0B
	jsr     pushwidx
;
; (void (*)())&getUnicodeChar,
;
	lda     #<(_getUnicodeChar)
	ldx     #>(_getUnicodeChar)
	jsr     pushax
;
; ) == 0) {
;
	lda     #<(_getUnicodeChar)
	ldx     #>(_getUnicodeChar)
	jsr     _strCompare
	cpx     #$00
	bne     L0AE8
	cmp     #$00
	bne     L0AE8
;
; freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; return expressionPtr->type == EXP_NOTIN;  /* FALSE */
;
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L0B02
	cmp     #$15
L0B02:	jsr     booleq
	jmp     L0A68
;
; ) {
;
L0AE8:	jsr     ldax0sp
	jmp     L1625
;
; freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
;
L0AE7:	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; return expressionPtr->type != EXP_NOTIN;  /* TRUE */
;
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L0B08
	cmp     #$15
L0B08:	jsr     boolne
	jmp     L0A68
;
; return FALSE;
;
L0AD7:	ldx     #$00
L1619:	txa
;
; }
;
L0A68:	ldy     #$0C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ getColumnCount (__near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
