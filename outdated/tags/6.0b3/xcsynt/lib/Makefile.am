bin_PROGRAMS =	xcsynt	\
		lc
#		csyntd

LNorCP=ln -s

SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

# normaliser les ND
ND =	$(top_srcdir)
ND_BUILD =	$(top_builddir)
NDETC=$(ND)/etc
NDETCINCL=$(NDETC)/lib

NDCSYNT=$(ND)/xcsynt
NDCSYNT_BUILD=$(ND_BUILD)/xcsynt

NDSRC=$(NDCSYNT)/src
NDLIB=$(NDCSYNT_BUILD)/lib
NDINCL=$(NDCSYNT)/incl

EXTRA_DIST =	$(NDINCL)/RLR.h		\
		$(NDINCL)/csynt_optim.h

AM_CPPFLAGS =	-I$(NDINCL) -I$(SXINCL) -I$(SXETCINCL)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c
CC2_FLAGS = $(CC) -I$(NDINCL) -I$(NDETCINCL) -c


../../bin/xcsynt.out:	xcsynt
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

xcsynt_SOURCES =$(NDSRC)/csynt.c		\
		$(NDSRC)/LR0.c			\
		$(NDSRC)/LALR1.c		\
		$(NDSRC)/ambig.c		\
		$(NDSRC)/ARC.c			\
		$(NDSRC)/XARC.c			\
		$(NDSRC)/ARP.c			\
		$(NDSRC)/RARP.c			\
		$(NDSRC)/messages_mngr.c	\
		$(NDSRC)/super_FSA.c		\
		$(NDSRC)/earley.c		\
		$(NDSRC)/first.c		\
		$(NDSRC)/single.c		\
		$(NDSRC)/algoV.c		\
		$(NDSRC)/optim_lo.c		\
		$(NDSRC)/xprdct_mngr.c		\
		$(NDSRC)/OPTIM.c
xcsynt_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

csynt.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/bstr.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(NDETC)/incl/release.h		\
			$(NDSRC)/csynt.c
	$(CC_FLAGS) $(NDSRC)/csynt.c

LR0.o:	$(NDINCL)/csynt_optim.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxYxZ.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/SS.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/LR0.c
	$(CC_FLAGS) $(NDSRC)/LR0.c

LALR1.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/bstr.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(SXETCINCL)/D_tables.h		\
			$(NDSRC)/LALR1.c
	$(CC_FLAGS) $(NDSRC)/LALR1.c

ARC.o:	$(NDINCL)/csynt_optim.h			\
			$(NDINCL)/RLR.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(SXETCINCL)/D_tables.h		\
			$(NDSRC)/ARC.c
	$(CC_FLAGS) $(NDSRC)/ARC.c

OPTIM.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/OPTIM.c
	$(CC_FLAGS) $(NDSRC)/OPTIM.c

xprdct_mngr.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/xprdct_mngr.c
	$(CC_FLAGS) $(NDSRC)/xprdct_mngr.c

optim_lo.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/bstr.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/optim_lo.c
	$(CC_FLAGS) $(NDSRC)/optim_lo.c

single.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxYxZ.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(ND)/incl/SS.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/single.c
	$(CC_FLAGS) $(NDSRC)/single.c

ambig.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/bstr.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(ND)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(NDSRC)/ambig.c
	$(CC_FLAGS) $(NDSRC)/ambig.c

XARC.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(ND)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/XARC.c
	$(CC_FLAGS) $(NDSRC)/XARC.c

ARP.o:		$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(ND)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/ARP.c
	$(CC_FLAGS) $(NDSRC)/ARP.c

RARP.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(ND)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/RARP.c
	$(CC_FLAGS) $(NDSRC)/RARP.c

earley.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(ND)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/earley.c
	$(CC_FLAGS) $(NDSRC)/earley.c

first.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(ND)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/first.c
	$(CC_FLAGS) $(NDSRC)/first.c

messages_mngr.o:	$(NDINCL)/csynt_optim.h	\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/bstr.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(ND)/incl/SS.h			\
			$(NDINCL)/RLR.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/messages_mngr.c
	$(CC_FLAGS) $(NDSRC)/messages_mngr.c

super_FSA.o:	$(ND)/incl/XxY.h		\
			$(ND)/incl/X.h			\
			$(SXINCL)/sxba.h		\
			$(NDSRC)/super_FSA.c
	$(CC_FLAGS) $(NDSRC)/super_FSA.c

algoV.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/algoV.c
	$(CC_FLAGS) $(NDSRC)/algoV.c


# csyntd_SOURCES =	$(NDSRC)/csyntd_main.c
# csyntd_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

# csyntd_main.o:	$(ND)/incl/sxunix.h		\
# 		$(ND)/incl/sxdynam_parser.h		\
# 		$(NDSRC)/csyntd_main.c
# 	$(CC2_FLAGS) $(NDSRC)/csyntd_main.c



lc_SOURCES =	$(NDSRC)/left_corner.c		\
		$(NDSRC)/LC0.c			\
		$(NDSRC)/first.c		\
		$(NDSRC)/single.c		\
		$(NDSRC)/algoV.c		\
		$(NDSRC)/optim_lo.c		\
		$(NDSRC)/xprdct_mngr.c		\
		$(NDSRC)/OPTIM.c
lc_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a


left_corner.o:	$(NDINCL)/csynt_optim.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/bstr.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/XH.h			\
			$(ND)/incl/X.h			\
			$(NDETC)/incl/release.h		\
			$(NDSRC)/left_corner.c
	$(CC_FLAGS) $(NDSRC)/left_corner.c

LC0.o:	$(NDINCL)/csynt_optim.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXETCINCL)/B_tables.h		\
			$(ND)/incl/XxYxZ.h		\
			$(ND)/incl/XxY.h		\
			$(ND)/incl/SS.h			\
			$(SXETCINCL)/P_tables.h		\
			$(NDSRC)/LC0.c
	$(CC_FLAGS) $(NDSRC)/LC0.c

