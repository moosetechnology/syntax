# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
ETCINCL=$(ETC)/incl

TEST=$(SX)/LIG_LC
SRC=$(TEST)/src
LIB=$(TEST)/lib

BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a

#CFLAGS= -O2
#CFLAGS= -g -DEBUG_ALLOC
CFLAGS= -g -DEBUG_ALLOC -DEBUG
#CFLAGS= -g -DEBUG_ALLOC -p

DEF1= -Dis_parser=1 -Dis_rcvr=1 -Ddelta_n=10 -Dis_1LA=1 -DXpack=1
DEF2= -Ddelta_n=0 -Dis_1LA=1
DEF3= -Dsxtables=earley_tables -Dsxndparser=sxearley_parser -DSEMACT=SXLIG_actions		\
      -Drecovery=sxp_rcvr_earley_lc -Dsxndprecovery=sxp_rcvr_earley_lc -DESAMBIG=sxvoid -DPARSACT_2=sxvoid
DEF4= -Dsxtables=earley_tables -Dsxndparser=sxearley_parser -DSEMACT=SXLIG_actions		\
      -Drecovery=sxbvoid -Dsxndprecovery=sxbvoid -DESAMBIG=sxvoid -DPARSACT_2=sxvoid
DEF5= -Ddelta_n=0 -Dis_parser=1 -Dis_1LA=1

CC=cd $(TEST); umask 7; exec cc -I$(SXINCL) -I$(ETCINCL) $(CFLAGS) -c

$(TEST)/everything .PRECIOUS:	$(TEST)/wcw.out		\
				$(TEST)/wcw_lr.out	\
				$(TEST)/nnf.out		\
				$(TEST)/nnf_lr.out	\
				$(TEST)/cyc.out		\
				$(TEST)/cyc_lr.out	\
				$(TEST)/cyc2.out	\
				$(TEST)/cyc2_lr.out	\
				$(TEST)/anbmcndm.out	\
				$(TEST)/anbmcndm_lr.out	\
				$(TEST)/anbncn.out


WCW=$(LIB)/wcw_pt.o		\
	$(LIB)/wcw_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/wcw_ligt.o

$(TEST)/wcw.out:	$(WCW)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(WCW) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/sxearley_main.o: $(SXINCL)/sxunix.h		\
			$(SXINCL)/semact.h		\
			$(SX)/csynt_lc/test/sxearley_main.c
	$(CC) $(SX)/csynt_lc/test/sxearley_main.c -o $(LIB)/sxearley_main.o

$(LIB)/wcw_pt.o:	$(SRC)/wcw_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h	\
		$(SXINCL)/semact.h		\
		$(SXINCL)/sxparser_earley_lc.h
	$(CC) 	$(SRC)/wcw_pt.c	-o $(LIB)/wcw_pt.o $(DEF1)

$(LIB)/wcw_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/wcw_t.c
	$(CC) $(SRC)/wcw_t.c -o $(LIB)/wcw_t.o $(DEF3)

$(LIB)/wcw_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/wcw_ligt.c
	$(CC) $(SRC)/wcw_ligt.c -o $(LIB)/wcw_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid


WCW_LR=$(LIB)/wcw_lr_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/wcw_lr_ligt.o

$(TEST)/wcw_lr.out:	$(WCW_LR)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(WCW_LR) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/wcw_lr_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/wcw_lr_t.c
	$(CC) $(SRC)/wcw_lr_t.c -o $(LIB)/wcw_lr_t.o -Dsxtables=earley_tables -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/wcw_lr_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/wcw_lr_ligt.c
	$(CC) $(SRC)/wcw_lr_ligt.c -o $(LIB)/wcw_lr_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid -Dis_sxndparser


NNF=$(LIB)/nnf_pt.o		\
	$(LIB)/nnf_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/nnf_ligt.o

$(TEST)/nnf.out:	$(NNF)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NNF) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/nnf_pt.o:	$(SRC)/nnf_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h	\
		$(SXINCL)/semact.h		\
		$(SXINCL)/sxparser_earley_lc.h
	$(CC) 	$(SRC)/nnf_pt.c	-o $(LIB)/nnf_pt.o $(DEF1)

$(LIB)/nnf_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/nnf_t.c
	$(CC) $(SRC)/nnf_t.c -o $(LIB)/nnf_t.o $(DEF3)

$(LIB)/nnf_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/nnf_ligt.c
	$(CC) $(SRC)/nnf_ligt.c -o $(LIB)/nnf_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid


NNF_LR=$(LIB)/nnf_lr_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/nnf_lr_ligt.o

$(TEST)/nnf_lr.out:	$(NNF_LR)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NNF_LR) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/nnf_lr_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/nnf_lr_t.c
	$(CC) $(SRC)/nnf_lr_t.c -o $(LIB)/nnf_lr_t.o -Dsxtables=earley_tables -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/nnf_lr_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/nnf_lr_ligt.c
	$(CC) $(SRC)/nnf_lr_ligt.c -o $(LIB)/nnf_lr_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid -Dis_sxndparser

CYC=$(LIB)/cyc_pt.o		\
	$(LIB)/cyc_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/cyc_ligt.o

$(TEST)/cyc.out:	$(CYC)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(CYC) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/cyc_pt.o:	$(SRC)/cyc_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h	\
		$(SXINCL)/semact.h		\
		$(SXINCL)/sxparser_earley_lc.h
	$(CC) 	$(SRC)/cyc_pt.c	-o $(LIB)/cyc_pt.o $(DEF1)

