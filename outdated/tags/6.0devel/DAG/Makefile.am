SUBDIRS =	lib

SXINCL  = $(top_srcdir)/incl
SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir); : $(MAKE)
	cd tmp; ../$(SXETCBIN)/make-makefile re | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile dag | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile udag | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile sdag | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile text -dummy_csynt | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	$(MAKE)
	cp -p $(srcdir)/tmp/re_t.c $(top_srcdir)/incl/re_t.h
	cp -p $(srcdir)/tmp/dag_t.c $(top_srcdir)/incl/dag_t.h
	cp -p $(srcdir)/tmp/udag_t.c $(top_srcdir)/incl/udag_t.h
	cp -p $(srcdir)/tmp/sdag_t.c $(top_srcdir)/incl/sdag_t.h
	cp -p $(srcdir)/tmp/text_t.c $(top_srcdir)/incl/text_t.h
	$(SXETCBIN)/menage-tmp tmp $(srcdir); : $(MAKE)
