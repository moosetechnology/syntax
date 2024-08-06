
include $(SX)/etc/make/gccoptions

ARN=$(SX)/../outdated/oldies/ARN
SRC=$(ARN)/src
LIB=$(ARN)/lib.$(ARCH)
INCL=$(ARN)/incl

CC=cc -I$(INCL) -I$(SXINCL) -I$(SX)/incl/util -I$(SX)/../extensions/incl $(CFLAGS) -DEBUG

all:	$(ARN)/ARN.out

NEEDS=  $(LIB)/ARN_no_parsact.o		\
	$(LIB)/ARN_t.o

all $(ARN)/ARN.out:	$(NEEDS)	$(SXBIBS)
	cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/ARN_no_parsact.o:	$(SXINCL)/sxunix.h 	\
			$(INCL)/ARN_parsact.h 	\
			$(SRC)/ARN_no_parsact.c
	$(CC) -o $@ -c $(SRC)/ARN_no_parsact.c

$(LIB)/ARN_t.o:		$(SXINCL)/sxunix.h 	\
			$(SRC)/ARN_t.c
	$(CC) -o $@ -c $(SRC)/ARN_t.c		\
		-DPARSACT=ARN_parsact		\
		-DSEMACT=sxwvoid			\
	        -DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

clean:
	/bin/rm -f $(LIB)/ARN_no_parsact.o $(LIB)/ARN_t.o 


