bin_PROGRAMS =	make_a_dico	\
		amlgm_cmpd

LNorCP=ln -s

MAKE_A_DICO =	$(top_srcdir)/make_a_dico
SRC =	$(MAKE_A_DICO)/src
INCL =	$(MAKE_A_DICO)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/make_a_dico.out:	make_a_dico
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

../../bin/amlgm_cmpd.out:	amlgm_cmpd
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


make_a_dico_SOURCES =	$(SRC)/make_a_dico.c	\
			$(SRC)/make_a_dico_t.c
make_a_dico_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

make_a_dico.o:		$(SXINCL)/sxunix.h		\
			$(SXETCINCL)/release.h		\
			$(SXINCL)/fsa.h			\
			$(SRC)/make_a_dico.c
	$(CC_FLAGS) $(SRC)/make_a_dico.c

make_a_dico_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/make_a_dico_t.c
	$(CC_FLAGS) $(SRC)/make_a_dico_t.c	\
		-DSCANACT=make_a_dico_scanact	\
		-DSEMACT=make_a_dico_action




amlgm_cmpd_SOURCES =	$(SRC)/amlgm_cmpd_main.c	\
			$(SRC)/amlgm_cmpd_smp.c		\
			$(SRC)/amlgm_cmpd_t.c
amlgm_cmpd_LDADD =	$(top_builddir)/lib/libsx.a

amlgm_cmpd_smp.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/fsa.h		\
			$(SXINCL)/varstr.h	\
			$(SRC)/amlgm_cmpd_smp.c
	$(CC_FLAGS) $(SRC)/amlgm_cmpd_smp.c

amlgm_cmpd_t.o:	$(SXINCL)/sxunix.h 		\
		$(SRC)/amlgm_cmpd_t.c
	$(CC_FLAGS) $(SRC)/amlgm_cmpd_t.c	\
		-Dsempass=amlgm_cmpd_smp

amlgm_cmpd_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/fsa.h		\
			$(SRC)/amlgm_cmpd_main.c
	$(CC_FLAGS) $(SRC)/amlgm_cmpd_main.c
