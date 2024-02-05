
include $(SX)/etc/make/gccoptions

BNF=$(SX)/bnf
LIB=$(BNF)/lib
BOOT=$(BNF)/boot

CC=$(COMPILER) -I$(BOOT) -I$(SXINCL) -I$(SXINCL)/util -I$(BNF) $(CFLAGS)

all: $(SXBIN)/bnf.out \
     $(SXAUX)/print_bt.out	\
     $(SXAUX)/check_bnf.out

$(SXBIN)/bnf.out:	$(BNF)/bnf.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS1=\
	$(LIB)/bnf_folcon.o		\
	$(LIB)/bnf_lo.o    		\
	$(LIB)/bnf_main.o		\
	$(LIB)/bnf_protest.o		\
	$(LIB)/bnf_sact.o		\
	$(LIB)/bnf_smp.o    		\
	$(LIB)/bnf_symtab.o		\
	$(LIB)/bnf_huge.o		\
	$(LIB)/bnf_terminal.o		\
	$(LIB)/bnf_t.o

$(BNF)/bnf.out:	$(NEEDS1)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/bnf_folcon.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			bnf_folcon.c
	$(CC) -o $@ -c bnf_folcon.c

$(LIB)/bnf_lo.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/put_edit.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			bnf_lo.c
	$(CC) -o $@ -c bnf_lo.c

$(LIB)/bnf_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			bnf_main.c
	$(CC) -o $@ -c bnf_main.c

$(LIB)/bnf_protest.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			bnf_protest.c
	$(CC) -o $@ -c bnf_protest.c

$(LIB)/bnf_sact.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			bnf_sact.c
	$(CC) -o $@ -c bnf_sact.c

$(LIB)/bnf_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			bnf_smp.c
	$(CC) -o $@ -c bnf_smp.c

$(LIB)/bnf_symtab.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h	\
			$(SXINCL)/varstr.h	\
			bnf_symtab.c
	$(CC) -o $@ -c bnf_symtab.c

$(LIB)/bnf_huge.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/sxword.h	\
			$(SXINCL)/util/B_tables.h	\
			bnf_huge.c
	$(CC) -o $@ -c bnf_huge.c

$(LIB)/bnf_terminal.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			bnf_terminal.c
	$(CC) -o $@ -c bnf_terminal.c

$(LIB)/bnf_t.o:		$(SXINCL)/sxunix.h 	\
			$(BOOT)/bnf_t.c
	$(CC) -o $@ -c $(BOOT)/bnf_t.c			\
		-Dsxtables=bnf_tables		\
		-DSCANACT=bnf_scan_act		\
		-Dsempass=sxbnfsmp

NEEDS2=\
	$(LIB)/bnf_dump.o		\
	$(LIB)/print_bt.o

$(SXAUX)/print_bt.out:	$(NEEDS2)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/bnf_dump.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			bnf_dump.c	
	$(CC) -o $@ -c bnf_dump.c

$(LIB)/print_bt.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			print_bt.c	
	$(CC) -o $@ -c print_bt.c


NEEDS3=$(LIB)/check_bnf.o

$(SXAUX)/check_bnf.out:	$(NEEDS3)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/check_bnf.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNF)/bnf_vars.h 	\
			check_bnf.c	
	$(CC) -o $@ -c check_bnf.c

# debut de code produit par "make-makefile bnf"

bootstrap: bnf_t.c bnf.c.skeleton 

bnf_t.c: bnf.bt bnf.st bnf.pt bnf.rt bnf.att
	tables_c bnf > $@

bnf.bt bnf.att bnf.c.skeleton: bnf.at
	semat bnf.at  > bnf.c.skeleton

bnf.st: bnf.bt bnf.lecl
	lecl bnf.lecl || [ $$? = 1 ]

bnf.pt: bnf.bt
	csynt bnf || [ $$? = 1 ]

bnf.rt: bnf.bt bnf.st bnf.recor
	recor bnf.recor

# fin de code produit par "make-makefile bnf"

clean:
	/bin/rm -f $(LIB)/*.o $(SXAUX)/print_bt.out $(SXAUX)/check_bnf.out $(BNF)/bnf.out
	/bin/rm -f bnf.att bnf.bt bnf.pt bnf.rt bnf.st
	/bin/rm -f bnf.bn.l bnf.lc.l bnf.rc.l
	/bin/rm -f bnf.c.skeleton bnf_t.c

