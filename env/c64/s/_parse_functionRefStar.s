.include "../code2.s"
.export _parse_functionRefStar
.proc	_parse_functionRefStar: near


;
; ) {
;
	jsr     pusheax
;
; if(queryData->parseMode != 1) {
;
	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L163C
	cmp     #$01
	beq     L0BE7
;
; return NULL;
;
L163C:	ldx     #$00
	txa
	jmp     L0BE6
;
; if(aggregationType != GRP_COUNT) {
;
L0BE7:	jsr     ldeax0sp
	jsr     pusheax
	ldx     #$00
	stx     sreg
	stx     sreg+1
	lda     #$05
	jsr     tosneeax
	beq     L0BEB
;
; fputs("only count(*) is valid",stderr);
;
	lda     #<(l0bee)
	ldx     #>(l0bee)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; exit(EXIT_FAILURE);
;
	ldx     #$00
	lda     #$01
	jsr     _exit
;
; queryData,
;
L0BEB:	ldy     #$07
	jsr     pushwysp
;
; GRP_STAR,
;
	ldx     #$00
	lda     #$07
	jsr     push0ax
;
; parse_scalarExpLiteral(queryData, ""),
;
	ldy     #$0D
	jsr     pushwysp
	lda     #<(l0bf8)
	ldx     #>(l0bf8)
	jsr     _parse_scalarExpLiteral
	jsr     pushax
;
; );
;
	ldx     #$00
	txa
	jsr     _parse_functionRef
;
; }
;
L0BE6:	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; int __near__ myfseek (__near__ struct _FILE *, long, int)
; ---------------------------------------------------------------


.endproc
