# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
SXETCINCL=$(SX)/etc/incl

TEST=$(SX)/rcg/test
SRC=$(TEST)/src
LIB=$(TEST)/lib

BIBS= $(SX)/etc/lib/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a

#CFLAGS= -O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#CFLAGS= -g -DEBUG2 -DEBUG -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=0 -DCYCLIC=0
#CFLAGS= -g -DEBUG2 -DEBUG -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1 -DCYCLIC=0
#CFLAGS= -g -DEBUG2 -DEBUG -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0
#CFLAGS= -g -DEBUG2 -DEBUG -DSINGLE_PASS=1 -DCYCLIC=0
#CFLAGS= -O2 -DEBUG2 -DSINGLE_PASS=1 -DCYCLIC=0
#CFLAGS= -DEBUG2 -DSINGLE_PASS=1 -DCYCLIC=0
#CFLAGS= -g -DEBUG -DEBUG2 -DLEX=1 -DFIRST_LAST=1 -DMEMO=1
#CFLAGS= -g -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=0 -p
CFLAGS= -g -DEBUG2
COMPILER=cc
CC=cd $(TEST); umask 7; exec $(COMPILER) -I$(SXINCL) -I$(SXETCINCL) $(CFLAGS) -c
cc=cd $(TEST); umask 7; exec $(COMPILER) -I$(SXINCL) -I$(SXETCINCL) -c

$(TEST)/everything .PRECIOUS:	$(LIB)/RCG_guided_main.o $(LIB)/RCG_main.o
#				$(TEST)/new_prime.out	\
#				$(TEST)/permute.out	\
#				$(TEST)/OLTAG.out	\
#				$(TEST)/fxtag.out	\
#				$(TEST)/FB.out		\
#				$(TEST)/test_idag.out	\
#				$(TEST)/gen_pluriel.out	\
#				$(TEST)/pluriel.out	\
#				$(TEST)/ww.out		\
#				$(TEST)/anbncn.out	\
#				$(TEST)/tag_eric.out	\
#				$(TEST)/count.out	\
#				$(TEST)/nil.out		\
#				$(TEST)/SCR.out		\
#				$(TEST)/RNA.out		\
#				$(TEST)/rnatag.out	\
#				$(TEST)/binary.out	\
#				$(TEST)/cfg.out		\
#				$(TEST)/anbm.out	\
#				$(TEST)/icg3.out	\
#				$(TEST)/icg2.out	\
#				$(TEST)/icg1.out	\
#				$(TEST)/rcvr.out	\
#				$(TEST)/distance.out	\
#				$(TEST)/gan.out		\
#				$(TEST)/w_pwr_w_pr.out	\
#				$(TEST)/prime6.out	\
#				$(TEST)/w_pwr_w.out	\
#				$(TEST)/arity0.out	\
#				$(TEST)/n2.out		\
#				$(TEST)/dtest4.out	\
#				$(TEST)/dtest3.out	\
#				$(TEST)/dtest2.out	\
#				$(TEST)/dtest.out	\
#				$(TEST)/meteo.out	\
#				$(TEST)/cyclic.out	\
#				$(TEST)/fas.out		\
#				$(TEST)/BetaSheets.out	\
#				$(TEST)/wcw.out		\
#				$(TEST)/ex1.out 	\
#				$(TEST)/ex2.out		\
#				$(TEST)/ex3.out		\
#				$(TEST)/an.out		\
#				$(TEST)/prime.out	\
#				$(TEST)/prime2.out	\
#				$(TEST)/prime3.out	\
#				$(TEST)/prime4.out	\
#				$(TEST)/prime5.out	\
#				$(TEST)/ex8.out		\


$(LIB)/RCG_guided_main.o: $(SXINCL)/sxunix.h		\
		$(SXETCINCL)/release.h	\
		$(SXINCL)/rcg_sglbl.h		\
		$(SRC)/RCG_guided_main.c
	$(CC) $(SRC)/RCG_guided_main.c -o $(LIB)/RCG_guided_main.o

$(LIB)/RCG_main.o: $(SXINCL)/sxunix.h		\
		$(SXETCINCL)/release.h	\
		$(SXINCL)/rcg_sglbl.h		\
		$(SRC)/RCG_main.c
	$(CC) $(SRC)/RCG_main.c -o $(LIB)/RCG_main.o

EX1=$(LIB)/RCG_main.o	\
	$(LIB)/ex1_t.o	\
	$(LIB)/ex1_rcgp.o\
	$(LIB)/ex1_parser.o

$(TEST)/ex1.out:	$(EX1)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(EX1) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/ex1_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/ex1_rcgp.c
	$(CC) $(SRC)/ex1_rcgp.c -o $(LIB)/ex1_rcgp.o

$(LIB)/ex1_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/ex1_parser.o\
		-DPID=1\
		-DINTERFACE="_ex1_if()"


$(LIB)/ex1_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/ex1_t.c
	$(CC) $(SRC)/ex1_t.c -o $(LIB)/ex1_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


EX2=$(LIB)/RCG_main.o	\
	$(LIB)/ex2_t.o	\
	$(LIB)/ex2_rcgp.o\
	$(LIB)/ex2_parser.o

$(TEST)/ex2.out:	$(EX2)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(EX2) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/ex2_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/ex2_rcgp.c
	$(CC) $(SRC)/ex2_rcgp.c -o $(LIB)/ex2_rcgp.o

$(LIB)/ex2_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/ex2_parser.o\
		-DPID=1\
		-DINTERFACE="_ex2_if()"


$(LIB)/ex2_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/ex2_t.c
	$(CC) $(SRC)/ex2_t.c -o $(LIB)/ex2_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



EX3=$(LIB)/RCG_main.o	\
	$(LIB)/ex3_t.o	\
	$(LIB)/ex3_rcgp.o\
	$(LIB)/ex3_parser.o

$(TEST)/ex3.out:	$(EX3)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(EX3) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/ex3_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/ex3_rcgp.c
	$(CC) $(SRC)/ex3_rcgp.c -o $(LIB)/ex3_rcgp.o

$(LIB)/ex3_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/ex3_parser.o\
		-DPID=1\
		-DINTERFACE="_ex3_if()"


$(LIB)/ex3_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/ex3_t.c
	$(CC) $(SRC)/ex3_t.c -o $(LIB)/ex3_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid




