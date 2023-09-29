
include $(SX)/etc/make/gccoptions

ACT=$(SX)/semact
LIB=$(ACT)/lib
BOOT=$(ACT)/boot

BNF=$(SX)/bnf
BNFLIB=$(BNF)/lib

CC=$(COMPILER) -I$(BOOT) -I$(BNF) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SX)/bin/semact.out

$(SX)/bin/semact.out:	$(ACT)/semact.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/semact_main.o		\
	$(LIB)/semact_sact.o		\
	$(LIB)/semact_t.o		\
	$(BNFLIB)/bnf_folcon.o		\
	$(BNFLIB)/bnf_lo.o    		\
	$(BNFLIB)/bnf_protest.o		\
	$(BNFLIB)/bnf_sact.o   		\
	$(BNFLIB)/bnf_smp.o    		\
	$(BNFLIB)/bnf_symtab.o		\
	$(BNFLIB)/bnf_t.o

$(ACT)/semact.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/semact_main.o:	$(SXINCL)/sxunix.h $(SXINCL)/util/B_tables.h $(BNF)/bnf_vars.h \
			semact_main.c
	$(CC) -o $@ -c semact_main.c

$(LIB)/semact_sact.o:	$(SXINCL)/sxunix.h $(SXINCL)/sem_by.h $(SXINCL)/util/B_tables.h $(BNF)/bnf_vars.h \
			semact_sact.c
	$(CC) -o $@ -c semact_sact.c

$(LIB)/semact_t.o:	$(SXINCL)/sxunix.h $(BOOT)/semact_t.c
	$(CC) -o $@ -c $(BOOT)/semact_t.c 		\
		-Dsxtables=semact_tables	\
		-DSEMACT=semact_semact

# debut de code produit par "make-makefile semact"

bootstrap: semact_t.c  

semact_t.c: semact.bt semact.st semact.pt semact.rt 
	tables_c semact > $@

semact.bt  : semact.bnf
	semact semact.bnf 

semact.st: semact.bt semact.lecl
	lecl semact.lecl || [ $$? = 1 ]

semact.pt: semact.bt
	csynt semact || [ $$? = 1 ]

semact.rt: semact.bt semact.st semact.recor
	recor semact.recor

# fin de code produit par "make-makefile semact"

clean:
	/bin/rm -f $(LIB)/*.o $(ACT)/semact.out
	/bin/rm -f semact_t.c
	/bin/rm -f semact.bt semact.pt semact.rt semact.st 
# pas de semact.dt 
	/bin/rm -f semact.bn.l semact.lc.l semact.rc.l 
# pas de semact.la.l semact.td.l semact.pr.l 

