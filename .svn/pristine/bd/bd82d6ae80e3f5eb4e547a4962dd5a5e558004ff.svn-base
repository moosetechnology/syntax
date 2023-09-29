SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir); : $(MAKE)
	cd tmp; ../$(SXETCBIN)/make-makefile recor | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile pprecor | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	$(SXETCBIN)/menage-tmp tmp $(srcdir); : $(MAKE)
	$(MAKE)
