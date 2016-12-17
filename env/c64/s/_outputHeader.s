.include "../code2.s"
.export _outputHeader
.proc	_outputHeader: near


;
; ) {
;
	jsr     pushax
;
; int firstColumn = TRUE;
;
	lda     #$01
	jsr     pusha0
;
; char *separator = (((query->params) & PRM_SPACE) != 0)?",":", ";
;
	jsr     decsp4
	ldy     #$07
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	and     #$02
	beq     L041F
	lda     #<(l041e)
	ldx     #>(l041e)
	jmp     L0422
L041F:	lda     #<(l0421)
	ldx     #>(l0421)
L0422:	jsr     pushax
;
; if(query->intoFileName) {
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$10
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0424
;
; query->newLine = "\r\n";
;
	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #<(l0427)
	ldy     #$12
	sta     (ptr1),y
	iny
	lda     #>(l0427)
	sta     (ptr1),y
;
; query->outputFile = fopen(query->intoFileName, "wb");
;
	ldy     #$0B
	jsr     pushwysp
	ldy     #$0B
	jsr     ldaxysp
	ldy     #$11
	jsr     pushwidx
	lda     #<(l042c)
	ldx     #>(l042c)
	jsr     _fopen
	ldy     #$14
	jsr     staxspidx
;
; if (query->outputFile == NULL) {
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$15
	jsr     ldaxidx
	cpx     #$00
	bne     L0435
	cmp     #$00
	bne     L0435
;
; fputs("opening output file failed", stderr);
;
	lda     #<(l0432)
	ldx     #>(l0432)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
;
; return;
;
	jmp     L1542
;
; query->outputFile = stdout;
;
L0424:	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     _stdout
	ldy     #$14
	sta     (ptr1),y
	iny
	lda     _stdout+1
	sta     (ptr1),y
;
; query->newLine = "\n";
;
	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #<(l0439)
	ldy     #$12
	sta     (ptr1),y
	iny
	lda     #>(l0439)
	sta     (ptr1),y
;
; outputFile = query->outputFile;
;
L0435:	ldy     #$09
	jsr     ldaxysp
	ldy     #$15
	jsr     ldaxidx
	ldy     #$02
	jsr     staxysp
;
; if(((query->params) & PRM_BOM) != 0) {
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$0F
	jsr     ldaxidx
	and     #$10
	beq     L043D
;
; fputs("\xEF\xBB\xBF", outputFile);
;
	lda     #<(l0443)
	ldx     #>(l0443)
	jsr     pushax
	ldy     #$05
	jsr     ldaxysp
	jsr     _fputs
;
; currentResultColumn = query->firstResultColumn;
;
L043D:	ldy     #$09
	jsr     ldaxysp
	ldy     #$1D
L1546:	jsr     ldaxidx
	ldy     #$04
	jsr     staxysp
;
; currentResultColumn != NULL;
;
	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0447
;
; if(currentResultColumn->isHidden == FALSE) {
;
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	cpx     #$00
	bne     L0448
	cmp     #$00
	bne     L0448
;
; if (!firstColumn) {
;
	ldy     #$07
	lda     (sp),y
	dey
	ora     (sp),y
	bne     L1545
;
; fputs(separator, outputFile);
;
	jsr     pushw0sp
	ldy     #$05
	jsr     ldaxysp
	jsr     _fputs
;
; else {
;
	jmp     L0458
;
; firstColumn = FALSE;
;
L1545:	txa
	jsr     staxysp
;
; fputs((currentResultColumn->resultColumnName)+1, outputFile);
;
L0458:	ldy     #$05
	jsr     ldaxysp
	ldy     #$13
	jsr     ldaxidx
	jsr     incax1
	jsr     pushax
	ldy     #$05
	jsr     ldaxysp
	jsr     _fputs
;
; ) {
;
L0448:	ldy     #$05
	jsr     ldaxysp
	ldy     #$17
	jmp     L1546
;
; fputs(query->newLine, outputFile);
;
L0447:	ldy     #$09
	jsr     ldaxysp
	ldy     #$13
	jsr     pushwidx
	ldy     #$05
	jsr     ldaxysp
L1542:	jsr     _fputs
;
; }
;
	ldy     #$0A
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ combiningCharCompare (__near__ const void *, __near__ const void *)
; ---------------------------------------------------------------


.endproc
