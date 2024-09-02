
include $(SX)/etc/make/gccoptions

DCG=$(SX)/../outdated/oldies/DCG
SRC=$(DCG)/src
LIB=$(DCG)/lib.$(ARCH)
INCL=$(DCG)/incl

CFLAGS= -g -DEBUG_ALLOC -DEBUG

CC=cc -I$(INCL) -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../outdated/oldies/incl $(CFLAGS)

ALL= $(DCG)/dcg2bnf.out			\
     $(DCG)/dcg_pars.out

all: $(ALL)

NEEDS1= $(LIB)/dcg_main.o		\
	$(LIB)/dcg_sact.o    		\
	$(LIB)/dcg_smp.o    		\
	$(LIB)/dcg_t.o


$(DCG)/dcg2bnf.out:	$(NEEDS1)	$(SXBIBS)
	cc -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/dcg_main.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/dcg.h	 	\
			$(SRC)/dcg_main.c
	$(CC) -o $@ -c $(SRC)/dcg_main.c



$(LIB)/dcg_sact.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/dcg.h	 	\
			$(INCL)/dcg_td.h 	\
			$(SRC)/dcg_sact.c
	$(CC) -o $@ -c $(SRC)/dcg_sact.c

$(LIB)/dcg_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/dcg_cst.h	\
			$(INCL)/dcg.h	 	\
			$(SRC)/dcg_smp.c
	$(CC) -o $@ -c -I$(HOME)/syntax/5.0/sun4/incl $(SRC)/dcg_smp.c

$(LIB)/dcg_t.o:		$(SXINCL)/sxunix.h 	\
			$(SRC)/dcg_t.c
	$(CC) -o $@ -c $(SRC)/dcg_t.c		\
		-DSCANACT=dcg_scan_act		\
		-DSEMPASS=dcg_smp

PARS=\
	$(LIB)/dcg_main.o		\
	$(LIB)/dcg_sact.o    		\
	$(LIB)/dcg_nosem_t.o


$(DCG)/dcg_pars.out:	$(PARS)	$(SXBIBS)
	cc -o $@ $(PARS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/dcg_nosem_t.o:		$(SXINCL)/sxunix.h 	\
			$(SRC)/dcg_t.c
	$(CC) -o $@ -c $(SRC)/dcg_t.c		\
		-DSCANACT=dcg_scan_act		\
		-DSEMPASS=sxvoid -o dcg_nosem_t.o

clean:
	rm -f $(ALL) $(NEEDS1) $(PARS)

