
include $(SX)/etc/make/gccoptions

TEST=$(SX)/../outdated/oldies/LIG_LC
LIB=$(TEST)/lib.$(ARCH)

#CFLAGS= -O2
#CFLAGS= -g -DEBUG_ALLOC
#CFLAGS= -g -DEBUG_ALLOC -p
CFLAGS= -g -DEBUG_ALLOC -DEBUG

DEF1= -Dis_parser=1 -Dis_rcvr=1 -Ddelta_n=10 -Dis_1LA=1 -DXpack=1
DEF2= -Ddelta_n=0 -Dis_1LA=1
DEF3= -Dsxndparser=sxearley_parser -DSEMACT=SXLIG_actions		\
      -Drecovery=sxp_rcvr_earley_lc -Dsxndprecovery=sxp_rcvr_earley_lc -DESAMBIG=sxvoid -DPARSACT_2=sxvoid
DEF4= -Dsxndparser=sxearley_parser -DSEMACT=SXLIG_actions		\
      -Drecovery=sxbvoid -Dsxndprecovery=sxbvoid -DESAMBIG=sxvoid -DPARSACT_2=sxvoid
DEF5= -Ddelta_n=0 -Dis_parser=1 -Dis_1LA=1

CC=cc -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -DEBUG

ALL=$(TEST)/wcw.out		\
     $(TEST)/wcw_lr.out		\
     $(TEST)/nnf.out		\
     $(TEST)/nnf_lr.out		\
     $(TEST)/cyc.out		\
     $(TEST)/cyc_lr.out		\
     $(TEST)/cyc2.out		\
     $(TEST)/cyc2_lr.out	\
     $(TEST)/anbmcndm.out	\
     $(TEST)/anbmcndm_lr.out	\
     $(TEST)/anbncn.out

all: $(ALL)

WCW=$(LIB)/wcw_pt.o		\
	$(LIB)/wcw_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/wcw_ligt.o

$(TEST)/wcw.out:	$(WCW)	$(SXBIBS)
	cc -o $@ $(WCW) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/sxearley_main.o: $(SXINCL)/sxunix.h		\
			$(SX)/csynt_lc/test/sxearley_main.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c $(SX)/csynt_lc/test/sxearley_main.c

$(LIB)/wcw_pt.o:	wcw_pt.c	\
		$(SXEXTINCL)/sxp_rcvr_earley_lc.h \
		sxparser_earley_lc.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c wcw_pt.c $(DEF1)

$(LIB)/wcw_t.o: 	$(SXINCL)/sxunix.h		\
			wcw_t.c
	$(CC) -o $@ -c wcw_t.c $(DEF3)

$(LIB)/wcw_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			wcw_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c wcw_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid



WCW_LR=$(LIB)/wcw_lr_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/wcw_lr_ligt.o

$(TEST)/wcw_lr.out:	$(WCW_LR)	$(SXBIBS)
	cc -o $@ $(WCW_LR) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/wcw_lr_t.o: 	$(SXINCL)/sxunix.h		\
			wcw_lr_t.c
	$(CC) -o $@ -c wcw_lr_t.c -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/wcw_lr_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			wcw_lr_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c wcw_lr_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid -Dis_sxndparser


NNF=$(LIB)/nnf_pt.o		\
	$(LIB)/nnf_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/nnf_ligt.o

$(TEST)/nnf.out:	$(NNF)	$(SXBIBS)
	cc -o $@ $(NNF) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/nnf_pt.o:	nnf_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h \
		sxparser_earley_lc.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c nnf_pt.c $(DEF1)

$(LIB)/nnf_t.o: 	$(SXINCL)/sxunix.h		\
			nnf_t.c
	$(CC) -o $@ -c nnf_t.c $(DEF3)

$(LIB)/nnf_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			nnf_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c nnf_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid


NNF_LR=$(LIB)/nnf_lr_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/nnf_lr_ligt.o

$(TEST)/nnf_lr.out:	$(NNF_LR)	$(SXBIBS)
	cc -o $@ $(NNF_LR) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/nnf_lr_t.o: 	$(SXINCL)/sxunix.h		\
			nnf_lr_t.c
	$(CC) -o $@ -c nnf_lr_t.c -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/nnf_lr_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			nnf_lr_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c nnf_lr_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid -Dis_sxndparser

CYC=$(LIB)/cyc_pt.o		\
	$(LIB)/cyc_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/cyc_ligt.o

$(TEST)/cyc.out:	$(CYC)	$(SXBIBS)
	cc -o $@ $(CYC) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/cyc_pt.o:	cyc_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h \
		sxparser_earley_lc.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c cyc_pt.c $(DEF1)

