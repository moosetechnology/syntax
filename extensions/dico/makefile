
include $(SX)/etc/make/gccoptions

DICO=$(SX)/../extensions/dico

BIN=$(DICO)/bin.$(ARCH)
LIB=$(DICO)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(BIN)/dico.out	

NEEDED=\
	$(LIB)/dico_main.o	\
	$(LIB)/dico_t.o		\
	$(LIB)/dico_act.o

$(BIN)/dico.out:	$(NEEDED)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDED) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/dico_act.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/sxdico.h	\
			dico_act.c	
	$(CC) -o $@ -c dico_act.c

$(LIB)/dico_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxdico.h	\
			dico_main.c
	$(CC) -o $@ -c dico_main.c

$(LIB)/dico_t.o:	$(SXINCL)/sxunix.h 	\
			dico_t.c
	$(CC) -o $@ -c dico_t.c			\
		-DSCANACT=dico_scan_act		\
		-DSEMACT=dico_sem_act

# debut de code produit par "make-makefile dico"

dico_t.c: dico.bt dico.st dico.pt dico.rt 
	tables_c dico > $@

dico.bt  : dico.bnf
	semact dico.bnf 

dico.st: dico.bt dico.lecl
	lecl dico.lecl || [ $$? = 1 ]

dico.pt: dico.bt
	csynt dico || [ $$? = 1 ]

dico.rt: dico.bt dico.st dico.recor
	recor dico.recor

# fin de code produit par "make-makefile dico"

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/dico.out
	/bin/rm -f $(DICO)/dico.*.l
	/bin/rm -f $(DICO)/dico.bt $(DICO)/dico.pt $(DICO)/dico.rt 
	/bin/rm -f $(DICO)/dico.st $(DICO)/dico_t.c
