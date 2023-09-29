bin_PROGRAMS =	csynt_lc

CSYNT_LC =	$(top_srcdir)/csynt_lc
SRC =	$(CSYNT_LC)/src
INCL =	$(CSYNT_LC)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

EXTRA_DIST =	$(INCL)/lc.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/csynt_lc.out:	csynt_lc
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

csynt_lc_SOURCES =	$(SRC)/lc_main.c	\
			$(SRC)/LC1.c
csynt_lc_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

lc_main.o:	$(INCL)/lc.h			\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/sxba.h		\
		$(SXETCINCL)/bstr.h		\
		$(SXETCINCL)/B_tables.h		\
		$(SXETCINCL)/release.h		\
		$(SRC)/lc_main.c
	$(CC_FLAGS) $(SRC)/lc_main.c

LC1.o:		$(INCL)/lc.h			\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/sxba.h		\
		$(SXETCINCL)/bstr.h		\
		$(SXETCINCL)/B_tables.h		\
		$(SXETCINCL)/P_tables.h		\
		$(SRC)/LC1.c
	$(CC_FLAGS) $(SRC)/LC1.c
