.include "../code2.s"
.export _updateRunningCounts
.proc	_updateRunningCounts: near


;
; void updateRunningCounts(struct qryData *query, struct resultColumnValue *match) {
;
	jsr     pushax
;
; char *tempString = NULL;
;
	jsr     decsp8
	jsr     push0
;
; char *tempString2 = NULL;
;
	jsr     push0
;
; query->groupCount++;
;
	jsr     decsp8
	ldy     #$17
	jsr     ldaxysp
	jsr     pushax
	ldy     #$0B
	jsr     ldaxidx
	jsr     incax1
	ldy     #$0A
	jsr     staxspidx
;
; for(i = 0; i < query->columnReferenceHashTable->size; i++) {
;
	ldx     #$00
	txa
L16CE:	ldy     #$02
	jsr     staxysp
	jsr     pushax
	ldy     #$19
	jsr     ldaxysp
	ldy     #$17
	jsr     ldaxidx
	jsr     ldaxi
	jsr     tosicmp
	jpl     L0FA2
;
; currentHashEntry = query->columnReferenceHashTable->table[i];
;
	ldy     #$17
	jsr     ldaxysp
	ldy     #$17
	jsr     ldaxidx
	ldy     #$03
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	jsr     aslax1
	clc
	adc     ptr1
	pha
	txa
	adc     ptr1+1
	tax
	pla
	ldy     #$01
;
; while(currentHashEntry != NULL) {
;
	jmp     L16CB
;
; currentReference = currentHashEntry->content;
;
L16C0:	jsr     ldaxysp
	ldy     #$03
;
; while(currentReference != NULL) {
;
	jmp     L16CC
;
; currentReference->referenceType == REF_EXPRESSION &&
;
L16C1:	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	jne     L0FC5
	cmp     #$02
	jne     L0FC5
;
; (currentResultColumn = currentReference->reference.calculatedPtr.firstResultColumn) != NULL &&
;
	ldy     #$11
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	ldy     #$0E
	jsr     staxysp
	cpx     #$00
	bne     L16C2
	cmp     #$00
	jeq     L0FC5
;
; currentResultColumn->groupType != GRP_NONE
;
L16C2:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
;
; ) {
;
	cpx     #$00
	bne     L0FB9
	cmp     #$00
	jeq     L0FC5
;
; field = &(match[currentResultColumn->resultColumnIndex]);
;
L0FB9:	ldy     #$17
	jsr     pushwysp
	ldy     #$11
	jsr     ldaxysp
	jsr     pushw
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	ldy     #$0C
	jsr     staxysp
;
; if(field->leftNull == FALSE) {
;
	ldy     #$09
	jsr     ldaxidx
	cpx     #$00
	jne     L0FC5
	cmp     #$00
	jne     L0FC5
;
; stringGet((unsigned char **)(&tempString), field, query->params);
;
	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
	ldy     #$11
	jsr     pushwysp
	ldy     #$1B
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _stringGet
;
; if(currentResultColumn->groupType > GRP_STAR) {
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
	cmp     #$08
	txa
	sbc     #$00
	bvs     L0FCF
	eor     #$80
L0FCF:	jpl     L0FF6
;
; if(query->groupCount > 1) {
;
	ldy     #$17
	jsr     ldaxysp
	ldy     #$0B
	jsr     ldaxidx
	cmp     #$02
	txa
	sbc     #$00
	bvs     L0FD2
	eor     #$80
L0FD2:	jpl     L0FD0
;
; for(j = 1; j < query->groupCount; j++) {
;
	ldx     #$00
	lda     #$01
L16CD:	jsr     stax0sp
	jsr     pushax
	ldy     #$19
	jsr     ldaxysp
	ldy     #$0B
	jsr     ldaxidx
	jsr     tosicmp
	jpl     L0FF6
;
; stringGet((unsigned char **)(&tempString2), &(match[(currentResultColumn->resultColumnIndex) - (query->columnCount)]), query->params);
;
	lda     #$08
	jsr     leaa0sp
	jsr     pushax
	ldy     #$19
	jsr     pushwysp
	ldy     #$13
	jsr     ldaxysp
	jsr     pushw
	ldy     #$1D
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	jsr     tossubax
	jsr     pushax
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	jsr     pushax
	ldy     #$1B
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _stringGet
;
; (unsigned char **)(&tempString),
;
	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
;
; (unsigned char **)(&tempString2),
;
	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
;
; TRUE,
;
	lda     #$01
	jsr     pusha0
;
; (void (*)())getUnicodeChar,
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
	bne     L0FE4
	cmp     #$00
	bne     L0FE4
;
; freeAndZero(tempString2);
;
	ldy     #$09
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$08
;
; break;
;
	jmp     L16BF
;
; freeAndZero(tempString2);
;
L0FE4:	ldy     #$09
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$08
	jsr     staxysp
;
; for(j = 1; j < query->groupCount; j++) {
;
	jsr     ldax0sp
	jsr     incax1
	jmp     L16CD
;
; j = query->groupCount;
;
L0FD0:	ldy     #$17
	jsr     ldaxysp
	ldy     #$0B
	jsr     ldaxidx
	ldy     #$00
L16BF:	jsr     staxysp
;
; switch(currentResultColumn->groupType) {
;
L0FF6:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$07
	jsr     ldaxidx
;
; }
;
	cpx     #$00
	jne     L0FFB
	cmp     #$01
	jeq     L1016
	cmp     #$02
	jeq     L1039
	cmp     #$03
	jeq     L1052
	cmp     #$04
	jeq     L1016
	cmp     #$05
	beq     L1002
	cmp     #$06
	jeq     L1030
	cmp     #$08
	beq     L1005
	cmp     #$09
	jeq     L1039
	cmp     #$0A
	jeq     L1052
	cmp     #$0B
	beq     L1005
	cmp     #$0C
	beq     L0FFD
	cmp     #$0D
	jeq     L1025
	jmp     L0FFB
;
; if(j == query->groupCount) {
;
L0FFD:	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$17
	jsr     ldaxysp
	ldy     #$0B
	jsr     ldaxidx
	cpx     sreg+1
	jne     L0FFB
	cmp     sreg
	jne     L0FFB
;
; currentResultColumn->groupCount++;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     pushax
	ldy     #$0F
	jsr     ldaxidx
	jsr     incax1
	ldy     #$0E
	jsr     staxspidx
;
; break;
;
	jmp     L0FFB
;
; currentResultColumn->groupCount++;
;
L1002:	ldy     #$0F
	jsr     ldaxysp
	jsr     pushax
	ldy     #$0F
	jsr     ldaxidx
	jsr     incax1
	ldy     #$0E
	jsr     staxspidx
;
; break;
;
	jmp     L0FFB
;
; if(j == query->groupCount) {
;
L1005:	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$17
	jsr     ldaxysp
	ldy     #$0B
	jsr     ldaxidx
	cpx     sreg+1
	jne     L0FFB
	cmp     sreg
	jne     L0FFB
;
; currentResultColumn->groupCount++;
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     pushax
	ldy     #$0F
	jsr     ldaxidx
	jsr     incax1
	ldy     #$0E
	jsr     staxspidx
;
; tempFloat = strtod(tempString, NULL);
;
	dey
	jsr     pushwysp
	ldx     #$00
	txa
	jsr     _strtod
	ldy     #$04
	jsr     steaxysp
;
; currentResultColumn->groupNum = fadd(currentResultColumn->groupNum, tempFloat);
;
	ldy     #$11
	jsr     pushwysp
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0D
	jsr     ldeaxidx
	jsr     pusheax
	ldy     #$0D
	jsr     ldeaxysp
	jsr     __fadd
	ldy     #$0A
	jsr     steaxspidx
;
; break;
;
	jmp     L0FFB
;
; currentResultColumn->groupCount++;
;
L1016:	ldy     #$0F
	jsr     ldaxysp
	jsr     pushax
	ldy     #$0F
	jsr     ldaxidx
	jsr     incax1
	ldy     #$0E
	jsr     staxspidx
;
; tempFloat = strtod(tempString, NULL);
;
	dey
	jsr     pushwysp
	ldx     #$00
	txa
	jsr     _strtod
	ldy     #$04
	jsr     steaxysp
;
; currentResultColumn->groupNum = fadd(currentResultColumn->groupNum, tempFloat);
;
	ldy     #$11
	jsr     pushwysp
	ldy     #$11
	jsr     ldaxysp
	ldy     #$0D
	jsr     ldeaxidx
	jsr     pusheax
	ldy     #$0D
	jsr     ldeaxysp
	jsr     __fadd
	ldy     #$0A
	jsr     steaxspidx
;
; break;
;
	jmp     L0FFB
;
; if(j == query->groupCount) {
;
L1025:	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$17
	jsr     ldaxysp
	ldy     #$0B
	jsr     ldaxidx
	cpx     sreg+1
	jne     L0FFB
	cmp     sreg
	jne     L0FFB
;
; &(currentResultColumn->groupText),
;
	ldy     #$0F
	jsr     ldaxysp
	jsr     incax8
	jsr     pushax
;
; "%s%s",
;
	lda     #<(l102b)
	ldx     #>(l102b)
	jsr     pushax
;
; currentResultColumn->groupText,
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$09
	jsr     pushwidx
;
; );
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$08
	jsr     _d_sprintf
;
; break;
;
	jmp     L0FFB
;
; &(currentResultColumn->groupText),
;
L1030:	ldy     #$0F
	jsr     ldaxysp
	jsr     incax8
	jsr     pushax
;
; "%s%s",
;
	lda     #<(l1034)
	ldx     #>(l1034)
	jsr     pushax
;
; currentResultColumn->groupText,
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$09
	jsr     pushwidx
;
; );
;
	ldy     #$13
	jsr     pushwysp
	ldy     #$08
	jsr     _d_sprintf
;
; break;
;
	jmp     L0FFB
;
; if(currentResultColumn->groupText == NULL || strCompare(
;
L1039:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	cpx     #$00
	bne     L16C3
	cmp     #$00
	beq     L103D
;
; (unsigned char **)(&tempString),
;
L16C3:	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
;
; (unsigned char **)(&(currentResultColumn->groupText)),
;
	ldy     #$11
	jsr     ldaxysp
	jsr     incax8
	jsr     pushax
;
; 2,    /* TRUE, */
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
; ) == -1) {
;
	lda     #<(_getUnicodeChar)
	ldx     #>(_getUnicodeChar)
	jsr     _strCompare
	cpx     #$FF
	jne     L0FFB
	cmp     #$FF
	jne     L0FFB
;
; free(currentResultColumn->groupText);
;
L103D:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	jsr     _free
;
; currentResultColumn->groupText = tempString;
;
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$08
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; continue;
;
	jmp     L16D4
;
; if(currentResultColumn->groupText == NULL || strCompare(
;
L1052:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	cpx     #$00
	bne     L16C5
	cmp     #$00
	beq     L1056
;
; (unsigned char **)(&tempString),
;
L16C5:	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
;
; (unsigned char **)(&(currentResultColumn->groupText)),
;
	ldy     #$11
	jsr     ldaxysp
	jsr     incax8
	jsr     pushax
;
; 2,    /* TRUE, */
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
; ) == 1) {
;
	lda     #<(_getUnicodeChar)
	ldx     #>(_getUnicodeChar)
	jsr     _strCompare
	cpx     #$00
	bne     L0FFB
	cmp     #$01
	bne     L0FFB
;
; free(currentResultColumn->groupText);
;
L1056:	ldy     #$0F
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	jsr     _free
;
; currentResultColumn->groupText = tempString;
;
	ldy     #$0F
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$08
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; continue;
;
	jmp     L16D4
;
; freeAndZero(tempString);
;
L0FFB:	ldy     #$0B
	jsr     ldaxysp
	jsr     _free
L16D4:	ldx     #$00
	txa
	ldy     #$0A
	jsr     staxysp
;
; currentReference = currentReference->nextReferenceWithName;
;
L0FC5:	ldy     #$11
	jsr     ldaxysp
	ldy     #$09
L16CC:	jsr     ldaxidx
	ldy     #$10
	jsr     staxysp
;
; while(currentReference != NULL) {
;
	ldy     #$10
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L16C1
;
; currentHashEntry = currentHashEntry->nextReferenceInHash;
;
	ldy     #$13
	jsr     ldaxysp
	ldy     #$05
L16CB:	jsr     ldaxidx
	ldy     #$12
	jsr     staxysp
;
; while(currentHashEntry != NULL) {
;
	ldy     #$12
	lda     (sp),y
	iny
	ora     (sp),y
	jne     L16C0
;
; for(i = 0; i < query->columnReferenceHashTable->size; i++) {
;
	ldy     #$03
	jsr     ldaxysp
	jsr     incax1
	jmp     L16CE
;
; }
;
L0FA2:	ldy     #$18
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ yyerror (__near__ struct qryData *, __near__ void *, __near__ const unsigned char *)
; ---------------------------------------------------------------


.endproc
