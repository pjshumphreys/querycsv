# SoftFloat

This repo combines Yasuhiro Matsumoto's implementation of the strtod function (yes the Ruby guy, here's a link to his github gist: https://gist.github.com/mattn/1890186), a collaboratively developed implementation of the dtoa function that was produced on stack overflow (https://stackoverflow.com/questions/2302969/convert-a-float-to-a-string/2303011#2303011), the log10 function taken from Sun microsystem's fdlibm, and a simple implementation of the pow10 function written by myself, all of which have been modified to work with John Hauser's Berkeley SoftFloat floating point emulation package and a set of macros inspired by cc65-floatlib.

I've also improved the precision of the dtoa function somewhat.

FYI, strtod converts from ascii strings to double precision floating point numbers, and the dtoa function converts from a double precision floating point number to a string.

I've done this a I plan to produce a binary for the ARM evaluation system (AES) addon for the BBC Micro. I have version 2 of the Norcroft C compiler (taken from the MAME emulator project) with which I can produce working C executables for this system, but code that uses floating point numbers relies on the FPA floating point co-processor instruction set, which the beebem emulator and the pitubedirect client don't implement. What this means is that as soon as any compiled executables run any floating point code they crash. There was a floating point emulator produced for this co-processor back in the 1980s, but it only works for Norcroft C 2 binaries running on early versions of Risc OS and not it's immediate predecessor (AES).

My solution to these problems is this codebase, which can support the functionality I need with my own software emulation.

I haven't included any trig functions here as I won't be needing them myself, but suitable code can be again sourced from Sun's fdlibm library if necessary. 

The latest version can be found at https://github.com/pjshumphreys/softfloat
