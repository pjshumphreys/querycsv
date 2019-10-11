|
|    Apple Macintosh Developer Technical Support
|
|    MultiFinder-Aware TextEdit Sample Application
|
|    TESample
|
|    TESampleGlue.a    -    Assembler Source
|
|    Copyright � 1989 Apple Computer, Inc.
|    All rights reserved.
|
|    Versions:
|                1.00                08/88
|                1.01                11/88
|                1.02                04/89
|                1.03                06/89
|
|    Components:
|                TESample.c            Feb.  1, 1990
|                TESampleGlue.a        Feb.  1, 1990
|                TESample.r            Feb.  1, 1990
|                TESample.h            Feb.  1, 1990
|                TESample.make        Feb.  1, 1990
|
|    TESample is an example application that demonstrates how
|    to initialize the commonly used toolbox managers, operate
|    successfully under MultiFinder, handle desk accessories and
|    create, grow, and zoom windows. The fundamental TextEdit
|    toolbox calls and TextEdit autoscroll are demonstrated. It
|    also shows how to create and maintain scrollbar controls.
|
|    It does not by any means demonstrate all the techniques you
|    need for a large application. In particular, Sample does not
|    cover exception handling, multiple windows/documents,
|    sophisticated memory management, printing, or undo. All of
|    these are vital parts of a normal full-sized application.
|
|    This application is an example of the form of a Macintosh
|    application; it is NOT a template. It is NOT intended to be
|    used as a foundation for the next world-class, best-selling,
|    600K application. A stick figure drawing of the human body may
|    be a good example of the form for a painting, but that does not
|    mean it should be used as the basis for the next Mona Lisa.
|
|    We recommend that you review this program or Sample before
|    beginning a new application. Sample is a simple app. which doesn�t
|    use TextEdit or the Control Manager.
|
|
|    AsmClikLoop
|
|    This routine gets called by the TextEdit Manager from TEClick.
|    It calls the old, default click loop routine that scrolls the
|    text, and then calls our own Pascal routine that handles
|    tracking the scroll bars to follow along.  It doesn't bother
|    with saving registers A0 and D0, because they are trashed
|    anyway by TextEdit.
|


.globl   ASMCLIKLOOP    | This symbol will be externally visible
ASMCLIKLOOP:
  movem.l %d1-%d2/%a1, -(%sp)    | D0 and A0 need not be saved
  clr.l -(%sp)                | make space for procedure pointer
  jsr GETOLDCLIKLOOP        | get the old clikLoop...
  movea.l (%sp)+, %a0          | ...into A0
  movem.l (%sp)+, %d1-%d2/%a1    | restore the world as it was...

  jsr (%a0)                   | ...and execute old clikLoop

  movem.l %d1-%d2/%a1, -(%sp)    | D0 and A0 need not be saved
  jsr PASCALCLIKLOOP        | do our clikLoop
  movem.l (%sp)+, %d1-%d2/%a1    | restore the world as it was
  moveq #1, %d0               | clear the zero flag so TextEdit keeps going
  rts
