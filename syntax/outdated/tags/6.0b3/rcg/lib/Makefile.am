bin_PROGRAMS =	rcg	\
		drcg

LNorCP =	ln -s

RCG =		$(top_srcdir)/rcg
SRC =		$(RCG)/src
INCL =		$(RCG)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl

EXTRA_DIST =	$(INCL)/rcg.h		\
		$(INCL)/rcg_td.h	\
		$(SXINCL)/dcg_cst.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -I$(BNFINCL) -DEBUG -c


../../bin/rcg.out:	rcg
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

../../bin/drcg.out:	drcg
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


rcg_SOURCES =	$(SRC)/rcg_main.c		\
		$(SRC)/rcg_sact.c		\
		$(SRC)/rcg_smp.c    		\
		$(SRC)/rcg_1rcg_2var.c 		\
		$(SRC)/rcg_gen_parser.c		\
		$(SRC)/rcg_gen_bnf.c   		\
		$(SRC)/rcg_t.c
rcg_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

rcg_main.o:	$(SXINCL)/sxunix.h	\
		$(SXETCINCL)/release.h	\
		$(INCL)/rcg.h		\
		$(SRC)/rcg_main.c
	$(CC_FLAGS) $(SRC)/rcg_main.c



rcg_sact.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/varstr.h	\
		$(INCL)/rcg.h		\
		$(SRC)/rcg_sact.c
	$(CC_FLAGS) $(SRC)/rcg_sact.c



rcg_smp.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/varstr.h	\
		$(SXINCL)/put_edit.h	\
		$(SXINCL)/XH.h		\
		$(SXINCL)/XxY.h		\
		$(INCL)/rcg_td.h	\
		$(INCL)/rcg.h		\
		$(SRC)/rcg_smp.c
	$(CC_FLAGS) $(SRC)/rcg_smp.c
#			-DEBUG_PRDCT=4 (code du prdct que l'on veut tracer)

rcg_1rcg_2var.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			$(INCL)/rcg_td.h	\
			$(INCL)/rcg.h		\
			$(SRC)/rcg_1rcg_2var.c
	$(CC_FLAGS) $(SRC)/rcg_1rcg_2var.c

rcg_gen_parser.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxba.h	\
			$(SXINCL)/varstr.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			$(INCL)/rcg_td.h	\
			$(INCL)/rcg.h		\
			$(SXINCL)/rcg_sglbl.h	\
			$(SRC)/rcg_gen_parser.c
	$(CC_FLAGS) $(SRC)/rcg_gen_parser.c

rcg_gen_bnf.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/varstr.h	\
		$(SXINCL)/XH.h		\
		$(SXINCL)/XxY.h		\
		$(INCL)/rcg_td.h	\
		$(INCL)/rcg.h		\
		$(SRC)/rcg_gen_bnf.c
	$(CC_FLAGS) $(SRC)/rcg_gen_bnf.c

rcg_t.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/rcg_t.c
	$(CC_FLAGS) $(SRC)/rcg_t.c			\
		-Dsxtables=rcg_tables		\
		-DSCANACT=rcg_scan_act		\
		-Dsempass=rcg_smp


drcg_SOURCES =	$(SRC)/drcg_main.c		\
		$(SRC)/rcg_sact.c		\
		$(SRC)/rcg_smp.c    		\
		$(SRC)/rcg_1rcg_2var.c 		\
		$(SRC)/rcg_gen_parser.c		\
		$(SRC)/rcg_gen_bnf.c   		\
		$(SRC)/rcg_t.c			\
		$(SRC)/drcg_smp.c    		\
		$(SRC)/drcg_t.c
drcg_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

drcg_main.o:	$(SXINCL)/sxunix.h	\
		$(SXETCINCL)/release.h	\
		$(INCL)/rcg.h		\
		$(SRC)/drcg_main.c
	$(CC_FLAGS) $(SRC)/drcg_main.c



drcg_smp.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/varstr.h	\
		$(SXINCL)/XH.h		\
		$(SXINCL)/XxY.h		\
		$(INCL)/rcg_td.h	\
		$(INCL)/rcg.h		\
		$(SXINCL)/dcg_cst.h	\
		$(SRC)/drcg_smp.c
	$(CC_FLAGS) $(SRC)/drcg_smp.c

drcg_t.o:	$(SXINCL)/sxunix.h 	\
		$(SRC)/drcg_t.c	
	$(CC_FLAGS) $(SRC)/drcg_t.c			\
		-Dsxtables=drcg_tables		\
		-DSCANACT=rcg_scan_act		\
		-Dsempass=drcg_smp

