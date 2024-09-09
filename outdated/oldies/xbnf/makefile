
include $(SX)/etc/make/gccoptions

XBNF=$(SX)/../outdated/oldies/xbnf
LIB=$(XBNF)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(XBNF)/xbnf.out 	\
     $(XBNF)/print_bt.out	\
     $(XBNF)/xbnf2lex_prdct.out

NEEDS1= $(LIB)/xbnf_folcon.o		\
	$(LIB)/xbnf_lo.o    		\
	$(LIB)/xbnf_main.o		\
	$(LIB)/xbnf_protest.o		\
	$(LIB)/xbnf_sact.o		\
	$(LIB)/xbnf_smp.o    		\
	$(LIB)/xbnf_symtab.o		\
	$(LIB)/xbnf_t.o

$(XBNF)/xbnf.out:	$(NEEDS1)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/xbnf_folcon.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/util/B_tables.h	\
			xbnf_vars.h 	\
			xbnf_folcon.c
	$(CC) -o $@ -c xbnf_folcon.c

$(LIB)/xbnf_lo.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/put_edit.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/util/B_tables.h	\
			xbnf_vars.h 	\
			xbnf_lo.c
	$(CC) -o $@ -c xbnf_lo.c

$(LIB)/xbnf_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			xbnf_vars.h 	\
			xbnf_main.c
	$(CC) -o $@ -c xbnf_main.c

$(LIB)/xbnf_protest.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/util/B_tables.h	\
			xbnf_vars.h 	\
			xbnf_protest.c
	$(CC) -o $@ -c xbnf_protest.c

$(LIB)/xbnf_sact.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			xbnf_vars.h 	\
			xbnf_sact.c
	$(CC) -o $@ -c xbnf_sact.c

$(LIB)/xbnf_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			xbnf_vars.h 	\
			xbnf_smp.c
	$(CC) -o $@ -c xbnf_smp.c

$(LIB)/xbnf_symtab.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/util/B_tables.h	\
			xbnf_vars.h	\
			xbnf_symtab.c
	$(CC) -o $@ -c xbnf_symtab.c

$(LIB)/xbnf_t.o:	$(SXINCL)/sxunix.h 	\
			xbnf_t.c
	$(CC) -o $@ -c xbnf_t.c			\
		-DSEMPASS=sxxbnfsmp

NEEDS2= $(LIB)/xbnf_dump.o		\
	$(LIB)/print_bt.o

$(XBNF)/print_bt.out:	$(NEEDS2)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/xbnf_dump.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			xbnf_vars.h 	\
			xbnf_dump.c	
	$(CC) -o $@ -c xbnf_dump.c

$(LIB)/print_bt.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			xbnf_vars.h 	\
			print_bt.c	
	$(CC) -o $@ -c print_bt.c

NEEDS3= $(LIB)/xbnf2lex_prdct_main.o	\
	$(LIB)/xbnf_sact.o		\
	$(LIB)/xbnf2lex_prdct_smp.o    	\
	$(LIB)/xbnf2lex_prdct.o		\
	$(LIB)/xbnf2lex_prdct_t.o

$(XBNF)/xbnf2lex_prdct.out:	$(NEEDS3)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/xbnf2lex_prdct_main.o:	$(SXINCL)/sxunix.h	\
			xbnf2lex_prdct_main.c
	$(CC)-o $@ -c  xbnf2lex_prdct_main.c

$(LIB)/xbnf2lex_prdct_smp.o:	$(SXINCL)/sxunix.h	\
			xbnf2lex_prdct_smp.c
	$(CC) -o $@ -c xbnf2lex_prdct_smp.c

$(LIB)/xbnf2lex_prdct.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XH.h	\
			$(SXINCL)/varstr.h	\
			xbnf2lex_prdct.c
	$(CC) -o $@ -c xbnf2lex_prdct.c

$(LIB)/xbnf2lex_prdct_t.o:		$(SXINCL)/sxunix.h 	\
			xbnf_t.c
	$(CC) -o $@ -c xbnf_t.c	-o $(LIB)/xbnf2lex_prdct_t.o		\
		-DSEMPASS=xbnf2lexprdct_smp

# debut de code produit par "make-makefile xbnf"

xbnf_t.c: xbnf.bt xbnf.st xbnf.pt xbnf.rt xbnf.att
	tables_c xbnf > $@

xbnf.bt xbnf.att xbnf_mix.c: xbnf.at
	semat xbnf.at  > xbnf_mix.c

xbnf.st: xbnf.bt xbnf.lecl
	lecl xbnf.lecl || [ $$? = 1 ]

xbnf.pt: xbnf.bt
	csynt xbnf || [ $$? = 1 ]

xbnf.rt: xbnf.bt xbnf.st xbnf.recor
	recor xbnf.recor

# fin de code produit par "make-makefile xbnf"

clean:
	/bin/rm -f $(LIB)/*.o $(XBNF)/xbnf2lex_prdct.out $(XBNF)/print_bt.out $(XBNF)/xbnf.out
	/bin/rm -f $(XBNF)/xbnf.*.l
	/bin/rm -f $(XBNF)/xbnf.att $(XBNF)/xbnf.bt $(XBNF)/xbnf.pt
	/bin/rm -f $(XBNF)/xbnf.rt $(XBNF)/xbnf.bt $(XBNF)/xbnf.st
	/bin/rm -f $(XBNF)/xbnf_mix.c $(XBNF)/xbnf.bt $(XBNF)/xbnf_t.c

