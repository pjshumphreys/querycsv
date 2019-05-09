#!/bin/bash
find `dirname "$0"` -path `dirname "$0"`/env/riscos -prune -o -name "Makefile" -exec bash -c 'expand --initial --tabs=2 "$0" > /tmp/e && mv /tmp/e "$0"' {} \;
find `dirname "$0"` -path `dirname "$0"`/env/riscos -prune -o -name "Makefile" -exec bash -c 'expand --tabs=1 "$0" > /tmp/e && mv /tmp/e "$0"' {} \;
find `dirname "$0"` -path `dirname "$0"`/env/riscos -prune -o -name "Makefile" -exec bash -c 'unexpand --first-only --tabs=2 "$0" > /tmp/e && mv /tmp/e "$0"' {} \;

#-print0 | xargs -0 unexpand -t2
