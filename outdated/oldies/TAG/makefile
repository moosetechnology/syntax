
include $(SX)/etc/make/gccoptions

TAG=$(SX)/../outdated/oldies/TAG
LIB=$(TAG)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXINCL)/util -I$(TAG)/../incl $(CFLAGS)

ALL=$(TAG)/xtag2lig.out

all: $(ALL)

NEEDS1=$(LIB)/xtag_main.o		\
       $(LIB)/xtag_sact.o		\
       $(LIB)/xtag_smp.o    		\
       $(LIB)/xtag_t.o

$(TAG)/xtag2lig.out:	$(NEEDS1)	$(SXBIBS)
	cc -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@;exit 1)

$(LIB)/xtag_main.o:	$(SXINCL)/sxunix.h	\
			xtag.h	 	\
			xtag_main.c
	$(CC) -o $@ -c xtag_main.c

$(LIB)/xtag_sact.o:	$(SXINCL)/sxunix.h	\
			xtag.h	 	\
			xtag_sact.c
	$(CC) -o $@ -c xtag_sact.c

$(LIB)/xtag_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/SS.h		\
			$(SXINCL)/varstr.h	\
			xtag.h	 	\
			xtag_smp.c
	$(CC) -o $@ -c xtag_smp.c

$(LIB)/xtag_t.o:	$(SXINCL)/sxunix.h 	\
			xtag_t.c
	$(CC) -o $@ -c xtag_t.c		\
		-DSEMACT=sxwvoid			\
		-DSEMPASS=xtag_smp

# debut de code produit par "make-makefile xtag"

xtag_t.c: xtag.bt xtag.st xtag.pt xtag.rt xtag.att
	tables_c xtag > $@

xtag.bt xtag.att xtag_mix.c: xtag.at
	semat xtag.at  > xtag_mix.c

xtag.st: xtag.bt xtag.lecl
	lecl xtag.lecl || [ $$? = 1 ]

xtag.pt: xtag.bt
	csynt xtag || [ $$? = 1 ]

xtag.rt: xtag.bt xtag.st xtag.recor
	recor xtag.recor

# fin de code produit par "make-makefile dag"

clean:
	rm -f $(ALL) $(NEEDS1)
	/bin/rm -f xtag.*.l
	/bin/rm -f xtag.att xtag.bt xtag.pt xtag.rt xtag.st
	/bin/rm -f xtag_mix.c xtag_t.c


