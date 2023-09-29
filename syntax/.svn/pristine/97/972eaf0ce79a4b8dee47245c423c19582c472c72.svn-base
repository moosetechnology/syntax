SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/etc/bin

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir)
	cd tmp; ../$(SXETCBIN)/make-makefile lecl | $(MAKE) -f -
	cd tmp; ../$(SXETCBIN)/make-makefile pplecl | $(MAKE) -f -
	$(MAKE)
	cp -p $(srcdir)/tmp/lecl_t.c $(top_srcdir)/incl/reg_exp_t.h
	$(SXETCBIN)/menage-tmp tmp $(srcdir)
