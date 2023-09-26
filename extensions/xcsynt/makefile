
include $(SX)/etc/make/gccoptions

ND=$(SX)/../extensions

NDCSYNT=$(ND)/xcsynt
NDSRC=$(NDCSYNT)/src
NDLIB=$(NDCSYNT)/lib
NDINCL=$(NDCSYNT)/incl

CC=$(COMPILER) -I$(NDINCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

CC2=$(COMPILER) -I$(ND)/incl -I$(ND)/incl/util $(CFLAGS)

all: $(SX)/bin/xcsynt.out	\
	$(NDCSYNT)/lc.out
#	$(NDCSYNT)/csyntd.out	\

$(SX)/bin/xcsynt.out:	$(NDCSYNT)/csynt.out
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

$(NDLIB)/csynt.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(NDSRC)/csynt.c
	$(CC) -o $@ -c $(NDSRC)/csynt.c

$(NDLIB)/LR0.o:	$(NDINCL)/csynt_optim.h			\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxYxZ.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/SS.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/LR0.c
	$(CC) -o $@ -c $(NDSRC)/LR0.c

$(NDLIB)/LALR1.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/D_tables.h	\
			$(NDSRC)/LALR1.c
	$(CC) -o $@ -c $(NDSRC)/LALR1.c

$(NDLIB)/ARC.o:	$(NDINCL)/csynt_optim.h			\
			$(NDINCL)/RLR.h			\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/D_tables.h	\
			$(NDSRC)/ARC.c
	$(CC) -o $@ -c $(NDSRC)/ARC.c

$(NDLIB)/OPTIM.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/OPTIM.c
	$(CC) -o $@ -c $(NDSRC)/OPTIM.c

$(NDLIB)/xprdct_mngr.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/xprdct_mngr.c
	$(CC) -o $@ -c $(NDSRC)/xprdct_mngr.c

$(NDLIB)/optim_lo.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/optim_lo.c
	$(CC) -o $@ -c $(NDSRC)/optim_lo.c

$(NDLIB)/single.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxYxZ.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/single.c
	$(CC) -o $@ -c $(NDSRC)/single.c

$(NDLIB)/ambig.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(NDSRC)/ambig.c
	$(CC) -o $@ -c $(NDSRC)/ambig.c

$(NDLIB)/XARC.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/XARC.c
	$(CC) -o $@ -c $(NDSRC)/XARC.c

$(NDLIB)/ARP.o:		$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/ARP.c
	$(CC) -o $@ -c $(NDSRC)/ARP.c

$(NDLIB)/RARP.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/RARP.c
	$(CC) -o $@ -c $(NDSRC)/RARP.c

$(NDLIB)/earley.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/earley.c
	$(CC) -o $@ -c $(NDSRC)/earley.c

$(NDLIB)/first.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/first.c
	$(CC) -o $@ -c $(NDSRC)/first.c

$(NDLIB)/messages_mngr.o:	$(NDINCL)/csynt_optim.h	\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SX)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/messages_mngr.c
	$(CC) -o $@ -c $(NDSRC)/messages_mngr.c

$(NDLIB)/super_FSA.o:	$(SX)/incl/XxY.h		\
			$(SX)/incl/X.h			\
			$(SX)/incl/sxba.h		\
			$(NDSRC)/super_FSA.c
	$(CC) -o $@ -c $(NDSRC)/super_FSA.c

$(NDLIB)/algoV.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/algoV.c
	$(CC) -o $@ -c $(NDSRC)/algoV.c

$(NDCSYNT)/csyntd.out:	$(NEEDS2)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(NDLIB)/csyntd_main.o:	$(ND)/incl/sxunix.h		\
		$(ND)/incl/sxdynam_parser.h		\
		$(NDSRC)/csyntd_main.c
	$(CC2) -o $@ -c $(NDSRC)/csyntd_main.c

$(NDCSYNT)/lc.out:	$(NEEDS3)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(NDLIB)/left_corner.o:	$(NDINCL)/csynt_optim.h		\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/XH.h			\
			$(SX)/incl/X.h			\
			$(NDSRC)/left_corner.c
	$(CC) -o $@ -c $(NDSRC)/left_corner.c

$(NDLIB)/LC0.o:	$(NDINCL)/csynt_optim.h			\
			$(SX)/incl/sxunix.h		\
			$(SX)/incl/sxba.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SX)/incl/XxYxZ.h		\
			$(SX)/incl/XxY.h		\
			$(SX)/incl/SS.h			\
			$(SXINCL)/util/P_tables.h	\
			$(NDSRC)/LC0.c
	$(CC) -o $@ -c $(NDSRC)/LC0.c

clean:
	/bin/rm -f $(NDLIB)/*.o $(NDCSYNT)/lc.out $(NDCSYNT)/csynt.out

