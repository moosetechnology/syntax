
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

# debut de code produit par "make-makefile lig"

lig_t.c: lig.bt lig.st lig.pt lig.rt lig.att
	tables_c lig > $@

lig.bt lig.att lig_mix.c: lig.at
	semat lig.at  > lig_mix.c

lig.st: lig.bt lig.lecl
	lecl lig.lecl || [ $$? = 1 ]

lig.pt: lig.bt
	csynt lig || [ $$? = 1 ]

lig.rt: lig.bt lig.st lig.recor
	recor lig.recor

# fin de code produit par "make-makefile lig"

clean:
	rm -f $(ALL) $(NEEDS1)
	/bin/rm -f lig.*.l
	/bin/rm -f lig.att lig.bt lig.pt lig.rt lig.st
	/bin/rm -f lig_mix.c lig_t.c

