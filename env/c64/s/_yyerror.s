.include "../code2.s"
.export _yyerror
.proc	_yyerror: near


;
; int yyerror(struct qryData *queryData, void *scanner, const char *msg) {
;
	jsr     pushax
;
; fputs(msg, stderr);
;
	jsr     pushw0sp
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; return 1;
;
	ldx     #$00
	lda     #$01
;
; }
;
	jsr     incsp6
	jmp     farret


; ---------------------------------------------------------------
; void __near__ readQuery (__near__ unsigned char *, __near__ struct qryData *)
; ---------------------------------------------------------------


.endproc
