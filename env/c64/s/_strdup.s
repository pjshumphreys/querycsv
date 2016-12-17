.include "../code2.s"
.export _strdup
.proc	_strdup: near


;
; char* __fastcall__ strdup(const char* s) {
;
	jsr     pushax
;
; char *res = NULL;
;
	jsr     push0
;
; reallocMsg((void**)&res, strlen(s) + 1);
;
	lda     sp
	ldx     sp+1
	jsr     pushax
	ldy     #$05
	jsr     ldaxysp
	jsr     _strlen
	jsr     incax1
	jsr     _reallocMsg
;
; strcpy(res, s);
;
	jsr     pushw0sp
	ldy     #$05
	jsr     ldaxysp
	jsr     _strcpy
;
; return res;
;
	jsr     ldax0sp
;
; }
;
	jsr     incsp4
	jmp     farret


; ---------------------------------------------------------------
; void __near__ getValue (__near__ struct expression *, __near__ struct resultColumnParam *)
; ---------------------------------------------------------------


.endproc
