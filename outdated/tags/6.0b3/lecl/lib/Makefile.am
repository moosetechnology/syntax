bin_PROGRAMS =	lecl		\
		pplecl		\
		st_to_c

noinst_PROGRAMS =	print_st
#		lecld

LNorCP=ln -s

LECL =	$(top_srcdir)/lecl
SRC =	$(LECL)/src
INCL =	$(LECL)/incl
SXINCL =	$(top_srcdir)/incl
SXETCINCL =	$(top_srcdir)/etc/incl
SXETCSRC =	$(top_srcdir)/etc/src

EXTRA_DIST =	$(INCL)/lecl_ag.h	\
		$(INCL)/lecl_nn.h	\
		$(INCL)/lecl_node.h	\
		$(INCL)/lecl_pcn.h	\
		$(INCL)/lecl_td.h

AM_CPPFLAGS =	-I$(INCL) -I$(SXINCL) -I$(SXETCINCL)
CC_FLAGS = $(CC) $(AM_CPPFLAGS) -c


../../bin/lecl.out:	lecl
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

../../bin/lecld.out:	lecld
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

../../bin/pplecl.out:	pplecl
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


../../bin/st_to_c.out:	st_to_c
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?


lecl_SOURCES =	$(SRC)/gen_check_kw.c		\
		$(SRC)/get_a_part.c		\
		$(SRC)/get_col_nb.c		\
		$(SRC)/lecl_bsc.c		\
		$(SRC)/lecl_cg.c		\
		$(SRC)/lecl_css.c		\
		$(SRC)/lecl_diagnos.c		\
		$(SRC)/lecl_ag_free.c		\
		$(SRC)/lecl_fsa.c		\
		$(SRC)/lecl_gn.c		\
		$(SRC)/lecl_lo.c		\
		$(SRC)/lecl_main.c		\
		$(SRC)/lecl_pact.c		\
		$(SRC)/lecl_pcn.c		\
		$(SRC)/lecl_pd.c		\
		$(SRC)/lecl_pi.c		\
		$(SRC)/lecl_prs.c		\
		$(SRC)/lecl_sact.c		\
		$(SRC)/lecl_sature.c		\
		$(SRC)/lecl_smp.c		\
		$(SRC)/lecl_st.c		\
		$(SRC)/lecl_stc.c		\
		$(SRC)/lecl_t.c			\
		$(SRC)/lecl_tout.c		\
		$(SRC)/lecl_tts.c
lecl_LDADD =	leclwrite.o			\
		leclread.o			\
		leclfree.o			\
		$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

#lecld_SOURCES =	$(SRC)/lecld_main.c

pplecl_SOURCES =	$(SRC)/pplecl_t.c		\
			$(SRC)/pplecl_main.c		\
			$(SRC)/lecl_pact.c		\
			$(SRC)/pplecl_sact.c
pplecl_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

print_st_SOURCES =	$(SRC)/print_st.c
print_st_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

st_to_c_SOURCES =	$(SRC)/st_to_c.c		\
			$(SXETCSRC)/leclread.c		\
			$(SXETCSRC)/leclfree.c
st_to_c_LDADD =	$(top_builddir)/etc/lib/libsxetc.a $(top_builddir)/lib/libsx.a

##PATHS=		$(SRC)/paths.c

##TOM=		$(SRC)/to_michel.c

get_a_part.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/get_a_part.c
	$(CC_FLAGS) $(SRC)/get_a_part.c

get_col_nb.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/get_col_nb.c
	$(CC_FLAGS) $(SRC)/get_col_nb.c

lecl_t.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/lecl_t.c
	$(CC_FLAGS) $(SRC)/lecl_t.c			\
		-Dsxtables=lecl_tables		\
		-DSCANACT=lecl_scan_act		\
		-DPARSACT=lecl_pars_act		\
		-Dsempass=lecl_smp

lecl_bsc.o:	$(SXINCL)/sxunix.h		\
		$(SRC)/lecl_bsc.c
	$(CC_FLAGS) $(SRC)/lecl_bsc.c

lecl_cg.o:	$(SXINCL)/sxunix.h		\
		$(SXINCL)/sxsstmt.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_cg.c
	$(CC_FLAGS) $(SRC)/lecl_cg.c

lecl_css.o:	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_nn.h		\
		$(INCL)/lecl_node.h		\
		$(SRC)/lecl_css.c
	$(CC_FLAGS) $(SRC)/lecl_css.c

lecl_diagnos.o: $(SXINCL)/sxunix.h		\
		$(SXINCL)/varstr.h		\
		$(SXINCL)/put_edit.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_diagnos.c
	$(CC_FLAGS) $(SRC)/lecl_diagnos.c

lecl_ag_free.o:	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_ag_free.c
	$(CC_FLAGS) $(SRC)/lecl_ag_free.c

lecl_fsa.o:	$(SXINCL)/sxunix.h		\
		$(SXINCL)/sxba.h		\
		$(SXINCL)/varstr.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_fsa.c
	$(CC_FLAGS) $(SRC)/lecl_fsa.c

gen_check_kw.o:	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/gen_check_kw.c
	$(CC_FLAGS) $(SRC)/gen_check_kw.c

