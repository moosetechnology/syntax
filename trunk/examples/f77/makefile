
include $(SX)/etc/make/gccoptions

F77=$(SX)/examples/f77
BIN=$(F77)/bin
LIB=$(F77)/lib

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

all: $(BIN)/f77.out $(BIN)/ppf77.out

# object files needed to build f77.out

NEEDS=	$(LIB)/f77_t.o			\
	$(LIB)/f77_main.o		\
	$(LIB)/sxscanner.o		\
	$(LIB)/sxs_rcvr.o		\
	$(LIB)/sxparser.o		\
	$(LIB)/sxp_rcvr.o		\
	$(LIB)/f77_analyse.o

# object files needed to build ppf77.out

PP=	$(LIB)/ppf77_t.o		\
	$(LIB)/ppf77_main.o		\
	$(LIB)/sxscanner.o		\
	$(LIB)/sxs_rcvr.o		\
	$(LIB)/sxparser.o		\
	$(LIB)/sxp_rcvr.o		\
	$(LIB)/sxppp.o			\
	$(LIB)/sxatcpp.o		\
	$(LIB)/f77_analyse.o


$(BIN)/f77.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/f77_t.o: 	$(SXINCL)/sxunix.h		\
			f77_t.c
	$(CC) -o $@ f77_t.c				\
			-Dsxtables=f77_tables		\
			-DSCANACT=f77_scanact		\
			-DPARSACT=f77_parsact		\
			-DSEMACT=sxvoid			\
			-DSCANACT_AND_SEMACT_ARE_IDENTICAL

$(LIB)/f77_main.o:	$(SXINCL)/sxunix.h		\
			f77_main.c
	$(CC) -o $@ f77_main.c

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


$(BIN)/ppf77.out:	$(PP)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(PP) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)


$(LIB)/ppf77_t.o: 	$(SXINCL)/sxunix.h		\
			ppf77_t.c
	$(CC) -o $@ ppf77_t.c				\
			-Dsxtables=ppf77_tables		\
			-DSCANACT=f77_scanact		\
			-DPARSACT=f77_parsact

$(LIB)/ppf77_main.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/varstr.h		\
			f77_td.h			\
			ppf77_main.c
	$(CC) -o $@ ppf77_main.c			\
			$(CFLAGS_PPF77_DEBUG)

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

# debut de code produit par "make-makefile f77"

# f77_t.c: f77.bt f77.st f77.pt f77.rt
f77_t.c: f77.st f77.pt f77.rt 
	tables_c f77 > $@

f77.bt  : f77.bnf
	bnf $(OPTIONS) f77.bnf 

f77.st: f77.bt f77.lecl
	lecl f77.lecl || [ $$? = 1 ]

f77.pt: f77.bt f77.dt
	csynt f77

f77.dt: f77.bt f77.prio
	prio f77.prio

f77.rt: f77.bt f77.st f77.recor
	recor f77.recor

f77_td.h: f77.bt f77.tdef
	tdef f77.tdef > $@

# fin de code produit par "make-makefile f77"

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

# fin de code produit par "make-makefile ppf77"

clean:
	rm -f $(BIN)/*.out $(LIB)/*.o
	rm -f f77.bt f77_td.h f77.dt f77.pt f77.st f77.rt f77_t.c
	rm -f f77.bn.l f77.la.l f77.lc.l f77.pr.l f77.rc.l f77.td.l 
	rm -f ppf77.bt ppf77.ppt ppf77.dt ppf77.pt ppf77.st ppf77.rt ppf77_t.c
	rm -f ppf77.bn.l ppf77.la.l ppf77.lc.l ppf77.pr.l ppf77.rc.l

