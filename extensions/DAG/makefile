
include $(SX)/etc/make/gccoptions

DAG=$(SX)/../extensions/DAG

LIB=$(DAG)/lib

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SX)/bin/dag2udag.out

$(SX)/bin/dag2udag.out:	$(DAG)/dag2udag.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

$(SX)/bin/amlgm_cmpd.out:	$(DAG)/amlgm_cmpd.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/dag2udag_main.o

$(DAG)/dag2udag.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/dag2udag_main.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxstack.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/fsa.h			\
			$(SXINCL)/XxYxZ.h		\
			$(SXINCL)/XxY.h			\
			dag2udag_main.c
	$(CC) -o $@ -c -I$(SX)/../extensions/incl dag2udag_main.c

# debut de code produit par "make-makefile re"

re_t.c: re.bt re.st re.pt re.rt re.att
	tables_c re > $@

re.bt re.att re_mix.c: re.at
	semat re.at  > re_mix.c

re.st: re.bt re.lecl
	lecl re.lecl || [ $$? = 1 ]

re.pt: re.bt
	csynt re || [ $$? = 1 ]

re.rt: re.bt re.st re.recor
	recor re.recor

# fin de code produit par "make-makefile re"

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

# debut de code produit par "make-makefile sdag"

# debut de code produit par "make-makefile text -dummy_csynt"

text_t.c: text.bt text.st text.pt text.rt 
	tables_c text > $@

text.bt  : text.bnf
	semact text.bnf 

text.st: text.bt text.lecl
	lecl text.lecl || [ $$? = 1 ]

text.pt: text.bt
	dummy_csynt text || [ $$? = 1 ]

text.rt: text.bt text.st text.recor
	recor text.recor

# fin de code produit par "make-makefile text -dummy_csynt"


clean:
	/bin/rm -f $(LIB)/*.o $(DAG)/dag2udag.out $(DAG)/amlgm_cmpd.out
