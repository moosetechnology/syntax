
include $(SX)/etc/make/gccoptions

PPADA=$(SX)/examples/ppada
BIN=$(PPADA)/bin.$(ARCH)
LIB=$(PPADA)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) $(CFLAGS) -c

all: $(BIN)/ppada.out

NEEDS=$(LIB)/ppada_t.o $(LIB)/ppada_args_t.o $(LIB)/ppada_main.o $(LIB)/ppada_sact.o

$(BIN)/ppada.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) -o $@ || (rm $@; exit 1)

$(LIB)/ppada_t.o:	ppada_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ -DSCANACT=ppada_scan_act ppada_t.c

$(LIB)/ppada_args_t.o:	ppada_args_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ -DSCANACT=ppada_args_scan_act -DSEMACT=ppada_args_semact ppada_args_t.c

$(LIB)/ppada_main.o:	ppada_main.c	\
			ppada_td.h	\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ ppada_main.c

$(LIB)/ppada_sact.o:	ppada_sact.c	\
			ppada_td.h	\
			$(SXINCL)/sxunix.h
	$(CC) -o $@ ppada_sact.c

# debut de code produit par "make-makefile ppada"

ppada_t.c: ppada.bt ppada.st ppada.pt ppada.rt ppada.ppt
	tables_c -name ppada_tables ppada > $@

ppada.bt ppada.ppt : ppada.paradis
	paradis ppada.paradis 

ppada.st: ppada.bt ppada.lecl
	lecl ppada.lecl || [ $$? = 1 ]

ppada.pt: ppada.bt
	csynt ppada || [ $$? = 1 ]

ppada.rt: ppada.bt ppada.st ppada.recor
	recor ppada.recor

ppada_td.h: ppada.bt ppada.tdef
	tdef ppada.tdef > $@

# fin de code produit par "make-makefile ppada"

# debut de code produit par "make-makefile ppada_args"

ppada_args_t.c: ppada_args.bt ppada_args.st ppada_args.pt ppada_args.rt 
	tables_c -name ppada_args_tables ppada_args > $@

ppada_args.bt  : ppada_args.bnf
	semact ppada_args.bnf 

ppada_args.st: ppada_args.bt ppada_args.lecl
	lecl ppada_args.lecl || [ $$? = 1 ]

ppada_args.pt: ppada_args.bt
	csynt ppada_args || [ $$? = 1 ]

ppada_args.rt: ppada_args.bt ppada_args.st ppada_args.recor
	recor ppada_args.recor

# fin de code produit par "make-makefile ppada_args"

clean:
	rm -f $(BIN)/*.out $(LIB)/*.o
	rm -f ppada.bt ppada.ppt ppada.pt ppada.rt ppada.st ppada_t.c ppada_td.h
	rm -f ppada.bn.l ppada.lc.l ppada.rc.l ppada.td.l
	rm -f ppada_args.bt ppada_args.ppt ppada_args.pt ppada_args.rt ppada_args.st ppada_args_t.c
	rm -f ppada_args.bn.l ppada_args.lc.l ppada_args.rc.l

