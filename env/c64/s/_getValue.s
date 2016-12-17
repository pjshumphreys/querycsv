.include "../code2.s"
.export _getValue
.proc	_getValue: near


;
; ) {
;
	jsr     pushax
;
; expressionPtr->leftNull = FALSE;
;
	ldy     #$0E
	jsr     subysp
	ldy     #$11
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$08
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; switch(expressionPtr->type) {
;
	ldy     #$11
	jsr     ldaxysp
	jsr     ldaxi
;
; }
;
	cpx     #$00
	jne     L0099
	cmp     #$01
	beq     L000D
	cmp     #$02
	jeq     L0024
	cmp     #$03
	jeq     L0029
	cmp     #$09
	jeq     L005C
	cmp     #$0A
	jeq     L0082
	cmp     #$16
	jeq     L003B
	jmp     L0099
;
; field = &(match->ptr[
;
L000D:	ldy     #$0F
	jsr     ldaxysp
;
; ((struct inputColumn*)(expressionPtr->unionPtrs.voidPtr))->
;
	ldy     #$03
	jsr     pushwidx
	ldy     #$13
	jsr     ldaxysp
;
; firstResultColumn->resultColumnIndex
;
	ldy     #$0F
	jsr     ldaxidx
;
; ]);
;
	ldy     #$07
	jsr     ldaxidx
	jsr     pushw
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	ldy     #$08
	jsr     staxysp
;
; if(field->leftNull) {
;
	ldy     #$08
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0014
;
; expressionPtr->leftNull = TRUE;
;
	ldy     #$11
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$08
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; expressionPtr->value = strdup("");
;
	ldy     #$13
	jsr     pushwysp
	lda     #<(l001a)
	ldx     #>(l001a)
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; else {
;
	jmp     L000B
;
; stringGet((unsigned char **)(&(expressionPtr->value)), field, match->params);
;
L0014:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldy     #$0D
	jsr     pushwysp
	ldy     #$13
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _stringGet
;
; } break;
;
	jmp     L000B
;
; expressionPtr->value = strdup((char *)expressionPtr->unionPtrs.voidPtr);
;
L0024:	ldy     #$13
	jsr     pushwysp
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; } break;
;
	jmp     L000B
;
; calculatedField = ((struct expression*)(expressionPtr->unionPtrs.voidPtr));
;
L0029:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0C
	jsr     staxysp
;
; getValue(calculatedField, match);
;
	jsr     pushax
	ldy     #$11
	jsr     ldaxysp
	jsr     _getValue
;
; expressionPtr->leftNull = calculatedField->leftNull;
;
	ldy     #$11
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	ldy     #$08
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; expressionPtr->value = strdup(calculatedField->value);
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$0D
	jsr     ldaxidx
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; freeAndZero(calculatedField->value);
;
	iny
	jsr     ldaxysp
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$0D
	jsr     ldaxysp
;
; } break;
;
	jmp     L14A9
;
; column = (struct resultColumn *)(expressionPtr->unionPtrs.voidPtr);
;
L003B:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0A
	jsr     staxysp
