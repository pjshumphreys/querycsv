.include "../code2.s"
.export _getColumnCount
.proc	_getColumnCount: near


;
; int getColumnCount(char *inputFileName) {
;
	jsr     pushax
;
; FILE *inputFile = NULL;
;
	jsr     push0
;
; int columnCount = 1;
;
	lda     #$01
	jsr     pusha0
;
; inputFile = skipBom(inputFileName, NULL);
;
	ldy     #$07
	jsr     pushwysp
	ldx     #$00
	txa
	jsr     _skipBom
	ldy     #$02
	jsr     staxysp
;
; if(inputFile == NULL) {
;
	cpx     #$00
	bne     L0B1B
	cmp     #$00
	bne     L0B1B
;
; fputs(TDB_COULDNT_OPEN_INPUT, stderr);
;
	lda     #<(l0b15)
	ldx     #>(l0b15)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; return -1;
;
	ldx     #$FF
	jmp     L1626
;
; columnCount++;
;
L0B19:	jsr     ldax0sp
	jsr     incax1
	jsr     stax0sp
;
; while(getCsvColumn(&inputFile, NULL, NULL, NULL, NULL, TRUE)) {
;
L0B1B:	lda     #$02
	jsr     leaa0sp
	jsr     pushax
	jsr     push0
	jsr     push0
	jsr     push0
	jsr     push0
	lda     #$01
	jsr     _getCsvColumn
	stx     tmp1
	ora     tmp1
	bne     L0B19
;
; fprintf(stdout, "%d", columnCount);
;
	lda     _stdout
	ldx     _stdout+1
	jsr     pushax
	lda     #<(l0b26)
	ldx     #>(l0b26)
	jsr     pushax
	ldy     #$07
	jsr     pushwysp
	ldy     #$06
	jsr     _fprintf
;
; fclose(inputFile);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     _fclose
;
; return 0;
;
	ldx     #$00
L1626:	txa
;
; }
;
	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct resultColumn * __near__ parse_expCommaList (__near__ struct qryData *, __near__ struct expression *, __near__ unsigned char *, int)
; ---------------------------------------------------------------


.endproc