AN=$(LIB)/RCG_main.o	\
	$(LIB)/an_t.o	\
	$(LIB)/an_rcgp.o\
	$(LIB)/an_parser.o

$(TEST)/an.out:	$(AN)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(AN) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/an_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/an_rcgp.c
	$(CC) $(SRC)/an_rcgp.c -o $(LIB)/an_rcgp.o

$(LIB)/an_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/an_parser.o\
		-DPID=1\
		-DINTERFACE="_an_if()"


$(LIB)/an_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/an_t.c
	$(CC) $(SRC)/an_t.c -o $(LIB)/an_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


PRIME=$(LIB)/RCG_main.o	\
	$(LIB)/prime_t.o	\
	$(LIB)/prime_rcgp.o\
	$(LIB)/prime_parser.o

$(TEST)/prime.out:	$(PRIME)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(PRIME) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/prime_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/prime_rcgp.c
	$(CC) $(SRC)/prime_rcgp.c -o $(LIB)/prime_rcgp.o

$(LIB)/prime_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/prime_parser.o\
		-DPID=1\
		-DINTERFACE="_prime_if()"


$(LIB)/prime_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/prime_t.c
	$(CC) $(SRC)/prime_t.c -o $(LIB)/prime_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


PRIME2=$(LIB)/RCG_main.o	\
	$(LIB)/prime2_t.o	\
	$(LIB)/prime2_rcgp.o\
	$(LIB)/prime2_parser.o

$(TEST)/prime2.out:	$(PRIME2)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(PRIME2) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/prime2_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/prime2_rcgp.c
	$(CC) $(SRC)/prime2_rcgp.c -o $(LIB)/prime2_rcgp.o

$(LIB)/prime2_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/prime2_parser.o\
		-DPID=1\
		-DINTERFACE="_prime2_if()"


$(LIB)/prime2_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/prime2_t.c
	$(CC) $(SRC)/prime2_t.c -o $(LIB)/prime2_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


PRIME3=$(LIB)/RCG_main.o	\
	$(LIB)/prime3_t.o	\
	$(LIB)/prime3_rcgp.o\
	$(LIB)/prime3_parser.o

$(TEST)/prime3.out:	$(PRIME3)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(PRIME3) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/prime3_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/prime3_rcgp.c
	$(CC) $(SRC)/prime3_rcgp.c -o $(LIB)/prime3_rcgp.o

$(LIB)/prime3_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/prime3_parser.o\
		-DPID=1\
		-DINTERFACE="_prime3_if()"


$(LIB)/prime3_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/prime3_t.c
	$(CC) $(SRC)/prime3_t.c -o $(LIB)/prime3_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


PRIME4=$(LIB)/RCG_main.o	\
	$(LIB)/prime4_t.o	\
	$(LIB)/prime4_rcgp.o\
	$(LIB)/prime4_parser.o

$(TEST)/prime4.out:	$(PRIME4)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(PRIME4) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/prime4_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/prime4_rcgp.c
	$(CC) $(SRC)/prime4_rcgp.c -o $(LIB)/prime4_rcgp.o

$(LIB)/prime4_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/prime4_parser.o\
		-DPID=1\
		-DINTERFACE="_prime4_if()"


$(LIB)/prime4_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/prime4_t.c
	$(CC) $(SRC)/prime4_t.c -o $(LIB)/prime4_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


PRIME5=$(LIB)/RCG_main.o	\
	$(LIB)/prime5_t.o	\
	$(LIB)/prime5_rcgp.o\
	$(LIB)/prime5_parser.o

$(TEST)/prime5.out:	$(PRIME5)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(PRIME5) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/prime5_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/prime5_rcgp.c
	$(CC) $(SRC)/prime5_rcgp.c -o $(LIB)/prime5_rcgp.o

$(LIB)/prime5_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/prime5_parser.o\
		-DPID=1\
		-DINTERFACE="_prime5_if()"


$(LIB)/prime5_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/prime5_t.c
	$(CC) $(SRC)/prime5_t.c -o $(LIB)/prime5_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



PRIME6=$(LIB)/RCG_main.o	\
	$(LIB)/prime6_t.o	\
	$(LIB)/prime6_rcgp.o	\
	$(LIB)/prime6_parsact.o	\
	$(LIB)/prime6_parser.o

$(TEST)/prime6.out:	$(PRIME6)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(PRIME6) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/prime6_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/prime6_rcgp.c
	$(CC) $(SRC)/prime6_rcgp.c -o $(LIB)/prime6_rcgp.o

$(LIB)/prime6_parsact.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SRC)/prime6_parsact.c
	$(CC) $(SRC)/prime6_parsact.c -o $(LIB)/prime6_parsact.o

$(LIB)/prime6_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/prime6_parser.o\
		-DPID=2\
		-DINTERFACE="_prime6_if(), _prime6_parsact_if()"


$(LIB)/prime6_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/prime6_t.c
	$(CC) $(SRC)/prime6_t.c -o $(LIB)/prime6_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



EX8=$(LIB)/RCG_main.o	\
	$(LIB)/ex8_t.o	\
	$(LIB)/ex8_rcgp.o\
	$(LIB)/ex8_parser.o

$(TEST)/ex8.out:	$(EX8)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(EX8) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/ex8_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/ex8_rcgp.c
	$(CC) $(SRC)/ex8_rcgp.c -o $(LIB)/ex8_rcgp.o

$(LIB)/ex8_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/ex8_parser.o\
		-DPID=1\
		-DINTERFACE="_ex8_if()"


$(LIB)/ex8_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/ex8_t.c
	$(CC) $(SRC)/ex8_t.c -o $(LIB)/ex8_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


WCW=$(LIB)/RCG_main.o			\
	$(LIB)/wcw_t.o			\
	$(LIB)/ext_prime4_rcgp.o	\
	$(LIB)/ext_mul_rcgp.o		\
	$(LIB)/ext_slices_rcgp.o	\
	$(LIB)/wcw_rcgp.o		\
	$(LIB)/wcw_parser.o

$(TEST)/wcw.out:	$(WCW)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(WCW) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/ext_prime4_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/ext_prime4_rcgp.c
	$(CC) $(SRC)/ext_prime4_rcgp.c -o $(LIB)/ext_prime4_rcgp.o"


$(LIB)/ext_mul_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/ext_mul_rcgp.c
	$(CC) $(SRC)/ext_mul_rcgp.c -o $(LIB)/ext_mul_rcgp.o"


