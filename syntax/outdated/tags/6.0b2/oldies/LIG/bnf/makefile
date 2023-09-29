# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(HOME)/syntax/5.0/sun4
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
ETCINCL=$(ETC)/incl

LIG=$(HOME)/syntax/5.0/sun4/LIG/bnf
SRC=$(LIG)
LIB=$(LIG)
INCL=$(LIG)

BIBS= $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a

CC=cd $(LIB); umask 7; exec cc -I$(INCL) -I$(SXINCL) -I$(ETC)/incl $(CFLAGS) -DEBUG -c

$(LIG)/everything .PRECIOUS:	$(LIG)/wcw.out		\
				$(LIG)/wcw2.out		\
				$(LIG)/wcw3.out		\
				$(LIG)/ndwcw3.out 	\
				$(LIG)/ndwcw3_post2.out \
				$(LIG)/cycle.out	\
				$(LIG)/cycle_mixt.out	\
				$(LIG)/cycle_bylev.out	\
				$(LIG)/cycle_post.out	\
				$(LIG)/cycle_post2.out


NEEDS=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/wcw_t.o

NEEDS2=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/wcw2_t.o

NEEDS3=\
	$(SXLIB)/sxmain.o		\
 	$(LIB)/wcw3_parsact.o		\
	$(LIB)/wcw3_t.o

NDNEEDS3=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/ndwcw3_t.o

NDNEEDS3_POST2=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/ndwcw3_post2_t.o

CYCLE=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/cycle_t.o

CYCLE_MIXT=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/cycle_mixt_t.o

CYCLE_BYLEV=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/cycle_bylev_t.o

CYCLE_POST=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/cycle_post_t.o

CYCLE_POST2=\
	$(SXLIB)/sxmain.o		\
	$(LIB)/cycle_post2_t.o

$(LIG)/wcw.out:	$(NEEDS)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/wcw_t.o:		$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/wcw_t.c
	$(CC) $(SRC)/wcw_t.c			\
		-DPARSACT=sxdligparsact		\
		-DSEMACT=sxvoid			\
	        -DPARSACT_2=sxvoid


$(LIG)/wcw2.out:	$(NEEDS2)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/wcw2_t.o:		$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/wcw2_t.c
	$(CC) $(SRC)/wcw2_t.c			\
		-DPARSACT=sxdligparsact		\
		-DSEMACT=sxvoid			\
	        -DPARSACT_2=sxvoid


$(LIG)/wcw3.out:	$(NEEDS3)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/wcw3_parsact.o:	$(SXINCL)/sxunix.h	\
			$(LIG)/force-make 	\
			$(INCL)/wcw3_tdef.h 	\
			$(SRC)/wcw3_parsact.c
	$(CC) $(SRC)/wcw3_parsact.c

$(LIB)/wcw3_t.o:	$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/wcw3_t.c
	$(CC) $(SRC)/wcw3_t.c			\
		-DPARSACT=sxdligparsact		\
		-DSEMACT=sxvoid			\
	        -DPARSACT_2=wcw3_parsact


$(LIG)/ndwcw3.out:	$(NDNEEDS3)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NDNEEDS3) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/ndwcw3_t.o:	$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/ndwcw3_t.c
	$(CC) $(SRC)/ndwcw3_t.c			\
		-DPARSACT=sxndligparsact	\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid


$(LIG)/ndwcw3_post2.out:	$(NDNEEDS3_POST2)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NDNEEDS3_POST2) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/ndwcw3_post2_t.o:	$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/ndwcw3_post2_t.c
	$(CC) $(SRC)/ndwcw3_post2_t.c		\
		-DPARSACT=sxndligpost2		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid


$(LIG)/cycle.out:	$(CYCLE)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(CYCLE) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/cycle_t.o:	$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/cycle_t.c
	$(CC) $(SRC)/cycle_t.c			\
		-DPARSACT=sxndligparsact	\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(LIG)/cycle_mixt.out:	$(CYCLE_MIXT)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(CYCLE_MIXT) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/cycle_mixt_t.o:	$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/cycle_mixt_t.c
	$(CC) $(SRC)/cycle_mixt_t.c		\
		-DPARSACT=sxndligmixt		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(LIG)/cycle_bylev.out:	$(CYCLE_BYLEV)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(CYCLE_BYLEV) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/cycle_bylev_t.o:	$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/cycle_bylev_t.c
	$(CC) $(SRC)/cycle_bylev_t.c		\
		-DPARSACT=sxndligbylev		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(LIG)/cycle_post.out:	$(CYCLE_POST)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(CYCLE_POST) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/cycle_post_t.o:	$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/cycle_post_t.c
	$(CC) $(SRC)/cycle_post_t.c		\
		-DPARSACT=sxndligpost		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(LIG)/cycle_post2.out:	$(CYCLE_POST2)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(CYCLE_POST2) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/cycle_post2_t.o:	$(SXINCL)/sxunix.h 	\
			$(LIG)/force-make 	\
			$(SRC)/cycle_post2_t.c
	$(CC) $(SRC)/cycle_post2_t.c		\
		-DPARSACT=sxndligpost2		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid
