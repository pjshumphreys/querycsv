.include "../code2.s"
.export _getNextRecordOffset
.proc	_getNextRecordOffset: near


;
; int getNextRecordOffset(char *inputFileName, long offset) {
;
	jsr     pusheax
;
; FILE *inputFile = NULL;
;
	jsr     push0
;
; inputFile = skipBom(inputFileName, NULL);
;
	ldy     #$09
	jsr     pushwysp
	ldx     #$00
	txa
	jsr     _skipBom
	jsr     stax0sp
;
; if(inputFile == NULL) {
;
	cpx     #$00
	bne     L0D2E
	cmp     #$00
	bne     L0D2E
;
; fputs(TDB_COULDNT_OPEN_INPUT, stderr);
;
	lda     #<(l0d32)
	ldx     #>(l0d32)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; return -1;
;
	ldx     #$FF
	jmp     L1652
;
; if(myfseek(inputFile, offset, SEEK_SET) != 0) {
;
L0D2E:	jsr     pushw0sp
	ldy     #$07
	jsr     ldeaxysp
	jsr     pusheax
	ldx     #$00
	lda     #$02
	jsr     _myfseek
	cpx     #$00
	bne     L1651
	cmp     #$00
	beq     L0D43
;
; fputs(TDB_COULDNT_SEEK, stderr);
;
L1651:	lda     #<(l0d3d)
	ldx     #>(l0d3d)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; return -1;
;
	ldx     #$FF
	jmp     L1652
;
; while(getCsvColumn(&inputFile, NULL, NULL, NULL, &offset, TRUE)) {
;
L0D43:	lda     sp
	ldx     sp+1
	jsr     pushax
	jsr     push0
	jsr     push0
	jsr     push0
	lda     #$0A
	jsr     leaa0sp
	jsr     pushax
	ldx     #$00
	lda     #$01
	jsr     _getCsvColumn
	stx     tmp1
	ora     tmp1
	bne     L0D43
;
; fprintf(stdout, "%ld", offset);
;
	lda     _stdout
	ldx     _stdout+1
	jsr     pushax
	lda     #<(l0d4d)
	ldx     #>(l0d4d)
	jsr     pushax
	ldy     #$09
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$08
	jsr     _fprintf
;
; fclose(inputFile);
;
	jsr     ldax0sp
	jsr     _fclose
;
; return 0;
;
	ldx     #$00
L1652:	txa
;
; }
;
	jsr     incsp8
	jmp     farret


; ---------------------------------------------------------------
; int __near__ getMatchingRecord (__near__ struct qryData *, __near__ struct resultColumnValue *)
; ---------------------------------------------------------------


.endproc
