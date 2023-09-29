# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
LNorCP=${SHELL} $(SX)/etc/bin/lien-relatif.sh
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
CX=$(SX)/examples/cx
SRC=$(CX)/src
LIB=$(CX)/lib

BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a
COMPILER=cc
CC=cd $(LIB); umask 7; exec $(COMPILER) -I$(SXINCL) -I$(SX)/etc/incl -Dsxtables=cx_tables $(CFLAGS) -c

all: $(SX)/bin/cx.out

$(CX)/ALL .PRECIOUS:	$(SX)/bin/cx.out

$(SX)/bin/cx.out:	$(CX)/cx.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=		$(LIB)/cx_t.o			\
		$(LIB)/cx_main.o		\
		$(LIB)/cx_act.o			\
		$(LIB)/cx_scnr.o

$(CX)/cx.out:	$(NEEDS)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; rm -f $@; ${SHELL} -c '$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(LIB)/cx_t.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/cx_t.c
	$(CC) $(SRC)/cx_t.c			\
			-Dsxtables=cx_tables	\
			-Dsxscanner=cx_scanner	\
			-Dsxscan_it=cx_scan_it	\
			-DSEMACT=cx_act		\
			-Dsempass=sxvoid

$(LIB)/cx_main.o:	$(SXINCL)/sxunix.h	\
			$(ETC)/incl/release.h	\
			$(SRC)/cx_main.c
	$(CC) $(SRC)/cx_main.c

$(LIB)/cx_scnr.o:	$(SXINCL)/sxunix.h	\
			$(SRC)/cx_scnr.c
	$(CC) $(SRC)/cx_scnr.c

$(LIB)/cx_act.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/put_edit.h	\
			$(SRC)/cx_act.c
	$(CC) $(SRC)/cx_act.c

clean:
	rm -f *~ $(LIB)/*.o
