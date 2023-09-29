# Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/../../etc/bin
PPC =	$(top_srcdir)
SRC =	$(PPC)/src
SPEC =	$(PPC)/spec

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:	$(SPEC)/ppc123.spec     \
		$(SPEC)/ppc_args.bnf    \
		$(SPEC)/ppc_args.lecl   \
		$(SPEC)/ppc_args.recor  \
		$(SRC)/ppc_main.c       \
		$(SRC)/ppc_sact.c
	$(SXETCBIN)/liens-tmp tmp $(srcdir)
	cd tmp; ../$(SXETCBIN)/make-makefile ppc_args | $(MAKE) -f -
	$(MAKE)
	cd tmp; ../$(SXETCBIN)/make-makefile ppc1 | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile ppc2 | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile ppc3 | $(MAKE) -f -
	$(SXETCBIN)/menage-tmp tmp $(srcdir)
	$(MAKE)
