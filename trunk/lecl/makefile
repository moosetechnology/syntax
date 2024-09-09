
include $(SX)/etc/make/gccoptions

LECL=$(SX)/lecl
LIB=$(LECL)/lib.$(ARCH)
BOOT=$(LECL)/boot

CC=$(COMPILER) -I$(LECL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXBIN)/lecl.out				\
     $(SXAUX)/print_st.out

NEEDS=		$(LIB)/gen_check_kw.o		\
		$(LIB)/get_a_part.o		\
		$(LIB)/get_col_nb.o		\
		$(LIB)/lecl_bsc.o		\
		$(LIB)/lecl_cg.o		\
		$(LIB)/lecl_css.o		\
		$(LIB)/lecl_diagnos.o		\
		$(LIB)/lecl_ag_free.o		\
		$(LIB)/lecl_fsa.o		\
		$(LIB)/lecl_gn.o		\
		$(LIB)/lecl_lo.o		\
		$(LIB)/lecl_main.o		\
		$(LIB)/lecl_pact.o		\
		$(LIB)/lecl_pcn.o		\
		$(LIB)/lecl_pd.o		\
		$(LIB)/lecl_pi.o		\
		$(LIB)/lecl_prs.o		\
		$(LIB)/lecl_sact.o		\
		$(LIB)/lecl_sature.o		\
		$(LIB)/lecl_smp.o		\
		$(LIB)/lecl_st.o		\
		$(LIB)/lecl_stc.o		\
		$(LIB)/lecl_t.o			\
		$(LIB)/lecl_tout.o		\
		$(LIB)/lecl_tts.o		\
		$(LIB)/leclwrite.o		\
		$(LIB)/leclfree.o

PRINT=		$(LIB)/print_st.o

$(SXBIN)/lecl.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/get_a_part.o: 	$(SXINCL)/sxunix.h		\
			get_a_part.c
	$(CC) -o $@ -c get_a_part.c

$(LIB)/get_col_nb.o: 	$(SXINCL)/sxunix.h		\
			get_col_nb.c
	$(CC) -o $@ -c get_col_nb.c

$(LIB)/lecl_t.o: 	$(SXINCL)/sxunix.h		\
			$(BOOT)/lecl_t.c
	$(CC) -o $@ -c $(BOOT)/lecl_t.c			\
			-DSEMPASS=lecl_smp

$(LIB)/lecl_bsc.o:	$(SXINCL)/sxunix.h		\
			lecl_bsc.c
	$(CC) -o $@ -c lecl_bsc.c

$(LIB)/lecl_cg.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxsstmt.h		\
			lecl_ag.h		\
			lecl_cg.c
	$(CC) -o $@ -c lecl_cg.c

$(LIB)/lecl_css.o:	$(SXINCL)/sxunix.h		\
			lecl_nn.h		\
			lecl_node.h		\
			lecl_css.c
	$(CC) -o $@ -c lecl_css.c

$(LIB)/lecl_diagnos.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/put_edit.h		\
			lecl_ag.h		\
			lecl_diagnos.c
	$(CC) -o $@ -c lecl_diagnos.c

$(LIB)/lecl_ag_free.o:	$(SXINCL)/sxunix.h		\
			lecl_ag.h		\
			lecl_ag_free.c
	$(CC) -o $@ -c lecl_ag_free.c

$(LIB)/lecl_fsa.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/varstr.h		\
			lecl_ag.h		\
			lecl_fsa.c
	$(CC) -o $@ -c lecl_fsa.c

$(LIB)/gen_check_kw.o:	$(SXINCL)/sxunix.h		\
			lecl_ag.h		\
			gen_check_kw.c
	$(CC) -o $@ -c gen_check_kw.c

$(LIB)/lecl_gn.o:	$(SXINCL)/sxunix.h		\
			lecl_ag.h		\
			lecl_gn.c
	$(CC) -o $@ -c lecl_gn.c

$(LIB)/lecl_lo.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxsstmt.h		\
			$(SXINCL)/put_edit.h		\
			lecl_ag.h		\
			lecl_lo.c
	$(CC) -o $@ -c lecl_lo.c

$(LIB)/lecl_main.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/varstr.h		\
			lecl_ag.h		\
			lecl_main.c
	$(CC) -o $@ -c lecl_main.c

$(LIB)/lecl_pcn.o:	$(SXINCL)/sxunix.h		\
			lecl_pcn.c
	$(CC) -o $@ -c lecl_pcn.c

