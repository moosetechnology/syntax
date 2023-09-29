# Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

aclocaldir = $(datadir)/aclocal
aclocal_DATA =	syntax.m4

# DOXYGEN SUPPORT
include $(top_srcdir)/aminclude.am

pkginclude_HEADERS = 	incl/sxcommon.h	\
			incl/sxunix.h	\
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
			incl/udag_scanner.h	\
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
			incl/fsa.h	\
			incl/text_t.h	\
			incl/udag_t.h	\
			incl/sdag_t.h	\
			incl/re_t.h	\
			\
			incl/sxversion.h	\
			\
			incl/lex_lfg.h	\
			incl/sxspell_rules.h

dist_pkgdata_DATA =	$(top_srcdir)/src/*.c	\
			make_a_dico/src/dico_consult.c

EXTRA_DIST =		syntax.m4			\
			\
			etc/make/gccoptions			\
			\
			etc/bin/make-makefile		\
			etc/bin/liens-tmp		\
			etc/bin/menage-tmp		\
			\
			doc/manual	\
			\
			$(top_srcdir)/examples/chimie/*		\
			$(top_srcdir)/examples/cx/*		\
			$(top_srcdir)/examples/f77/*		\
			$(top_srcdir)/examples/idcounter/*	\
			$(top_srcdir)/examples/lustre/*		\
			$(top_srcdir)/examples/ppada/*		\
			$(top_srcdir)/examples/ppc/*

noinst_HEADERS =	incl/util/B_tables.h	\
			incl/util/S_tables.h	\
			incl/util/P_tables.h	\
			incl/util/D_tables.h	\
			incl/util/R_tables.h	\
			incl/util/T_tables.h	\
			incl/util/PP_tables.h	\
			incl/util/bstr.h

SUBDIRS =	lib	\
		doc	\
		etc	\
		\
		dico	\
		lfg	\
		\
		bnf		\
		csynt		\
		csynt_lc	\
		dummy_csynt	\
		lecl		\
		make_a_dico	\
		paradis		\
		prio		\
		rcg		\
		recor		\
		semact		\
		semat		\
		semc		\
		tables		\
		tdef		\
		xbnf		\
		xcsynt		\
		yax		\
		ysx		\
		\
		DAG

BOOTSTRAP =	DAG	\
		bnf	\
		dico	\
		lecl	\
		lfg	\
		paradis	\
		prio	\
		rcg	\
		recor	\
		semact	\
		semat	\
		semc	\
		tdef	\
		xbnf	\
		yax	\
		ysx

TESTS =	hypergenese-1	\
	hypergenese-2


TMP_BIN=$(abs_top_builddir)/dico/lib/:$(abs_top_builddir)/lfg/lib/:$(abs_top_builddir)/bnf/lib/:$(abs_top_builddir)/csynt/lib/:$(abs_top_builddir)/csynt_lc/lib/:$(abs_top_builddir)/dummy_csynt/lib/:$(abs_top_builddir)/lecl/lib/:$(abs_top_builddir)/make_a_dico/lib/:$(abs_top_builddir)/paradis/lib/:$(abs_top_builddir)/prio/lib/:$(abs_top_builddir)/rcg/lib/:$(abs_top_builddir)/recor/lib/:$(abs_top_builddir)/semact/lib/:$(abs_top_builddir)/semat/lib/:$(abs_top_builddir)/semc/lib/:$(abs_top_builddir)/tables/lib/:$(abs_top_builddir)/tdef/lib/:$(abs_top_builddir)/xbnf/lib/:$(abs_top_builddir)/xcsynt/lib/:$(abs_top_builddir)/yax/lib/:$(abs_top_builddir)/ysx/lib/


hypergenese: nickel chrome

bootstrap: hypergenese

clean-hypergeneses:
	if (test -f $(top_builddir)/hypergenese-1) then rm $(top_builddir)/hypergenese-1; fi
	if (test -f $(top_builddir)/hypergenese-2) then rm $(top_builddir)/hypergenese-2; fi

nickel:
	@echo ""
	@echo "#####################################"
	@echo "###### Cleaning bootstrap data ######"
	@echo "#####################################"
	rm -rf $(top_builddir)/*/tmp $(top_builddir)/*/*/tmp $(top_builddir)/*/*/*/tmp

clean-local: nickel clean-hypergeneses

chrome:
	@echo ""
	@echo "#####################################"
	@echo "########### Bootstrapping ###########"
	@echo "#####################################"
	@for FILE in $(BOOTSTRAP);					\
	do								\
		echo "";						\
		echo "";						\
		echo "###### Bootstrapping $$FILE ######";		\
		echo "";						\
		cd $$FILE;						\
		PATH=$(TMP_BIN):$(PATH) $(MAKE) bootstrap;		\
		cd ..;							\
	done;								\
	$(MAKE);

hypergenese-1 hypergenese-2: clean-hypergeneses
	$(MAKE) nickel
	chmod -R +w $(top_srcdir)
	echo '#!/bin/sh' > $@
	if $(MAKE) chrome; then echo "exit 0" >> $@; else echo "exit 1" >> $@; fi
	chmod +x $@

#	chmod -R -w $(top_srcdir)
#	chmod -R +w $(top_builddir)

#	chmod -R +w $(prefix)

cpp:
	echo '#include <stdio.h>'|gcc -Wp,-C,-dD,-dM -E -x c -

macpackage:
	mkdir SYNTAX && \
	./configure --prefix `pwd`/SYNTAX && \
	make && \
	make install && \
	echo Now please open SYNTAX.pmdoc in PackageMaker and clic Build. Then you can safely remove the temporary SYNTAX directory

