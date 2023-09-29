bin_PROGRAMS =	csynt

LNorCP=ln -s

CSYNT =	$(top_srcdir)/csynt
SRC =	$(CSYNT)/src
INCL =	$(CSYNT)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

EXTRA_DIST =	$(INCL)/csynt_optim.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/csynt.out:	csynt
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

csynt_SOURCES =	$(SRC)/csynt.c			\
		$(SRC)/LALR1.c			\
		$(SRC)/path.c			\
		$(SRC)/single.c			\
		$(SRC)/algoV.c			\
		$(SRC)/optim_lo.c		\
		$(SRC)/xprdct_mngr.c		\
		$(SRC)/OPTIM.c
csynt_LDADD =	$(SXBIBS)

csynt.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/sxversion.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/csynt.c
	$(CC_FLAGS) $(SRC)/csynt.c

LALR1.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/sxversion.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/D_tables.h	\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/LALR1.c
	$(CC_FLAGS) $(SRC)/LALR1.c

path.o:			$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxversion.h		\
			$(SRC)/path.c
	$(CC_FLAGS) $(SRC)/path.c

optim_lo.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/sxversion.h		\
			$(SXINCL)/sxunix.h		\
			$(SRC)/optim_lo.c
	$(CC_FLAGS) $(SRC)/optim_lo.c

xprdct_mngr.o:	$(SXINCL)/sxunix.h		\
			$(SRC)/xprdct_mngr.c
	$(CC_FLAGS) $(SRC)/xprdct_mngr.c

single.o:	$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/sxunix.h		\
			$(SRC)/single.c
	$(CC_FLAGS) $(SRC)/single.c

algoV.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/sxunix.h		\
			$(SRC)/algoV.c
	$(CC_FLAGS) $(SRC)/algoV.c

OPTIM.o:		$(INCL)/csynt_optim.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/P_tables.h	\
			$(SXINCL)/sxunix.h		\
			$(SRC)/OPTIM.c
	$(CC_FLAGS) $(SRC)/OPTIM.c
