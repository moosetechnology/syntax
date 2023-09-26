# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(HOME)/syntax/5.0/sun4
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
ETCINCL=$(ETC)/incl

TAG=$(HOME)/syntax/5.0/sun4/TAG/wcw
SRC=$(TAG)
LIB=$(TAG)
INCL=$(TAG)

BIBS= $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a

CC=cd $(LIB); umask 7; exec cc -I$(INCL) -I$(SXINCL) -I$(ETC)/incl $(CFLAGS) -DEBUG -c

$(TAG)/everything .PRECIOUS:	$(TAG)/wcw.out		\
				$(TAG)/wcw_mixt.out	\
				$(TAG)/wcw_bylev.out	\
				$(TAG)/wcw_post.out	\
				$(TAG)/wcw_post2.out


NEEDS=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/wcw_t.o

NEEDS2=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/wcw_post_t.o

NEEDS3=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/wcw_mixt_t.o

NEEDS4=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/wcw_bylev_t.o

NEEDS5=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/wcw_post2_t.o

$(TAG)/wcw.out:	$(NEEDS)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/wcw_t.o:		$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/wcw_t.c
	$(CC) $(SRC)/wcw_t.c			\
		-DPARSACT=sxndligparsact	\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/wcw_post.out:	$(NEEDS2)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/wcw_post_t.o:	$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/wcw_post_t.c
	$(CC) $(SRC)/wcw_post_t.c		\
		-DPARSACT=sxndligpost		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/wcw_mixt.out:	$(NEEDS3)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/wcw_mixt_t.o:	$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/wcw_mixt_t.c
	$(CC) $(SRC)/wcw_mixt_t.c		\
		-DPARSACT=sxndligmixt		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/wcw_bylev.out:	$(NEEDS4)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS4) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/wcw_bylev_t.o:	$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/wcw_bylev_t.c
	$(CC) $(SRC)/wcw_bylev_t.c		\
		-DPARSACT=sxndligbylev		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/wcw_post2.out:	$(NEEDS5)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS5) $(CFLAGS) $(LDFLAGS) $(SXLIB)/libsx5.0.a $(ETCLIB)/libsxetc5.0.a || (rm $@;exit 1)'

$(LIB)/wcw_post2_t.o:	$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/wcw_post2_t.c
	$(CC) $(SRC)/wcw_post2_t.c		\
		-DPARSACT=sxndligpost2		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid
