.include "../code2.s"
.export _parse_atomCommaList
.proc	_parse_atomCommaList: near


;
; ) {
;
	jsr     pushax
;
; if(queryData->parseMode != 1) {
;
	jsr     decsp2
	ldy     #$07
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L1602
	cmp     #$01
	beq     L0A25
;
; return NULL;
;
L1602:	ldx     #$00
	txa
	jmp     L0A24
;
; reallocMsg((void**)(&newEntryPtr), sizeof(struct atomEntry));
;
L0A25:	lda     sp
	ldx     sp+1
	jsr     pushax
	ldx     #$00
	lda     #$06
	jsr     _reallocMsg
;
; if(lastEntryPtr == NULL) {
;
	ldy     #$04
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L0A2D
;
; newEntryPtr->index = 1;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	lda     #$01
	ldy     #$00
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; else {
;
	jmp     L160C
;
; newEntryPtr->index = lastEntryPtr->index+1;
;
L0A2D:	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$05
	jsr     ldaxysp
	jsr     ldaxi
	jsr     incax1
	ldy     #$00
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; newEntryPtr->nextInList = lastEntryPtr->nextInList;
;
	jsr     ldax0sp
	sta     sreg
	stx     sreg+1
	ldy     #$05
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$04
	sta     (sreg),y
	iny
	txa
	sta     (sreg),y
;
; lastEntryPtr->nextInList = newEntryPtr;
;
L160C:	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	jsr     ldax0sp
	ldy     #$04
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; newEntryPtr->content = newEntry;
;
	jsr     ldax0sp
	sta     ptr1
	stx     ptr1+1
	ldy     #$03
	jsr     ldaxysp
	ldy     #$02
	sta     (ptr1),y
	iny
	txa
	sta     (ptr1),y
;
; return newEntryPtr;
;
	jsr     ldax0sp
;
; }
;
L0A24:	jsr     incsp8
	jmp     farret


; ---------------------------------------------------------------
; void __near__ cleanup_query (__near__ struct qryData *)
; ---------------------------------------------------------------


.endproc