lecl_gn.o:	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_gn.c
	$(CC_FLAGS) $(SRC)/lecl_gn.c

lecl_lo.o:	$(SXINCL)/sxunix.h		\
		$(SXINCL)/sxsstmt.h		\
		$(SXINCL)/put_edit.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_lo.c
	$(CC_FLAGS) $(SRC)/lecl_lo.c

lecl_main.o:	$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/release.h		\
		$(SXINCL)/varstr.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_main.c
	$(CC_FLAGS) $(SRC)/lecl_main.c

lecl_pcn.o:	$(SXINCL)/sxunix.h		\
		$(SRC)/lecl_pcn.c
	$(CC_FLAGS) $(SRC)/lecl_pcn.c

lecl_pact.o:	$(SXINCL)/sxunix.h		\
		$(SRC)/lecl_pact.c
	$(CC_FLAGS) $(SRC)/lecl_pact.c

lecl_pd.o:	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_nn.h		\
		$(INCL)/lecl_node.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_pd.c
	$(CC_FLAGS) $(SRC)/lecl_pd.c

lecl_pi.o:	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_nn.h		\
		$(INCL)/lecl_node.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_pi.c
	$(CC_FLAGS) $(SRC)/lecl_pi.c

lecl_prs.o:	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_nn.h		\
		$(INCL)/lecl_node.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_prs.c
	$(CC_FLAGS) $(SRC)/lecl_prs.c

lecl_sact.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/lecl_sact.c
	$(CC_FLAGS) $(SRC)/lecl_sact.c

lecl_sature.o: 	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_sature.c
	$(CC_FLAGS) $(SRC)/lecl_sature.c

lecl_smp.o:	$(SXINCL)/sxunix.h		\
		$(SXINCL)/varstr.h		\
		$(INCL)/lecl_ag.h		\
		$(INCL)/lecl_node.h		\
		$(INCL)/lecl_pcn.h		\
		$(SXETCINCL)/B_tables.h		\
		$(SXETCINCL)/S_tables.h		\
		$(SRC)/lecl_smp.c
	$(CC_FLAGS) $(SRC)/lecl_smp.c

lecl_st.o:	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_st.c
	$(CC_FLAGS) $(SRC)/lecl_st.c

lecl_stc.o:	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_nn.h		\
		$(INCL)/lecl_node.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_stc.c
	$(CC_FLAGS) $(SRC)/lecl_stc.c

lecl_tout.o: 	$(SXINCL)/sxunix.h		\
		$(INCL)/lecl_ag.h		\
		$(SXETCINCL)/S_tables.h	\
		$(SRC)/lecl_tout.c
	$(CC_FLAGS) $(SRC)/lecl_tout.c

lecl_tts.o:	$(SXINCL)/sxunix.h	        \
		$(SXINCL)/varstr.h		\
		$(INCL)/lecl_nn.h		\
		$(INCL)/lecl_node.h		\
		$(INCL)/lecl_ag.h		\
		$(SRC)/lecl_tts.c
	$(CC_FLAGS) $(SRC)/lecl_tts.c





lecld_main.o:	$(SXINCL)/sxdynam_scanner.h	\
		$(SXETCINCL)/B_tables.h		\
		$(SRC)/lecld_main.c
	$(CC_FLAGS) $(SRC)/lecld_main.c





pplecl_main.o:	$(SXINCL)/sxunix.h		\
		$(SRC)/pplecl_main.c
	$(CC_FLAGS) $(SRC)/pplecl_main.c


pplecl_t.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/pplecl_t.c
	$(CC_FLAGS) $(SRC)/pplecl_t.c			\
		-Dsxtables=pp_lecl_tables	\
		-DPARSACT=lecl_pars_act		\
		-DSCANACT=pp_lecl_scan_act

pplecl_sact.o: 	$(SXINCL)/sxunix.h		\
		$(SRC)/pplecl_sact.c
	$(CC_FLAGS) $(SRC)/pplecl_sact.c






print_st.o:	$(SXETCINCL)/S_tables.h		\
		$(SXINCL)/sxsstmt.h		\
		$(SXINCL)/sxunix.h		\
		$(SRC)/print_st.c
	$(CC_FLAGS) $(SRC)/print_st.c






st_to_c.o:	$(SXINCL)/sxunix.h		\
		$(SXINCL)/sxsstmt.h		\
		$(SXINCL)/put_edit.h		\
		$(SXINCL)/varstr.h		\
		$(SXETCINCL)/S_tables.h		\
		$(SRC)/st_to_c.c
	$(CC_FLAGS) $(SRC)/st_to_c.c


leclread.o: 	$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/S_tables.h		\
		$(SXETCSRC)/leclread.c
	$(CC_FLAGS) $(SXETCSRC)/leclread.c

leclwrite.o: 	$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/S_tables.h		\
		$(SXETCSRC)/leclwrite.c
	$(CC_FLAGS) $(SXETCSRC)/leclwrite.c

leclfree.o: 	$(SXINCL)/sxunix.h		\
		$(SXETCINCL)/S_tables.h		\
		$(SXETCSRC)/leclfree.c
	$(CC_FLAGS) $(SXETCSRC)/leclfree.c
