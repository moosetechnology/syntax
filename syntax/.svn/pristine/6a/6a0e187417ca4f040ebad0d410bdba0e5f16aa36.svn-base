
include $(SX)/etc/make/gccoptions

DICO=$(SX)/../extensions/dico

LIB=$(DICO)/lib

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SX)/bin/dico.out	

$(SX)/bin/dico.out:	$(DICO)/dico.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDED=\
	$(LIB)/dico_main.o	\
	$(LIB)/dico_t.o		\
	$(LIB)/dico_act.o

$(DICO)/dico.out:	$(NEEDED)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDED) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/dico_act.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/sxdico.h	\
			dico_act.c	
	$(CC) -o $@ -c dico_act.c			\
		-Dsxtables=dico_tables

$(LIB)/dico_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxdico.h	\
			dico_main.c
	$(CC) -o $@ -c dico_main.c		\
		-Dsxtables=dico_tables

$(LIB)/dico_t.o:	$(SXINCL)/sxunix.h 	\
			dico_t.c
	$(CC) -o $@ -c dico_t.c			\
		-Dsxtables=dico_tables		\
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
	/bin/rm -f $(LIB)/*.o $(DICO)/dico.out
	/bin/rm -f $(DICO)/dico.*.l
	/bin/rm -f $(DICO)/dico.bt $(DICO)/dico.pt $(DICO)/dico.rt 
	/bin/rm -f $(DICO)/dico.st $(DICO)/dico_t.c