$(LIB)/cyc_t.o: 	$(SXINCL)/sxunix.h		\
			cyc_t.c
	$(CC) -o $@ -c cyc_t.c $(DEF3)

$(LIB)/cyc_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			cyc_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c cyc_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=4 -Dfeatures_action=sxvoid

CYC_LR=$(LIB)/cyc_lr_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/cyc_lr_ligt.o

$(TEST)/cyc_lr.out:	$(CYC_LR)	$(SXBIBS)
	cc -o $@ $(CYC_LR) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/cyc_lr_t.o: 	$(SXINCL)/sxunix.h		\
			cyc_lr_t.c
	$(CC) -o $@ -c cyc_lr_t.c -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/cyc_lr_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			cyc_lr_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c cyc_lr_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=4 -Dfeatures_action=sxvoid -Dis_sxndparser

CYC2=$(LIB)/cyc2_pt.o		\
	$(LIB)/cyc2_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/cyc2_ligt.o

$(TEST)/cyc2.out:	$(CYC2)	$(SXBIBS)
	cc -o $@ $(CYC2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/cyc2_pt.o:	cyc2_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h \
		sxparser_earley_lc.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c cyc2_pt.c	$(DEF1)

$(LIB)/cyc2_t.o: 	$(SXINCL)/sxunix.h		\
			cyc2_t.c
	$(CC) -o $@ -c cyc2_t.c $(DEF3)

$(LIB)/cyc2_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			cyc2_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c cyc2_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=4 -Dfeatures_action=sxvoid

CYC2_LR=$(LIB)/cyc2_lr_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/cyc2_lr_ligt.o

$(TEST)/cyc2_lr.out:	$(CYC2_LR)	$(SXBIBS)
	cc -o $@ $(CYC2_LR) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/cyc2_lr_t.o: 	$(SXINCL)/sxunix.h		\
			cyc2_lr_t.c
	$(CC) -o $@ -c cyc2_lr_t.c -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/cyc2_lr_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			cyc2_lr_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c cyc2_lr_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=4 -Dfeatures_action=sxvoid -Dis_sxndparser

ANBM=$(LIB)/anbmcndm_pt.o	\
	$(LIB)/anbmcndm_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/anbmcndm_ligt.o

$(TEST)/anbmcndm.out:	$(ANBM)	$(SXBIBS)
	cc -o $@ $(ANBM) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/anbmcndm_pt.o:	anbmcndm_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h \
		sxparser_earley_lc.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c anbmcndm_pt.c $(DEF1)

$(LIB)/anbmcndm_t.o: 	$(SXINCL)/sxunix.h		\
			anbmcndm_t.c
	$(CC) -o $@ -c anbmcndm_t.c $(DEF3)

$(LIB)/anbmcndm_ligt.o: $(SXINCL)/sxunix.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			anbmcndm_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c anbmcndm_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid

ANBM_LR=$(LIB)/anbmcndm_lr_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/anbmcndm_lr_ligt.o

$(TEST)/anbmcndm_lr.out:	$(ANBM_LR)	$(SXBIBS)
	cc -o $@ $(ANBM_LR) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/anbmcndm_lr_t.o: 	$(SXINCL)/sxunix.h	\
			anbmcndm_lr_t.c
	$(CC) -o $@ -c anbmcndm_lr_t.c -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/anbmcndm_lr_ligt.o: 	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			anbmcndm_lr_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c anbmcndm_lr_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid -Dis_sxndparser

ANBNCN=$(LIB)/anbncn_pt.o	\
	$(LIB)/anbncn_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/anbncn_ligt.o

$(TEST)/anbncn.out:	$(ANBNCN)	$(SXBIBS)
	cc -o $@ $(ANBNCN) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/anbncn_pt.o:	anbncn_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h \
		sxparser_earley_lc.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c anbncn_pt.c $(DEF1)

$(LIB)/anbncn_t.o: 	$(SXINCL)/sxunix.h		\
			anbncn_t.c
	$(CC) -o $@ -c anbncn_t.c $(DEF3)

$(LIB)/anbncn_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			anbncn_ligt.c
#  $(SXINCL)/semact.h
	$(CC) -o $@ -c anbncn_ligt.c -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid

clean:
	rm -f $(ALL) $(WCW) $(WCW_LR) $(NNF) $(NNF_LR) $(CYC) $(CYC_LR)
	rm -f $(CYC2) $(CYC2_LR) $(ANBM) $(ANBM_LR) $(ANBNCN) 

