
include $(SX)/etc/make/gccoptions

DCG=$(SX)/../outdated/oldies/DCG
LIB=$(DCG)/lib.$(ARCH)

# CFLAGS= -g -DEBUG_ALLOC -DEBUG

CC=cc -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) -I$(SX)/../outdated/oldies/incl $(CFLAGS)

ALL= $(DCG)/dcg2bnf.out			\
     $(DCG)/dcg_pars.out

all: $(ALL)

NEEDS1= $(LIB)/dcg_main.o		\
	$(LIB)/dcg_sact.o    		\
	$(LIB)/dcg_smp.o    		\
	$(LIB)/dcg_t.o


$(DCG)/dcg2bnf.out:	$(NEEDS1)	$(SXBIBS)
	cc -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/dcg_main.o:	$(SXINCL)/sxunix.h	\
			dcg.h	 	\
			dcg_main.c
	$(CC) -o $@ -c dcg_main.c



$(LIB)/dcg_sact.o:	$(SXINCL)/sxunix.h	\
			dcg.h	 	\
			dcg_td.h 	\
			dcg_sact.c
	$(CC) -o $@ -c dcg_sact.c

$(LIB)/dcg_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			$(SXEXTINCL)/dcg_cst.h	\
			dcg.h	 	\
			dcg_smp.c
	$(CC) -o $@ -c dcg_smp.c

$(LIB)/dcg_t.o:		$(SXINCL)/sxunix.h 	\
			dcg_t.c
	$(CC) -o $@ -c dcg_t.c		\
		-DSEMPASS=dcg_smp

PARS=\
	$(LIB)/dcg_main.o		\
	$(LIB)/dcg_sact.o    		\
	$(LIB)/dcg_nosem_t.o


$(DCG)/dcg_pars.out:	$(PARS)	$(SXBIBS)
	cc -o $@ $(PARS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/dcg_nosem_t.o:		$(SXINCL)/sxunix.h 	\
			dcg_t.c
	$(CC) -o $@ -c dcg_t.c		\
		-DSEMPASS=sxsvoid

# debut de code produit par "make-makefile dcg"

dcg_t.c: dcg.bt dcg.st dcg.pt dcg.rt dcg.att
	tables_c dcg > $@

dcg.bt dcg.att dcg_mix.c: dcg.at
	semat dcg.at  > dcg_mix.c

dcg.st: dcg.bt dcg.lecl
	lecl dcg.lecl || [ $$? = 1 ]

dcg.pt: dcg.bt
	csynt dcg || [ $$? = 1 ]

dcg.rt: dcg.bt dcg.st dcg.recor
	recor dcg.recor

dcg_td.h: dcg.bt dcg.tdef
	tdef dcg.tdef > $@

# fin de code produit par "make-makefile dcg"

clean:
	rm -f $(ALL) $(NEEDS1) $(PARS)
	/bin/rm -f dcg.*.l
	/bin/rm -f dcg.att dcg.bt dcg.pt dcg.rt dcg.st
	/bin/rm -f dcg_mix.c dcg_t.c

