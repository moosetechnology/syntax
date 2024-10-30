
include $(SX)/etc/make/gccoptions

RLR=$(SX)/../outdated/oldies/rlr

LIB=$(RLR)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../outdated/oldies/incl $(CFLAGS)

ALL=$(RLR)/rlr.out

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

$(RLR)/rlr.out:	$(NEEDS)	$(SXBIBS)
	rm -f $@; cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(RLR)/rlrd.out:	$(NEEDS2)	$(SXBIBS)
	rm -f $@; cc -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/rlr_main.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			rlr_main.c
	$(CC) -o $@ -c rlr_main.c

$(LIB)/LR0.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			LR0.c
	$(CC) -o $@ -c LR0.c

$(LIB)/LALR1.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/D_tables.h	\
			LALR1.c
	$(CC) -o $@ -c LALR1.c

$(LIB)/ambig.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			$(SXINCL)/util/bstr.h		\
			RLR.h			\
			ambig.c
	$(CC) -o $@ -c ambig.c

$(LIB)/ARC.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			ARC.c
	$(CC) -o $@ -c ARC.c

$(LIB)/XARC.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			XARC.c
	$(CC) -o $@ -c XARC.c

$(LIB)/ARP.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			ARP.c
	$(CC) -o $@ -c ARP.c

$(LIB)/RARP.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			RARP.c
	$(CC) -o $@ -c RARP.c

$(LIB)/earley.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			earley.c
	$(CC) -o $@ -c earley.c

$(LIB)/first.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			first.c
	$(CC) -o $@ -c first.c

$(LIB)/messages_mngr.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/P_tables.h	\
			messages_mngr.c
	$(CC) -o $@ -c messages_mngr.c

$(LIB)/optim_lo.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/P_tables.h	\
			optim_lo.c
	$(CC) -o $@ -c optim_lo.c

$(LIB)/xprdct_mngr.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/P_tables.h	\
			xprdct_mngr.c
	$(CC) -o $@ -c xprdct_mngr.c

$(LIB)/super_FSA.o:	$(SXINCL)/XxY.h		\
			$(SXINCL)/X.h			\
			$(SXINCL)/sxba.h		\
			super_FSA.c
	$(CC) -o $@ -c super_FSA.c

$(LIB)/single.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/P_tables.h	\
			single.c
	$(CC) -o $@ -c single.c

$(LIB)/algoV.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/P_tables.h	\
			algoV.c
	$(CC) -o $@ -c algoV.c

$(LIB)/OPTIM.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/put_edit.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/P_tables.h	\
			OPTIM.c
	$(CC) -o $@ -c OPTIM.c

$(LIB)/rlrd_main.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxdynam_parser.h	\
			rlrd_main.c
	$(CC) -o $@ -c rlrd_main.c

clean:
	rm -f $(ALL) $(NEEDS) $(NEEDS2)
	rm -f $(RLR)/rlrd.out

