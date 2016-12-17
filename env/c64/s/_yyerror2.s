.include "../code2.s"
.export _yyerror2
.proc	_yyerror2: near


;
; void yyerror2(long lineno, char *text) {
;
	jsr     pushax
;
; fprintf(stderr, TDB_LEX_UNTERMINATED);
;
	lda     _stderr
	ldx     _stderr+1
	jsr     pushax
	lda     #<(l147c)
	ldx     #>(l147c)
	jsr     pushax
	ldy     #$09
	jsr     ldeaxysp
	jsr     pusheax
	ldy     #$0B
	jsr     pushwysp
	ldy     #$0A
	jsr     _fprintf
;
; }
;
	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; void __near__ cleanup_orderByClause (__near__ struct sortingList *)
; ---------------------------------------------------------------


.endproc
