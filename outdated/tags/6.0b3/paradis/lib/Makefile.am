bin_PROGRAMS =	paradis		\
		print_ppt

LNorCP =	ln -s

PARADIS =	$(top_srcdir)/paradis
SRC =		$(PARADIS)/src
INCL =		$(PARADIS)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

BNFLIB =	$(top_builddir)/bnf/lib
BNFINCL =	$(top_srcdir)/bnf/incl

EXTRA_DIST =	$(SXINCL)/sem_by.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL)
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
			$(top_builddir)/etc/lib/libsxetc.a	\
			$(top_builddir)/lib/libsx.a


paradis_t.o: 	$(SXINCL)/sxunix.h $(SRC)/paradis_t.c
	$(CC_FLAGS) $(SRC)/paradis_t.c		\
		-Dsxtables=paradis_tables	\
		-DSCANACT=paradis_scan_act	\
		-Dsempass=paradis_smp

paradis_main.o:	$(SXINCL)/sxunix.h	\
			$(SXETCINCL)/B_tables.h 	\
			$(SXETCINCL)/release.h 	\
			$(BNFINCL)/bnf_vars.h 	\
			$(SRC)/paradis_main.c
	$(CC_FLAGS) $(SRC)/paradis_main.c

paradis_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sem_by.h	\
			$(SXETCINCL)/PP_tables.h	\
			$(SXETCINCL)/B_tables.h	\
			$(BNFINCL)/bnf_vars.h	\
			$(SXINCL)/put_edit.h	\
			$(SXINCL)/varstr.h	\
			$(SRC)/paradis_smp.c
	$(CC_FLAGS) $(SRC)/paradis_smp.c

print_ppt_SOURCES =	$(SRC)/print_ppt.c
print_ppt_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

print_ppt.o:	$(SXETCINCL)/PP_tables.h	\
		$(SXINCL)/sxunix.h	\
		$(SRC)/print_ppt.c
	$(CC_FLAGS) -I$(SXETCINCL) -c $(SRC)/print_ppt.c
