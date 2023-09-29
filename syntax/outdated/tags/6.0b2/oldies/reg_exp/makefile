# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)

SXINCL=$(SX)/incl
SXLIB=$(SX)/lib

ETC=$(SX)/etc
ETCINCL=$(ETC)/incl
ETCLIB=$(ETC)/lib

LECLSRC=$(SX)/lecl/src
LECLINCL=$(SX)/lecl/incl

LECL=$(SX)/lecl
LECLSRC=$(LECL)/src
LECLLIB=$(LECL)/lib
LECLINCL=$(LECL)/incl

RE=$(SX)/reg_exp
RESRC=$(RE)/src
RELIB=$(RE)/lib
REINCL=$(RE)/incl


BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a

CC=cd $(RELIB); umask 7; exec cc -I$(REINCL) -I$(LECLINCL) -I$(SXINCL) -I$(ETCINCL) -I$(LECLINCL) $(CFLAGS) -c

$(LECL)/ALL .PRECIOUS:	$(RE)/reg_exp.out

NEEDS=		$(RELIB)/reg_exp_main.o

$(RE)/reg_exp.out:	$(NEEDS)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(RELIB)/reg_exp_main.o: $(SXINCL)/sxunix.h		\
			$(LECLINCL)/lecl_ag.h		\
			$(SXINCL)/sxdynam_scanner.h	\
			$(RESRC)/reg_exp_main.c
	$(CC) $(RESRC)/reg_exp_main.c

