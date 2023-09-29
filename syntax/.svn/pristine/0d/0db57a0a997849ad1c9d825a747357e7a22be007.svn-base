noinst_PROGRAMS =	print_bt	\
			xbnf2lex_prdct

bin_PROGRAMS =	xbnf

LNorCP=ln -s

XBNF =	$(top_srcdir)/xbnf
SRC =	$(XBNF)/src
INCL =	$(XBNF)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

EXTRA_DIST =	$(INCL)/xbnf_vars.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/xbnf.out:	xbnf
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

xbnf_SOURCES =	$(SRC)/xbnf_folcon.c		\
		$(SRC)/xbnf_lo.c    		\
		$(SRC)/xbnf_main.c		\
		$(SRC)/xbnf_protest.c		\
		$(SRC)/xbnf_sact.c		\
		$(SRC)/xbnf_smp.c    		\
		$(SRC)/xbnf_symtab.c		\
		$(SRC)/xbnf_t.c
xbnf_LDADD =	$(SXBIBS)

xbnf_folcon.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/xbnf_vars.h 	\
		$(SRC)/xbnf_folcon.c
	$(CC_FLAGS) $(SRC)/xbnf_folcon.c

xbnf_lo.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/put_edit.h	\
		$(SXINCL)/varstr.h	\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/xbnf_vars.h 	\
		$(SRC)/xbnf_lo.c
	$(CC_FLAGS) $(SRC)/xbnf_lo.c

xbnf_main.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/xbnf_vars.h 	\
		$(SRC)/xbnf_main.c
	$(CC_FLAGS) $(SRC)/xbnf_main.c

xbnf_protest.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/varstr.h	\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/xbnf_vars.h 	\
		$(SRC)/xbnf_protest.c
	$(CC_FLAGS) $(SRC)/xbnf_protest.c

xbnf_sact.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/xbnf_vars.h 	\
		$(SRC)/xbnf_sact.c
	$(CC_FLAGS) $(SRC)/xbnf_sact.c

xbnf_smp.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/xbnf_vars.h 	\
		$(SRC)/xbnf_smp.c
	$(CC_FLAGS) $(SRC)/xbnf_smp.c

xbnf_symtab.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/varstr.h	\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/xbnf_vars.h	\
		$(SRC)/xbnf_symtab.c
	$(CC_FLAGS) $(SRC)/xbnf_symtab.c

xbnf_t.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/xbnf_t.c
	$(CC_FLAGS) $(SRC)/xbnf_t.c			\
		-Dsxtables=xbnf_tables		\
		-DSCANACT=xbnf_scan_act		\
		-Dsempass=sxxbnfsmp

print_bt_SOURCES =	$(SRC)/xbnf_dump.c	\
			$(SRC)/print_bt.c
print_bt_LDADD =	$(SXBIBS)


xbnf_dump.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/xbnf_vars.h 	\
		$(SRC)/xbnf_dump.c	
	$(CC_FLAGS) $(SRC)/xbnf_dump.c

print_bt.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/xbnf_vars.h 	\
		$(SRC)/print_bt.c	
	$(CC_FLAGS) $(SRC)/print_bt.c

xbnf2lex_prdct_SOURCES =	$(SRC)/xbnf2lex_prdct_main.c	\
				$(SRC)/xbnf_sact.c		\
				$(SRC)/xbnf2lex_prdct_smp.c    	\
				$(SRC)/xbnf2lex_prdct.c
xbnf2lex_prdct_LDADD =	xbnf2lex_prdct_t.o			\
			$(SXBIBS)

xbnf2lex_prdct_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxversion.h	\
			$(SRC)/xbnf2lex_prdct_main.c
	$(CC_FLAGS) $(SRC)/xbnf2lex_prdct_main.c

xbnf2lex_prdct_smp.o:	$(SXINCL)/sxunix.h	\
			$(SRC)/xbnf2lex_prdct_smp.c
	$(CC_FLAGS) $(SRC)/xbnf2lex_prdct_smp.c

xbnf2lex_prdct.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XH.h	\
			$(SXINCL)/varstr.h	\
			$(SRC)/xbnf2lex_prdct.c
	$(CC_FLAGS) $(SRC)/xbnf2lex_prdct.c


xbnf2lex_prdct_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/xbnf_t.c
	$(CC_FLAGS) $(SRC)/xbnf_t.c	-o xbnf2lex_prdct_t.o	\
		-Dsxtables=xbnf_tables		\
		-DSCANACT=xbnf_scan_act		\
		-Dsempass=xbnf2lexprdct_smp
