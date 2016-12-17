.include "../code2.s"
.export _getColumnValue
.proc	_getColumnValue: near


;
; ) {
;
	jsr     pushax
;
; FILE *inputFile = NULL;
;
	jsr     push0
;
; char *output = (char*)malloc(1);
;
	lda     #$01
	jsr     _malloc
	jsr     pushax
;
; size_t strSize = 0;
;
	jsr     push0
;
; int currentColumn = 0;
;
	jsr     push0
;
; inputFile = skipBom(inputFileName, NULL);
;
	ldy     #$11
	jsr     pushwysp
	ldx     #$00
	txa
	jsr     _skipBom
	ldy     #$06
	jsr     staxysp
;
; if(inputFile == NULL) {
;
	cpx     #$00
	bne     L09C2
	cmp     #$00
	bne     L09C2
;
; fputs(TDB_COULDNT_OPEN_INPUT, stderr);
;
	lda     #<(l09c6)
	ldx     #>(l09c6)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; freeAndZero(output);
;
	ldy     #$05
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$04
	jsr     staxysp
;
; return -1;
;
	dex
	jmp     L15FE
;
; if(myfseek(inputFile, offset, SEEK_SET) != 0) {
;
L09C2:	ldy     #$09
	jsr     pushwysp
	ldy     #$0F
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$00
	lda     #$02
	jsr     _myfseek
	cpx     #$00
	bne     L15FC
	cmp     #$00
	beq     L09DF
;
; fputs(TDB_COULDNT_SEEK, stderr);
;
L15FC:	lda     #<(l09d5)
	ldx     #>(l09d5)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; freeAndZero(output);
;
	ldy     #$05
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$04
	jsr     staxysp
;
; return -1;
;
	dex
	jmp     L15FE
;
; ++currentColumn != columnIndex ?
;
L09DF:	ldx     #$00
	lda     #$01
	jsr     addeq0sp
	ldy     #$08
	cmp     (sp),y
	bne     L15FD
	txa
	iny
	cmp     (sp),y
;
; getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE):
;
	beq     L09E1
L15FD:	lda     #$06
	jsr     leaa0sp
	jsr     pushax
	jsr     push0
	jsr     push0
	jsr     push0
	jsr     push0
	lda     #$01
	jsr     _getCsvColumn
;
; (getCsvColumn(&inputFile, &output, &strSize, NULL, NULL, TRUE) && FALSE)
;
	jmp     L09F3
L09E1:	lda     #$06
	jsr     leaa0sp
	jsr     pushax
	lda     #$06
	jsr     leaa0sp
	jsr     pushax
	lda     #$06
	jsr     leaa0sp
	jsr     pushax
	jsr     push0
	jsr     push0
	lda     #$01
	jsr     _getCsvColumn
	stx     tmp1
	ora     tmp1
	beq     L09F2
	lda     #$00
L09F2:	tax
;
; ) {
;
L09F3:	stx     tmp1
	ora     tmp1
	bne     L09DF
;
; fputs(output, stdout);
;
	ldy     #$07
	jsr     pushwysp
	lda     _stdout
	ldx     _stdout+1
	jsr     _fputs
;
; freeAndZero(output);
;
	ldy     #$05
	jsr     ldaxysp
	jsr     _free
	ldx     #$00
	txa
	ldy     #$04
	jsr     staxysp
;
; fclose(inputFile);
;
	ldy     #$07
	jsr     ldaxysp
	jsr     _fclose
;
; return 0;
;
	ldx     #$00
L15FE:	txa
;
; }
;
	ldy     #$10
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ cleanup_expression (__near__ struct expression *)
; ---------------------------------------------------------------


.endproc
