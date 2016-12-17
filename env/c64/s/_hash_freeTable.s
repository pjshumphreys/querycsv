.include "../code2.s"
.export _hash_freeTable
.proc	_hash_freeTable: near


;
; void hash_freeTable(struct columnReferenceHash *hashtable) {
;
	jsr     pushax
;
; if (hashtable==NULL) {
;
	jsr     decsp6
	ldy     #$06
	lda     (sp),y
	iny
	ora     (sp),y
;
; return;
;
	jeq     L065C
;
; for(i=0; i<hashtable->size; i++) {
;
	ldx     #$00
	txa
L1598:	ldy     #$04
	jsr     staxysp
	jsr     pushax
	ldy     #$09
	jsr     ldaxysp
	jsr     ldaxi
	jsr     tosicmp
	bpl     L0661
;
; list = hashtable->table[i];
;
	ldy     #$07
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	sta     ptr1
	stx     ptr1+1
	ldy     #$05
	jsr     ldaxysp
	jsr     aslax1
	clc
	adc     ptr1
	pha
	txa
	adc     ptr1+1
	tax
	pla
	jsr     ldaxi
	ldy     #$02
	jsr     staxysp
;
; while(list!=NULL) {
;
	jmp     L066D
;
; temp = list;
;
L1596:	jsr     ldaxysp
	jsr     stax0sp
;
; list = list->nextReferenceInHash;
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$02
	jsr     staxysp
;
; free(temp->referenceName);
;
	jsr     ldax0sp
	jsr     ldaxi
	jsr     _free
;
; free(temp);
;
	jsr     ldax0sp
	jsr     _free
;
; while(list!=NULL) {
;
L066D:	ldy     #$02
	lda     (sp),y
	iny
	ora     (sp),y
	bne     L1596
;
; for(i=0; i<hashtable->size; i++) {
;
	ldy     #$05
	jsr     ldaxysp
	jsr     incax1
	jmp     L1598
;
; free(hashtable->table);
;
L0661:	ldy     #$07
	jsr     ldaxysp
	ldy     #$03
	jsr     ldaxidx
	jsr     _free
;
; free(hashtable);
;
	ldy     #$07
	jsr     ldaxysp
	jsr     _free
;
; }
;
L065C:	jsr     incsp8
	jmp     farret


; ---------------------------------------------------------------
; __near__ struct expression * __near__ parse_scalarExp (__near__ struct qryData *, __near__ struct expression *, int, __near__ struct expression *)
; ---------------------------------------------------------------


.endproc