$(LIB)/ext_slices_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/ext_slices_rcgp.c
	$(CC) $(SRC)/ext_slices_rcgp.c -o $(LIB)/ext_slices_rcgp.o"


$(LIB)/wcw_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/wcw_rcgp.c
	$(CC) $(SRC)/wcw_rcgp.c -o $(LIB)/wcw_rcgp.o


$(LIB)/wcw_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/wcw_parser.o\
		-DPID=4\
		-DINTERFACE="_wcw_if(),_ext_prime4_if(),_ext_mul_if(),_ext_slices_if()"

$(LIB)/wcw_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/wcw_t.c
	$(CC) $(SRC)/wcw_t.c -o $(LIB)/wcw_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


BETA_SHEETS=$(LIB)/RCG_main.o	\
	$(LIB)/BetaSheets_t.o	\
	$(LIB)/BetaSheets_rcgp.o\
	$(LIB)/BetaSheets_parser.o

$(TEST)/BetaSheets.out:	$(BETA_SHEETS)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(BETA_SHEETS) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/BetaSheets_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/BetaSheets_rcgp.c
	$(CC) $(SRC)/BetaSheets_rcgp.c -o $(LIB)/BetaSheets_rcgp.o


$(LIB)/BetaSheets_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/BetaSheets_parser.o\
		-DPID=1\
		-DINTERFACE="_BetaSheets_if()"


$(LIB)/BetaSheets_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/BetaSheets_t.c
	$(CC) $(SRC)/BetaSheets_t.c -o $(LIB)/BetaSheets_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


FAS=$(LIB)/RCG_main.o	\
	$(LIB)/fas_t.o	\
	$(LIB)/fas_rcgp.o\
	$(LIB)/fas_parser.o

$(TEST)/fas.out:	$(FAS)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(FAS) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/fas_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/fas_rcgp.c
	$(CC) $(SRC)/fas_rcgp.c -o $(LIB)/fas_rcgp.o


$(LIB)/fas_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/fas_parser.o\
		-DPID=1\
		-DINTERFACE="_fas_if()"


$(LIB)/fas_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/fas_t.c
	$(CC) $(SRC)/fas_t.c -o $(LIB)/fas_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid

CYCLIC=$(LIB)/RCG_main.o	\
	$(LIB)/cyclic_t.o	\
	$(LIB)/cyclic_rcgp.o\
	$(LIB)/cyclic_parser.o

$(TEST)/cyclic.out:	$(CYCLIC)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(CYCLIC) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/cyclic_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/cyclic_rcgp.c
	$(CC) $(SRC)/cyclic_rcgp.c -o $(LIB)/cyclic_rcgp.o


$(LIB)/cyclic_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/cyclic_parser.o\
		-DPID=1\
		-DINTERFACE="_cyclic_if()"


$(LIB)/cyclic_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/cyclic_t.c
	$(CC) $(SRC)/cyclic_t.c -o $(LIB)/cyclic_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid

METEO=$(LIB)/RCG_main.o		\
	$(LIB)/meteo_t.o	\
	$(LIB)/meteo_rcgp.o	\
	$(LIB)/meteo_drcgt.o	\
	$(LIB)/meteo_sem_fun.o	\
	$(LIB)/meteo_parser.o

$(TEST)/meteo.out:	$(METEO)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(METEO) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/meteo_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/meteo_rcgp.c
	$(CC) $(SRC)/meteo_rcgp.c -o $(LIB)/meteo_rcgp.o


$(LIB)/meteo_drcgt.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/sxba.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SRC)/meteo_drcgt.c
	$(CC) $(SRC)/meteo_drcgt.c -o $(LIB)/meteo_drcgt.o


$(LIB)/meteo_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/meteo_parser.o\
		-DPID=1\
		-DINTERFACE="_meteo_if()"


$(LIB)/meteo_sem_fun.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/dcg_cst.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/DRCG_sem_fun.c
	$(CC) $(SRC)/DRCG_sem_fun.c -o $(LIB)/meteo_sem_fun.o\
		-DPID=1


$(LIB)/meteo_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/meteo_t.c
	$(CC) $(SRC)/meteo_t.c -o $(LIB)/meteo_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid

DTEST=$(LIB)/RCG_main.o		\
	$(LIB)/dtest_t.o	\
	$(LIB)/dtest_rcgp.o	\
	$(LIB)/dtest_drcgt.o	\
	$(LIB)/dtest_sem_fun.o	\
	$(LIB)/dtest_parser.o

$(TEST)/dtest.out:	$(DTEST)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(DTEST) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/dtest_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/dtest_rcgp.c
	$(CC) $(SRC)/dtest_rcgp.c -o $(LIB)/dtest_rcgp.o


$(LIB)/dtest_drcgt.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/sxba.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SRC)/dtest_drcgt.c
	$(CC) $(SRC)/dtest_drcgt.c -o $(LIB)/dtest_drcgt.o

$(LIB)/dtest_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/dtest_parser.o\
		-DPID=1\
		-DINTERFACE="_dtest_if()"

$(LIB)/dtest_sem_fun.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/dcg_cst.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/DRCG_sem_fun.c
	$(CC) $(SRC)/DRCG_sem_fun.c -o $(LIB)/dtest_sem_fun.o\
		-DPID=1

$(LIB)/dtest_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/dtest_t.c
	$(CC) $(SRC)/dtest_t.c -o $(LIB)/dtest_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid

DTEST2=$(LIB)/RCG_main.o	\
	$(LIB)/dtest2_t.o	\
	$(LIB)/dtest2_rcgp.o	\
	$(LIB)/dtest2_drcgt.o	\
	$(LIB)/dtest2_sem_fun.o	\
	$(LIB)/dtest2_parser.o

$(TEST)/dtest2.out:	$(DTEST2)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(DTEST2) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/dtest2_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/dtest2_rcgp.c
	$(CC) $(SRC)/dtest2_rcgp.c -o $(LIB)/dtest2_rcgp.o


$(LIB)/dtest2_drcgt.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/sxba.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SRC)/dtest2_drcgt.c
	$(CC) $(SRC)/dtest2_drcgt.c -o $(LIB)/dtest2_drcgt.o


$(LIB)/dtest2_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/dtest2_parser.o\
		-DPID=1\
		-DINTERFACE="_dtest2_if()"

