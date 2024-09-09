
include $(SX)/etc/make/gccoptions

RCG=$(SX)/../extensions/rcg

LIB=$(RCG)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

RCG_CFLAGS= 
# RCG_CFLAGS= -g -DEBUG -DSTATS

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../extensions/incl $(RCG_CFLAGS) $(CFLAGS)

all: $(SXEXTBIN)/rcg.out

NEEDS=\
	$(LIB)/rcg_main.o		\
	$(LIB)/rcg_sact.o		\
	$(LIB)/rcg_smp.o    		\
	$(LIB)/rcg_1rcg_2var.o 		\
	$(LIB)/rcg_gen_parser.o		\
	$(LIB)/rcg_gen_bnf.o   		\
	$(LIB)/rcg_t.o

$(SXEXTBIN)/rcg.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(RCG_CFLAGS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/rcg_main.o:	$(SXINCL)/sxunix.h	\
			rcg_td.h	\
			rcg.h		\
			rcg_main.c
	$(CC) -o $@ -c rcg_main.c

$(LIB)/rcg_sact.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/varstr.h	\
			rcg_td.h	\
			rcg.h		\
			rcg_sact.c
	$(CC) -o $@ -c rcg_sact.c

$(LIB)/rcg_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/put_edit.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			rcg_td.h	\
			rcg.h		\
			rcg_smp.c
	$(CC) -o $@ -c rcg_smp.c
#			-DEBUG_PRDCT=4 (code du prdct que l'on veut tracer)

$(LIB)/rcg_1rcg_2var.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			rcg_td.h	\
			rcg.h		\
			rcg_1rcg_2var.c
	$(CC) -o $@ -c rcg_1rcg_2var.c

$(LIB)/rcg_gen_parser.o:			\
			$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			rcg_td.h		\
			rcg.h			\
			$(SX)/../extensions/incl/rcg_sglbl.h	\
			rcg_gen_parser.c
	$(CC) -o $@ -c rcg_gen_parser.c

$(LIB)/rcg_gen_bnf.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			rcg_td.h		\
			rcg.h			\
			rcg_gen_bnf.c
	$(CC) -o $@ -c rcg_gen_bnf.c

$(LIB)/rcg_t.o:		$(SXINCL)/sxunix.h 	\
			rcg_t.c
	$(CC) -o $@ -c rcg_t.c			\
		-DSEMPASS=rcg_smp

# debut de code produit par "make-makefile rcg"

rcg_t.c: rcg.bt rcg.st rcg.pt rcg.rt rcg.att
	tables_c rcg > $@

rcg.bt rcg.att rcg_mix.c: rcg.at
	semat rcg.at > rcg_mix.c

rcg.st: rcg.bt rcg.lecl
	lecl rcg.lecl || [ $$? = 1 ]

rcg.pt: rcg.bt
	csynt rcg || [ $$? = 1 ]

rcg.rt: rcg.bt rcg.st rcg.recor
	recor rcg.recor

rcg_td.h: rcg.bt rcg.tdef
	tdef rcg.tdef > $@

# fin de code produit par "make-makefile rcg"

clean:
	/bin/rm -f $(LIB)/*.o $(SXEXTBIN)/rcg.out
	/bin/rm -f rcg.*.l
	/bin/rm -f rcg.att rcg.bt rcg.pt rcg.rt rcg.st rcg_mix.c rcg_t.c rcg_td.h


