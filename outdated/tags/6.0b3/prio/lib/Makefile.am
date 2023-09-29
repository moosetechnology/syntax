bin_PROGRAMS =	prio

LNorCP=ln -s

PRIO =	$(top_srcdir)/prio
SRC =	$(PRIO)/src
INCL =	$(PRIO)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl


AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c



../../bin/prio.out:	prio
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


prio_SOURCES =	$(SRC)/prio_t.c			\
		$(SRC)/prio_main.c		\
		$(SRC)/prio_sact.c		\
		$(SRC)/prio_action.c
prio_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a



prio_t.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/prio_t.c
	$(CC_FLAGS) $(SRC)/prio_t.c			\
		-Dsxtables=prio_tables		\
		-DSCANACT=prio_scan_act		\
		-DSEMACT=prio_action

prio_main.o:	$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/release.h		\
		$(SXINCL)/put_edit.h		\
		$(SRC)/prio_main.c
	$(CC_FLAGS) $(SRC)/prio_main.c

prio_sact.o:	$(SXINCL)/sxunix.h		\
		$(SRC)/prio_sact.c
	$(CC_FLAGS) $(SRC)/prio_sact.c

prio_action.o:	$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/D_tables.h		\
		$(SXETCINCL)/B_tables.h		\
		$(SXINCL)/put_edit.h		\
		$(SXINCL)/varstr.h		\
		$(SRC)/prio_action.c
	$(CC_FLAGS) $(SRC)/prio_action.c
