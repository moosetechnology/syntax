SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir)
	cd tmp; ../$(SXETCBIN)/make-makefile recor | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile pprecor | $(MAKE) -f -
	$(SXETCBIN)/menage-tmp tmp $(srcdir)
	$(MAKE)
