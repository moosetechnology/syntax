# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
LNorCP=${SHELL} $(SX)/etc/bin/lien-relatif.sh
SXLIB=$(SX)/lib
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib

RLR=$(SX)/rlr
SRC=$(RLR)/src
LIB=$(RLR)/lib
INCL=$(RLR)/incl

BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a

CC=cd $(LIB); umask 7; exec cc -I$(INCL) -I$(SX)/incl -I$(ETC)/incl $(CFLAGS) -c

$(RLR)/ALL .PRECIOUS:	$(SX)/bin/rlr.out	\
			$(RLR)/primes_nb.out

$(SX)/bin/rlr.out:	$(RLR)/rlr.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/rlr_main.o		\
	$(LIB)/LR0.o			\
	$(LIB)/LALR1.o			\
	$(LIB)/ambig.o			\
	$(LIB)/ARC.o			\
	$(LIB)/XARC.o			\
	$(LIB)/ARP.o			\
	$(LIB)/RARP.o			\
	$(LIB)/messages_mngr.o		\
	$(LIB)/super_FSA.o		\
	$(LIB)/earley.o			\
	$(LIB)/first.o			\
	$(LIB)/single.o			\
	$(LIB)/algoV.o			\
	$(LIB)/optim_lo.o		\
	$(LIB)/xprdct_mngr.o		\
	$(LIB)/OPTIM.o

NEEDS2= $(LIB)/rlrd_main.o

NEEDS3= $(LIB)/primes_nb.o



$(RLR)/rlr.out:	$(NEEDS)	$(BIBS)
	@echo '	cc -o $@'; rm -f $@; ${SHELL} -c 'cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(RLR)/rlrd.out:	$(NEEDS2)	$(BIBS)
	@echo '	cc -o $@'; rm -f $@; ${SHELL} -c 'cc -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(RLR)/primes_nb.out:	$(NEEDS3)	$(BIBS)
	@echo '	cc -o $@'; rm -f $@; ${SHELL} -c 'cc -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'



$(LIB)/rlr_main.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/bstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(ETC)/incl/release.h		\
			$(SRC)/rlr_main.c
	$(CC) $(SRC)/rlr_main.c

$(LIB)/LR0.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SRC)/LR0.c
	$(CC) $(SRC)/LR0.c

$(LIB)/LALR1.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(ETC)/incl/bstr.h		\
			$(ETC)/incl/D_tables.h		\
			$(SRC)/LALR1.c
	$(CC) $(SRC)/LALR1.c

$(LIB)/ambig.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(ETC)/incl/bstr.h		\
			$(INCL)/RLR.h			\
			$(SRC)/ambig.c
	$(CC) $(SRC)/ambig.c


$(LIB)/ARC.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/ARC.c
	$(CC) $(SRC)/ARC.c

$(LIB)/XARC.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/XARC.c
	$(CC) $(SRC)/XARC.c

$(LIB)/ARP.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/ARP.c
	$(CC) $(SRC)/ARP.c

$(LIB)/RARP.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/RARP.c
	$(CC) $(SRC)/RARP.c

$(LIB)/earley.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/earley.c
	$(CC) $(SRC)/earley.c

$(LIB)/first.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/first.c
	$(CC) $(SRC)/first.c

$(LIB)/messages_mngr.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(ETC)/incl/bstr.h		\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/messages_mngr.c
	$(CC) $(SRC)/messages_mngr.c

$(LIB)/optim_lo.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/bstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/optim_lo.c
	$(CC) $(SRC)/optim_lo.c

$(LIB)/xprdct_mngr.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/xprdct_mngr.c
	$(CC) $(SRC)/xprdct_mngr.c

$(LIB)/super_FSA.o:	$(SX)/incl/XxY.h		\
			$(SX)/incl/X.h			\
			$(SX)/incl/sxba.h		\
			$(SRC)/super_FSA.c
	$(CC) $(SRC)/super_FSA.c

$(LIB)/single.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/single.c
	$(CC) $(SRC)/single.c

$(LIB)/algoV.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/algoV.c
	$(CC) $(SRC)/algoV.c

$(LIB)/OPTIM.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(ETC)/incl/B_tables.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(ETC)/incl/P_tables.h		\
			$(SRC)/OPTIM.c
	$(CC) $(SRC)/OPTIM.c


$(LIB)/rlrd_main.o:	$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxdynam_parser.h	\
			$(SRC)/rlrd_main.c
	$(CC) $(SRC)/rlrd_main.c


$(LIB)/primes_nb.o:	$(SX)/incl/sxunix.h		\
			$(SRC)/primes_nb.c
	$(CC) $(SRC)/primes_nb.c
