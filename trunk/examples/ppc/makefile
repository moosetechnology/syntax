
include $(SX)/etc/make/gccoptions

PPC=$(SX)/examples/ppc
BIN=$(PPC)/bin.$(ARCH)
LIB=$(PPC)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) $(CFLAGS) -c

all: $(BIN)/ppc.out

NEEDS=	$(LIB)/ppc1_t.o $(LIB)/ppc2_t.o $(LIB)/ppc3_t.o	\
	$(LIB)/ppc_args_t.o $(LIB)/ppc_main.o $(LIB)/ppc_sact.o

$(BIN)/ppc.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) -o $@ || (rm $@; exit 1)

$(LIB)/ppc1_t.o:	ppc1_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ -Dsxtables=ppc1_tables -DSCANACT=ppc_scan_act ppc1_t.c

$(LIB)/ppc2_t.o:	ppc2_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ -Dsxtables=ppc2_tables -DSCANACT=ppc_scan_act ppc2_t.c

$(LIB)/ppc3_t.o:	ppc3_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ -Dsxtables=ppc3_tables -DSCANACT=ppc_scan_act ppc3_t.c

$(LIB)/ppc_args_t.o:	ppc_args_t.c	\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ -Dsxtables=ppc_args_tables -DSCANACT=ppc_args_scan_act -DSEMACT=ppc_args_semact ppc_args_t.c

$(LIB)/ppc_main.o:	ppc_main.c	\
			ppc1_td.h	\
			ppc2_td.h	\
			ppc3_td.h	\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ ppc_main.c

$(LIB)/ppc_sact.o:	ppc_sact.c	\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ ppc_sact.c

# debut de code produit par "make-makefile ppc1"

ppc1_t.c: ppc1.bt ppc1.st ppc1.pt ppc1.rt ppc1.ppt
	tables_c ppc1 > $@

ppc1.bt ppc1.ppt : ppc1.paradis
	paradis ppc1.paradis 

ppc1.st: ppc1.bt ppc1.lecl
	lecl ppc1.lecl || [ $$? = 1 ]

ppc1.pt: ppc1.bt ppc1.dt
	csynt ppc1 || [ $$? = 1 ]

ppc1.dt: ppc1.bt ppc1.prio
	prio ppc1.prio

ppc1.rt: ppc1.bt ppc1.st ppc1.recor
	recor ppc1.recor

ppc1_td.h: ppc1.bt ppc1.tdef
	tdef ppc1.tdef > $@

# fin de code produit par "make-makefile ppc1"

# debut de code produit par "make-makefile ppc2"

ppc2_t.c: ppc2.bt ppc2.st ppc2.pt ppc2.rt ppc2.ppt
	tables_c ppc2 > $@

ppc2.bt ppc2.ppt : ppc2.paradis
	paradis ppc2.paradis 

ppc2.st: ppc2.bt ppc2.lecl
	lecl ppc2.lecl || [ $$? = 1 ]

ppc2.pt: ppc2.bt ppc2.dt
	csynt ppc2 || [ $$? = 1 ]

ppc2.dt: ppc2.bt ppc2.prio
	prio ppc2.prio

ppc2.rt: ppc2.bt ppc2.st ppc2.recor
	recor ppc2.recor

ppc2_td.h: ppc2.bt ppc2.tdef
	tdef ppc2.tdef > $@

# fin de code produit par "make-makefile ppc2"

# debut de code produit par "make-makefile ppc3"

ppc3_t.c: ppc3.bt ppc3.st ppc3.pt ppc3.rt ppc3.ppt
	tables_c ppc3 > $@

ppc3.bt ppc3.ppt : ppc3.paradis
	paradis ppc3.paradis 

ppc3.st: ppc3.bt ppc3.lecl
	lecl ppc3.lecl || [ $$? = 1 ]

ppc3.pt: ppc3.bt ppc3.dt
	csynt ppc3 || [ $$? = 1 ]

ppc3.dt: ppc3.bt ppc3.prio
	prio ppc3.prio

ppc3.rt: ppc3.bt ppc3.st ppc3.recor
	recor ppc3.recor

ppc3_td.h: ppc3.bt ppc3.tdef
	tdef ppc3.tdef > $@

# fin de code produit par "make-makefile ppc3"

# debut de code produit par "make-makefile ppc_args"

ppc_args_t.c: ppc_args.bt ppc_args.st ppc_args.pt ppc_args.rt 
	tables_c ppc_args > $@

ppc_args.bt  : ppc_args.bnf
	semact ppc_args.bnf 

ppc_args.st: ppc_args.bt ppc_args.lecl
	lecl ppc_args.lecl || [ $$? = 1 ]

ppc_args.pt: ppc_args.bt
	csynt ppc_args || [ $$? = 1 ]

ppc_args.rt: ppc_args.bt ppc_args.st ppc_args.recor
	recor ppc_args.recor

# fin de code produit par "make-makefile ppc_args"

clean:
	rm -f $(BIN)/*.out $(LIB)/*.o
	rm -f ppc1.bt ppc1.dt ppc1.ppt ppc1.pt ppc1.rt ppc1.st ppc1_t.c ppc1_td.h
	rm -f ppc1.bn.l ppc1.la.l ppc1.lc.l ppc1.pr.l ppc1.rc.l ppc1.td.l
	rm -f ppc2.bt ppc2.dt ppc2.ppt ppc2.pt ppc2.rt ppc2.st ppc2_t.c ppc2_td.h
	rm -f ppc2.bn.l ppc2.la.l ppc2.lc.l ppc2.pr.l ppc2.rc.l ppc2.td.l
	rm -f ppc3.bt ppc3.dt ppc3.ppt ppc3.pt ppc3.rt ppc3.st ppc3_t.c ppc3_td.h
	rm -f ppc3.bn.l ppc3.la.l ppc3.lc.l ppc3.pr.l ppc3.rc.l ppc3.td.l
	rm -f ppc_args.bt ppc_args.ppt ppc_args.pt ppc_args.rt ppc_args.st ppc_args_t.c
	rm -f ppc_args.bn.l ppc_args.lc.l ppc_args.rc.l

