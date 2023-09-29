# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
LNorCP=${SHELL} $(SX)/etc/bin/lien-relatif.sh
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
MIX=$(SX)/oldies/mix
SRC=$(MIX)/src
LIB=$(MIX)/lib

BIBS= $(SXLIB)/libsx$(sxV).a

CC=cd $(LIB); umask 7; exec cc -I$(SXINCL) $(CFLAGS) -c


$(MIX)/everything .PRECIOUS:	$(SX)/bin/mix.out


$(SX)/bin/mix.out:	$(MIX)/mix.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/mix_main.o

$(MIX)/mix.out:	$(NEEDS)	$(BIBS)
	@echo '	cc -o $@'; rm -f $@; ${SHELL} -c 'cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(LIB)/mix_main.o:	$(SXINCL)/sxunix.h	\
			$(SRC)/mix_main.c
	$(CC) $(SRC)/mix_main.c

