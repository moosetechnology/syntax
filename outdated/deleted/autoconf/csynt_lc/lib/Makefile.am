bin_PROGRAMS =	csynt_lc

CSYNT_LC =	$(top_srcdir)/csynt_lc
SRC =	$(CSYNT_LC)/src
INCL =	$(CSYNT_LC)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

EXTRA_DIST =	$(INCL)/lc.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/csynt_lc.out:	csynt_lc
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

csynt_lc_SOURCES =	$(SRC)/lc_main.c	\
			$(SRC)/LC1.c
csynt_lc_LDADD =	$(SXBIBS)

lc_main.o:	$(INCL)/lc.h			\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/sxba.h		\
		$(SXINCL)/util/bstr.h		\
		$(SXINCL)/util/B_tables.h	\
		$(SXINCL)/sxversion.h		\
		$(SRC)/lc_main.c
	$(CC_FLAGS) $(SRC)/lc_main.c

LC1.o:		$(INCL)/lc.h			\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/sxversion.h		\
		$(SXINCL)/sxba.h		\
		$(SXINCL)/util/bstr.h		\
		$(SXINCL)/util/B_tables.h	\
		$(SXINCL)/util/P_tables.h	\
		$(SRC)/LC1.c
	$(CC_FLAGS) $(SRC)/LC1.c
