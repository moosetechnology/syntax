bin_PROGRAMS =	paradis		\
		print_ppt

LNorCP =	ln -s

PARADIS =	$(top_srcdir)/paradis
SRC =		$(PARADIS)/src
INCL =		$(PARADIS)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

BNFLIB =	$(top_builddir)/bnf/lib
BNFINCL =	$(top_srcdir)/bnf/incl

EXTRA_DIST =	$(SXINCL)/sem_by.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -I$(BNFINCL) -Dsxtables=paradis_tables -c


paradis_SOURCES =	$(SRC)/paradis_t.c	\
			$(SRC)/paradis_main.c	\
			$(SRC)/paradis_smp.c
paradis_LDADD =		$(BNFLIB)/bnf_sact.o	\
			$(BNFLIB)/bnf_t.o    	\
			$(BNFLIB)/bnf_smp.o    	\
			$(BNFLIB)/bnf_folcon.o	\
			$(BNFLIB)/bnf_protest.o	\
			$(BNFLIB)/bnf_lo.o    	\
			$(BNFLIB)/bnf_symtab.o	\
			$(SXBIBS)


paradis_t.o: 	$(SXINCL)/sxunix.h $(SRC)/paradis_t.c
	$(CC_FLAGS) $(SRC)/paradis_t.c		\
		-Dsxtables=paradis_tables	\
		-DSCANACT=paradis_scan_act	\
		-Dsempass=paradis_smp

paradis_main.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/util/B_tables.h \
		$(SXINCL)/sxversion.h	\
		$(BNFINCL)/bnf_vars.h 	\
		$(SRC)/paradis_main.c
	$(CC_FLAGS) $(SRC)/paradis_main.c

paradis_smp.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(SXINCL)/sem_by.h	\
		$(SXINCL)/util/PP_tables.h\
		$(SXINCL)/util/B_tables.h	\
		$(BNFINCL)/bnf_vars.h	\
		$(SXINCL)/put_edit.h	\
		$(SXINCL)/varstr.h	\
		$(SRC)/paradis_smp.c
	$(CC_FLAGS) $(SRC)/paradis_smp.c

print_ppt_SOURCES =	$(SRC)/print_ppt.c
print_ppt_LDADD =	$(SXBIBS)

print_ppt.o:	$(SXINCL)/util/PP_tables.h	\
		$(SXINCL)/sxunix.h	\
		$(SRC)/print_ppt.c
	$(CC_FLAGS) -I$(SXINCL)/util -c $(SRC)/print_ppt.c
