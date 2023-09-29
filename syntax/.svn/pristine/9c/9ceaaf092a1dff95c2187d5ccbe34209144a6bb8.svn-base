bin_PROGRAMS =	yax

LNorCP=ln -s

YAX =	$(top_srcdir)/yax
SRC =	$(YAX)/src
INCL =	$(YAX)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

BNFLIB =	$(top_builddir)/bnf/lib
BNFINCL =	$(top_srcdir)/bnf/incl

EXTRA_DIST =	$(INCL)/yax_vars.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -I$(BNFINCL) -c


../../bin/yax.out:	yax
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


yax_SOURCES =	$(SRC)/yax_put.c	\
		$(SRC)/yax.c		\
		$(SRC)/yax_main.c	\
		$(SRC)/dec_act.c	\
		$(SRC)/dec_t.c		\
		$(SRC)/def_act.c	\
		$(SRC)/def_t.c
yax_LDADD =	$(BNFLIB)/bnf_folcon.o	\
		$(BNFLIB)/bnf_lo.o	\
		$(BNFLIB)/bnf_protest.o	\
		$(BNFLIB)/bnf_sact.o	\
		$(BNFLIB)/bnf_smp.o	\
		$(BNFLIB)/bnf_symtab.o	\
		$(BNFLIB)/bnf_t.o	\
		$(top_builddir)/etc/lib/libsxetc.a	\
		$(top_builddir)/lib/libsx.a

yax_main.o:	$(SXINCL)/sxunix.h	\
	 	$(SXETCINCL)/B_tables.h	\
		$(INCL)/yax_vars.h	\
		$(BNFINCL)/bnf_vars.h	\
		$(SRC)/yax_main.c
	$(CC_FLAGS) $(SRC)/yax_main.c

yax.o: 		$(SXINCL)/sxunix.h	\
	 	$(SXETCINCL)/B_tables.h	\
	 	$(SXINCL)/put_edit.h	\
		$(INCL)/yax_vars.h	\
		$(BNFINCL)/bnf_vars.h	\
		$(SRC)/yax.c
	$(CC_FLAGS) $(SRC)/yax.c

yax_put.o:	$(SXINCL)/sxunix.h	\
	 	$(SXETCINCL)/B_tables.h	\
		$(INCL)/yax_vars.h	\
		$(BNFINCL)/bnf_vars.h	\
		$(SRC)/yax_put.c
	$(CC_FLAGS) $(SRC)/yax_put.c

dec_act.o:	$(SXINCL)/sxunix.h	\
	 	$(SXETCINCL)/B_tables.h	\
		$(INCL)/yax_vars.h	\
		$(BNFINCL)/bnf_vars.h	\
		$(SRC)/dec_act.c
	$(CC_FLAGS) $(SRC)/dec_act.c

def_act.o:	$(SXINCL)/sxunix.h	\
		$(SXETCINCL)/B_tables.h	\
		$(INCL)/yax_vars.h	\
		$(BNFINCL)/bnf_vars.h	\
		$(SRC)/def_act.c
	$(CC_FLAGS) $(SRC)/def_act.c

dec_t.o:	$(SXINCL)/sxunix.h	\
		$(SRC)/dec_t.c
	$(CC_FLAGS) $(SRC)/dec_t.c	\
		-Dsxtables=dec_tables	\
		-DSCANACT=dec_act	\
		-DSEMACT=dec_act


def_t.o:	$(SXINCL)/sxunix.h	\
		$(SRC)/def_t.c
	$(CC_FLAGS) $(SRC)/def_t.c	\
		-Dsxtables=def_tables	\
		-DSCANACT=def_act	\
		-DSEMACT=def_act
