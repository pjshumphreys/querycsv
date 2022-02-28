.include "../../header.inc"
.include "../labels2.s"
.segment "CODE"
.export __updateSpinner
.proc __updateSpinner: near
  tya
  pha
  lda _spinnerEnabled
  jeq l0083wait

  lda _cursorOutput
  jeq l0090wait

  lda #$9d
  jsr $ffd2

l0085wait:
	lda _currentWaitCursorChar
  ldx _currentWaitCursorChar+1
  ldy #$00

  sta ptr1
  stx ptr1+1
  lda (ptr1),y

  sta _currentWaitCursor
  jeq l0090wait
  jmp l008cwait

l0090wait:
	lda #<_spinner
  ldx #>_spinner
  sta _currentWaitCursorChar
  stx _currentWaitCursorChar+1
  jmp l0085wait

l008cwait:
	jsr $ffd2
  inc _currentWaitCursorChar
  bne l0099wait
  inc _currentWaitCursorChar+1

l0099wait:
  lda _currentWaitCursorChar
  ldx _currentWaitCursorChar+1
  lda #$01
  sta _cursorOutput

l0083wait:
	pla
	tay
	rts
.endproc

_spinner:
	.asciiz "...OOooOO" ; because upper case and lower case are swapped in petscii
