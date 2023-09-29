# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
LNorCP=${SHELL} $(SX)/etc/bin/lien-relatif.sh
SXINCL=$(SX)/incl
SXLIB=$(SX)/lib

PPC=$(SX)/examples/ppc
LIB=$(PPC)/lib
SPEC=$(PPC)/spec
INCL=$(PPC)/incl
SRC=$(PPC)/src
TMP=$(PPC)/tmp
COMPILER=cc
BIB= $(SXLIB)/libsx$(sxV).a

$(PPC)/everything .PRECIOUS:	$(SX)/bin/ppc.out

$(SX)/bin/ppc.out:	$(PPC)/ppc.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


NEEDS=	$(LIB)/ppc1_t.o $(LIB)/ppc2_t.o $(LIB)/ppc3_t.o	\
	$(LIB)/ppc_args_t.o $(LIB)/ppc_main.o $(LIB)/ppc_sact.o

$(PPC)/ppc.out:	$(NEEDS)	$(BIB)
	@echo '	$(COMPILER) -o $@'; rm -f $@; ${SHELL} -c '$(COMPILER) $(NEEDS) $(CFLAGS) $(LDFLAGS) $(BIB) -o $@ || (rm $@;exit 1)'


CC=cd $(LIB); umask 7; exec $(COMPILER) -I$(INCL) -I$(SXINCL) $(CFLAGS) -c


$(LIB)/ppc1_t.o:	$(SRC)/ppc1_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -Dsxtables=ppc1_tables -DSCANACT=ppc_scan_act $(SRC)/ppc1_t.c

$(LIB)/ppc2_t.o:	$(SRC)/ppc2_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -Dsxtables=ppc2_tables -DSCANACT=ppc_scan_act $(SRC)/ppc2_t.c

$(LIB)/ppc3_t.o:	$(SRC)/ppc3_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -Dsxtables=ppc3_tables -DSCANACT=ppc_scan_act $(SRC)/ppc3_t.c

$(LIB)/ppc_args_t.o:	$(SRC)/ppc_args_t.c	\
			$(SXINCL)/sxunix.h
	$(CC) -Dsxtables=ppc_args_tables -DSCANACT=ppc_args_scan_act -DSEMACT=ppc_args_semact $(SRC)/ppc_args_t.c

$(LIB)/ppc_main.o:	$(SRC)/ppc_main.c	\
			$(INCL)/ppc1_td.h	\
			$(INCL)/ppc2_td.h	\
			$(INCL)/ppc3_td.h	\
			$(SXINCL)/sxunix.h
	$(CC) $(SRC)/ppc_main.c

$(LIB)/ppc_sact.o:	$(SRC)/ppc_sact.c	\
			$(SXINCL)/sxunix.h
	$(CC) $(SRC)/ppc_sact.c
