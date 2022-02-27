spinner:
  defb "...ooOOoo"
spinnerEnd:

setSpinner:
  ld a, l
  ld (spinnerEnabled), a
  or a  ; cp 0
  jr nz, skipBackspace
printBackspace:
  ld a, (cursorOutput)
  or a  ; cp 0
  jr z, skipBackspace
  xor a ;ld a, 0
  ld (cursorOutput), a
  ld hl, 8  ; backspace
  push hl
  call fputc_cons
  pop hl
skipBackspace:
  ret

updateSpinner:
  ld a, (spinnerEnabled)
  or a  ; cp 0
  jr z, skipBackspace
  ld a, (cursorOutput)
  or a  ; cp 0
  jr z, skipBackspace2
  ld hl, 8  ; backspace
  push hl
  call fputc_cons
  pop hl
skipBackspace2:
  ld hl, (currentWaitCursor)
  ld de, spinnerEnd
  xor a
  sbc hl, de
  add hl, de
  jp nz, skipReset
  ld hl, spinner
skipReset:
  inc hl
  ld (currentWaitCursor), hl
  dec hl
  ld l, (hl)
  ld h, 0
  push hl
  call fputc_cons
  pop hl
  ld a, 1
  ld (cursorOutput), a
  ret

spinnerEnabled:
  defb 1

cursorOutput:
  defb 0

currentWaitCursor:
  defw spinner
