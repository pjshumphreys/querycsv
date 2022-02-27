#!/bin/sh
LANG='' sed 's/[^0123456789\n]//g' $1 > $1.txt