$(LIB)/lecl_pact.o:	$(SXINCL)/sxunix.h		\
			lecl_pact.c
	$(CC) -o $@ -c -I$(BOOT) lecl_pact.c

$(LIB)/lecl_pd.o:	$(SXINCL)/sxunix.h		\
			lecl_nn.h		\
			lecl_node.h		\
			lecl_ag.h		\
			lecl_pd.c
	$(CC) -o $@ -c lecl_pd.c

$(LIB)/lecl_pi.o:	$(SXINCL)/sxunix.h		\
			lecl_nn.h		\
			lecl_node.h		\
			lecl_ag.h		\
			lecl_pi.c
	$(CC) -o $@ -c lecl_pi.c

$(LIB)/lecl_prs.o:	$(SXINCL)/sxunix.h		\
			lecl_nn.h		\
			lecl_node.h		\
			lecl_ag.h		\
			lecl_prs.c
	$(CC) -o $@ -c lecl_prs.c

$(LIB)/lecl_sact.o: 	$(SXINCL)/sxunix.h		\
			lecl_sact.c
	$(CC) -o $@ -c lecl_sact.c

$(LIB)/lecl_sature.o: 	$(SXINCL)/sxunix.h		\
			lecl_ag.h		\
			lecl_sature.c
	$(CC) -o $@ -c lecl_sature.c

$(LIB)/lecl_smp.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/varstr.h		\
			lecl_ag.h		\
			lecl_node.h		\
			lecl_pcn.h		\
			$(SXINCL)/util/B_tables.h	\
			$(SXINCL)/util/S_tables.h	\
			lecl_smp.c
	$(CC) -o $@ -c lecl_smp.c

$(LIB)/lecl_st.o:	$(SXINCL)/sxunix.h		\
			lecl_ag.h		\
			lecl_st.c
	$(CC) -o $@ -c lecl_st.c

$(LIB)/lecl_stc.o:	$(SXINCL)/sxunix.h		\
			lecl_nn.h		\
			lecl_node.h		\
			lecl_ag.h		\
			lecl_stc.c
	$(CC) -o $@ -c lecl_stc.c

$(LIB)/lecl_tout.o: 	$(SXINCL)/sxunix.h		\
			lecl_ag.h		\
			$(SXINCL)/util/S_tables.h	\
			lecl_tout.c
	$(CC) -o $@ -c lecl_tout.c

$(LIB)/lecl_tts.o:	$(SXINCL)/sxunix.h	        \
			$(SXINCL)/varstr.h		\
			lecl_nn.h		\
			lecl_node.h		\
			lecl_ag.h		\
			lecl_tts.c
	$(CC) -o $@ -c lecl_tts.c


$(SXAUX)/print_st.out:	$(PRINT)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(PRINT) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/print_st.o:	$(SXINCL)/util/S_tables.h	\
			$(SXINCL)/sxsstmt.h		\
			$(SXINCL)/sxunix.h		\
			print_st.c
	$(CC) -o $@ -c print_st.c

$(LIB)/leclwrite.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/S_tables.h	\
			$(SXUTIL)/leclwrite.c
	$(CC) -o $@ -c $(SXUTIL)/leclwrite.c

$(LIB)/leclfree.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/S_tables.h	\
			$(SXUTIL)/leclfree.c
	$(CC) -o $@ -c $(SXUTIL)/leclfree.c

# debut de code produit par "make-makefile lecl"

bootstrap: lecl_t.c lecl.c.skeleton  lecl_td.h

lecl_t.c: lecl.bt lecl.st lecl.pt lecl.rt lecl.att
	tables_c lecl > $@

lecl.bt lecl.att lecl.c.skeleton: lecl.at
	semat lecl.at  > lecl.c.skeleton

lecl.st: lecl.bt lecl.lecl
	lecl lecl.lecl || [ $$? = 1 ]

lecl.pt: lecl.bt
	csynt lecl || [ $$? = 1 ]

lecl.rt: lecl.bt lecl.st lecl.recor
	recor lecl.recor

lecl_td.h: lecl.bt lecl.tdef
	tdef lecl.tdef > $@

# fin de code produit par "make-makefile lecl"

clean:
	/bin/rm -f $(LIB)/*.o $(SXAUX)/print_st.out
	/bin/rm -f lecl.att lecl.bt lecl.pt lecl.rt lecl.st
	/bin/rm -f lecl.bn.l lecl.lc.l lecl.rc.l lecl.td.l
	/bin/rm -f lecl.c.skeleton lecl_t.c lecl_td.h

