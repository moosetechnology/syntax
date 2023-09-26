SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir)
	cd tmp; ../$(SXETCBIN)/make-makefile tdef | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile pptdef | $(MAKE) -f -
	$(MAKE)
	cp -p $(srcdir)/tmp/tdef_t.c $(top_srcdir)/src/tdef_t.c
	$(SXETCBIN)/menage-tmp tmp $(srcdir)
