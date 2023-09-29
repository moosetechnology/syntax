bin_PROGRAMS =	dag2udag

LNorCP = ln -s

DAG =	$(top_srcdir)/DAG
SRC =	$(DAG)/src
INCL =	$(DAG)/incl
SXINCL =	$(top_srcdir)/incl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
# -DEBUG -DLOG -ggdb


../../bin/dag2udag.out:	dag2udag
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

dag2udag_SOURCES = $(SRC)/dag2udag_main.c
dag2udag_LDADD = $(top_builddir)/lib/libsx.a
dag2udag_CPPFLAGS = $(AM_CPPFLAGS)

