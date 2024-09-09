
include $(SX)/etc/make/gccoptions

all: udag_t.c

# debut de code produit par "make-makefile udag"

udag_t.c: udag.bt udag.st udag.pt udag.rt 
	tables_c udag > $@

udag.bt  : udag.bnf
	semact udag.bnf 

udag.st: udag.bt udag.lecl
	lecl udag.lecl || [ $$? = 1 ]

udag.pt: udag.bt
	csynt udag || [ $$? = 1 ]

udag.rt: udag.bt udag.st udag.recor
	recor udag.recor

# fin de code produit par "make-makefile udag"

clean:
	/bin/rm -f udag.*.l
	/bin/rm -f udag.bt udag.pt udag.rt udag.st
	/bin/rm -f udag_t.c

