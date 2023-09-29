# Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = foreign

aclocaldir = $(datadir)/aclocal
aclocal_DATA =	syntax.m4

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
			incl/lex_lfg.h	\
			incl/sxspell_rules.h

dist_pkgdata_DATA =	$(top_srcdir)/src/*.c

EXTRA_DIST =		syntax.m4			\
			\
			etc/bin/make-makefile		\
			etc/bin/liens-tmp		\
			etc/bin/menage-tmp

noinst_HEADERS =	etc/incl/B_tables.h	\
			etc/incl/S_tables.h	\
			etc/incl/P_tables.h	\
			etc/incl/D_tables.h	\
			etc/incl/R_tables.h	\
			etc/incl/T_tables.h	\
			etc/incl/PP_tables.h	\
			etc/incl/bstr.h		\
			etc/incl/release.h

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
		tabc		\
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
		tabc	\
		tdef	\
		xbnf	\
		yax	\
		ysx

TESTS =	hypergenese-1	\
	hypergenese-2


PWD=`pwd`
TMP_BIN=$(PWD)/dico/lib/:$(PWD)/lfg/lib/:$(PWD)/bnf/lib/:$(PWD)/csynt/lib/:$(PWD)/csynt_lc/lib/:$(PWD)/dummy_csynt/lib/:$(PWD)/lecl/lib/:$(PWD)/make_a_dico/lib/:$(PWD)/paradis/lib/:$(PWD)/prio/lib/:$(PWD)/rcg/lib/:$(PWD)/recor/lib/:$(PWD)/semact/lib/:$(PWD)/semat/lib/:$(PWD)/tabc/lib/:$(PWD)/tables/lib/:$(PWD)/tdef/lib/:$(PWD)/xbnf/lib/:$(PWD)/xcsynt/lib/:$(PWD)/yax/lib/:$(PWD)/ysx/lib/

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
	@time sh -c '							\
	for FILE in $(BOOTSTRAP)					\
	;								\
	do								\
		echo "";						\
		echo "";						\
		echo "###### Bootstrapping $$FILE ######";		\
		echo "";						\
		cd $$FILE;						\
		PATH=$(TMP_BIN):$(PATH) $(MAKE) bootstrap		\
				CFLAGS="$(CFLAGS)"			\
				LDFLAGS="$(LDFLAGS)"			\
				sxV="$(VERSION)";			\
		cd ..;							\
	done;								\
	$(MAKE);							\
	echo "";							\
	echo "Bootstrapping time:"					\
	'	< /dev/null 2>&1

hypergenese-1 hypergenese-2: clean-hypergeneses
	$(MAKE) nickel
	chmod -R +w $(top_srcdir)
	echo "#!/bin/sh" > $@
	echo -n "exit " >> $@
	if $(MAKE) chrome; then echo 0 >> $@; else echo 1 >> $@; fi
	chmod +x $@

#	chmod -R -w $(top_srcdir)
#	chmod -R +w $(top_builddir)

#	chmod -R +w $(prefix)

cpp:
	echo '#include <stdio.h>'|gcc -Wp,-C,-dD,-dM -E -x c -
