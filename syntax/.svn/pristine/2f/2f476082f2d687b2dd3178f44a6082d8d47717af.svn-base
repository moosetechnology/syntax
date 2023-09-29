bin_PROGRAMS =	tdef	\
		pptdef

LNorCP=ln -s

TDEF =	$(top_srcdir)/tdef
SRC =	$(TDEF)/src
INCL =	$(TDEF)/incl
SXSRC =	$(top_srcdir)/src
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL) $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/tdef.out:	tdef
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

../../bin/pptdef.out:	pptdef
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


tdef_SOURCES =	$(SRC)/tdef_t.c		\
		$(SRC)/tdef_main.c	\
		$(SRC)/tdef_sact.c	\
		$(SRC)/tdef_action.c
tdef_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

pptdef_SOURCES =	$(SRC)/pptdef_t.c	\
			$(SRC)/pptdef_main.c
pptdef_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a


tdef_t.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/tdef_t.c
	$(CC_FLAGS) $(SRC)/tdef_t.c		\
		-Dsxtables=tdef_tables	\
		-DSCANACT=tdef_scan_act	\
		-DSEMACT=tdef_action

tdef_main.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(SRC)/tdef_main.c
	$(CC_FLAGS) $(SRC)/tdef_main.c
	chmod -R +w $(SXSRC)
	cp -p $(SRC)/tdef_main.c $(SXSRC)/tdef_main.c
	chmod -R -w $(SXSRC)
	chmod -R +w $(top_builddir)

tdef_sact.o:	$(SXINCL)/sxunix.h		\
		$(SRC)/tdef_sact.c
	$(CC_FLAGS) $(SRC)/tdef_sact.c

tdef_action.o:	$(SXINCL)/sxunix.h		\
		$(SXINCL)/put_edit.h	\
		$(SXETCINCL)/B_tables.h	\
		$(SRC)/tdef_action.c
	$(CC_FLAGS) $(SRC)/tdef_action.c




pptdef_t.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/pptdef_t.c
	$(CC_FLAGS) $(SRC)/pptdef_t.c		\
		-Dsxtables=pptdef_tables

pptdef_main.o:	$(SXINCL)/sxunix.h	\
		$(SRC)/pptdef_main.c
	$(CC_FLAGS) $(SRC)/pptdef_main.c

