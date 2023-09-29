bin_PROGRAMS =	dummy_csynt

LNorCP=ln -s

DUMMY_CSYNT =	$(top_srcdir)/dummy_csynt
SRC =	$(DUMMY_CSYNT)/src
INCL =	$(DUMMY_CSYNT)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL) $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/dummy_csynt.out:	dummy_csynt
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

dummy_csynt_SOURCES =	$(SRC)/dummy_csynt.c
dummy_csynt_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

dummy_csynt.o:	$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/B_tables.h		\
		$(SXETCINCL)/P_tables.h		\
		$(SXINCL)/sxversion.h		\
		$(SRC)/dummy_csynt.c
	$(CC_FLAGS) $(SRC)/dummy_csynt.c