$(LIB)/dtest2_sem_fun.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/dcg_cst.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/DRCG_sem_fun.c
	$(CC) $(SRC)/DRCG_sem_fun.c -o $(LIB)/dtest2_sem_fun.o\
		-DPID=1

$(LIB)/dtest2_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/dtest2_t.c
	$(CC) $(SRC)/dtest2_t.c -o $(LIB)/dtest2_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



DTEST3=$(LIB)/RCG_main.o	\
	$(LIB)/dtest3_t.o	\
	$(LIB)/dtest3_rcgp.o	\
	$(LIB)/dtest3_drcgt.o	\
	$(LIB)/dtest3_sem_fun.o	\
	$(LIB)/dtest3_parser.o

$(TEST)/dtest3.out:	$(DTEST3)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(DTEST3) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/dtest3_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/dtest3_rcgp.c
	$(CC) $(SRC)/dtest3_rcgp.c -o $(LIB)/dtest3_rcgp.o

$(LIB)/dtest3_drcgt.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/sxba.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SRC)/dtest3_drcgt.c
	$(CC) $(SRC)/dtest3_drcgt.c -o $(LIB)/dtest3_drcgt.o

$(LIB)/dtest3_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/dtest3_parser.o\
		-DPID=1\
		-DINTERFACE="_dtest3_if()"

$(LIB)/dtest3_sem_fun.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/dcg_cst.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/DRCG_sem_fun.c
	$(CC) $(SRC)/DRCG_sem_fun.c -o $(LIB)/dtest3_sem_fun.o\
		-DPID=1

$(LIB)/dtest3_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/dtest3_t.c
	$(CC) $(SRC)/dtest3_t.c -o $(LIB)/dtest3_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid




DTEST4=$(LIB)/RCG_main.o	\
	$(LIB)/dtest4_t.o	\
	$(LIB)/dtest4_rcgp.o	\
	$(LIB)/dtest4_drcgt.o	\
	$(LIB)/dtest4_sem_fun.o	\
	$(LIB)/dtest4_parser.o

$(TEST)/dtest4.out:	$(DTEST4)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(DTEST4) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/dtest4_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/dtest4_rcgp.c
	$(CC) $(SRC)/dtest4_rcgp.c -o $(LIB)/dtest4_rcgp.o


$(LIB)/dtest4_drcgt.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/sxba.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SRC)/dtest4_drcgt.c
	$(CC) $(SRC)/dtest4_drcgt.c -o $(LIB)/dtest4_drcgt.o

$(LIB)/dtest4_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/dtest4_parser.o\
		-DPID=1\
		-DINTERFACE="_dtest4_if()"

$(LIB)/dtest4_sem_fun.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/dcg_cst.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/DRCG_sem_fun.c
	$(CC) $(SRC)/DRCG_sem_fun.c -o $(LIB)/dtest4_sem_fun.o\
		-DPID=1

$(LIB)/dtest4_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/dtest4_t.c
	$(CC) $(SRC)/dtest4_t.c -o $(LIB)/dtest4_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


N2=$(LIB)/RCG_main.o		\
	$(LIB)/n2_t.o		\
	$(LIB)/n2_rcgp.o	\
	$(LIB)/n2_parser.o

$(TEST)/n2.out:	$(N2)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(N2) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/n2_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/n2_rcgp.c
	$(CC) $(SRC)/n2_rcgp.c -o $(LIB)/n2_rcgp.o


$(LIB)/n2_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/n2_parser.o\
		-DPID=1\
		-DINTERFACE="_n2_if()"


$(LIB)/n2_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/n2_t.c
	$(CC) $(SRC)/n2_t.c -o $(LIB)/n2_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


ARITY0=$(LIB)/RCG_main.o	\
	$(LIB)/arity0_t.o	\
	$(LIB)/arity0_rcgp.o\
	$(LIB)/arity0_parser.o

$(TEST)/arity0.out:	$(ARITY0)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(ARITY0) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/arity0_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/arity0_rcgp.c
	$(CC) $(SRC)/arity0_rcgp.c -o $(LIB)/arity0_rcgp.o

$(LIB)/arity0_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/arity0_parser.o\
		-DPID=1\
		-DINTERFACE="_arity0_if()"


$(LIB)/arity0_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/arity0_t.c
	$(CC) $(SRC)/arity0_t.c -o $(LIB)/arity0_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid

W_PWR_W=$(LIB)/RCG_main.o		\
	$(LIB)/w_pwr_w_t.o		\
	$(LIB)/w_pwr_w_rcgp.o	\
	$(LIB)/w_pwr_w_parser.o

$(TEST)/w_pwr_w.out:	$(W_PWR_W)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(W_PWR_W) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/w_pwr_w_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/w_pwr_w_rcgp.c
	$(CC) $(SRC)/w_pwr_w_rcgp.c -o $(LIB)/w_pwr_w_rcgp.o


$(LIB)/w_pwr_w_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/w_pwr_w_parser.o\
		-DPID=1\
		-DINTERFACE="_w_pwr_w_if()"


$(LIB)/w_pwr_w_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/w_pwr_w_t.c
	$(CC) $(SRC)/w_pwr_w_t.c -o $(LIB)/w_pwr_w_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



W_PWR_W_PR=$(LIB)/RCG_main.o		\
	$(LIB)/w_pwr_w_pr_t.o	\
	$(LIB)/w_pwr_w_pr_rcgp.o	\
	$(LIB)/w_pwr_w_rcgp.o		\
	$(LIB)/prime6_rcgp.o		\
	$(LIB)/prime6_parsact.o		\
	$(LIB)/w_pwr_w_pr_parser.o

$(TEST)/w_pwr_w_pr.out:	$(W_PWR_W_PR)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(W_PWR_W_PR) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/w_pwr_w_pr_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/w_pwr_w_pr_rcgp.c
	$(CC) $(SRC)/w_pwr_w_pr_rcgp.c -o $(LIB)/w_pwr_w_pr_rcgp.o


$(LIB)/w_pwr_w_pr_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/w_pwr_w_pr_parser.o\
		-DPID=4\
		-DINTERFACE="_w_pwr_w_pr_if(), _w_pwr_w_if(), _prime6_if(), _prime6_parsact_if()"


$(LIB)/w_pwr_w_pr_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/w_pwr_w_pr_t.c
	$(CC) $(SRC)/w_pwr_w_pr_t.c -o $(LIB)/w_pwr_w_pr_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


