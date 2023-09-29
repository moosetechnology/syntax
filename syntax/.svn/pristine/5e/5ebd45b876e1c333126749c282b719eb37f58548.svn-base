bin_PROGRAMS =	semat

noinst_PROGRAMS =	print_att

SEMAT =	$(top_srcdir)/semat
SRC =	$(SEMAT)/src
INCL =	$(SEMAT)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

BNFLIB =	$(top_builddir)/bnf/lib
BNFINCL =	$(top_srcdir)/bnf/incl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -I$(BNFINCL) -c


../../bin/semat.out:	semat
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

semat_SOURCES =	$(SRC)/genat_c.c		\
		$(SRC)/semat_main.c		\
		$(SRC)/semat_sact.c		\
		$(SRC)/semat_t.c
semat_LDADD =	$(BNFLIB)/bnf_folcon.o	\
		$(BNFLIB)/bnf_lo.o    	\
		$(BNFLIB)/bnf_protest.o	\
		$(BNFLIB)/bnf_sact.o   	\
		$(BNFLIB)/bnf_smp.o    	\
		$(BNFLIB)/bnf_symtab.o	\
		$(BNFLIB)/bnf_t.o	\
		$(SXBIBS)

semat_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/util/B_tables.h	\
			$(BNFINCL)/bnf_vars.h	\
			$(SXINCL)/sxversion.h	\
			$(SRC)/semat_main.c
	$(CC_FLAGS) $(SRC)/semat_main.c

genat_c.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/util/B_tables.h	\
		$(SXINCL)/util/T_tables.h	\
		$(BNFINCL)/bnf_vars.h	\
		$(SRC)/genat_c.c
	$(CC_FLAGS) $(SRC)/genat_c.c

semat_sact.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(SXINCL)/sem_by.h	\
		$(SXINCL)/util/B_tables.h	\
		$(BNFINCL)/bnf_vars.h 	\
		$(SXINCL)/util/T_tables.h	\
		$(SXINCL)/put_edit.h	\
		$(SXINCL)/varstr.h	\
		$(SRC)/semat_sact.c
	$(CC_FLAGS) $(SRC)/semat_sact.c

semat_t.o:	$(SXINCL)/sxunix.h $(SRC)/semat_t.c
	$(CC_FLAGS) $(SRC)/semat_t.c			\
		-Dsxtables=semat_tables		\
		-DSEMACT=semat_semact		\
		-Dsempass=semat_sem

print_att_SOURCES =	$(SRC)/print_att.c
print_att_LDADD =	$(SXBIBS)

print_att.o:	$(SXINCL)/util/T_tables.h	\
		$(SRC)/print_att.c
	$(CC_FLAGS) $(SRC)/print_att.c
