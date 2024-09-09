
include $(SX)/etc/make/gccoptions

LIG=$(SX)/../outdated/oldies/LIG
SRC=$(LIG)/src
LIB=$(LIG)/lib.$(ARCH)
INCL=$(LIG)/incl

CFLAGS= -g -DEBUG_ALLOC -DEBUG

CC=cc -I$(INCL) -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../outdated/oldies/incl $(CFLAGS)

ALL=$(LIG)/lig2bnf.out

all: $(ALL)

NEEDS1=$(LIB)/lig_main.o		\
	$(LIB)/lig_sact.o		\
	$(LIB)/lig_smp.o    		\
	$(LIB)/lig_t.o

$(LIG)/lig2bnf.out:	$(NEEDS1)	$(SXBIBS)
	cc -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/lig_main.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/lig.h	 	\
			$(SRC)/lig_main.c
	$(CC) -o $@ -c $(SRC)/lig_main.c

$(LIB)/lig_sact.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/lig.h	 	\
			$(SRC)/lig_sact.c
	$(CC) -o $@ -c $(SRC)/lig_sact.c

$(LIB)/lig_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XH.h		\
			$(INCL)/lig.h	 	\
			$(SX)/../outdated/oldies/incl/SXLIGparsact.h 	\
			$(SRC)/lig_smp.c
	$(CC) -o $@ -c -I$(SX)/../outdated/oldies/incl $(SRC)/lig_smp.c

$(LIB)/lig_t.o:		$(SXINCL)/sxunix.h 	\
			$(SRC)/lig_t.c
	$(CC) -o $@ -c $(SRC)/lig_t.c		\
		-DSEMPASS=lig_smp

clean:
	rm -f $(ALL) $(NEEDS1)

