SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir); : $(MAKE)
	cd tmp; ../$(SXETCBIN)/make-makefile bnf | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile make_proper | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	$(MAKE)
	cp -p $(srcdir)/tmp/make_proper_t.c $(top_srcdir)/src/make_proper_t.c
	$(SXETCBIN)/menage-tmp tmp $(srcdir); : $(MAKE)
