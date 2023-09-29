SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir); : $(MAKE)
	cd tmp; ../$(SXETCBIN)/make-makefile amlgm_cmpd | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile make_a_dico | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	$(MAKE)
	$(SXETCBIN)/menage-tmp tmp $(srcdir); : $(MAKE)
