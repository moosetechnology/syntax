
include $(SX)/etc/make/gccoptions

ACT=$(SX)/semact
LIB=$(ACT)/lib.$(ARCH)
BOOT=$(ACT)/boot

CC=$(COMPILER) -I$(BOOT) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXBIN)/semact.out

NEEDS=	$(LIB)/semact_main.o		\
	$(LIB)/semact_sact.o		\
	$(LIB)/semact_t.o

$(SXBIN)/semact.out:	$(NEEDS) $(SXBIBS) $(SXLIB)/libsxbnf.a
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXLIB)/libsxbnf.a $(SXBIBS) || (rm $@; exit 1)

$(LIB)/semact_main.o:	$(SXINCL)/sxunix.h $(SXINCL)/util/B_tables.h $(SXINCL)/util/bnf_vars.h \
			semact_main.c
	$(CC) -o $@ -c semact_main.c

$(LIB)/semact_sact.o:	$(SXINCL)/sxunix.h $(SXINCL)/sem_by.h $(SXINCL)/util/B_tables.h $(SXINCL)/util/bnf_vars.h \
			semact_sact.c
	$(CC) -o $@ -c semact_sact.c

$(LIB)/semact_t.o:	$(SXINCL)/sxunix.h $(BOOT)/semact_t.c
	$(CC) -o $@ -c $(BOOT)/semact_t.c 		\
		-DSEMACT=semact_semact

# debut de code produit par "make-makefile semact"

bootstrap: semact_t.c  

semact_t.c: semact.bt semact.st semact.pt semact.rt 
	tables_c semact > $@

semact.bt: semact.bnf
	semact semact.bnf 

semact.st: semact.bt semact.lecl
	lecl semact.lecl || [ $$? = 1 ]

semact.pt: semact.bt
	csynt semact || [ $$? = 1 ]

semact.rt: semact.bt semact.st semact.recor
	recor semact.recor

# fin de code produit par "make-makefile semact"

clean:
	/bin/rm -f $(LIB)/*.o
	/bin/rm -f semact_t.c
	/bin/rm -f semact.bt semact.pt semact.rt semact.st 
# pas de semact.dt 
	/bin/rm -f semact.bn.l semact.lc.l semact.rc.l 
# pas de semact.la.l semact.td.l semact.pr.l 

