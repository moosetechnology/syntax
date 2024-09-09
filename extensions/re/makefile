
include $(SX)/etc/make/gccoptions

all: re_t.c

# debut de code produit par "make-makefile re"

re_t.c: re.bt re.st re.pt re.rt re.att
	tables_c re > $@

re.bt re.att re_mix.c: re.at
	semat re.at > re_mix.c

re.st: re.bt re.lecl
	lecl re.lecl || [ $$? = 1 ]

re.pt: re.bt
	csynt re || [ $$? = 1 ]

re.rt: re.bt re.st re.recor
	recor re.recor

# fin de code produit par "make-makefile re"

clean:
	/bin/rm -f re.*.l
	/bin/rm -f re.att re.bt re.pt re.rt re.st
	/bin/rm -f re_mix.c re_t.c

