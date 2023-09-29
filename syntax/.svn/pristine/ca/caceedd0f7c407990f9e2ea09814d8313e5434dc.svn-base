bin_PROGRAMS =	tables_c

TABLES =	$(top_srcdir)/tables
SRC =		$(TABLES)/src
INCL =		$(TABLES)/incl
SXINCL =	$(top_srcdir)/incl
SXBIBS =	$(top_builddir)/lib/libsxutil.a $(top_builddir)/lib/libsx.a

BNFLIB =	$(top_builddir)/bnf/lib
BNFINCL =	$(top_srcdir)/bnf/incl

EXTRA_DIST =	$(INCL)/out.h		\
		$(INCL)/tables.h	\
		$(SXINCL)/util/lig.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -I$(BNFINCL) -c


../../bin/tables_c.out:	tables_c
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


tables_c_SOURCES =	$(SRC)/init_tbl.c	\
			$(SRC)/out_P_tbl.c	\
			$(SRC)/out_RCVR_tbl.c	\
			$(SRC)/out_S_tbl.c	\
			$(SRC)/out_PP_tbl.c	\
			$(SRC)/out_T_tbl.c	\
			$(SRC)/out_DP_tbl.c	\
			$(SRC)/out_DS_tbl.c	\
			$(SRC)/out_lig_tbl.c	\
			$(SRC)/out.c		\
			$(SRC)/tables_c.c
tables_c_LDADD =	$(SXBIBS)

tables_c.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxversion.h	\
		$(INCL)/tables.h	\
		$(INCL)/out.h		\
		$(SXINCL)/sem_by.h	\
		$(SXINCL)/sxba.h	\
		$(TABLES)/src/tables_c.c
	$(CC_FLAGS) $(TABLES)/src/tables_c.c

init_tbl.o:	$(SXINCL)/sxunix.h	\
		$(INCL)/tables.h	\
		$(SXINCL)/util/P_tables.h	\
		$(SXINCL)/util/S_tables.h	\
		$(SXINCL)/util/PP_tables.h	\
		$(SXINCL)/util/T_tables.h	\
		$(SRC)/init_tbl.c
	$(CC_FLAGS) $(SRC)/init_tbl.c

out_S_tbl.o:	$(SXINCL)/sxunix.h	\
		$(INCL)/tables.h	\
		$(INCL)/out.h		\
		$(SXINCL)/util/S_tables.h	\
		$(TABLES)/src/out_S_tbl.c
	$(CC_FLAGS) $(TABLES)/src/out_S_tbl.c

out_PP_tbl.o:	$(SXINCL)/sxunix.h	\
		$(INCL)/tables.h	\
		$(INCL)/out.h		\
		$(SXINCL)/util/PP_tables.h	\
		$(SRC)/out_PP_tbl.c
	$(CC_FLAGS) $(SRC)/out_PP_tbl.c

out_T_tbl.o:	$(SXINCL)/sxunix.h	\
		$(INCL)/tables.h	\
		$(INCL)/out.h		\
		$(SXINCL)/util/T_tables.h	\
		$(SRC)/out_T_tbl.c
	$(CC_FLAGS) $(SRC)/out_T_tbl.c

out_P_tbl.o:	$(SXINCL)/sxunix.h	\
		$(INCL)/tables.h	\
		$(INCL)/out.h		\
		$(SXINCL)/util/P_tables.h	\
		$(TABLES)/src/out_P_tbl.c
	$(CC_FLAGS) $(TABLES)/src/out_P_tbl.c

out_RCVR_tbl.o:	$(SXINCL)/sxunix.h	\
		$(INCL)/tables.h	\
		$(INCL)/out.h		\
		$(SXINCL)/util/P_tables.h	\
		$(SXINCL)/util/R_tables.h	\
		$(SXINCL)/sxba.h	\
		$(TABLES)/src/out_RCVR_tbl.c
	$(CC_FLAGS) $(TABLES)/src/out_RCVR_tbl.c

out_DP_tbl.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxdynam_parser.h	\
		$(TABLES)/src/out_DP_tbl.c
	$(CC_FLAGS) $(TABLES)/src/out_DP_tbl.c

out_DS_tbl.o:	$(SXINCL)/sxunix.h	\
		$(SXINCL)/sxdynam_scanner.h	\
		$(TABLES)/src/out_DS_tbl.c
	$(CC_FLAGS) $(TABLES)/src/out_DS_tbl.c

out_lig_tbl.o:	$(SXINCL)/sxunix.h	\
		$(INCL)/tables.h	\
		$(INCL)/out.h		\
		$(SXINCL)/util/lig.h	\
		$(TABLES)/src/out_lig_tbl.c
	$(CC_FLAGS) -I$(SXINCL)/util $(TABLES)/src/out_lig_tbl.c

out.o:		$(SXINCL)/sxunix.h	\
		$(SXINCL)/sem_by.h	\
		$(INCL)/tables.h	\
		$(SXINCL)/util/P_tables.h	\
		$(SXINCL)/util/S_tables.h	\
		$(SRC)/out.c
	$(CC_FLAGS) $(SRC)/out.c
