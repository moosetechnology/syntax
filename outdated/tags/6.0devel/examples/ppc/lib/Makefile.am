bin_PROGRAMS = ppc

LNorCP = ln -s

PPC =	$(top_srcdir)
SRC =	$(PPC)/src
INCL =	$(PPC)/incl
TMP=$(PPC)/tmp
#BIBS = $(syntaxlib)
BIBS = $(syntaxlibdir)/libsx.debug.a
SXINCL = $(syntaxincl)

EXTRA_DIST =	$(INCL)/bnf_vars.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL) $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c
COMPILER=cc

ppc_SOURCES =	$(SRC)/ppc1_t.c		\
		$(SRC)/ppc2_t.c		\
		$(SRC)/ppc3_t.c		\
		$(SRC)/ppc_args_t.c	\
		$(SRC)/ppc_main.c	\
		$(SRC)/ppc_sact.c
ppc_LDADD =	$(BIBS)


ppc1_t.o:	$(SRC)/ppc1_t.c		\
			$(SXINCL)/sxunix.h
	$(CC_FLAGS) -Dsxtables=ppc1_tables -DSCANACT=ppc_scan_act $(SRC)/ppc1_t.c

ppc2_t.o:	$(SRC)/ppc2_t.c		\
			$(SXINCL)/sxunix.h
	$(CC_FLAGS) -Dsxtables=ppc2_tables -DSCANACT=ppc_scan_act $(SRC)/ppc2_t.c

ppc3_t.o:	$(SRC)/ppc3_t.c		\
			$(SXINCL)/sxunix.h
	$(CC_FLAGS) -Dsxtables=ppc3_tables -DSCANACT=ppc_scan_act $(SRC)/ppc3_t.c

ppc_args_t.o:	$(SRC)/ppc_args_t.c	\
			$(SXINCL)/sxunix.h
	$(CC_FLAGS) -Dsxtables=ppc_args_tables -DSCANACT=ppc_args_scan_act -DSEMACT=ppc_args_semact $(SRC)/ppc_args_t.c

ppc_main.o:	$(SRC)/ppc_main.c	\
			$(INCL)/ppc1_td.h	\
			$(INCL)/ppc2_td.h	\
			$(INCL)/ppc3_td.h	\
			$(SXINCL)/sxunix.h
	$(CC_FLAGS) $(SRC)/ppc_main.c

ppc_sact.o:	$(SRC)/ppc_sact.c	\
			$(SXINCL)/sxunix.h
	$(CC_FLAGS) $(SRC)/ppc_sact.c
