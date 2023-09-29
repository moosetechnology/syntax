bin_PROGRAMS =	prio

LNorCP=ln -s

PRIO =	$(top_srcdir)/prio
SRC =	$(PRIO)/src
INCL =	$(PRIO)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/prio.out:	prio
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


prio_SOURCES =	$(SRC)/prio_t.c			\
		$(SRC)/prio_main.c		\
		$(SRC)/prio_sact.c		\
		$(SRC)/prio_action.c
prio_LDADD =	$(SXBIBS)

prio_t.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/prio_t.c
	$(CC_FLAGS) $(SRC)/prio_t.c			\
		-Dsxtables=prio_tables		\
		-DSCANACT=prio_scan_act		\
		-DSEMACT=prio_action

prio_main.o:	$(SXINCL)/sxunix.h		\
		$(SXINCL)/sxversion.h		\
		$(SXINCL)/put_edit.h		\
		$(SRC)/prio_main.c
	$(CC_FLAGS) $(SRC)/prio_main.c

prio_sact.o:	$(SXINCL)/sxunix.h		\
		$(SRC)/prio_sact.c
	$(CC_FLAGS) $(SRC)/prio_sact.c

prio_action.o:	$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/D_tables.h	\
		$(SXINCL)/util/B_tables.h	\
		$(SXINCL)/put_edit.h		\
		$(SXINCL)/varstr.h		\
		$(SRC)/prio_action.c
	$(CC_FLAGS) $(SRC)/prio_action.c
