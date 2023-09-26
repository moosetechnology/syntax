bin_PROGRAMS =	dico

LNorCP=ln -s
#LNorCP=${SHELL} $(SX)/etc/bin/lien-relatif.sh

DICO =	$(top_srcdir)/dico
SRC =	$(DICO)/src
INCL =	$(DICO)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL) $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/dico.out:	dico
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

dico_SOURCES =	$(SRC)/dico_main.c	\
		$(SRC)/dico_t.c		\
		$(SRC)/dico_act.c
dico_LDADD =	$(top_builddir)/lib/libsx.a

dico_act.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/sxdico.h	\
		$(SRC)/dico_act.c	
	$(CC_FLAGS) $(SRC)/dico_act.c		\
		-Dsxtables=dico_tables

dico_main.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(SXINCL)/sxdico.h	\
		$(SRC)/dico_main.c
	$(CC_FLAGS) $(SRC)/dico_main.c		\
		-Dsxtables=dico_tables

dico_t.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/dico_t.c
	$(CC_FLAGS) $(SRC)/dico_t.c		\
		-Dsxtables=dico_tables		\
		-DSCANACT=dico_scan_act		\
		-DSEMACT=dico_sem_act


