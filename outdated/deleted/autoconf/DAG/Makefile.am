SUBDIRS =	lib

SXINCL  = $(top_srcdir)/incl
SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir)
	cd tmp; ../$(SXETCBIN)/make-makefile re | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile dag | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile udag | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile sdag | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile text -dummy_csynt | $(MAKE) -f -
	$(MAKE)
	cp -p $(srcdir)/tmp/re_t.c $(top_srcdir)/incl/re_t.h
	cp -p $(srcdir)/tmp/dag_t.c $(top_srcdir)/incl/dag_t.h
	cp -p $(srcdir)/tmp/udag_t.c $(top_srcdir)/incl/udag_t.h
	cp -p $(srcdir)/tmp/sdag_t.c $(top_srcdir)/incl/sdag_t.h
	cp -p $(srcdir)/tmp/text_t.c $(top_srcdir)/incl/text_t.h
	$(SXETCBIN)/menage-tmp tmp $(srcdir)
