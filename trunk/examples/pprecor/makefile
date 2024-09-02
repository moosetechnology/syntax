
include $(SX)/etc/make/gccoptions

PPRECOR=$(SX)/examples/pprecor
BIN=$(PPRECOR)/bin.$(ARCH)
LIB=$(PPRECOR)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) $(CFLAGS) -c

all: $(BIN)/pprecor.out

NEEDS=	$(LIB)/pprecor_t.o		\
	$(LIB)/pprecor_main.o		\
	$(LIB)/pprecor_sact.o

$(BIN)/pprecor.out:	$(NEEDS) $(SXBIBS)
	@rm -f $@
	$(COMPILER) $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) -o $@ || (rm $@;exit 1)

$(LIB)/pprecor_main.o:	$(SXINCL)/sxunix.h	\
			pprecor_main.c
	$(CC) -o $@ pprecor_main.c

$(LIB)/pprecor_t.o: 	$(SXINCL)/sxunix.h		\
			pprecor_t.c
	$(CC) -o $@ pprecor_t.c			\
			-DSCANACT=pp_recor_scan_act

$(LIB)/pprecor_sact.o:	$(SXINCL)/sxunix.h		\
			pprecor_sact.c
	$(CC) -o $@ pprecor_sact.c

# debut de code produit par "make-makefile pprecor"

pprecor_t.c: pprecor.bt pprecor.st pprecor.pt pprecor.rt pprecor.ppt
	tables_c pprecor > $@

pprecor.bt pprecor.ppt : pprecor.paradis
	paradis pprecor.paradis 

pprecor.st: pprecor.bt pprecor.lecl
	lecl pprecor.lecl || [ $$? = 1 ]

pprecor.pt: pprecor.bt
	csynt pprecor || [ $$? = 1 ]

pprecor.rt: pprecor.bt pprecor.st pprecor.recor
	recor pprecor.recor

# fin de code produit par "make-makefile pprecor"

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/pprecor.out
	rm -f pprecor.bt pprecor.ppt pprecor.pt pprecor.rt pprecor.st pprecor_t.c 
	rm -f pprecor.bn.l pprecor.lc.l pprecor.rc.l

