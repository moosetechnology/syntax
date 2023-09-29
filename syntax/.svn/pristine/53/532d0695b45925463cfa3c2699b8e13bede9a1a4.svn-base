SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir)
	cd tmp; ../$(SXETCBIN)/make-makefile rcg | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile drcg | $(MAKE) -f -
	$(SXETCBIN)/menage-tmp tmp $(srcdir)
	$(MAKE)
