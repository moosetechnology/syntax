
include $(SX)/etc/make/gccoptions

LIG=$(SX)/../outdated/oldies/LIG
LIB=$(LIG)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../outdated/oldies/incl $(CFLAGS) -DEBUG

ALL=$(LIG)/lig2bnf.out

all: $(ALL)

NEEDS1=$(LIB)/lig_main.o		\
	$(LIB)/lig_sact.o		\
	$(LIB)/lig_smp.o    		\
	$(LIB)/lig_t.o

$(LIG)/lig2bnf.out:	$(NEEDS1)	$(SXBIBS)
	cc -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@;exit 1)

$(LIB)/lig_main.o:	$(SXINCL)/sxunix.h	\
			lig.h	 	\
			lig_main.c
	$(CC) -o $@ -c lig_main.c

$(LIB)/lig_sact.o:	$(SXINCL)/sxunix.h	\
			lig.h	 	\
			lig_sact.c
	$(CC) -o $@ -c lig_sact.c

$(LIB)/lig_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XH.h		\
			lig.h	 	\
			$(SX)/../outdated/oldies/incl/SXLIGparsact.h 	\
			lig_smp.c
	$(CC) -o $@ -c -I$(SX)/../outdated/oldies/incl lig_smp.c

$(LIB)/lig_t.o:		$(SXINCL)/sxunix.h 	\
			lig_t.c
	$(CC) -o $@ -c lig_t.c		\
		-DSEMPASS=lig_smp

clean:
	rm -f $(ALL) $(NEEDS1)

