bin_PROGRAMS =	lfg	\
		lfg_lex

bin_SCRIPTS =	disamb	\
		ntf

CLEANFILES =	disamb	\
		ntf

LNorCP=ln -s
#LNorCP=${SHELL} $(SX)/etc/bin/lien-relatif.sh

LFG = $(top_srcdir)/lfg
SRC =	$(LFG)/src
INCL =	$(LFG)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

EXTRA_DIST =	$(LFG)/disamb.pl	\
		$(LFG)/ntf.pl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/lfg.out:	lfg
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

../../bin/lfg_lex.out:	lfg_lex
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

../../bin/disamb.pl:	../disamb.pl
	cp -p $< $@

../../bin/ntf.pl:	../ntf.pl
	cp -p $< $@


ntf:	$(LFG)/ntf.pl
	cp -p $< $@

disamb:	$(LFG)/disamb.pl
	cp -p $< $@

#####################################################################################################################
lfg_SOURCES =	$(SRC)/lfg_main.c		\
		$(SRC)/lfg_lex_scanact.c	\
		$(SRC)/lfg_smp.c		\
		$(SRC)/lfg_t.c
lfg_LDADD =	$(top_builddir)/lib/libsx.a

lfg_smp.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/lfg_smp.c
	$(CC_FLAGS) $(SRC)/lfg_smp.c
#		-DEBUG

lfg_main.o:	$(SXINCL)/sxunix.h 	\
		$(SXETCINCL)/release.h 	\
		$(SRC)/lfg_main.c
	$(CC_FLAGS) $(SRC)/lfg_main.c

lfg_t.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/lfg_t.c
	$(CC_FLAGS) $(SRC)/lfg_t.c	\
		-Dsxtables=lfg_tables	\
		-Dsempass=lfg_smp	\
		-DSCANACT=lfg_lex_scanact



#####################################################################################################################

lfg_lex_SOURCES = 	$(SRC)/lfg_lex_main.c		\
			$(SRC)/lfg_lex_scanact.c	\
			$(SRC)/lfg_lex_smp.c		\
			$(SRC)/lfg_lex_t.c
lfg_lex_LDADD =	$(top_builddir)/lib/libsx.a

#	$(SRC)/sxdico_process.d.o

lfg_lex_main.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/lfg_lex_main.c
	$(CC_FLAGS) $(SRC)/lfg_lex_main.c	\
		-DSXVERSION=\"$(VERSION)\"

lfg_lex_scanact.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/lfg_lex_scanact.c
	$(CC_FLAGS) $(SRC)/lfg_lex_scanact.c

lfg_lex_smp.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/lfg_lex_smp.c
	$(CC_FLAGS) $(SRC)/lfg_lex_smp.c
#		-DEBUG

lfg_lex_t.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/lfg_lex_t.c
	$(CC_FLAGS) $(SRC)/lfg_lex_t.c		\
		-Dsxtables=lfg_lex_tables	\
		-Dsempass=lfg_lex_smp		\
		-DSCANACT=lfg_lex_scanact

