
include $(SX)/etc/make/gccoptions

all: dag_t.c

# debut de code produit par "make-makefile dag"

dag_t.c: dag.bt dag.st dag.pt dag.rt dag.att
	tables_c dag > $@

dag.bt dag.att dag_mix.c: dag.at
	semat dag.at  > dag_mix.c

dag.st: dag.bt dag.lecl
	lecl dag.lecl || [ $$? = 1 ]

dag.pt: dag.bt
	csynt dag || [ $$? = 1 ]

dag.rt: dag.bt dag.st dag.recor
	recor dag.recor

# fin de code produit par "make-makefile dag"

clean:
	/bin/rm -f dag.*.l
	/bin/rm -f dag.att dag.bt dag.pt dag.rt dag.st
	/bin/rm -f dag_mix.c dag_t.c

