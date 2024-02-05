
include $(SX)/etc/make/gccoptions

RCG=$(SX)/../extensions/rcg
LIB=$(RCG)/lib

RCG_CFLAGS= 
# RCG_CFLAGS= -g -DEBUG -DSTATS

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../extensions/incl $(RCG_CFLAGS) $(CFLAGS)

all: $(RCG)/rcg.out \
     $(RCG)/drcg.out

# $(SXBIN)/rcg.out:	$(RCG)/rcg.out
#	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS1=\
	$(LIB)/rcg_main.o		\
	$(LIB)/rcg_sact.o		\
	$(LIB)/rcg_smp.o    		\
	$(LIB)/rcg_1rcg_2var.o 		\
	$(LIB)/rcg_gen_parser.o		\
	$(LIB)/rcg_gen_bnf.o   		\
	$(LIB)/rcg_t.o

$(RCG)/rcg.out:	$(NEEDS1)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS1) $(RCG_CFLAGS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/rcg_main.o:	$(SXINCL)/sxunix.h	\
			rcg.h		\
			rcg_main.c
	$(CC) -o $@ -c rcg_main.c

$(LIB)/rcg_sact.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/varstr.h	\
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
		-Dsxtables=rcg_tables		\
		-DSCANACT=rcg_scan_act		\
		-Dsempass=rcg_smp

$(SXBIN)/drcg.out:	$(RCG)/drcg.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS2=\
	$(LIB)/drcg_main.o		\
	$(LIB)/rcg_sact.o		\
	$(LIB)/rcg_smp.o    		\
	$(LIB)/rcg_1rcg_2var.o 		\
	$(LIB)/rcg_gen_parser.o		\
	$(LIB)/rcg_gen_bnf.o   		\
	$(LIB)/rcg_t.o			\
	$(LIB)/drcg_smp.o    		\
	$(LIB)/drcg_t.o


$(RCG)/drcg.out:	$(NEEDS2)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS2) $(RCG_CFLAGS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)


$(LIB)/drcg_main.o:	$(SXINCL)/sxunix.h	\
			rcg.h			\
			drcg_main.c
	$(CC) -o $@ -c drcg_main.c



$(LIB)/drcg_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			rcg_td.h		\
			rcg.h			\
			$(SXINCL)/dcg_cst.h	\
			drcg_smp.c
	$(CC) -o $@ -c drcg_smp.c

$(LIB)/drcg_t.o:	$(SXINCL)/sxunix.h 	\
			drcg_t.c	
	$(CC) -o $@ -c drcg_t.c			\
		-Dsxtables=drcg_tables		\
		-DSCANACT=rcg_scan_act		\
		-Dsempass=drcg_smp

# debut de code produit par "make-makefile rcg"

rcg_t.c: rcg.bt rcg.st rcg.pt rcg.rt rcg.att
	tables_c rcg > $@

rcg.bt rcg.att rcg_mix.c: rcg.at
	semat rcg.at  > rcg_mix.c

rcg.st: rcg.bt rcg.lecl
	lecl rcg.lecl || [ $$? = 1 ]

rcg.pt: rcg.bt
	csynt rcg || [ $$? = 1 ]

rcg.rt: rcg.bt rcg.st rcg.recor
	recor rcg.recor

rcg_td.h: rcg.bt rcg.tdef
	tdef rcg.tdef > $@

# fin de code produit par "make-makefile rcg"

# debut de code produit par "make-makefile drcg"

drcg_t.c: drcg.bt drcg.st drcg.pt drcg.rt drcg.att
	tables_c drcg > $@

drcg.bt drcg.att drcg_mix.c: drcg.at
	semat drcg.at  > drcg_mix.c

drcg.st: drcg.bt drcg.lecl
	lecl drcg.lecl || [ $$? = 1 ]

drcg.pt: drcg.bt
	csynt drcg || [ $$? = 1 ]

drcg.rt: drcg.bt drcg.st drcg.recor
	recor drcg.recor

# fin de code produit par "make-makefile drcg"

clean:
	/bin/rm -f $(LIB)/*.o $(RCG)/rcg.out $(RCG)/drcg.out
	/bin/rm -f rcg.*.l drcg.*.l
	/bin/rm -f rcg.att rcg.bt rcg.pt rcg.rt rcg.st rcg_mix.c rcg_t.c rcg_td.h
	/bin/rm -f drcg.att drcg.bt drcg.pt drcg.rt drcg.st drcg_mix.c drcg_t.c


