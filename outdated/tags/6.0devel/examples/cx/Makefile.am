# Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

SUBDIRS =	lib

SXETCBIN  = $(top_srcdir)/../../etc/bin
CX =	$(top_srcdir)
SRC =	$(CX)/src
SPEC =	$(CX)/spec

EXTRA_DIST =	$(srcdir)/spec/*.*


bootstrap:
	$(SXETCBIN)/liens-tmp tmp $(srcdir); : $(MAKE)
	cd tmp; ../$(SXETCBIN)/make-makefile cx -st_to_c | CYGWIN="$$CYGWIN binmode" $(MAKE) -f -
	$(SXETCBIN)/menage-tmp tmp $(srcdir); : $(MAKE)
	$(MAKE)
