.include "../code2.s"
.export _getCurrentDate
.proc	_getCurrentDate: near


;
; char *output = NULL;
;
	ldy     #$16
	jsr     subysp
	jsr     push0
;
; time(&now);
;
	lda     #$14
	jsr     leaa0sp
	jsr     _time
;
; if(d_tztime(&now, &local, NULL, &output) == FALSE) {
;
	lda     #$14
	jsr     leaa0sp
	jsr     pushax
	lda     #$04
	jsr     leaa0sp
	jsr     pushax
	jsr     push0
	lda     #$06
	jsr     leaa0sp
	jsr     _d_tztime
	cpx     #$00
	bne     L0F44
	cmp     #$00
	bne     L0F44
;
; fputs(TDB_TZTIMED_FAILED, stderr);
;
	lda     #<(l0f4c)
	ldx     #>(l0f4c)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; freeAndZero(output);
;
	jsr     ldax0sp
	jsr     _free
	ldx     #$00
	txa
	jsr     stax0sp
;
; return -1;
;
	dex
	txa
	jmp     L0F40
;
; if(d_sprintf(&output, "%%Y-%%m-%%dT%%H:%%M:%%S%s", output) == FALSE) {
;
L0F44:	lda     sp
	ldx     sp+1
	jsr     pushax
	lda     #<(l0f57)
	ldx     #>(l0f57)
	jsr     pushax
	ldy     #$07
	jsr     pushwysp
	ldy     #$06
	jsr     _d_sprintf
	cpx     #$00
	bne     L0F54
	cmp     #$00
	bne     L0F54
;
; fputs(TDB_SPRINTFD_FAILED, stderr);
;
	lda     #<(l0f5c)
	ldx     #>(l0f5c)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; freeAndZero(output);
;
	jsr     ldax0sp
	jsr     _free
	ldx     #$00
	txa
	jsr     stax0sp
;
; return -1;
;
	dex
	txa
	jmp     L0F40
;
; if(d_strftime(&output, output, &local) == FALSE) {
;
L0F54:	lda     sp
	ldx     sp+1
	jsr     pushax
	ldy     #$05
	jsr     pushwysp
	lda     #$06
	jsr     leaa0sp
	jsr     _d_strftime
	cpx     #$00
	bne     L0F64
	cmp     #$00
	bne     L0F64
;
; fputs(TDB_STRFTIMED_FAILED, stderr);
;
	lda     #<(l0f6b)
	ldx     #>(l0f6b)
	jsr     pushax
	lda     _stderr
	ldx     _stderr+1
	jsr     _fputs
;
; freeAndZero(output);
;
	jsr     ldax0sp
	jsr     _free
	ldx     #$00
	txa
	jsr     stax0sp
;
; return -1;
;
	dex
	txa
	jmp     L0F40
;
; fputs(output, stdout);
;
L0F64:	jsr     pushw0sp
	lda     _stdout
	ldx     _stdout+1
	jsr     _fputs
;
; freeAndZero(output);
;
	jsr     ldax0sp
	jsr     _free
	ldx     #$00
	txa
	jsr     stax0sp
;
; }
;
L0F40:	ldy     #$18
	jsr     addysp
	jmp     farret


; ---------------------------------------------------------------
; int __near__ needsEscaping (__near__ unsigned char *, int)
; ---------------------------------------------------------------


.endproc
