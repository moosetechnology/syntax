SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir); : $(MAKE)
	cd tmp; ../$(SXETCBIN)/make-makefile dec | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile def | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	$(SXETCBIN)/menage-tmp tmp $(srcdir); : $(MAKE)
	$(MAKE)
