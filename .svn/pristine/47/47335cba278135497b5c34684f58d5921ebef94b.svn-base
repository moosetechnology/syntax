bin_PROGRAMS =	recor		\
		pprecor

noinst_PROGRAMS =	print_rt


LNorCP=ln -s

RECOR =	$(top_srcdir)/recor
SRC =	$(RECOR)/src
INCL =	$(RECOR)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

LECLLIB =	$(top_builddir)/lecl/lib

EXTRA_DIST =	$(INCL)/recor_node.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c



../../bin/recor.out:	recor
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

../../bin/pprecor.out:	pprecor
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


recor_SOURCES =	$(SRC)/recor_t.c		\
		$(SRC)/recor_main.c		\
		$(SRC)/recor_sact.c		\
		$(SRC)/recor_smp.c
recor_LDADD =	$(LECLLIB)/leclread.o	\
		$(LECLLIB)/leclfree.o	\
		$(SXBIBS)

pprecor_SOURCES =	$(SRC)/pprecor_t.c		\
			$(SRC)/pprecor_main.c		\
			$(SRC)/pprecor_sact.c
pprecor_LDADD =	$(SXBIBS)

print_rt_SOURCES =	$(SRC)/print_rt.c
print_rt_LDADD =	$(SXBIBS)


recor_t.o: 	$(SXINCL)/sxunix.h	\
		$(SRC)/recor_t.c
	$(CC_FLAGS) $(SRC)/recor_t.c		\
		-Dsxtables=recor_tables	\
		-DSCANACT=recor_scan_act\
		-Dsempass=recor_smp

recor_main.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(SRC)/recor_main.c
	$(CC_FLAGS) $(SRC)/recor_main.c

recor_sact.o: 	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(SRC)/recor_sact.c
	$(CC_FLAGS) $(SRC)/recor_sact.c

recor_smp.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(SXINCL)/put_edit.h	\
		$(INCL)/recor_node.h	\
		$(SXINCL)/util/R_tables.h	\
		$(SXINCL)/util/B_tables.h	\
		$(SXINCL)/util/S_tables.h	\
		$(SRC)/recor_smp.c
	$(CC_FLAGS) $(SRC)/recor_smp.c




pprecor_main.o:	$(SXINCL)/sxunix.h	\
		$(SRC)/pprecor_main.c
	$(CC_FLAGS) $(SRC)/pprecor_main.c


pprecor_t.o: 	$(SXINCL)/sxunix.h	\
		$(SRC)/pprecor_t.c
	$(CC_FLAGS) $(SRC)/pprecor_t.c		\
		-Dsxtables=pp_recor_tables	\
		-DSCANACT=pp_recor_scan_act

pprecor_sact.o:	$(SXINCL)/sxunix.h	\
		$(SRC)/pprecor_sact.c
	$(CC_FLAGS) $(SRC)/pprecor_sact.c




print_rt.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/util/R_tables.h	\
		$(SRC)/print_rt.c
	$(CC_FLAGS) $(SRC)/print_rt.c
