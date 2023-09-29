bin_PROGRAMS =	lfg	\
		lfg_lex

LNorCP=ln -s
#LNorCP=$(SHELL) $(SX)/etc/bin/lien-relatif.sh

LFG = $(top_srcdir)/lfg
SRC =	$(LFG)/src
INCL =	$(LFG)/incl
SXINCL =	$(top_srcdir)/incl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c -O2


../../bin/lfg.out:	lfg
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

../../bin/lfg_lex.out:	lfg_lex
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

#####################################################################################################################
lfg_SOURCES =	$(SRC)/lfg_main.c		\
		$(SRC)/lfg_lex_scanact.c	\
		$(SRC)/lfg_smp.c		\
		$(SRC)/lfg_t.c
lfg_LDADD =	$(top_builddir)/lib/libsx.a

lfg_smp.o:	$(SXINCL)/sxunix.h 	\
		$(SXINCL)/sxversion.h	\
		$(SRC)/lfg_smp.c
	$(CC_FLAGS) $(SRC)/lfg_smp.c
#		-DEBUG

lfg_main.o:	$(SXINCL)/sxunix.h 	\
		$(SXINCL)/sxversion.h	\
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
		$(SXINCL)/sxversion.h	\
		$(SRC)/lfg_lex_main.c
	$(CC_FLAGS) $(SRC)/lfg_lex_main.c

lfg_lex_scanact.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/lfg_lex_scanact.c
	$(CC_FLAGS) $(SRC)/lfg_lex_scanact.c

lfg_lex_smp.o:	$(SXINCL)/sxunix.h 	\
		$(SXINCL)/sxversion.h	\
		$(SRC)/lfg_lex_smp.c
	$(CC_FLAGS) $(SRC)/lfg_lex_smp.c
#		-DEBUG

lfg_lex_t.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/lfg_lex_t.c
	$(CC_FLAGS) $(SRC)/lfg_lex_t.c		\
		-Dsxtables=lfg_lex_tables	\
		-Dsempass=lfg_lex_smp		\
		-DSCANACT=lfg_lex_scanact

