bin_PROGRAMS =	ysx

LNorCP=ln -s

YSX =	$(top_srcdir)/ysx
SRC =	$(YSX)/src
INCL =	$(YSX)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

BNFLIB =	$(top_builddir)/bnf/lib
BNFINCL =	$(top_srcdir)/bnf/incl

EXTRA_DIST =	$(INCL)/ysx_vars.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL) $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c

../../bin/ysx.out:	ysx
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


ysx_SOURCES =	$(SRC)/ysx_main.c	\
		$(SRC)/ysx_smp.c	\
		$(SRC)/ysx_t.c		\
		$(SRC)/yaction_t.c
ysx_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

ysx_main.o:	$(SXINCL)/sxunix.h	\
		$(INCL)/ysx_vars.h	\
		$(SXINCL)/put_edit.h	\
		$(SXINCL)/sxversion.h	\
		$(SRC)/ysx_main.c
	$(CC_FLAGS) $(SRC)/ysx_main.c

ysx_smp.o:	$(SXINCL)/sxunix.h	\
		$(INCL)/ysx_vars.h	\
		$(SXINCL)/put_edit.h	\
		$(SXINCL)/varstr.h	\
		$(SRC)/ysx_smp.c
	$(CC_FLAGS) $(SRC)/ysx_smp.c

ysx_t.o:	$(SXINCL)/sxunix.h	\
		$(SRC)/ysx_t.c
	$(CC_FLAGS) $(SRC)/ysx_t.c 		\
		-Dsempass=ysx_smp 	\
		-Dsxtables=ysx_tables

yaction_t.o:	$(SXINCL)/sxunix.h	\
		$(SRC)/yaction_t.c
	$(CC_FLAGS) $(SRC)/yaction_t.c 	\
		-DSCANACT=yaction 	\
		-DSEMACT=yaction	\
		-DSCANACT_AND_SEMACT_ARE_IDENTICAL 	\
		-Dsxtables=yaction_tables
