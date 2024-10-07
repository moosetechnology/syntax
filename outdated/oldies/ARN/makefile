
include $(SX)/etc/make/gccoptions

ARN=$(SX)/../outdated/oldies/ARN
LIB=$(ARN)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=cc -I$(SXINCL) -I$(SX)/incl/util -I$(SXEXTINCL) $(CFLAGS) -DEBUG

all:	$(ARN)/ARN.out

NEEDS=  $(LIB)/ARN_no_parsact.o		\
	$(LIB)/ARN_t.o

all $(ARN)/ARN.out:	$(NEEDS)	$(SXEXTBIBS)
	cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@;exit 1)

$(LIB)/ARN_no_parsact.o:	$(SXINCL)/sxunix.h 	\
			ARN_parsact.h 	\
			ARN_no_parsact.c
	$(CC) -o $@ -c ARN_no_parsact.c

$(LIB)/ARN_t.o:		$(SXINCL)/sxunix.h 	\
			ARN_t.c
	$(CC) -o $@ -c ARN_t.c		\
		-DSEMACT=sxwvoid		\
	        -DESAMBIG=sxvoid_int		\
	        -DPARSACT_2=sxvoid

clean:
	/bin/rm -f $(LIB)/ARN_no_parsact.o $(LIB)/ARN_t.o 


