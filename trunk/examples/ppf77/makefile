
include $(SX)/etc/make/gccoptions

PPF77=$(SX)/examples/ppf77
BIN=$(PPF77)/bin.$(ARCH)
LIB=$(PPF77)/lib.$(ARCH)

OPTIONS=-rhs 11

# uncomment the definitions below when debugging is needed
# CFLAGS_SXSCANNER_DEBUG=-DBUG=1
# CFLAGS_SXPARSER_DEBUG=-DBUG=1
# CFLAGS_ANALYSER_DEBUG=-DBUG=1 -DEBUG=1
# CFLAGS_PPF77_DEBUG=-DEBUG=1
# CFLAGS_SXPPP_DEBUG=-DBUG=1
# CFLAGS_SXATCPP_DEBUG=-DBUG=1

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -c

# CC=$(COMPILER) -g -DEBUG=1 -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -c

all: $(BIN)/ppf77.out

# object files needed to build ppf77.out

NEEDS=	$(LIB)/ppf77_t.o		\
	$(LIB)/ppf77_main.o		\
	$(LIB)/sxscanner.o		\
	$(LIB)/sxs_rcvr.o		\
	$(LIB)/sxparser.o		\
	$(LIB)/sxp_rcvr.o		\
	$(LIB)/sxppp.o			\
	$(LIB)/sxatcpp.o		\
	$(LIB)/f77_analyse.o

$(BIN)/ppf77.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/ppf77_t.o: 	$(SXINCL)/sxunix.h		\
			ppf77_t.c
	$(CC) -o $@ ppf77_t.c

$(LIB)/ppf77_main.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/varstr.h		\
			ppf77_td.h			\
			ppf77_main.c
	$(CC) -o $@ ppf77_main.c			\
			$(CFLAGS_PPF77_DEBUG)

$(LIB)/sxscanner.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxsstmt.h		\
			$(SXSRC)/sxscanner.c
	$(CC) -o $@ $(SXSRC)/sxscanner.c			\
			-Dsxnextchar=f77_next_char	\
			-Dsxnext_char=f77_next_char	\
			-Dsxlanext_char=f77_lanext_char	\
			-Dsxlafirst_char=f77_lafirst_char	\
			-Dsxlaback=f77_laback		\
			-DsxX=f77_X			\
			-Dsxsrcpush=f77_srcpush		\
			$(CFLAGS_SXSCANNER_DEBUG)

$(LIB)/sxs_rcvr.o:	$(SXINCL)/sxunix.h		\
			$(SXSRC)/sxs_rcvr.c
	$(CC) -o $@ $(SXSRC)/sxs_rcvr.c			\
			-Dsxnextchar=f77_next_char	\
			-Dsxnext_char=f77_next_char	\
			-Dsxlanext_char=f77_lanext_char	\
			-Dsxlafirst_char=f77_lafirst_char	\
			-Dsxlaback=f77_laback		\
			-DsxX=f77_X			\
			-Dsxsrcpush=f77_srcpush

$(LIB)/sxparser.o:	$(SXINCL)/sxunix.h		\
			$(SXSRC)/sxparser.c
	$(CC) -o $@ $(SXSRC)/sxparser.c			\
			$(CFLAGS_SXPARSER_DEBUG)

$(LIB)/sxp_rcvr.o:	$(SXINCL)/sxunix.h		\
			$(SXSRC)/sxp_rcvr.c
	$(CC) -o $@ $(SXSRC)/sxp_rcvr.c

$(LIB)/f77_analyse.o:	$(SXINCL)/sxunix.h		\
			f77_td.h		\
			f77_analyse.c
	$(CC) -o $@ f77_analyse.c			\
			$(CFLAGS_ANALYSER_DEBUG)

$(LIB)/sxppp.o:		$(SXINCL)/sxunix.h		\
			$(SXSRC)/sxppp.c
	$(CC) -o $@ $(SXSRC)/sxppp.c				\
			-DUSERS_COMMENTS=1		\
			$(CFLAGS_SXPPP_DEBUG)

$(LIB)/sxatcpp.o:	$(SXINCL)/sxunix.h		\
			$(SXSRC)/sxatcpp.c
	$(CC) -o $@ $(SXSRC)/sxatcpp.c			\
			-DBEFORE_PPTREE_WALK=f77_process_postponed_comments \
			$(CFLAGS_SXATCPP_DEBUG)

# debut de code produit par "make-makefile ppf77"

# ppf77_t.c: ppf77.bt ppf77.st ppf77.pt ppf77.rt ppf77.ppt
ppf77_t.c: ppf77.st ppf77.pt ppf77.rt ppf77.ppt
	tables_c ppf77 > $@

ppf77.bt ppf77.ppt : ppf77.paradis
	paradis $(OPTIONS) ppf77.paradis 

ppf77.st: ppf77.bt ppf77.lecl
	lecl ppf77.lecl || [ $$? = 1 ]

ppf77.pt: ppf77.bt ppf77.dt
	csynt ppf77 || [ $$? = 1 ]

ppf77.dt: ppf77.bt ppf77.prio
	prio ppf77.prio

ppf77.rt: ppf77.bt ppf77.st ppf77.recor
	recor ppf77.recor

ppf77_td.h: ppf77.bt ppf77.tdef
	tdef ppf77.tdef > $@

f77_td.h: ppf77_td.h
	@ln -s ppf77_td.h f77_td.h

# fin de code produit par "make-makefile ppf77"

clean:
	rm -f $(BIN)/*.out $(LIB)/*.o
	rm -f ppf77.bt ppf77.ppt ppf77.dt ppf77.pt ppf77.st ppf77.rt ppf77_t.c
	rm -f ppf77.bn.l ppf77.la.l ppf77.lc.l ppf77.pr.l ppf77.rc.l
	rm -f ppf77.td.l ppf77_td.h f77_td.h