# Test en generation a partir de gan.drcg (ecrite a la main)
# L'option -DGENERATOR=1 ds gan_rcgp.c modifie les macros (L)SCAN(i,t) en TRUE
GAN=$(LIB)/RCG_main.o		\
	$(LIB)/gan_t.o		\
	$(LIB)/gan_rcgp.o	\
	$(LIB)/gan_drcgt.o	\
	$(LIB)/gan_sem_fun.o	\
	$(LIB)/gan_parser.o

$(TEST)/gan.out:	$(GAN)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(GAN) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/gan_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/gan_rcgp.c
	$(CC) $(SRC)/gan_rcgp.c -o $(LIB)/gan_rcgp.o\
		-DGENERATOR=1

$(LIB)/gan_drcgt.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/sxba.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SRC)/gan_drcgt.c
	$(CC) $(SRC)/gan_drcgt.c -o $(LIB)/gan_drcgt.o

$(LIB)/gan_sem_fun.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/dcg_cst.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/DRCG_sem_fun.c
	$(CC) $(SRC)/DRCG_sem_fun.c -o $(LIB)/gan_sem_fun.o\
		-DPID=1

$(LIB)/gan_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/gan_parser.o\
		-DGENERATOR=1\
		-DPID=1\
		-DINTERFACE="_gan_if()"

$(LIB)/gan_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/gan_t.c
	$(CC) $(SRC)/gan_t.c -o $(LIB)/gan_t.o -Dsxtables=RCG_tables -Dsxscanner=sxvoid -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


DISTANCE=$(LIB)/RCG_main.o	\
	$(LIB)/distance_t.o	\
	$(LIB)/distance_rcgp.o\
	$(LIB)/distance_parser.o

$(TEST)/distance.out:	$(DISTANCE)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(DISTANCE) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/distance_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/distance_semact.h	\
		$(SRC)/distance_rcgp.c
	$(cc) $(SRC)/distance_rcgp.c -o $(LIB)/distance_rcgp.o -I$(SRC)\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=0\
		-Dstore_elem_tree=_distance_first_pass_for_semact


$(LIB)/distance_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/distance_parser.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=0\
		-DPID=1\
		-DINTERFACE="_distance_if()"


$(LIB)/distance_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/distance_t.c
	$(CC) $(SRC)/distance_t.c -o $(LIB)/distance_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


# Test de la correction d'erreur a partir de rcvr.drcg (ecrite a la main depuis rcvr.rcg)
# L'option -DGENERATOR=1 ds rcvr_rcgp.c modifie les macros (L)SCAN(i,t) en TRUE
RCVR=$(LIB)/RCG_main.o		\
	$(LIB)/rcvr_t.o		\
	$(LIB)/rcvr_rcgp.o	\
	$(LIB)/rcvr_parser.o	\
	$(LIB)/distance_rcgp.o	\
	$(LIB)/grcvr_rcgp.o	\
	$(LIB)/grcvr_drcgt.o	\
	$(LIB)/grcvr_sem_fun.o


$(TEST)/rcvr.out:	$(RCVR)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(RCVR) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

# rcvr_rcgp.c est compile' 2 fois une fois en analyse et une fois en generation
$(LIB)/rcvr_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/rcvr_rcgp.c
	$(cc) $(SRC)/rcvr_rcgp.c -o $(LIB)/rcvr_rcgp.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=1 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0\
		-DSXDRCG_rcvr_sem_fun=sxvoid

$(LIB)/grcvr_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/rcvr_rcgp.c
	$(cc) $(SRC)/grcvr_rcgp.c -o $(LIB)/grcvr_rcgp.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0\
		-DGENERATOR=1

$(LIB)/grcvr_drcgt.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/sxba.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SRC)/grcvr_drcgt.c
	$(CC) $(SRC)/grcvr_drcgt.c -o $(LIB)/grcvr_drcgt.o

$(LIB)/grcvr_sem_fun.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/dcg_cst.h	\
		$(SXINCL)/drcg_glbl.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/DRCG_sem_fun.c
	$(cc) $(SRC)/DRCG_sem_fun.c -o $(LIB)/grcvr_sem_fun.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=1 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0\
		-DPID=1

$(LIB)/rcvr_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/rcvr_parser.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=1 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0\
		-DRCVR=1\
		-DPID=1\
		-DINTERFACE="_rcvr_if()"\
		-DGINTERFACE="_grcvr_if()"\
		-DMAX_DIST=3

$(LIB)/rcvr_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/rcvr_t.c
	$(CC) $(SRC)/rcvr_t.c -o $(LIB)/rcvr_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid





ICG1=$(LIB)/RCG_main.o	\
	$(LIB)/icg1_t.o	\
	$(LIB)/icg1_rcgp.o\
	$(LIB)/icg1_parser.o

$(TEST)/icg1.out:	$(ICG1)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(ICG1) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/icg1_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/icg1_rcgp.c
	$(CC) $(SRC)/icg1_rcgp.c -o $(LIB)/icg1_rcgp.o

$(LIB)/icg1_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/icg1_parser.o\
		-DPID=1\
		-DINTERFACE="_icg1_if()"


$(LIB)/icg1_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/icg1_t.c
	$(CC) $(SRC)/icg1_t.c -o $(LIB)/icg1_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid




ICG2=$(LIB)/RCG_main.o	\
	$(LIB)/icg2_t.o	\
	$(LIB)/icg2_rcgp.o\
	$(LIB)/icg2_parser.o

$(TEST)/icg2.out:	$(ICG2)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(ICG2) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/icg2_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/icg2_rcgp.c
	$(CC) $(SRC)/icg2_rcgp.c -o $(LIB)/icg2_rcgp.o

$(LIB)/icg2_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/icg2_parser.o\
		-DPID=1\
		-DINTERFACE="_icg2_if()"


$(LIB)/icg2_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/icg2_t.c
	$(CC) $(SRC)/icg2_t.c -o $(LIB)/icg2_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


ICG3=$(LIB)/RCG_main.o	\
	$(LIB)/icg3_t.o	\
	$(LIB)/icg3_rcgp.o\
	$(LIB)/icg3_parser.o

$(TEST)/icg3.out:	$(ICG3)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(ICG3) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/icg3_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/icg3_rcgp.c
	$(CC) $(SRC)/icg3_rcgp.c -o $(LIB)/icg3_rcgp.o

