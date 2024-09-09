
include $(SX)/etc/make/gccoptions

# F77=$(SX)/examples/f77
# F77=$(SX)/examples/j77
F77=$(SX)/examples/esope77
BIN=$(F77)/bin.$(ARCH)
LIB=$(F77)/lib.$(ARCH)

OPTIONS=-rhs 11

# uncomment the definitions below when debugging is needed
# CFLAGS_SXSCANNER_DEBUG=-DBUG=1
# CFLAGS_SXPARSER_DEBUG=-DBUG=1
# CFLAGS_ANALYSER_DEBUG=-DBUG=1 -DEBUG=1

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -c

# CC=$(COMPILER) -g -DEBUG=1 -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -c

all: $(BIN)/f77.out

# object files needed to build f77.out

NEEDS=	$(LIB)/f77_s.o			\
	$(LIB)/f77_t.o			\
	$(LIB)/f77_main.o		\
	$(LIB)/sxscanner.o		\
	$(LIB)/sxs_rcvr.o		\
	$(LIB)/sxparser.o		\
	$(LIB)/sxp_rcvr.o		\
	$(LIB)/f77_analyse.o

$(BIN)/f77.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) -lm || (rm $@; exit 1)

$(LIB)/f77_s.o: 	$(SXINCL)/sxunix.h		\
			f77_s.c
	$(CC) -o $@ f77_s.c

$(LIB)/f77_t.o: 	$(SXINCL)/sxunix.h		\
			f77_t.c
	$(CC) -o $@ f77_t.c				\
			-DSEMACT=f77_act

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

# debut de code produit par "make-makefile f77"

# f77_t.c: f77.bt f77.st f77.pt f77.rt
f77_t.c: f77.st f77.pt f77.rt 
	tables_c f77 > $@

f77_s.c:  f77.semc
	semc $(OPTIONS) f77.semc > $@

f77.bt  : f77.semc
	bnf $(OPTIONS) f77.semc 

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

clean:
	rm -f $(BIN)/*.out $(LIB)/*.o
	rm -f f77.bt f77_td.h f77.dt f77.pt f77.st f77.rt f77_s.c f77_t.c
	rm -f f77.bn.l f77.la.l f77.lc.l f77.pr.l f77.rc.l f77.td.l 

