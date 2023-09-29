# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
LNorCP=${SHELL} $(SX)/etc/bin/lien-relatif.sh
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
PPP=$(SX)/oldies/ppp
SRC=$(PPP)/src
LIB=$(PPP)/lib
INCL=$(PPP)/incl
REX=$(SX)/reg_exp

BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a

CC=cd $(LIB); umask 7; exec cc -I$(INCL) -I$(SXINCL) -I$(ETC)/incl -I$(REX)/incl $(CFLAGS) -c

$(PPP)/ALL .PRECIOUS:	$(SX)/bin/ppp.out

$(SX)/bin/ppp.out:	$(PPP)/ppp.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

PP=		$(LIB)/ppp_t.o		\
		$(LIB)/ppp_main.o		\
		$(LIB)/ppp_pact.o		\
		$(LIB)/ppp_sact.o

$(PPP)/ppp.out:	$(PP)	$(BIBS)
	@echo '	cc -o $@'; rm -f $@; ${SHELL} -c 'cc -o $@ $(PP) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(LIB)/ppp_main.o:	$(SXINCL)/sxunix.h		\
			$(SRC)/ppp_main.c
	$(CC) $(SRC)/ppp_main.c


$(LIB)/ppp_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/ppp_t.c
	$(CC) $(SRC)/ppp_t.c				\
			-Dsxtables=pp_ppp_tables	\
			-DPARSACT=ppp_pars_act		\
			-DSCANACT=pp_ppp_scan_act

$(LIB)/ppp_pact.o:	$(SXINCL)/sxunix.h		\
			$(SRC)/ppp_pact.c
	$(CC) $(SRC)/ppp_pact.c

$(LIB)/ppp_sact.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/ppp_sact.c
	$(CC) $(SRC)/ppp_sact.c
