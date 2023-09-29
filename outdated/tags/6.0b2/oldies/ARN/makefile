# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
ETCINCL=$(ETC)/incl

ARN=$(SX)/oldies/ARN
SRC=$(ARN)/src
LIB=$(ARN)/lib
INCL=$(ARN)/incl

BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a

CC=cd $(LIB); umask 7; exec cc -I$(INCL) -I$(SXINCL) -I$(ETC)/incl $(CFLAGS) -DEBUG -c

$(ARN)/everything .PRECIOUS:	$(ARN)/ARN.out


NEEDS=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/ARN_no_parsact.o		\
	$(LIB)/ARN_t.o

$(ARN)/ARN.out:	$(NEEDS)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(BIBS) || (rm $@;exit 1)'

$(LIB)/ARN_no_parsact.o:	$(SXINCL)/sxunix.h 	\
			$(ARN)/force-make 	\
			$(INCL)/ARN_parsact.h 	\
			$(SRC)/ARN_no_parsact.c
	$(CC) $(SRC)/ARN_no_parsact.c

$(LIB)/ARN_t.o:		$(SXINCL)/sxunix.h 	\
			$(ARN)/force-make 	\
			$(SRC)/ARN_t.c
	$(CC) $(SRC)/ARN_t.c			\
		-DPARSACT=ARN_parsact		\
		-DSEMACT=sxvoid			\
	        -DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

