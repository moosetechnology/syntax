bin_PROGRAMS =	semc

TAB =	$(top_srcdir)/semc
SRC =	$(TAB)/src
INCL =	$(TAB)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

BNFLIB =	$(top_builddir)/bnf/lib
BNFINCL =	$(top_srcdir)/bnf/incl

EXTRA_DIST =	$(INCL)/semc_vars.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -I$(BNFINCL) -c


../../bin/semc.out:	semc
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?



semc_SOURCES =	$(SRC)/semc_put.c	\
		$(SRC)/semc.c		\
		$(SRC)/semc_main.c	\
		$(SRC)/dec.c		\
		$(SRC)/def.c		\
		$(SRC)/def_sact.c	\
		$(SRC)/dec_t.c		\
		$(SRC)/def_t.c		\
		$(SRC)/semc_lo.c
semc_LDADD =	$(BNFLIB)/bnf_folcon.o	\
		$(BNFLIB)/bnf_lo.o    	\
		$(BNFLIB)/bnf_protest.o	\
		$(BNFLIB)/bnf_sact.o   	\
		$(BNFLIB)/bnf_smp.o    	\
		$(BNFLIB)/bnf_symtab.o	\
		$(BNFLIB)/bnf_t.o	\
		$(SXBIBS)

semc_main.o:	$(SXINCL)/sxversion.h	\
		$(SRC)/semc_main.c 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/semc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/semc_main.c

semc_lo.o:	$(SRC)/semc_lo.c 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/semc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/semc_lo.c

semc.o: 	$(SRC)/semc.c		\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/semc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/semc.c

semc_put.o:	$(SRC)/semc_put.c 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/semc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/semc_put.c

dec.o:		$(SRC)/dec.c	 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/semc_vars.h	\
		$(BNFINCL)/bnf_vars.h
	$(CC_FLAGS) $(SRC)/dec.c

def.o:		$(SRC)/def.c	 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		$(INCL)/semc_vars.h	\
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
