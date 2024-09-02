
include $(SX)/etc/make/gccoptions

PPLECL=$(SX)/examples/pplecl
BIN=$(PPLECL)/bin.$(ARCH)
LIB=$(PPLECL)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -c

all:	$(BIN)/pplecl.out

NEEDS=		$(LIB)/pplecl_t.o		\
		$(LIB)/pplecl_main.o		\
		$(LIB)/lecl_pact.o		\
		$(LIB)/pplecl_sact.o

$(BIN)/pplecl.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/pplecl_main.o:	$(SXINCL)/sxunix.h		\
			pplecl_main.c
	$(CC) -o $@ pplecl_main.c

$(LIB)/pplecl_t.o: 	$(SXINCL)/sxunix.h		\
			pplecl_t.c
	$(CC) -o $@ pplecl_t.c				\
			-DPARSACT=lecl_pars_act		\
			-DSCANACT=pp_lecl_scan_act

$(LIB)/pplecl_sact.o: 	$(SXINCL)/sxunix.h		\
			pplecl_sact.c
	$(CC) -o $@ pplecl_sact.c

$(LIB)/lecl_pact.o:	$(SXINCL)/sxunix.h		\
			lecl_pact.c
	$(CC) -o $@ lecl_pact.c

# debut de code produit par "make-makefile pplecl"

pplecl_t.c: pplecl.bt pplecl.st pplecl.pt pplecl.rt pplecl.ppt
	tables_c pplecl > $@

pplecl.bt pplecl.ppt : pplecl.paradis
	paradis pplecl.paradis 

pplecl.st: pplecl.bt pplecl.lecl
	lecl pplecl.lecl || [ $$? = 1 ]

pplecl.pt: pplecl.bt
	csynt pplecl || [ $$? = 1 ]

pplecl.rt: pplecl.bt pplecl.st pplecl.recor
	recor pplecl.recor

# fin de code produit par "make-makefile pplecl"

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/pplecl.out
	rm -f pplecl.bt pplecl.ppt pplecl.pt pplecl.rt pplecl.st pplecl_t.c 
	rm -f pplecl.bn.l pplecl.lc.l pplecl.rc.l