$(LIB)/icg3_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/icg3_parser.o\
		-DPID=1\
		-DINTERFACE="_icg3_if()"


$(LIB)/icg3_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/icg3_t.c
	$(CC) $(SRC)/icg3_t.c -o $(LIB)/icg3_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


ANBM=$(LIB)/RCG_main.o	\
	$(LIB)/anbm_t.o	\
	$(LIB)/anbm_rcgp.o\
	$(LIB)/anbm_parser.o

$(TEST)/anbm.out:	$(ANBM)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(ANBM) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/anbm_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/anbm_rcgp.c
	$(CC) $(SRC)/anbm_rcgp.c -o $(LIB)/anbm_rcgp.o

$(LIB)/anbm_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/anbm_parser.o\
		-DPID=1\
		-DINTERFACE="_anbm_if()"


$(LIB)/anbm_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/anbm_t.c
	$(CC) $(SRC)/anbm_t.c -o $(LIB)/anbm_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


CFG=$(LIB)/RCG_main.o	\
	$(LIB)/cfg_t.o	\
	$(LIB)/cfg_rcgp.o\
	$(LIB)/cfg_parser.o

$(TEST)/cfg.out:	$(CFG)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(CFG) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/cfg_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/cfg_semact.h	\
		$(SRC)/cfg_rcgp.c
	$(cc) $(SRC)/cfg_rcgp.c -o $(LIB)/cfg_rcgp.o -I$(SRC)\
		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1


$(LIB)/cfg_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/cfg_parser.o\
		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
		-DPID=1\
		-DINTERFACE="_cfg_if()"
#		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\

$(LIB)/cfg_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/cfg_t.c
	$(CC) $(SRC)/cfg_t.c -o $(LIB)/cfg_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



BINARY=$(LIB)/RCG_main.o	\
	$(LIB)/binary_t.o	\
	$(LIB)/binary_rcgp.o	\
	$(LIB)/binary_parser.o

$(TEST)/binary.out:	$(BINARY)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(BINARY) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/binary_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/binary_semact.h	\
		$(SRC)/binary_rcgp.c
	$(cc) $(SRC)/binary_rcgp.c -o $(LIB)/binary_rcgp.o -I$(SRC)\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1


$(LIB)/binary_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/binary_parser.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
		-DPID=1\
		-DINTERFACE="_binary_if()"
#		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\

$(LIB)/binary_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/binary_t.c
	$(CC) $(SRC)/binary_t.c -o $(LIB)/binary_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid




RNATAG=$(LIB)/RCG_main.o	\
	$(LIB)/rnatag_t.o	\
	$(LIB)/rnatag_rcgp.o	\
	$(LIB)/rnatag_parser.o

$(TEST)/rnatag.out:	$(RNATAG)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(RNATAG) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/rnatag_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/rnatag_semact.h	\
		$(SRC)/rnatag_rcgp.c
	$(cc) $(SRC)/rnatag_rcgp.c -o $(LIB)/rnatag_rcgp.o -I$(SRC)\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0
#		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0


$(LIB)/rnatag_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/rnatag_parser.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0\
		-DPID=1\
		-DINTERFACE="_rnatag_if()"
#		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0

$(LIB)/rnatag_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/rnatag_t.c
	$(CC) $(SRC)/rnatag_t.c -o $(LIB)/rnatag_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid





RNA=$(LIB)/RCG_main.o	\
	$(LIB)/RNA_t.o	\
	$(LIB)/RNA_rcgp.o	\
	$(LIB)/RNA_parser.o

$(TEST)/RNA.out:	$(RNA)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(RNA) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/RNA_rcgp.o: $(SXINCL)/sxunix.h 	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/RNA_semact.h	\
		$(SRC)/RNA_rcgp.c
	$(cc) $(SRC)/RNA_rcgp.c -o $(LIB)/RNA_rcgp.o -I$(SRC)\
		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0
#		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0


$(LIB)/RNA_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/RNA_parser.o\
		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0\
		-DPID=1\
		-DINTERFACE="_RNA_if()"
#		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0\
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1 -DCYCLIC=0\

$(LIB)/RNA_t.o: 	$(SXINCL)/sxunix.h	\
			$(SRC)/RNA_t.c
	$(CC) $(SRC)/RNA_t.c -o $(LIB)/RNA_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


SCR=$(LIB)/RCG_main.o	\
	$(LIB)/SCR_t.o	\
	$(LIB)/SCR_rcgp.o	\
	$(LIB)/SCR_parser.o

$(TEST)/SCR.out:	$(SCR)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(SCR) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/SCR_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/SCR_semact.h	\
		$(SRC)/SCR_rcgp.c
	$(cc) $(SRC)/SCR_rcgp.c -o $(LIB)/SCR_rcgp.o -I$(SRC)\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1


$(LIB)/SCR_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/SCR_parser.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
		-DPID=1\
		-DINTERFACE="_SCR_if()"
#		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\

$(LIB)/SCR_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/SCR_t.c
	$(CC) $(SRC)/SCR_t.c -o $(LIB)/SCR_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



NIL=$(LIB)/RCG_main.o	\
	$(LIB)/nil_t.o	\
	$(LIB)/nil_rcgp.o\
	$(LIB)/nil_parser.o

$(TEST)/nil.out:	$(NIL)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(NIL) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/nil_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/nil_rcgp.c
	$(cc) $(SRC)/nil_rcgp.c -o $(LIB)/nil_rcgp.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1


$(LIB)/nil_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/nil_parser.o\
		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
		-DPID=1\
		-DINTERFACE="_nil_if()"
#		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\


$(LIB)/nil_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/nil_t.c
	$(CC) $(SRC)/nil_t.c -o $(LIB)/nil_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid




COUNT=$(LIB)/RCG_main.o	\
	$(LIB)/count_t.o	\
	$(LIB)/count_rcgp.o\
	$(LIB)/cnt_result_rcgp.o\
	$(LIB)/count_parser.o

$(TEST)/count.out:	$(COUNT)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(COUNT) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/count_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/count_rcgp.c
	$(cc) $(SRC)/count_rcgp.c -o $(LIB)/count_rcgp.o\
		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-O2 -DEBUG2 -DRECOGNIZER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1


$(LIB)/cnt_result_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/cnt_result_semact.h	\
		$(SRC)/cnt_result_rcgp.c
	$(cc) $(SRC)/cnt_result_rcgp.c -o $(LIB)/cnt_result_rcgp.o\
		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1


