#!/bin/sh

EXCLUSIONS=./exclusions

cat > $EXCLUSIONS << \EOF
oldies/
bin.win32/
lib.win32/
bin.sun5/
lib.sun5/
bin.sun5.gcc/
lib.sun5.gcc/
bin.iX86/
lib.iX86/
bin.iX86.icc/
lib.iX86.icc/
bin.macOS/
lib.macOS/
bin.mac86/
lib.mac86/
bin.sun64/
lib.sun64/
bin.sun64.gcc/
lib.sun64.gcc/
bin.ia64/
lib.ia64/
doc/
syntax-core/
EOF

RSYNC="/opt/sfw/bin/rsync --recursive --links --times --verbose --update --delete --exclude-from=$EXCLUSIONS"

umask 022

SOURCE=$sx
DESTINATION=rlacroix@massif:tmp/syntax-bootstrap-x64/

$RSYNC $SOURCE/ 			$DESTINATION

rm -f $EXCLUSIONS