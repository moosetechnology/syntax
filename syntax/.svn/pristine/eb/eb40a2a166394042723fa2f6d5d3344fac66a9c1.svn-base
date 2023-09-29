# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
LNorCP=${SHELL} $(SX)/etc/bin/lien-relatif.sh
SXLIB=$(SX)/lib
SXINCL=$(sx)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
F77=$(SX)/examples/f77
SRC=$(F77)/src
LIB=$(F77)/lib
INCL=$(F77)/incl

BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a
COMPILER=cc
CC=cd $(LIB); umask 7; exec $(COMPILER)  -I$(INCL) -I$(SXINCL) -I$(SX)/etc/incl $(CFLAGS) -c

all: $(F77)/everything

$(F77)/everything .PRECIOUS:	$(F77)/f77.out		\
				$(SX)/bin/ppf77.out


$(SX)/bin/ppf77.out:	$(F77)/ppf77.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


NEEDS=	$(LIB)/f77_t.o			\
	$(LIB)/f77_main.o		\
	$(LIB)/sxs_rcvr.o		\
	$(LIB)/f77_analyse.o

PP=	$(LIB)/ppf77_t.o		\
	$(LIB)/ppf77_main.o		\
	$(LIB)/sxs_rcvr.o		\
	$(LIB)/f77_analyse.o


$(F77)/f77.out:	$(NEEDS)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; rm -f $@; ${SHELL} -c '$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(LIB)/f77_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/f77_t.c
	$(CC) $(SRC)/f77_t.c				\
			-Dsxtables=f77_tables		\
			-DSCANACT=f77_scanact		\
			-DPARSACT=f77_parsact		\
			-DSEMACT=sxvoid

$(LIB)/f77_main.o:	$(SXINCL)/sxunix.h		\
			$(SRC)/f77_main.c
	$(CC) $(SRC)/f77_main.c

$(LIB)/sxscanner.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxsstmt.h		\
			$(SX)/src/sxscanner.c
	$(CC) $(SX)/src/sxscanner.c			\
			-Dsxnextchar=f77_next_char	\
			-Dsxnext_char=f77_next_char	\
			-Dsxlanext_char=f77_lanext_char	\
			-Dsxlafirst_char=f77_lafirst_char	\
			-Dsxlaback=f77_laback		\
			-DsxX=f77_X			\
			-Dsxsrcpush=f77_srcpush

$(LIB)/sxs_rcvr.o:	$(SXINCL)/sxunix.h		\
			$(SX)/src/sxs_rcvr.c
	$(CC) $(SX)/src/sxs_rcvr.c			\
			-Dsxnextchar=f77_next_char	\
			-Dsxnext_char=f77_next_char	\
			-Dsxlanext_char=f77_lanext_char	\
			-Dsxlafirst_char=f77_lafirst_char	\
			-Dsxlaback=f77_laback		\
			-DsxX=f77_X			\
			-Dsxsrcpush=f77_srcpush

$(LIB)/f77_analyse.o:	$(SXINCL)/sxunix.h		\
			$(INCL)/f77_td.h		\
			$(SRC)/f77_analyse.c
	$(CC) $(SRC)/f77_analyse.c


$(F77)/ppf77.out:	$(PP)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(PP) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'


$(LIB)/ppf77_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/ppf77_t.c
	$(CC) $(SRC)/ppf77_t.c				\
			-Dsxtables=ppf77_tables		\
			-DSCANACT=f77_scanact		\
			-DPARSACT=f77_parsact

$(LIB)/ppf77_main.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/varstr.h		\
			$(SRC)/ppf77_main.c
	$(CC) $(SRC)/ppf77_main.c



clean:
	rm -fr *~ $(LIB)/*.o
