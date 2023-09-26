# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
ETCINCL=$(ETC)/incl

DCG=$(SX)/DCG
SRC=$(DCG)/src
LIB=$(DCG)/lib
INCL=$(DCG)/incl

BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a

CFLAGS= -g -DEBUG_ALLOC -DEBUG

CC=cd $(LIB); umask 7; exec cc -I$(INCL) -I$(SXINCL) -I$(ETC)/incl $(CFLAGS) -c

$(DCG)/everything .PRECIOUS:	$(DCG)/dcg2bnf.out	\
				$(DCG)/dcg_pars.out

NEEDS1=\
	$(LIB)/dcg_main.o		\
	$(LIB)/dcg_sact.o    		\
	$(LIB)/dcg_smp.o    		\
	$(LIB)/dcg_t.o



$(DCG)/dcg2bnf.out:	$(NEEDS1)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(LIB)/dcg_main.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/dcg.h	 	\
			$(SRC)/dcg_main.c
	$(CC) $(SRC)/dcg_main.c



$(LIB)/dcg_sact.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/dcg.h	 	\
			$(INCL)/dcg_td.h 	\
			$(SRC)/dcg_sact.c
	$(CC) $(SRC)/dcg_sact.c

$(LIB)/dcg_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/dcg_cst.h	\
			$(INCL)/dcg.h	 	\
			$(SRC)/dcg_smp.c
	$(CC) -I$(HOME)/syntax/5.0/sun4/incl $(SRC)/dcg_smp.c

$(LIB)/dcg_t.o:		$(SXINCL)/sxunix.h 	\
			$(SRC)/dcg_t.c
	$(CC) $(SRC)/dcg_t.c			\
		-Dsxtables=dcg_tables		\
		-DSCANACT=dcg_scan_act		\
		-Dsempass=dcg_smp

PARS=\
	$(LIB)/dcg_main.o		\
	$(LIB)/dcg_sact.o    		\
	$(LIB)/dcg_nosem_t.o


$(DCG)/dcg_pars.out:	$(PARS)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(PARS) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(LIB)/dcg_nosem_t.o:		$(SXINCL)/sxunix.h 	\
			$(SRC)/dcg_t.c
	$(CC) $(SRC)/dcg_t.c			\
		-Dsxtables=dcg_tables		\
		-DSCANACT=dcg_scan_act		\
		-Dsempass=sxvoid -o dcg_nosem_t.o

