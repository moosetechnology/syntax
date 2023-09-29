noinst_PROGRAMS =	print_bt	\
			check_bnf

bin_PROGRAMS =	bnf

LNorCP = ln -s

BNF =	$(top_srcdir)/bnf
SRC =	$(BNF)/src
INCL =	$(BNF)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

EXTRA_DIST =	$(INCL)/bnf_vars.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/bnf.out:	bnf
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

bnf_SOURCES =	$(SRC)/bnf_folcon.c		\
		$(SRC)/bnf_lo.c    		\
		$(SRC)/bnf_main.c		\
		$(SRC)/bnf_protest.c		\
		$(SRC)/bnf_sact.c		\
		$(SRC)/bnf_smp.c    		\
		$(SRC)/bnf_symtab.c		\
		$(SRC)/bnf_huge.c		\
		$(SRC)/bnf_t.c
bnf_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

bnf_folcon.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h 	\
		$(SRC)/bnf_folcon.c
	$(CC_FLAGS) $(SRC)/bnf_folcon.c

bnf_lo.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/put_edit.h	\
		$(SXINCL)/varstr.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h 	\
		$(SRC)/bnf_lo.c
	$(CC_FLAGS) $(SRC)/bnf_lo.c

bnf_main.o:	$(SXINCL)/sxunix.h	\
		$(SXETCINCL)/release.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h 	\
		$(SRC)/bnf_main.c
	$(CC_FLAGS) $(SRC)/bnf_main.c

bnf_protest.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/varstr.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h 	\
		$(SRC)/bnf_protest.c
	$(CC_FLAGS) $(SRC)/bnf_protest.c

bnf_sact.o:	$(SXINCL)/sxunix.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h 	\
		$(SRC)/bnf_sact.c
	$(CC_FLAGS) $(SRC)/bnf_sact.c

bnf_smp.o:	$(SXINCL)/sxunix.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h 	\
		$(SRC)/bnf_smp.c
	$(CC_FLAGS) $(SRC)/bnf_smp.c

bnf_symtab.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/varstr.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h	\
		$(SXINCL)/varstr.h	\
		$(SRC)/bnf_symtab.c
	$(CC_FLAGS) $(SRC)/bnf_symtab.c

bnf_huge.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/varstr.h	\
		$(SXINCL)/sxword.h	\
		$(SXETCINCL)/B_tables.h	\
		$(SRC)/bnf_huge.c
	$(CC_FLAGS) $(SRC)/bnf_huge.c

bnf_t.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/bnf_t.c
	$(CC_FLAGS) $(SRC)/bnf_t.c			\
		-Dsxtables=bnf_tables		\
		-DSCANACT=bnf_scan_act		\
		-Dsempass=sxbnfsmp

print_bt_SOURCES =	$(SRC)/bnf_dump.c		\
			$(SRC)/print_bt.c
print_bt_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

bnf_dump.o:	$(SXINCL)/sxunix.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h 	\
		$(SRC)/bnf_dump.c	
	$(CC_FLAGS) $(SRC)/bnf_dump.c

print_bt.o:	$(SXINCL)/sxunix.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h 	\
		$(SRC)/print_bt.c	
	$(CC_FLAGS) $(SRC)/print_bt.c


check_bnf_SOURCES =	$(SRC)/check_bnf.c
check_bnf_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

check_bnf.o:	$(SXINCL)/sxunix.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/bnf_vars.h 	\
		$(SRC)/check_bnf.c	
	$(CC_FLAGS) $(SRC)/check_bnf.c

