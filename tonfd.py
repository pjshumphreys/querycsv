#!/usr/bin/env python
import sys
import codecs
import unicodedata
(utf8_encode, utf8_decode, utf8_reader, utf8_writer) = codecs.lookup('utf-8')
outfile = utf8_writer(sys.stdout)
infile=utf8_reader(sys.stdin)
outfile.write(unicodedata.normalize('NFD',infile.read()))
sys.exit(0)
