
include $(SX)/etc/make/gccoptions

PRIO=$(SX)/prio
LIB=$(PRIO)/lib
BOOT=$(PRIO)/boot

CC=$(COMPILER) -I$(BOOT) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SX)/bin/prio.out

$(SX)/bin/prio.out:	$(PRIO)/prio.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/prio_t.o			\
	$(LIB)/prio_main.o		\
	$(LIB)/prio_sact.o		\
	$(LIB)/prio_action.o


$(PRIO)/prio.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/prio_t.o: 	$(SXINCL)/sxunix.h		\
			$(BOOT)/prio_t.c
	$(CC) -o $@ -c $(BOOT)/prio_t.c			\
			-Dsxtables=prio_tables		\
			-DSCANACT=prio_scan_act		\
			-DSEMACT=prio_action

$(LIB)/prio_main.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/put_edit.h		\
			prio_main.c
	$(CC) -o $@ -c prio_main.c

$(LIB)/prio_sact.o:	$(SXINCL)/sxunix.h		\
			prio_sact.c
	$(CC) -o $@ -c prio_sact.c

$(LIB)/prio_action.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/D_tables.h	\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			prio_action.c
	$(CC) -o $@ -c prio_action.c

# debut de code produit par "make-makefile prio"

bootstrap: prio_t.c

prio_t.c: prio.bt prio.st prio.pt prio.rt 
	tables_c prio > $@

prio.bt  : prio.bnf
	semact prio.bnf 

prio.st: prio.bt prio.lecl
	lecl prio.lecl || [ $$? = 1 ]

prio.pt: prio.bt
	csynt prio || [ $$? = 1 ]

prio.rt: prio.bt prio.st prio.recor
	recor prio.recor

# fin de code produit par "make-makefile prio"

clean:
	/bin/rm -f $(LIB)/*.o $(PRIO)/prio.out
	/bin/rm -f prio_td.h prio_t.c
	/bin/rm -f prio.bt prio.pt prio.rt prio.st 
# pas de prio.dt 
	/bin/rm -f prio.bn.l prio.lc.l prio.pr.l prio.rc.l 
# pas de prio.la.l prio.td.l 


