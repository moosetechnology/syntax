SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*	\
		bnf2proba.pl


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir)
	cd tmp; ../$(SXETCBIN)/make-makefile bnf | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile make_proper | $(MAKE) -f -
	$(MAKE)
	cp -p $(srcdir)/tmp/make_proper_t.c $(top_srcdir)/src/make_proper_t.c
	$(SXETCBIN)/menage-tmp tmp $(srcdir)
