
include $(SX)/etc/make/gccoptions

RCG=$(SX)/../extensions/drcg
LIB=$(RCG)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

RCG_CFLAGS= 
# RCG_CFLAGS= -g -DEBUG -DSTATS

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(RCG_CFLAGS) $(CFLAGS)

all: $(SXEXTBIN)/drcg.out

NEEDS=\
 	$(LIB)/drcg_sact.o		\
	$(LIB)/drcg_main.o		\
	$(LIB)/drcg_smp.o    		\
	$(LIB)/drcg_t.o

$(SXEXTBIN)/drcg.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(RCG_CFLAGS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/drcg_sact.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/varstr.h	\
			drcg_td.h	\
			rcg.h		\
			drcg_sact.c
	$(CC) -o $@ -c drcg_sact.c

$(LIB)/drcg_main.o:	$(SXINCL)/sxunix.h	\
			drcg_td.h	\
			rcg.h			\
			drcg_main.c
	$(CC) -o $@ -c drcg_main.c

$(LIB)/drcg_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			drcg_td.h		\
			rcg.h			\
			$(SXEXTINCL)/dcg_cst.h	\
			drcg_smp.c
	$(CC) -o $@ -c drcg_smp.c

$(LIB)/drcg_t.o:	$(SXINCL)/sxunix.h 	\
			drcg_t.c	
	$(CC) -o $@ -c drcg_t.c			\
		-DSEMPASS=drcg_smp

# debut de code produit par "make-makefile drcg"

drcg_t.c: drcg.bt drcg.st drcg.pt drcg.rt drcg.att
	tables_c drcg > $@

drcg.bt drcg.att drcg_mix.c: drcg.at
	semat drcg.at > drcg_mix.c

drcg.st: drcg.bt drcg.lecl
	lecl drcg.lecl || [ $$? = 1 ]

drcg.pt: drcg.bt
	csynt drcg || [ $$? = 1 ]

drcg.rt: drcg.bt drcg.st drcg.recor
	recor drcg.recor

drcg_td.h: drcg.bt drcg.tdef
	tdef drcg.tdef > $@

# fin de code produit par "make-makefile drcg"

clean:
	/bin/rm -f $(LIB)/*.o $(SXEXTBIN)/drcg.out
	/bin/rm -f drcg.*.l
	/bin/rm -f drcg.att drcg.bt drcg.pt drcg.rt drcg.st drcg_mix.c drcg_t.c

