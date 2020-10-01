.include "../../header.inc"
.include "../labels2.s"
.segment "CODE"
.export __updateSpinner
.proc	__updateSpinner: near


;
; if(spinnerEnabled) {
;
	lda     _spinnerEnabled
	jeq     l0083wait
;
; if(cursorOutput) {
;
	lda     _cursorOutput
	jeq     l0085wait
;
; __asm__ ("lda #8");
;
	lda     #8
;
; __asm__ ("jsr $ffd2");
;
	jsr     $ffd2
;
; currentWaitCursor = *currentWaitCursorChar;
;
l0085wait:	lda     _currentWaitCursorChar
	ldx     _currentWaitCursorChar+1
	ldy     #$00
	;jsr     ldauidx
	sta     ptr1
	stx     ptr1+1
	ldx     #0
	lda     (ptr1),y
	
	sta     _currentWaitCursor
;
; if(currentWaitCursor) {
;
	lda     _currentWaitCursor
	jeq     l0090wait
;
; break;
;
	jmp     l008cwait
;
; currentWaitCursorChar = spinner;
;
l0090wait:	lda     _spinner
	ldx     _spinner+1
	sta     _currentWaitCursorChar
	stx     _currentWaitCursorChar+1
;
; } while(1);
;
	jmp     l0085wait
;
; __asm__ ("jsr $ffd2");
;
l008cwait:	jsr     $ffd2
;
; currentWaitCursorChar+=1;
;
	inc     _currentWaitCursorChar
	bne     l0099wait
	inc     _currentWaitCursorChar+1
l0099wait:	lda     _currentWaitCursorChar
	ldx     _currentWaitCursorChar+1
;
; cursorOutput = 1;
;
	ldx     #$00
	lda     #$01
	sta     _cursorOutput
;
; }
;
l0083wait:	rts
.endproc
