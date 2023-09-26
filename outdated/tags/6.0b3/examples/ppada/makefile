# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
LNorCP=${SHELL} $(SX)/etc/bin/lien-relatif.sh
SXINCL=$(SX)/incl
SXLIB=$(SX)/lib

PPADA=$(SX)/examples/ppada
LIB=$(PPADA)/lib
SPEC=$(PPADA)/spec
INCL=$(PPADA)/incl
SRC=$(PPADA)/src
TMP=$(PPADA)/tmp
COMPILER=cc
BIB= $(SXLIB)/libsx$(sxV).a

all: $(PPADA)/everything

$(PPADA)/everything .PRECIOUS:	$(SX)/bin/ppada.out

$(SX)/bin/ppada.out:	$(PPADA)/ppada.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


NEEDS=$(LIB)/ppada_t.o $(LIB)/ppada_args_t.o $(LIB)/ppada_main.o $(LIB)/ppada_sact.o

$(PPADA)/ppada.out:	$(NEEDS)	$(BIB)
	@echo '	$(COMPILER) -o $@'; rm -f $@; ${SHELL} -c '$(COMPILER) $(NEEDS) $(CFLAGS) $(LDFLAGS) $(BIB) -o $@ || (rm $@;exit 1)'


CC=cd $(LIB); umask 7; exec $(COMPILER) -I$(INCL) -I$(SXINCL) $(CFLAGS) -c


$(LIB)/ppada_t.o:	$(SRC)/ppada_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -Dsxtables=ppada_tables -DSCANACT=ppada_scan_act $(SRC)/ppada_t.c

$(LIB)/ppada_args_t.o:	$(SRC)/ppada_args_t.c		\
			$(SXINCL)/sxunix.h
	$(CC) -Dsxtables=ppada_args_tables -DSCANACT=ppada_args_scan_act -DSEMACT=ppada_args_semact $(SRC)/ppada_args_t.c

$(LIB)/ppada_main.o:	$(SRC)/ppada_main.c	\
			$(INCL)/ppada_td.h	\
			$(SXINCL)/sxunix.h
	$(CC) $(SRC)/ppada_main.c

$(LIB)/ppada_sact.o:	$(SRC)/ppada_sact.c	\
			$(INCL)/ppada_td.h	\
			$(SXINCL)/sxunix.h
	$(CC) $(SRC)/ppada_sact.c

clean:
	rm -fr *~ $(LIB)/*.o