$(LIB)/count_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/count_parser.o\
		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
		-DPID=2\
		-DINTERFACE="_count_if(), _cnt_result_if()"
#		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DPARSER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\


$(LIB)/count_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/count_t.c
	$(CC) $(SRC)/count_t.c -o $(LIB)/count_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


TAG_ERIC=$(LIB)/RCG_main.o	\
	$(LIB)/tag_eric_t.o	\
	$(LIB)/tag_eric_rcgp.o\
	$(LIB)/tag_eric_parser.o

$(TEST)/tag_eric.out:	$(TAG_ERIC)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(TAG_ERIC) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/tag_eric_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/tag_eric_rcgp.c
	$(cc) $(SRC)/tag_eric_rcgp.c -o $(LIB)/tag_eric_rcgp.o\
		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1
#		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1


$(LIB)/tag_eric_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(cc) $(SRC)/RCG_parser.c -o $(LIB)/tag_eric_parser.o\
		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1\
		-DPID=1\
		-DINTERFACE="_tag_eric_if()"
#		-O2 -DEBUG2 -DPARSER -DLEX=0 -DFIRST_LAST=0 -DMEMO=1 -DSINGLE_PASS=1\
#		-g -DEBUG_ALLOC -DEBUG2 -DEBUG -DRECOGNIZER -DFIRST_LAST=0 -DMEMO=0 -DSINGLE_PASS=1\


$(LIB)/tag_eric_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/tag_eric_t.c
	$(CC) $(SRC)/tag_eric_t.c -o $(LIB)/tag_eric_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid

ANBNCN=$(LIB)/RCG_main.o	\
	$(LIB)/anbncn_t.o	\
	$(LIB)/anbncn_rcgp.o\
	$(LIB)/anbncn_parser.o

$(TEST)/anbncn.out:	$(ANBNCN)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(ANBNCN) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/anbncn_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/anbncn_rcgp.c
	$(CC) $(SRC)/anbncn_rcgp.c -o $(LIB)/anbncn_rcgp.o	\
		-DGENERATOR	\
		-DANY=1

$(LIB)/anbncn_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/anbncn_parser.o	\
		-DGENERATOR	\
		-DANY=1		\
		-DPID=1		\
		-DINTERFACE="_anbncn_if()"


$(LIB)/anbncn_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/anbncn_t.c
	$(CC) $(SRC)/anbncn_t.c -o $(LIB)/anbncn_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


WW=$(LIB)/RCG_main.o		\
	$(LIB)/ww_t.o		\
	$(LIB)/ww_rcgp.o	\
	$(LIB)/ww_parser.o

$(TEST)/ww.out:	$(WW)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(WW) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/ww_rcgp.o: $(SXINCL)/sxunix.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/ww_rcgp.c
	$(CC) $(SRC)/ww_rcgp.c -o $(LIB)/ww_rcgp.o	\
		-DGENERATOR	\
		-DANY=1

$(LIB)/ww_parser.o: $(SXINCL)/sxunix.h	\
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/ww_parser.o\
		-DGENERATOR	\
		-DANY=1		\
		-DPID=1		\
		-DINTERFACE="_ww_if()"


$(LIB)/ww_t.o: 	$(SXINCL)/sxunix.h	\
			$(SRC)/ww_t.c
	$(CC) $(SRC)/ww_t.c -o $(LIB)/ww_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


PLURIEL=$(LIB)/RCG_main.o	\
	$(LIB)/pluriel_t.o	\
	$(LIB)/pluriel_rcgp.o\
	$(LIB)/pluriel_parser.o

$(TEST)/pluriel.out:	$(PLURIEL)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(PLURIEL) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/pluriel_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/pluriel_rcgp.c
	$(CC) $(SRC)/pluriel_rcgp.c -o $(LIB)/pluriel_rcgp.o

$(LIB)/pluriel_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/pluriel_parser.o	\
		-DPID=1		\
		-DINTERFACE="_pluriel_if()"


$(LIB)/pluriel_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/pluriel_t.c
	$(CC) $(SRC)/pluriel_t.c -o $(LIB)/pluriel_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



GEN_PLURIEL=$(LIB)/RCG_main.o		\
	$(LIB)/pluriel_0_t.o		\
	$(LIB)/pluriel_0_rcgp.o	\
	$(LIB)/pluriel_1_rcgp.o	\
	$(LIB)/pluriel_0_parser.o

$(TEST)/gen_pluriel.out:	$(GEN_PLURIEL)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(GEN_PLURIEL) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/pluriel_0_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/pluriel_0_rcgp.c
	$(CC) $(SRC)/pluriel_0_rcgp.c -o $(LIB)/pluriel_0_rcgp.o	\
		-DGENERATOR	\
		-DANY=1

$(LIB)/pluriel_1_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/pluriel_1_rcgp.c
	$(CC) $(SRC)/pluriel_1_rcgp.c -o $(LIB)/pluriel_1_rcgp.o	\
		-DGENERATOR	\
		-DANY=1

$(LIB)/pluriel_0_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/pluriel_0_parser.o	\
		-DGENERATOR	\
		-DANY=1		\
		-DPID=2		\
		-DINTERFACE="_pluriel_0_if(), _pluriel_1_if()"


$(LIB)/pluriel_0_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/pluriel_0_t.c
	$(CC) $(SRC)/pluriel_0_t.c -o $(LIB)/pluriel_0_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



TEST_IDAG=$(LIB)/RCG_main.o	\
	$(LIB)/test_idag_t.o	\
	$(LIB)/test_idag_dagrcgp.o\
	$(LIB)/test_idag_parser.o

$(TEST)/test_idag.out:	$(TEST_IDAG)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(TEST_IDAG) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/test_idag_dagrcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/test_idag_dagrcgp.c
	$(CC) $(SRC)/test_idag_dagrcgp.c -o $(LIB)/test_idag_dagrcgp.o	\
		-DAG		\
		-DGENERATOR=1	\
		-DANY=1

$(LIB)/test_idag_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/test_idag_parser.o	\
		-DAG		\
		-DGENERATOR=1	\
		-DANY=1		\
		-DPID=1		\
		-DINTERFACE="_test_idag_if()"


$(LIB)/test_idag_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/test_idag_t.c
	$(CC) $(SRC)/test_idag_t.c -o $(LIB)/test_idag_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



