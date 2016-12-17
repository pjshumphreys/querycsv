.include "../code2.s"
.export _tree_walkAndCleanup
.proc	_tree_walkAndCleanup: near


;
; ) {
;
	jsr     pushax
;
; int i = 0;
;
	jsr     decsp4
	jsr     push0
;
; if(root == NULL || *root == NULL) {
;
	ldy     #$08
	lda     (sp),y
	iny
	ora     (sp),y
	jeq     L0C12
	jsr     ldaxysp
	jsr     ldaxi
	cpx     #$00
	bne     L0C0A
	cmp     #$00
	jeq     L0C12
;
; currentResult = *root;
;
L0C0A:	ldy     #$09
L1646:	jsr     ldaxysp
	ldy     #$01
L1644:	jsr     ldaxidx
	ldy     #$04
	jsr     staxysp
;
; if(currentResult->left) {
;
L0C14:	ldy     #$05
	jsr     ldaxysp
	ldy     #$00
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0C15
;
; currentResult = currentResult->left;
;
	ldy     #$05
;
; continue;
;
	jmp     L1646
;
; if((currentResult->type) != TRE_FREED) {
;
L0C15:	ldy     #$05
	jsr     ldaxysp
	ldy     #$09
	jsr     ldaxidx
	cpx     #$00
	bne     L1640
	cmp     #$03
	beq     L0C19
;
; query,
;
L1640:	ldy     #$09
	jsr     pushwysp
	ldy     #$0F
	jsr     pushwysp
;
; currentResult->columns,
;
	ldy     #$09
	jsr     ldaxysp
	ldy     #$07
	jsr     pushwidx
;
; );
;
	ldy     #$07
	jsr     ldaxysp
	pha
	ldy     #$04
	lda     (sp),y
	sta     jmpvec+1
	iny
	lda     (sp),y
	sta     jmpvec+2
	pla
	jsr     jmpvec
	jsr     incsp2
;
; currentResult->type = TRE_FREED;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$03
	ldy     #$08
	sta     (ptr1),y
	iny
	lda     #$00
	sta     (ptr1),y
;
; i+=1;
;
	tax
	lda     #$01
	jsr     addeq0sp
;
; if(currentResult->right) {
;
L0C19:	ldy     #$05
	jsr     ldaxysp
	ldy     #$02
	sta     ptr1
	stx     ptr1+1
	lda     (ptr1),y
	iny
	ora     (ptr1),y
	beq     L0C25
;
; currentResult = currentResult->right;
;
	ldy     #$05
	jsr     ldaxysp
	ldy     #$03
;
; continue;
;
	jmp     L1644
;
; parentResult = currentResult->parent;
;
L0C25:	ldy     #$05
	jsr     ldaxysp
	ldy     #$05
	jsr     ldaxidx
	ldy     #$02
	jsr     staxysp
;
; if(parentResult != NULL) {
;
	cpx     #$00
	bne     L1641
	cmp     #$00
	beq     L0C2B
;
; if(parentResult->left == currentResult) {
;
L1641:	ldy     #$03
	jsr     ldaxysp
	jsr     ldaxi
	ldy     #$04
	cmp     (sp),y
	bne     L0C2E
	txa
	iny
	cmp     (sp),y
	bne     L0C2E
;
; currentResult = parentResult;
;
	ldy     #$03
	jsr     ldaxysp
	ldy     #$04
	jsr     staxysp
;
; free(currentResult->left);
;
	jsr     ldaxi
	jsr     _free
;
; currentResult->left = NULL;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; else {
;
	jmp     L0C14
;
; currentResult = parentResult;
;
L0C2E:	ldy     #$03
	jsr     ldaxysp
	ldy     #$04
	jsr     staxysp
;
; free(currentResult->right);
;
	ldy     #$03
	jsr     ldaxidx
	jsr     _free
;
; currentResult->right = NULL;
;
	ldy     #$05
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	ldy     #$02
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; else {
;
	jmp     L0C14
;
; free(currentResult);
;
L0C2B:	ldy     #$05
	jsr     ldaxysp
	jsr     _free
;
; *root = NULL;
;
	ldy     #$09
	jsr     ldaxysp
	sta     ptr1
	stx     ptr1+1
	lda     #$00
	tay
	sta     (ptr1),y
	iny
	sta     (ptr1),y
;
; }
;
L0C12:	ldy     #$0C
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ runQuery (__near__ unsigned char *)
; ---------------------------------------------------------------


.endproc
