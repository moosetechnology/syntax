bin_PROGRAMS =	dummy_csynt

LNorCP=ln -s

SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

DUMMY_CSYNT =	$(top_srcdir)/dummy_csynt
SRC =	$(DUMMY_CSYNT)/src
INCL =	$(DUMMY_CSYNT)/incl
SXINCL =	$(top_srcdir)/incl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/dummy_csynt.out:	dummy_csynt
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

dummy_csynt_SOURCES =	$(SRC)/dummy_csynt.c
dummy_csynt_LDADD =	$(SXBIBS)

dummy_csynt.o:	$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		$(SXINCL)/util/P_tables.h	\
		$(SXINCL)/sxversion.h		\
		$(SRC)/dummy_csynt.c
	$(CC_FLAGS) $(SRC)/dummy_csynt.c

