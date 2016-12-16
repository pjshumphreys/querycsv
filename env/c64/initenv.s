.export initenv
.export __RAM2_TOTAL__
.import __RAM2_LAST__,__RAM2_START__
__RAM2_TOTAL__ = __RAM2_LAST__ - __RAM2_START__

.segment "DATA"
.incbin "floatlibdata2.bin"

.segment "INIT"
.incbin "floatlib.bin"

.segment "CODE"

initenv:
      lda #0
      tax
      rts
