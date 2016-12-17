.include "../code2.s"
.export _outputResult
.proc	_outputResult: near


;
; ) {
;
	jsr     pushax
;
; int firstColumn = TRUE, j;
;
	jsr     decsp4
	lda     #$01
	jsr     pusha0
;
; FILE *outputFile = query->outputFile;
;
	jsr     decsp2
	ldy     #$0D
	jsr     ldaxysp
	ldy     #$15
	jsr     pushwidx
;
; char *separator = (((query->params) & PRM_SPACE) != 0)?",":", ";
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	and     #$02
	beq     L0E31
	lda     #<(l0e30)
	ldx     #>(l0e30)
	jmp     L0E34
L0E31:	lda     #<(l0e33)
	ldx     #>(l0e33)
L0E34:	jsr     pushax
;
; char *string = NULL;
;
	jsr     push0
;
; char *string2 = NULL;
;
	jsr     push0
;
; j=0;
;
	ldy     #$08
	jsr     staxysp
;
; currentResultColumn = query->firstResultColumn;
;
	ldy     #$15
	jsr     ldaxysp
	ldy     #$1D
L1674:	jsr     ldaxidx
	ldy     #$0E
	jsr     staxysp
;
; currentResultColumn != NULL;
;
	ldy     #$0E
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L0E3B
;
; if(currentResultColumn->isHidden == FALSE) {
;
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	jne     L0E55
	cmp     #$00
	jne     L0E55
;
; if (!firstColumn) {
;
	ldy     #$0B
	lda     (sp),y
	dey
	ora     (sp),y
	bne     L1672
;
; fputs(separator, outputFile);
;
	ldy     #$07
	jsr     pushwysp
	ldy     #$09
	jsr     ldaxysp
	jsr     _fputs
;
; else {
;
	jmp     L0E4C
;
; firstColumn = FALSE;
;
L1672:	txa
	jsr     staxysp
;
; field = &(columns[j]);
;
L0E4C:	ldy     #$15
	jsr     pushwysp
	ldy     #$0D
	jsr     pushwysp
	lda     #$0E
	jsr     tosmula0
	jsr     tosaddax
	ldy     #$0C
	jsr     staxysp
;
; switch(field->leftNull) {
;
	ldy     #$09
	jsr     ldaxidx
;
; }
;
	cpx     #$00
	bne     L0E61
	cmp     #$01
	bne     L0E61
;
; if(((query->params) & PRM_EXPORT) != 0) {
;
	ldy     #$15
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	and     #$08
	jeq     L0E55
;
; fputs("\\N", outputFile);
;
	lda     #<(l0e5e)
	ldx     #>(l0e5e)
;
; } break;
;
	jmp     L1675
;
; stringGet((unsigned char **)(&string), field, query->params);
;
L0E61:	lda     #$02
	jsr     leaa0sp
	jsr     pushax
	ldy     #$11
	jsr     pushwysp
	ldy     #$19
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _stringGet
;
; if(*string == '\0') {
;
	ldy     #$03
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	ldy     #$00
	lda     (ptr1),y
	bne     L0E67
;
; fputs("\"\"", outputFile);  /* empty string always needs escaping */
;
	lda     #<(l0e6a)
	ldx     #>(l0e6a)
;
; else if(needsEscaping(string, query->params)) {
;
	jmp     L1675
L0E67:	ldy     #$05
	jsr     pushwysp
	ldy     #$17
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	jsr     _needsEscaping
	stx     tmp1
	ora     tmp1
	beq     L0E6E
;
; string2 = strReplace("\"","\"\"", string);
;
	lda     #<(l0e74)
	ldx     #>(l0e74)
	jsr     pushax
	lda     #<(l0e76)
	ldx     #>(l0e76)
	jsr     pushax
	ldy     #$07
	jsr     ldaxysp
	jsr     _strReplace
	jsr     stax0sp
;
; fputs("\"", outputFile);
;
	lda     #<(l0e7a)
	ldx     #>(l0e7a)
	jsr     pushax
	ldy     #$09
	jsr     ldaxysp
	jsr     _fputs
;
; fputs(string2, outputFile);
;
	jsr     pushw0sp
	ldy     #$09
	jsr     ldaxysp
	jsr     _fputs
;
; fputs("\"", outputFile);
;
	lda     #<(l0e81)
	ldx     #>(l0e81)
	jsr     pushax
	ldy     #$09
	jsr     ldaxysp
	jsr     _fputs
;
; freeAndZero(string2);
;
	jsr     ldax0sp
	jsr     _free
	ldx     #$00
	txa
	jsr     stax0sp
;
; else {
;
	jmp     L0E55
;
; fputs(string, outputFile);
;
L0E6E:	ldy     #$03
	jsr     ldaxysp
L1675:	jsr     pushax
	ldy     #$09
	jsr     ldaxysp
	jsr     _fputs
;
; j++;
;
L0E55:	ldy     #$09
	jsr     ldaxysp
	jsr     incax1
	ldy     #$08
	jsr     staxysp
;
; ) {
;
	ldy     #$0F
	jsr     ldaxysp
	ldy     #$17
	jmp     L1674
;
; freeAndZero(string);
;
L0E3B:	ldy     #$03
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$02
	jsr     staxysp
;
; cleanup_matchValues(query, &columns);
;
	ldy     #$17
	jsr     pushwysp
	lda     #$14
	jsr     leaa0sp
	jsr     _cleanup_matchValues
;
; fputs(query->newLine, outputFile);
;
	ldy     #$15
	jsr     ldaxysp
	ldy     #$13
	jsr     pushwidx
	ldy     #$09
	jsr     ldaxysp
	jsr     _fputs
;
; }
;
	ldy     #$16
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ recordCompare (__near__ const void *, __near__ const void *, __near__ void *)
; ---------------------------------------------------------------


.endproc
