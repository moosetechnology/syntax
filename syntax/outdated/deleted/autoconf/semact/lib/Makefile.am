bin_PROGRAMS =	semact

ACT =	$(top_srcdir)/semact
SRC =	$(ACT)/src
INCL =	$(ACT)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

BNFLIB =	$(top_builddir)/bnf/lib
BNFINCL =	$(top_srcdir)/bnf/incl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -I$(BNFINCL) -c


../..//bin/semact.out:	semact
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


semact_SOURCES =	$(SRC)/semact_main.c	\
			$(SRC)/semact_sact.c	\
			$(SRC)/semact_t.c

semact_LDADD =	$(BNFLIB)/bnf_folcon.o	\
		$(BNFLIB)/bnf_lo.o    	\
		$(BNFLIB)/bnf_protest.o	\
		$(BNFLIB)/bnf_sact.o   	\
		$(BNFLIB)/bnf_smp.o    	\
		$(BNFLIB)/bnf_symtab.o	\
		$(BNFLIB)/bnf_t.o	\
		$(SXBIBS)

semact_main.o:	$(SXINCL)/sxunix.h $(SXINCL)/util/B_tables.h $(BNFINCL)/bnf_vars.h \
			$(SXINCL)/sxversion.h $(SRC)/semact_main.c
	$(CC_FLAGS) $(SRC)/semact_main.c

semact_sact.o:	$(SXINCL)/sxunix.h $(SXINCL)/sem_by.h $(SXINCL)/util/B_tables.h $(BNFINCL)/bnf_vars.h \
			$(SXINCL)/sxversion.h $(SRC)/semact_sact.c
	$(CC_FLAGS) $(SRC)/semact_sact.c

semact_t.o:	$(SXINCL)/sxunix.h $(SRC)/semact_t.c
	$(CC_FLAGS) $(SRC)/semact_t.c 		\
		-Dsxtables=semact_tables	\
		-DSEMACT=semact_semact