;
; if(column->groupingDone) {
;
	ldy     #$10
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L003F
;
; field = &(match->ptr[column->resultColumnIndex]);
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$03
	jsr     pushwidx
	ldy     #$0D
	jsr     ldaxysp
	jsr     pushw
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	ldy     #$08
	jsr     staxysp
;
; if(field->leftNull == FALSE) {
;
	ldy     #$09
	jsr     ldaxidx
	cpx     #$00
	bne     L004F
	cmp     #$00
	bne     L004F
;
; stringGet((unsigned char **)(&(expressionPtr->value)), field, match->params);
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldy     #$0D
	jsr     pushwysp
	ldy     #$13
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _stringGet
;
; break;
;
	jmp     L000B
;
; else if(column->groupText != NULL) {
;
L003F:	ldy     #$0B
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	cpx     #$00
	bne     L14A2
	cmp     #$00
	beq     L004F
;
; expressionPtr->value = strdup(column->groupText);
;
L14A2:	ldy     #$13
	jsr     pushwysp
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; break;
;
	jmp     L000B
;
; expressionPtr->leftNull = TRUE;
;
L004F:	ldy     #$11
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$08
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; expressionPtr->value = strdup("");
;
	ldy     #$13
	jsr     pushwysp
	lda     #<(l0059)
	ldx     #>(l0059)
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; } break;
;
	jmp     L000B
;
; expressionPtr->unionPtrs.leaves.leftPtr,
;
L005C:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
;
; );
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _getValue
;
; if(expressionPtr->unionPtrs.leaves.leftPtr->leftNull) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$08
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0060
;
; expressionPtr->leftNull = TRUE;
;
	ldy     #$11
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$08
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; expressionPtr->value = strdup("");
;
	ldy     #$13
	jsr     pushwysp
	lda     #<(l0066)
	ldx     #>(l0066)
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; else {
;
	jmp     L0068
;
; temp1 = strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
;
L0060:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
	jsr     _strtod
	ldy     #$04
	jsr     steaxysp
;
; ftostr(&(expressionPtr->value), temp1);
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldx     #$00
	lda     #$21
	jsr     _reallocMsg
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushw
	ldy     #$09
	jsr     ldeaxysp
	jsr     __ftostr
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     ldaxi
	jsr     _strlen
	jsr     incax1
	jsr     _reallocMsg
;
; freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
;
L0068:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
;
; } break;
;
	jmp     L14AA
;
; expressionPtr->unionPtrs.leaves.leftPtr,
;
L0082:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
;
; );
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _getValue
;
; if(expressionPtr->unionPtrs.leaves.leftPtr->leftNull) {
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$08
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0086
;
; expressionPtr->leftNull = TRUE;
;
	ldy     #$11
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$08
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; expressionPtr->value = strdup("");
;
	ldy     #$13
	jsr     pushwysp
	lda     #<(l008c)
	ldx     #>(l008c)
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; else {
;
	jmp     L008E
;
; &(expressionPtr->value),
;
L0086:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
;
; strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL)
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
;
; );
;
	jsr     _strtod
	jsr     _exp_uminus
;
; freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
;
L008E:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
;
; } break;
;
	jmp     L14AA
;
; if(expressionPtr->type > EXP_CONCAT) {
;
L0099:	ldy     #$11
	jsr     ldaxysp
	jsr     ldaxi
	cmp     #$09
	txa
	sbc     #$00
	bvs     L009C
	eor     #$80
L009C:	bpl     L009A
;
; expressionPtr->value = strdup("");
;
	ldy     #$13
	jsr     pushwysp
	lda     #<(l009f)
	ldx     #>(l009f)
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; break;
;
	jmp     L000B
;
; expressionPtr->unionPtrs.leaves.leftPtr,
;
L009A:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     pushwidx
;
; );
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _getValue
;
; expressionPtr->unionPtrs.leaves.rightPtr,
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$11
	jsr     pushwidx
;
; );
;
	ldy     #$11
	jsr     ldaxysp
	jsr     _getValue
;
; expressionPtr->unionPtrs.leaves.leftPtr->leftNull ||
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$08
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	bne     L00A8
;
; expressionPtr->unionPtrs.leaves.rightPtr->leftNull
;
	ldy     #$11
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
	beq     L00A7
;
; expressionPtr->leftNull = TRUE;
;
L00A8:	ldy     #$11
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$08
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; expressionPtr->value = strdup("");
;
	ldy     #$13
	jsr     pushwysp
	lda     #<(l00ae)
	ldx     #>(l00ae)
	jsr     _strdup
	ldy     #$0C
	jsr     staxspidx
