#!/bin/sh

# Ce script est appelle par le makefile trunk/vasy/vasymake
# Il a besoin que la variable d'environnement $sx pointe sur le dossier de syntax
# Il fait une copie de ce dossier sur une machine distante qui supporte l'architecture x64

EXCLUSIONS=./exclusions

cat > $EXCLUSIONS << \EOF
oldies/
bin.sun5/
lib.sun5/
bin.sun5.gcc/
lib.sun5.gcc/
bin.sun64/
lib.sun64/
bin.sun64.gcc/
lib.sun64.gcc/
bin.sol86/
lib.sol86/
bin.sol86.gcc/
lib.sol86.gcc/
bin.sol64/
lib.sol64/
bin.sol64.gcc/
lib.sol64.gcc/
bin.iX86/
lib.iX86/
bin.iX86.icc/
lib.iX86.icc/
bin.ia64/
lib.ia64/
bin.win32/
lib.win32/
bin.macOS/
lib.macOS/
bin.mac86/
lib.mac86/
bin.mac64/
lib.mac64/
doc/
syntax-core/
EOF

RSYNC="/opt/sfw/bin/rsync --recursive --links --times --verbose --update --delete --exclude-from=$EXCLUSIONS"

umask 022

SOURCE=$sx
DESTINATION=rlacroix@massif:tmp/syntax-bootstrap-x64/

$RSYNC $SOURCE/ 			$DESTINATION

rm -f $EXCLUSIONS
