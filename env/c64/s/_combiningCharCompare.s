.include "../code2.s"
.export _combiningCharCompare
.proc	_combiningCharCompare: near


;
; int combiningCharCompare(const void *a, const void *b) {
;
	jsr     pushax
;
; return (*((long*)a) < *((long*)b)) ? -1 : (*((long*)a) != *((long*)b) ? 1 : 0);
;
	ldy     #$03
	jsr     ldaxysp
	jsr     ldeaxi
	jsr     pusheax
	ldy     #$05
	jsr     ldaxysp
	jsr     ldeaxi
	jsr     toslteax
	beq     L0467
	ldx     #$FF
	jmp     L1547
L0467:	ldy     #$03
	jsr     ldaxysp
	jsr     ldeaxi
	jsr     pusheax
	ldy     #$05
	jsr     ldaxysp
	jsr     ldeaxi
	jsr     tosneeax
	beq     L046E
	ldx     #$00
	lda     #$01
	jmp     L0462
L046E:	ldx     #$00
L1547:	txa
;
; }
;
L0462:	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; void __near__ getCalculatedColumns (__near__ struct qryData *, __near__ struct resultColumnValue *, int)
; ---------------------------------------------------------------


.endproc