FB=$(LIB)/RCG_main.o	\
	$(LIB)/FB_t.o	\
	$(LIB)/FB_rcgp.o\
	$(LIB)/FB_parser.o

$(TEST)/FB.out:	$(FB)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(FB) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/FB_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/FB_rcgp.c
	$(CC) $(SRC)/FB_rcgp.c -o $(LIB)/FB_rcgp.o	\
		-DPARSER 	\
		-DLEX=1		\
		-DLEX2=1	\
		-DDYNAM_LEX=1	\
		-DFIRST_LAST=0	\
		-DMEMO=1 

$(LIB)/FB_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/FB_parser.o	\
		-DPARSER 	\
		-DLEX=1		\
		-DLEX2=1	\
		-DDYNAM_LEX=1	\
		-DFIRST_LAST=0	\
		-DMEMO=1 	\
		-DPID=1		\
		-DINTERFACE="_FB_if()"


$(LIB)/FB_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/FB_t.c
	$(CC) $(SRC)/FB_t.c -o $(LIB)/FB_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid


OLTAG=$(LIB)/RCG_main.o	\
	$(LIB)/OLTAG_t.o	\
	$(LIB)/OLTAG_rcgp.o\
	$(LIB)/OLTAG_parser.o

$(TEST)/OLTAG.out:	$(OLTAG)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(OLTAG) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/OLTAG_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/OLTAG_rcgp.c
	$(CC) $(SRC)/OLTAG_rcgp.c -o $(LIB)/OLTAG_rcgp.o	\
		-DPARSER 	\
		-DSDAG=1 	\
		-DLEX=1		\
		-DLEX2=0	\
		-DFIRST_LAST=0	\
		-DMEMO=1 

$(LIB)/OLTAG_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/OLTAG_parser.o	\
		-DOLTAG=1 	\
		-DPARSER 	\
		-DSDAG=1 	\
		-DLEX=1		\
		-DLEX2=0	\
		-DFIRST_LAST=0	\
		-DMEMO=1 	\
		-DPID=1		\
		-DINTERFACE="_OLTAG_if()"


$(LIB)/OLTAG_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/OLTAG_t.c
	$(CC) $(SRC)/OLTAG_t.c -o $(LIB)/OLTAG_t.o -Dsxtables=RCG_tables -DSEMACT=oltag_semact


FXTAG=$(LIB)/RCG_main.o	\
	$(LIB)/fxtag_t.o	\
	$(LIB)/fxtag_rcgp.o\
	$(LIB)/fxtag_parser.o

$(TEST)/fxtag.out:	$(FXTAG)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(FXTAG) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/fxtag_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/fxtag_rcgp.c
	$(CC) $(SRC)/fxtag_rcgp.c -o $(LIB)/fxtag_rcgp.o	\
		-DPARSER 	\
		-DSDAG=1 	\
		-DLEX=1		\
		-DLEX2=1	\
		-DDYNAM_LEX=1	\
		-DFIRST_LAST=0	\
		-DMEMO=1 

$(LIB)/fxtag_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/fxtag_parser.o	\
		-DOLTAG=1 	\
		-DPARSER 	\
		-DSDAG=1 	\
		-DLEX=1		\
		-DLEX2=1	\
		-DDYNAM_LEX=1	\
		-DFIRST_LAST=0	\
		-DMEMO=1 	\
		-DPID=1		\
		-DINTERFACE="_fxtag_if()"


$(LIB)/fxtag_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/fxtag_t.c
	$(CC) $(SRC)/fxtag_t.c -o $(LIB)/fxtag_t.o -Dsxtables=RCG_tables -DSEMACT=oltag_semact


PERMUTE=$(LIB)/RCG_main.o	\
	$(LIB)/permute_t.o	\
	$(LIB)/permute_rcgp.o\
	$(LIB)/permute_parser.o

$(TEST)/permute.out:	$(PERMUTE)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(PERMUTE) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/permute_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/permute_rcgp.c
	$(CC) $(SRC)/permute_rcgp.c -o $(LIB)/permute_rcgp.o	\
		-DRECOGNIZER 	\
		-DFIRST_LAST=0	\
		-DMEMO=0 

$(LIB)/permute_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/permute_parser.o	\
		-DRECOGNIZER 	\
		-DFIRST_LAST=0	\
		-DMEMO=0 	\
		-DPID=1		\
		-DINTERFACE="_permute_if()"


$(LIB)/permute_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/permute_t.c
	$(CC) $(SRC)/permute_t.c -o $(LIB)/permute_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid



NEW_PRIME=$(LIB)/RCG_main.o	\
	$(LIB)/new_prime_t.o	\
	$(LIB)/new_prime_rcgp.o\
	$(LIB)/new_prime_parser.o

$(TEST)/new_prime.out:	$(NEW_PRIME)	$(BIBS)
	@echo '	$(COMPILER) -o $@'; ${SHELL} -c '$(COMPILER) -o $@ $(NEW_PRIME) $(CFLAGS) $(LDFLAGS) $(BIBS) /usr/lib/libm.a || (rm $@;exit 1)'

$(LIB)/new_prime_rcgp.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SXINCL)/X.h		\
		$(SXINCL)/XxY.h		\
		$(SXINCL)/XxYxZ.h	\
		$(SXINCL)/XH.h		\
		$(SRC)/new_prime_rcgp.c
	$(CC) $(SRC)/new_prime_rcgp.c -o $(LIB)/new_prime_rcgp.o	\
		-DRECOGNIZER 	\
		-DFIRST_LAST=0	\
		-DMEMO=0 

$(LIB)/new_prime_parser.o: $(SXINCL)/sxunix.h \
		$(SXINCL)/rcg_glbl.h	\
		$(SXINCL)/sxba.h	\
		$(SRC)/RCG_parser.c
	$(CC) $(SRC)/RCG_parser.c -o $(LIB)/new_prime_parser.o\
		-DRECOGNIZER 	\
		-DFIRST_LAST=0	\
		-DMEMO=0 	\
		-DPID=1		\
		-DINTERFACE="_new_prime_if()"


$(LIB)/new_prime_t.o: 	$(SXINCL)/sxunix.h		\
			$(SRC)/new_prime_t.c
	$(CC) $(SRC)/new_prime_t.c -o $(LIB)/new_prime_t.o -Dsxtables=RCG_tables -Dsxparser=sxrcg_parser -DSEMACT=sxvoid




