
include $(SX)/etc/make/gccoptions

RLR=$(SX)/../outdated/oldies/rlr
SRC=$(RLR)/src
BIN=$(RLR)/bin.$(ARCH)
LIB=$(RLR)/lib.$(ARCH)
INCL=$(RLR)/incl

CC=cc -I$(INCL) -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../outdated/oldies/incl $(CFLAGS)

ALL=$(BIN)/rlr.out			\
     $(BIN)/primes_nb.out

all: $(ALL)

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

$(BIN)/rlr.out:	$(NEEDS)	$(SXBIBS)
	rm -f $@; cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(BIN)/rlrd.out:	$(NEEDS2)	$(SXBIBS)
	rm -f $@; cc -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(BIN)/primes_nb.out:	$(NEEDS3)	$(SXBIBS)
	rm -f $@; cc -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/rlr_main.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/../outdated/oldies/incl/release.h		\
			$(SRC)/rlr_main.c
	$(CC) -o $@ -c $(SRC)/rlr_main.c

$(LIB)/LR0.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SRC)/LR0.c
	$(CC) -o $@ -c $(SRC)/LR0.c

$(LIB)/LALR1.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/D_tables.h	\
			$(SRC)/LALR1.c
	$(CC) -o $@ -c $(SRC)/LALR1.c

$(LIB)/ambig.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(SXINCL)/util/bstr.h		\
			$(INCL)/RLR.h			\
			$(SRC)/ambig.c
	$(CC) -o $@ -c $(SRC)/ambig.c

$(LIB)/ARC.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/ARC.c
	$(CC) -o $@ -c $(SRC)/ARC.c

$(LIB)/XARC.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/XARC.c
	$(CC) -o $@ -c $(SRC)/XARC.c

$(LIB)/ARP.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/ARP.c
	$(CC) -o $@ -c $(SRC)/ARP.c

$(LIB)/RARP.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/RARP.c
	$(CC) -o $@ -c $(SRC)/RARP.c

$(LIB)/earley.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/earley.c
	$(CC) -o $@ -c $(SRC)/earley.c

$(LIB)/first.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/first.c
	$(CC) -o $@ -c $(SRC)/first.c

$(LIB)/messages_mngr.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(INCL)/RLR.h			\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/messages_mngr.c
	$(CC) -o $@ -c $(SRC)/messages_mngr.c

$(LIB)/optim_lo.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/optim_lo.c
	$(CC) -o $@ -c $(SRC)/optim_lo.c

$(LIB)/xprdct_mngr.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/xprdct_mngr.c
	$(CC) -o $@ -c $(SRC)/xprdct_mngr.c

$(LIB)/super_FSA.o:	$(SX)/incl/XxY.h		\
			$(SX)/incl/X.h			\
			$(SX)/incl/sxba.h		\
			$(SRC)/super_FSA.c
	$(CC) -o $@ -c $(SRC)/super_FSA.c

$(LIB)/single.o:	$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/single.c
	$(CC) -o $@ -c $(SRC)/single.c

$(LIB)/algoV.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/algoV.c
	$(CC) -o $@ -c $(SRC)/algoV.c

$(LIB)/OPTIM.o:		$(INCL)/rlr_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SX)/incl/put_edit.h		\
			$(SX)/incl/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/P_tables.h	\
			$(SRC)/OPTIM.c
	$(CC) -o $@ -c $(SRC)/OPTIM.c

$(LIB)/rlrd_main.o:	$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxdynam_parser.h	\
			$(SRC)/rlrd_main.c
	$(CC) -o $@ -c $(SRC)/rlrd_main.c

$(LIB)/primes_nb.o:	$(SX)/incl/sxunix.h		\
			$(SRC)/primes_nb.c
	$(CC) -o $@ -c $(SRC)/primes_nb.c

clean:
	rm -f $(ALL) $(NEEDS) $(NEEDS2) $(NEEDS3)
	rm -f $(BIN)/rlrd.out