;
; else {
;
	jmp     L00B3
;
; switch(expressionPtr->type){
;
L00A7:	ldy     #$11
	jsr     ldaxysp
	jsr     ldaxi
;
; }
;
	cpx     #$00
	jne     L00B3
	cmp     #$04
	beq     L00B5
	cmp     #$05
	jeq     L00D3
	cmp     #$06
	jeq     L00F1
	cmp     #$07
	jeq     L010F
	cmp     #$08
	jeq     L011A
	jmp     L00B3
;
; temp1 = strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
;
L00B5:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
	jsr     _strtod
	ldy     #$04
	jsr     steaxysp
;
; temp2 = fadd(temp1, strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL));
;
	ldy     #$07
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$15
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
	jsr     _strtod
	jsr     __fadd
	jsr     steax0sp
;
; ftostr(&(expressionPtr->value), temp2);
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldx     #$00
	lda     #$21
	jsr     _reallocMsg
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushw
	ldy     #$05
	jsr     ldeaxysp
	jsr     __ftostr
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     ldaxi
	jsr     _strlen
	jsr     incax1
	jsr     _reallocMsg
;
; break;
;
	jmp     L00B3
;
; temp1 = strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
;
L00D3:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
	jsr     _strtod
	ldy     #$04
	jsr     steaxysp
;
; temp2 = fsub(temp1, strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL));
;
	ldy     #$07
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$15
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
	jsr     _strtod
	jsr     __fsub
	jsr     steax0sp
;
; ftostr(&(expressionPtr->value), temp2);
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldx     #$00
	lda     #$21
	jsr     _reallocMsg
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushw
	ldy     #$05
	jsr     ldeaxysp
	jsr     __ftostr
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     ldaxi
	jsr     _strlen
	jsr     incax1
	jsr     _reallocMsg
;
; break;
;
	jmp     L00B3
;
; temp1 = strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL);
;
L00F1:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
	jsr     _strtod
	ldy     #$04
	jsr     steaxysp
;
; temp2 = fmul(temp1, strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL));
;
	ldy     #$07
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$15
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
	jsr     _strtod
	jsr     __fmul
	jsr     steax0sp
;
; ftostr(&(expressionPtr->value), temp2);
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldx     #$00
	lda     #$21
	jsr     _reallocMsg
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushw
	ldy     #$05
	jsr     ldeaxysp
	jsr     __ftostr
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     ldaxi
	jsr     _strlen
	jsr     incax1
	jsr     _reallocMsg
;
; break;
;
	jmp     L00B3
;
; &(expressionPtr->value),
;
L010F:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
;
; strtod(expressionPtr->unionPtrs.leaves.leftPtr->value, NULL),
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
	jsr     _strtod
	jsr     pusheax
;
; strtod(expressionPtr->unionPtrs.leaves.rightPtr->value, NULL)
;
	ldy     #$17
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldx     #$00
	txa
;
; );
;
	jsr     _strtod
	jsr     _exp_divide
;
; break;
;
	jmp     L00B3
;
; &(expressionPtr->value),
;
L011A:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0C
	jsr     incaxy
	jsr     pushax
;
; "%s%s",
;
	lda     #<(l011e)
	ldx     #>(l011e)
	jsr     pushax
;
; expressionPtr->unionPtrs.leaves.leftPtr->value,
;
	ldy     #$15
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
;
; expressionPtr->unionPtrs.leaves.rightPtr->value
;
	ldy     #$17
	jsr     ldaxysp
;
; );
;
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0D
	jsr     pushwidx
	ldy     #$08
	jsr     _d_sprintf
;
; freeAndZero(expressionPtr->unionPtrs.leaves.leftPtr->value);
;
L00B3:	ldy     #$11
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$11
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
	ldy     #$11
	jsr     ldaxysp
	ldy     #$11
	jsr     ldaxidx
	ldy     #$0D
	jsr     ldaxidx
	jsr     _free
	ldy     #$11
	jsr     ldaxysp
	ldy     #$11
L14AA:	jsr     ldaxidx
L14A9:	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$0C
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; }
;
L000B:	ldy     #$12
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ parse_orderingSpec (__near__ struct qryData *, __near__ struct expression *, int)
; ---------------------------------------------------------------


.endproc
