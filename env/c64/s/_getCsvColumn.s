.include "../code2.s"
.export _getCsvColumn
.proc	_getCsvColumn: near


;
; ) {
;
	jsr     pushax
;
; char *tempString = NULL;
;
	jsr     decsp2
	jsr     push0
;
; int canEnd = TRUE;
;
	lda     #$01
	jsr     pusha0
;
; int quotePossible = TRUE;
;
	jsr     pusha0
;
; int exitCode = 0;
;
	jsr     push0
;
; char *minSize = NULL;
;
	jsr     push0
;
; long offset = 0;
;
	jsr     pushl0
;
; if(quotedValue != NULL) {
;
	ldy     #$14
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0509
;
; *quotedValue = FALSE;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; if(strSize != NULL) {
;
L0509:	ldy     #$16
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L050E
;
; *strSize = 0;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; if(value == NULL) {
;
L050E:	ldy     #$18
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0513
;
; value = &tempString;
;
	lda     #$0C
	jsr     leaa0sp
	dey
	jsr     staxysp
;
; if(startPosition != NULL) {
;
L0513:	ldy     #$12
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0518
;
; offset = *startPosition;
;
	jsr     ldaxysp
	jsr     ldeaxi
	jsr     steax0sp
;
; if(feof(*inputFile) != 0) {
;
L0518:	ldy     #$1B
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _feof
	cpx     #$00
	bne     L1561
	cmp     #$00
	beq     L051D
;
; return FALSE;
;
L1561:	ldx     #$00
	txa
	jmp     L0502
;
; offset++;
;
L051D:	jsr     ldeax0sp
	jsr     saveeax
	ldy     #$01
	jsr     inceaxy
	jsr     steax0sp
	jsr     resteax
;
; c = fgetc(*inputFile);
;
	ldy     #$1B
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _fgetc
	ldy     #$0E
	jsr     staxysp
;
; }
;
	cpx     #$00
	bne     L05BE
	cmp     #$00
	jeq     L0564
	cmp     #$0A
	beq     L053A
	cmp     #$0D
	jeq     L054A
	cmp     #$20
	beq     L052D
	cmp     #$22
	jeq     L1559
	cmp     #$2C
	jeq     L05A9
	jmp     L05AA
L05BE:	cpx     #$FF
	jne     L05AA
	cmp     #$FF
	jeq     L056B
	jmp     L05AA
;
; if(!canEnd) {
;
L052D:	ldy     #$0B
	lda     (sp),y
	dey
	ora     (sp),y
	bne     L052E
;
; minSize = &((*value)[*strSize]);
;
	ldy     #$19
	jsr     ldaxysp
	jsr     ldaxi
	sta     sreg
	stx     sreg+1
	ldy     #$17
	jsr     ldaxysp
	jsr     ldaxi
	clc
	adc     sreg
	pha
	txa
	adc     sreg+1
	tax
	pla
	ldy     #$04
	jsr     staxysp
;
; strAppend(' ', value, strSize);
;
L052E:	lda     #$20
;
; break;
;
	jmp     L1569
;
; offset++;
;
L053A:	jsr     ldeax0sp
	jsr     saveeax
	ldy     #$01
	jsr     inceaxy
	jsr     steax0sp
	jsr     resteax
;
; if((c = fgetc(*inputFile)) != '\n' && c != EOF) {
;
	ldy     #$1B
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _fgetc
	ldy     #$0E
	jsr     staxysp
	cpx     #$00
	bne     L1562
	cmp     #$0D
	beq     L053C
L1562:	ldy     #$0F
	lda     (sp),y
	cmp     #$FF
	bne     L053D
	dey
	lda     (sp),y
	cmp     #$FF
	beq     L053C
;
; offset--;
;
L053D:	jsr     ldeax0sp
	jsr     saveeax
	ldy     #$01
	jsr     deceaxy
	jsr     steax0sp
	jsr     resteax
;
; ungetc(c, *inputFile);
;
	ldy     #$11
	jsr     pushwysp
	ldy     #$1D
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _ungetc
;
; else if (c == EOF) {
;
	jmp     L054A
L053C:	ldy     #$0F
	lda     (sp),y
	cmp     #$FF
	bne     L054A
	dey
	lda     (sp),y
	cmp     #$FF
	bne     L054A
;
; exitCode = 2;
;
	ldx     #$00
	lda     #$02
	ldy     #$06
	jsr     staxysp
;
; break;
;
	jmp     L052B
;
; if(canEnd) {
;
L054A:	ldy     #$0B
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L0551
;
; exitCode = 2;
;
	ldx     #$00
	lda     #$02
	ldy     #$06
	jsr     staxysp
;
; break;
;
	jmp     L052B
;
; if (quotedValue != NULL) {
;
L0551:	ldy     #$14
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0556
;
; *quotedValue = TRUE;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; strAppend('\r', value, strSize);
;
L0556:	lda     #$0A
	jsr     pusha
	ldy     #$1C
	jsr     pushwysp
	ldy     #$1A
	jsr     ldaxysp
	jsr     _strAppend
;
; strAppend('\n', value, strSize);
;
	lda     #$0D
;
; break;
;
	jmp     L1569
;
; if (quotedValue != NULL) {
;
L0564:	ldy     #$14
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L052B
;
; *quotedValue = TRUE;
;
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	lda     #$01
	ldy     #$00
	sta     (sreg),y
	iny
	lda     #$00
	sta     (sreg),y
;
; break;
;
	jmp     L052B
;
; exitCode = 2;
;
L056B:	inx
	lda     #$02
	ldy     #$06
	jsr     staxysp
;
; break;
;
	jmp     L052B
;
; canEnd = FALSE;
;
L1559:	txa
	ldy     #$0A
	jsr     staxysp
;
; if (quotedValue != NULL) {
;
	ldy     #$14
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L0572
;
; *quotedValue = TRUE;
;
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	ldx     #$00
	lda     #$01
	ldy     #$00
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; if(quotePossible) {
;
L0572:	ldy     #$09
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L0577
;
; if(strSize != NULL) {
;
	ldy     #$16
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L155B
;
; *strSize = 0;
;
	jsr     ldaxysp
	sta     sreg
	stx     sreg+1
	lda     #$00
	tay
	sta     (sreg),y
	iny
	sta     (sreg),y
;
; quotePossible = FALSE;
;
	tax
L155B:	ldy     #$08
	jsr     staxysp
;
; else {
;
	jmp     L052B
;
; offset++;
;
L0577:	jsr     ldeax0sp
	jsr     saveeax
	ldy     #$01
	jsr     inceaxy
	jsr     steax0sp
	jsr     resteax
;
; c = fgetc(*inputFile);
;
	ldy     #$1B
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _fgetc
	ldy     #$0E
	jsr     staxysp
;
; }
;
	cpx     #$00
	bne     L05A7
	cmp     #$0A
	beq     L155C
	cmp     #$0D
	beq     L155C
	cmp     #$20
	beq     L155C
	cmp     #$22
	jeq     L1569
	cmp     #$2C
	beq     L155C
	jmp     L059E
L05A7:	cpx     #$FF
	bne     L059E
	cmp     #$FF
	bne     L059E
;
; canEnd = TRUE;
;
	inx
L155C:	lda     #$01
	ldy     #$0A
	jsr     staxysp
;
; offset--;
;
	jsr     ldeax0sp
	jsr     saveeax
	ldy     #$01
	jsr     deceaxy
	jsr     steax0sp
	jsr     resteax
;
; ungetc(c, *inputFile);
;
	ldy     #$11
	jsr     pushwysp
	ldy     #$1D
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _ungetc
;
; break;
;
	jmp     L052B
;
; strAppend('"', value, strSize);
;
L059E:	lda     #$22
	jsr     pusha
	ldy     #$1C
	jsr     pushwysp
	ldy     #$1A
	jsr     ldaxysp
	jsr     _strAppend
;
; offset--;
;
	jsr     ldeax0sp
	jsr     saveeax
	ldy     #$01
	jsr     deceaxy
	jsr     steax0sp
	jsr     resteax
;
; ungetc(c, *inputFile);
;
	ldy     #$11
	jsr     pushwysp
	ldy     #$1D
	jsr     ldaxysp
	jsr     ldaxi
	jsr     _ungetc
;
; break;
;
	jmp     L052B
;
; if(canEnd) {
;
L05A9:	ldy     #$0B
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L05AA
;
; exitCode = 1;
;
	lda     #$01
	ldy     #$06
	jsr     staxysp
;
; break;
;
	jmp     L052B
;
; if(doTrim && quotePossible) {
;
L05AA:	ldy     #$11
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L1564
	ldy     #$09
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L1564
	ldx     #$00
;
; if(strSize != NULL) {
;
	ldy     #$16
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L155F
;
; *strSize = 0;
;
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; quotePossible = FALSE;
;
L1564:	tax
L155F:	ldy     #$08
	jsr     staxysp
;
; strAppend(c, value, strSize);
;
	ldy     #$0E
	lda     (sp),y
L1569:	jsr     pusha
	ldy     #$1C
	jsr     pushwysp
	ldy     #$1A
	jsr     ldaxysp
	jsr     _strAppend
;
; } while (exitCode == 0);
;
L052B:	ldy     #$06
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L051D
;
; if(doTrim) {
;
	ldy     #$11
	lda     (sp),y
	dey
	ora     (sp),y
	beq     L1560
;
; strRTrim(value, strSize, minSize);
;
	ldy     #$1B
	jsr     pushwysp
	ldy     #$1B
	jsr     pushwysp
	ldy     #$09
	jsr     ldaxysp
	jsr     _strRTrim
;
; strAppend('\0', value, strSize);
;
	lda     #$00
L1560:	jsr     pusha
	ldy     #$1C
	jsr     pushwysp
	ldy     #$1A
	jsr     ldaxysp
	jsr     _strAppend
;
; if(strSize != NULL) {
;
	ldy     #$16
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L05CB
;
; (*strSize)--;
;
	jsr     ldaxysp
	jsr     pushax
	jsr     ldaxi
	sta     regsave
	stx     regsave+1
	jsr     decax1
	ldy     #$00
	jsr     staxspidx
;
; if(startPosition != NULL) {
;
L05CB:	ldy     #$12
	lda     (sp),y
	iny
	ora     (sp),y
	beq     L05D0
;
; *startPosition = offset;
;
	ldy     #$15
	jsr     pushwysp
	ldy     #$05
	jsr     ldeaxysp
	ldy     #$00
	jsr     steaxspidx
;
; free(tempString);
;
L05D0:	ldy     #$0D
	jsr     ldaxysp
	jsr     _free
;
; return exitCode == 1;
;
	ldy     #$07
	jsr     ldaxysp
	cpx     #$00
	bne     L05D8
	cmp     #$01
L05D8:	jsr     booleq
;
; }
;
L0502:	ldy     #$1C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; void __near__ parse_whereClause (__near__ struct qryData *, __near__ struct expression *)
; ---------------------------------------------------------------


.endproc
