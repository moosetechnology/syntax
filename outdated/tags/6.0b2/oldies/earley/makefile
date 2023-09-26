# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
ETCINCL=$(ETC)/incl

EARLEY=$(SX)/earley
SRC=$(EARLEY)/src
LIB=$(EARLEY)/lib
INCL=$(EARLEY)/incl

BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a



CC=cd $(LIB); umask 7; exec cc -I$(INCL) -I$(SXINCL) -I$(ETC)/incl $(CFLAGS) -c

$(EARLEY)/everything .PRECIOUS:	$(EARLEY)/earley.out	\
				$(EARLEY)/earley_LC_OK.out \
				$(EARLEY)/earley_LC.out



NEEDS=\
	$(LIB)/earley.o	

NEEDS_LC=\
	$(LIB)/earley_LC.o

N_LC_OK=\
	$(LIB)/earley_LC_OK.o

$(EARLEY)/earley.out:	$(NEEDS)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(LIB)/earley.o:	$(SXINCL)/XxY.h	\
			$(SRC)/earley.c
	$(CC) $(SRC)/earley.c

$(EARLEY)/earley_LC.out:	$(NEEDS_LC)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS_LC) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(LIB)/earley_LC.o:	$(SXINCL)/XxY.h	\
			$(SRC)/earley_LC.c
	$(CC) $(SRC)/earley_LC.c

$(EARLEY)/earley_LC_OK.out:	$(N_LC_OK)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(N_LC_OK) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/earley_LC_OK.o:	$(SXINCL)/XxY.h	\
			$(SRC)/earley_LC_OK.c
	$(CC) $(SRC)/earley_LC_OK.c

