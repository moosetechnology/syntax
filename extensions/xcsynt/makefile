
include $(SX)/etc/make/gccoptions

ND=$(SX)/../extensions

NDCSYNT=$(ND)/xcsynt
NDLIB=$(NDCSYNT)/lib

CC=$(COMPILER) -I$(NDCSYNT) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

CC2=$(COMPILER) -I$(ND)/incl -I$(ND)/incl/util $(CFLAGS)

all: $(SXBIN)/xcsynt.out	\
	$(NDCSYNT)/lc.out
#	$(NDCSYNT)/csyntd.out	\

$(SXBIN)/xcsynt.out:	$(NDCSYNT)/csynt.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(NDLIB)/csynt.o		\
	$(NDLIB)/LR0.o			\
	$(NDLIB)/LALR1.o		\
	$(NDLIB)/ambig.o		\
	$(NDLIB)/ARC.o			\
	$(NDLIB)/XARC.o			\
	$(NDLIB)/ARP.o			\
	$(NDLIB)/RARP.o			\
	$(NDLIB)/messages_mngr.o	\
	$(NDLIB)/super_FSA.o		\
	$(NDLIB)/earley.o		\
	$(NDLIB)/first.o		\
	$(NDLIB)/single.o		\
	$(NDLIB)/algoV.o		\
	$(NDLIB)/optim_lo.o		\
	$(NDLIB)/xprdct_mngr.o		\
	$(NDLIB)/OPTIM.o

NEEDS2= $(NDLIB)/csyntd_main.o

NEEDS3=	$(NDLIB)/left_corner.o		\
	$(NDLIB)/LC0.o			\
	$(NDLIB)/first.o		\
	$(NDLIB)/single.o		\
	$(NDLIB)/algoV.o		\
	$(NDLIB)/optim_lo.o		\
	$(NDLIB)/xprdct_mngr.o		\
	$(NDLIB)/OPTIM.o

$(NDCSYNT)/csynt.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(NDLIB)/csynt.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			csynt.c
	$(CC) -o $@ -c csynt.c

$(NDLIB)/LR0.o:	csynt_optim.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxYxZ.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/SS.h			\
			$(SXINCL)/util/P_tables.h	\
			LR0.c
	$(CC) -o $@ -c LR0.c

$(NDLIB)/LALR1.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/D_tables.h	\
			LALR1.c
	$(CC) -o $@ -c LALR1.c

$(NDLIB)/ARC.o:	csynt_optim.h			\
			RLR.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/D_tables.h	\
			ARC.c
	$(CC) -o $@ -c ARC.c

$(NDLIB)/OPTIM.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/P_tables.h	\
			OPTIM.c
	$(CC) -o $@ -c OPTIM.c

$(NDLIB)/xprdct_mngr.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/P_tables.h	\
			xprdct_mngr.c
	$(CC) -o $@ -c xprdct_mngr.c

$(NDLIB)/optim_lo.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/P_tables.h	\
			optim_lo.c
	$(CC) -o $@ -c optim_lo.c

$(NDLIB)/single.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxYxZ.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			$(SXINCL)/util/P_tables.h	\
			single.c
	$(CC) -o $@ -c single.c

$(NDLIB)/ambig.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			ambig.c
	$(CC) -o $@ -c ambig.c

$(NDLIB)/XARC.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			XARC.c
	$(CC) -o $@ -c XARC.c

$(NDLIB)/ARP.o:		csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			ARP.c
	$(CC) -o $@ -c ARP.c

$(NDLIB)/RARP.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			RARP.c
	$(CC) -o $@ -c RARP.c

$(NDLIB)/earley.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			earley.c
	$(CC) -o $@ -c earley.c

$(NDLIB)/first.o:	csynt_optim.h		\
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

$(NDLIB)/messages_mngr.o:	csynt_optim.h	\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/SS.h			\
			RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			messages_mngr.c
	$(CC) -o $@ -c messages_mngr.c

$(NDLIB)/super_FSA.o:	$(SXINCL)/XxY.h		\
			$(SXINCL)/X.h			\
			$(SXINCL)/sxba.h		\
			super_FSA.c
	$(CC) -o $@ -c super_FSA.c

$(NDLIB)/algoV.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			$(SXINCL)/util/P_tables.h	\
			algoV.c
	$(CC) -o $@ -c algoV.c

$(NDCSYNT)/csyntd.out:	$(NEEDS2)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(NDLIB)/csyntd_main.o:	$(ND)/incl/sxunix.h		\
		$(ND)/incl/sxdynam_parser.h		\
		csyntd_main.c
	$(CC2) -o $@ -c csyntd_main.c

$(NDCSYNT)/lc.out:	$(NEEDS3)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(NDLIB)/left_corner.o:	csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/XH.h			\
			$(SXINCL)/X.h			\
			left_corner.c
	$(CC) -o $@ -c left_corner.c

$(NDLIB)/LC0.o:	csynt_optim.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/XxYxZ.h		\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/SS.h			\
			$(SXINCL)/util/P_tables.h	\
			LC0.c
	$(CC) -o $@ -c LC0.c

clean:
	/bin/rm -f $(NDLIB)/*.o $(NDCSYNT)/lc.out $(NDCSYNT)/csynt.out

