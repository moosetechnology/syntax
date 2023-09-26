SXINCL  = $(top_srcdir)/incl
SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir); : $(MAKE)
	cd tmp; ../$(SXETCBIN)/make-makefile dag | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile udag | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	$(SXETCBIN)/menage-tmp tmp $(srcdir); : $(MAKE)
	$(MAKE)
	cp -p $(srcdir)/src/dag_t.c $(top_srcdir)/incl/dag_t.h
	cp -p $(srcdir)/src/udag_t.c $(top_srcdir)/incl/udag_t.h
