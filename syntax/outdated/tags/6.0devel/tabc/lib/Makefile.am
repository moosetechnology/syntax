bin_PROGRAMS =	tabc

TAB =	$(top_srcdir)/tabc
SRC =	$(TAB)/src
INCL =	$(TAB)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

BNFLIB =	$(top_builddir)/bnf/lib
BNFINCL =	$(top_srcdir)/bnf/incl

EXTRA_DIST =	$(INCL)/tabc_vars.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL) $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -I$(BNFINCL) -c


../../bin/tabc.out:	tabc
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?



tabc_SOURCES =	$(SRC)/tabc_put.c	\
		$(SRC)/tabc.c		\
		$(SRC)/tabc_main.c	\
		$(SRC)/dec.c		\
		$(SRC)/def.c		\
		$(SRC)/def_sact.c	\
		$(SRC)/dec_t.c		\
		$(SRC)/def_t.c		\
		$(SRC)/tabc_lo.c
tabc_LDADD =	$(BNFLIB)/bnf_folcon.o	\
		$(BNFLIB)/bnf_lo.o    	\
		$(BNFLIB)/bnf_protest.o	\
		$(BNFLIB)/bnf_sact.o   	\
		$(BNFLIB)/bnf_smp.o    	\
		$(BNFLIB)/bnf_symtab.o	\
		$(BNFLIB)/bnf_t.o	\
		$(top_builddir)/etc/lib/libsxetc.a	\
		$(top_builddir)/lib/libsx.a

tabc_main.o:	$(SXINCL)/sxversion.h	\
		$(SRC)/tabc_main.c 	\
		$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/tabc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/tabc_main.c

tabc_lo.o:	$(SRC)/tabc_lo.c 	\
		$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/tabc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/tabc_lo.c

tabc.o: 	$(SRC)/tabc.c		\
		$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/tabc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/tabc.c

tabc_put.o:	$(SRC)/tabc_put.c 	\
		$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/tabc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/tabc_put.c

dec.o:		$(SRC)/dec.c	 	\
		$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/tabc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/dec.c

def.o:		$(SRC)/def.c	 	\
		$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/tabc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/def.c

def_sact.o:	$(SXINCL)/sxversion.h	\
		$(SRC)/def_sact.c 	\
		$(SXINCL)/sxunix.h
	$(CC_FLAGS) $(SRC)/def_sact.c

dec_t.o:	$(SRC)/dec_t.c	\
		$(SXINCL)/sxunix.h
	$(CC_FLAGS) $(SRC)/dec_t.c	-Dsxtables=dec_tables\
		-DSCANACT=def_scan_act	\
		-DSEMACT=dec_act

def_t.o:	$(SRC)/def_t.c		\
		$(SXINCL)/sxunix.h
	$(CC_FLAGS) $(SRC)/def_t.c	-Dsxtables=def_tables \
		-DSCANACT=def_scan_act	\
		-DSEMACT=def_act
