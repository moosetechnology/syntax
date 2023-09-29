# Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

aclocaldir = $(datadir)/aclocal
aclocal_DATA = syntax-core.m4

pkginclude_HEADERS = 	incl/sxcommon.h	\
			incl/sxunix.h	\
			incl/sxu2.h	\
			incl/SXante.h	\
			incl/SXpost.h	\
			incl/sxalloc.h	\
			incl/sxindex.h	\
			incl/sxword.h	\
			incl/varstr.h	\
			incl/sxba_bag.h	\
			incl/X_root_rep.h	\
			incl/X_rep.h	\
			incl/XxY_rep.h	\
			incl/XxYxZ_rep.h	\
			incl/reg_exp_t.h	\
			incl/reg_exp_pact.h	\
			incl/reg_exp_sact.h	\
			incl/reg_exp_nn.h	\
			incl/sxcheck_keyword.h	\
			incl/lecl_td.h	\
			incl/sxsstmt.h	\
			incl/sxmatrix2vector.h	\
			incl/sxscrmbl.h	\
			incl/sxsymbol_table.h	\
			incl/put_edit.h	\
			incl/rcg_sglbl.h	\
			incl/SS.h	\
			incl/X.h	\
			incl/XH.h	\
			incl/XT.h	\
			incl/X_root.h	\
			incl/XxY.h	\
			incl/XxYxZ.h	\
			incl/bag.h	\
			incl/dag_scanner.h	\
			incl/dag_t.h	\
			incl/earley.h	\
			incl/nbest.h	\
			incl/sxSL.h	\
			incl/sxba.h	\
			incl/sxdico.h	\
			incl/sxdynam_parser.h	\
			incl/sxdynam_scanner.h	\
			incl/sxlist.h	\
			incl/sxnd.h	\
			incl/sxpart.h	\
			incl/sxset.h	\
			incl/sxspell.h	\
			incl/sxspf.h	\
			incl/sxstack.h	\
			incl/sxsubset.h	\
			incl/udag_t.h	\
			\
			incl/lex_lfg.h	\
			incl/sxspell_rules.h

noinst_HEADERS =	etc/incl/B_tables.h

dist_pkgdata_DATA =	src/lex_lfg.c		\
			src/dag_scanner.c	\
			src/sxspell_mngr.c

EXTRA_DIST =		syntax-core.m4

SUBDIRS =	lib	\
		doc	\
		\
		lfg	\
		dico