$(LIB)/cyc_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/cyc_t.c
	$(CC) $(SRC)/cyc_t.c -o $(LIB)/cyc_t.o $(DEF3)

$(LIB)/cyc_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/cyc_ligt.c
	$(CC) $(SRC)/cyc_ligt.c -o $(LIB)/cyc_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=4 -Dfeatures_action=sxvoid

CYC_LR=$(LIB)/cyc_lr_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/cyc_lr_ligt.o

$(TEST)/cyc_lr.out:	$(CYC_LR)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(CYC_LR) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/cyc_lr_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/cyc_lr_t.c
	$(CC) $(SRC)/cyc_lr_t.c -o $(LIB)/cyc_lr_t.o -Dsxtables=earley_tables -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/cyc_lr_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/cyc_lr_ligt.c
	$(CC) $(SRC)/cyc_lr_ligt.c -o $(LIB)/cyc_lr_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=4 -Dfeatures_action=sxvoid -Dis_sxndparser

CYC2=$(LIB)/cyc2_pt.o		\
	$(LIB)/cyc2_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/cyc2_ligt.o

$(TEST)/cyc2.out:	$(CYC2)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(CYC2) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/cyc2_pt.o:	$(SRC)/cyc2_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h	\
		$(SXINCL)/semact.h		\
		$(SXINCL)/sxparser_earley_lc.h
	$(CC) 	$(SRC)/cyc2_pt.c	-o $(LIB)/cyc2_pt.o $(DEF1)

$(LIB)/cyc2_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/cyc2_t.c
	$(CC) $(SRC)/cyc2_t.c -o $(LIB)/cyc2_t.o $(DEF3)

$(LIB)/cyc2_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/cyc2_ligt.c
	$(CC) $(SRC)/cyc2_ligt.c -o $(LIB)/cyc2_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=4 -Dfeatures_action=sxvoid

CYC2_LR=$(LIB)/cyc2_lr_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/cyc2_lr_ligt.o

$(TEST)/cyc2_lr.out:	$(CYC2_LR)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(CYC2_LR) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/cyc2_lr_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/cyc2_lr_t.c
	$(CC) $(SRC)/cyc2_lr_t.c -o $(LIB)/cyc2_lr_t.o -Dsxtables=earley_tables -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/cyc2_lr_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/cyc2_lr_ligt.c
	$(CC) $(SRC)/cyc2_lr_ligt.c -o $(LIB)/cyc2_lr_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=4 -Dfeatures_action=sxvoid -Dis_sxndparser

ANBM=$(LIB)/anbmcndm_pt.o		\
	$(LIB)/anbmcndm_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/anbmcndm_ligt.o

$(TEST)/anbmcndm.out:	$(ANBM)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(ANBM) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/anbmcndm_pt.o:	$(SRC)/anbmcndm_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h	\
		$(SXINCL)/semact.h		\
		$(SXINCL)/sxparser_earley_lc.h
	$(CC) 	$(SRC)/anbmcndm_pt.c	-o $(LIB)/anbmcndm_pt.o $(DEF1)

$(LIB)/anbmcndm_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/anbmcndm_t.c
	$(CC) $(SRC)/anbmcndm_t.c -o $(LIB)/anbmcndm_t.o $(DEF3)

$(LIB)/anbmcndm_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/anbmcndm_ligt.c
	$(CC) $(SRC)/anbmcndm_ligt.c -o $(LIB)/anbmcndm_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid

ANBM_LR=$(LIB)/anbmcndm_lr_t.o	\
	$(LIB)/sxearley_main.o	\
	$(LIB)/anbmcndm_lr_ligt.o

$(TEST)/anbmcndm_lr.out:	$(ANBM_LR)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(ANBM_LR) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/anbmcndm_lr_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/anbmcndm_lr_t.c
	$(CC) $(SRC)/anbmcndm_lr_t.c -o $(LIB)/anbmcndm_lr_t.o -Dsxtables=earley_tables -DESAMBIG=sxvoid -DPARSACT=sxvoid -DSEMACT=SXLIG_actions

$(LIB)/anbmcndm_lr_ligt.o: 	$(SXINCL)/sxunix.h	\
			$(SXINCL)/sxnd.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/anbmcndm_lr_ligt.c
	$(CC) $(SRC)/anbmcndm_lr_ligt.c -o $(LIB)/anbmcndm_lr_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid -Dis_sxndparser

ANBNCN=$(LIB)/anbncn_pt.o		\
	$(LIB)/anbncn_t.o		\
	$(LIB)/sxearley_main.o	\
	$(LIB)/anbncn_ligt.o

$(TEST)/anbncn.out:	$(ANBNCN)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(ANBNCN) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/anbncn_pt.o:	$(SRC)/anbncn_pt.c	\
		$(SXINCL)/sxp_rcvr_earley_lc.h	\
		$(SXINCL)/semact.h		\
		$(SXINCL)/sxparser_earley_lc.h
	$(CC) 	$(SRC)/anbncn_pt.c	-o $(LIB)/anbncn_pt.o $(DEF1)

$(LIB)/anbncn_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/anbncn_t.c
	$(CC) $(SRC)/anbncn_t.c -o $(LIB)/anbncn_t.o $(DEF3)

$(LIB)/anbncn_ligt.o: 	$(SXINCL)/sxunix.h		\
			$(SXINCL)/semact.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/SXLIGparsact.h	\
			$(SRC)/anbncn_ligt.c
	$(CC) $(SRC)/anbncn_ligt.c -o $(LIB)/anbncn_ligt.o -DSXLIGuse_reduced=0 -DRDGmax_cycles=0 -Dfeatures_action=sxvoid


