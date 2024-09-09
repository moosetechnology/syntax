
include $(SX)/etc/make/gccoptions

all: sdag_t.c

# debut de code produit par "make-makefile sdag"

sdag_t.c: sdag.bt sdag.st sdag.pt sdag.rt 
	tables_c sdag > $@

sdag.bt  : sdag.bnf
	semact sdag.bnf 

sdag.st: sdag.bt sdag.lecl
	lecl sdag.lecl || [ $$? = 1 ]

sdag.pt: sdag.bt
	csynt sdag || [ $$? = 1 ]

sdag.rt: sdag.bt sdag.st sdag.recor
	recor sdag.recor

# fin de code produit par "make-makefile sdag"

clean:
	/bin/rm -f sdag.*.l
	/bin/rm -f sdag.bt sdag.pt sdag.rt sdag.st
	/bin/rm -f sdag_t.c

