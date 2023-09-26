# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(HOME)/syntax/5.0/sun4
SXLIB=$(SX)/lib
SXINCL=$(SX)/incl
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib
ETCINCL=$(ETC)/incl

TAG=$(HOME)/syntax/5.0/sun4/TAG/hard
SRC=$(TAG)
LIB=$(TAG)
INCL=$(TAG)

BIBS= $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a

CC=cd $(LIB); umask 7; exec cc -I$(INCL) -I$(SXINCL) -I$(ETC)/incl $(CFLAGS) -DEBUG -c

$(TAG)/everything .PRECIOUS:	$(TAG)/hard.out		\
				$(TAG)/hard_mixt.out	\
				$(TAG)/hard_bylev.out	\
				$(TAG)/hard_post.out	\
				$(TAG)/hard_post2.out


NEEDS=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/hard_t.o

NEEDS2=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/hard_post_t.o

NEEDS3=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/hard_mixt_t.o

NEEDS4=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/hard_bylev_t.o

NEEDS5=					\
	$(SXLIB)/sxmain.o		\
	$(LIB)/hard_post2_t.o

$(TAG)/hard.out:	$(NEEDS)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/hard_t.o:		$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/hard_t.c
	$(CC) $(SRC)/hard_t.c			\
		-DPARSACT=sxndligparsact	\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/hard_post.out:	$(NEEDS2)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/hard_post_t.o:	$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/hard_post_t.c
	$(CC) $(SRC)/hard_post_t.c		\
		-DPARSACT=sxndligpost		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/hard_mixt.out:	$(NEEDS3)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/hard_mixt_t.o:	$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/hard_mixt_t.c
	$(CC) $(SRC)/hard_mixt_t.c		\
		-DPARSACT=sxndligmixt		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/hard_bylev.out:	$(NEEDS4)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS4) $(CFLAGS) $(LDFLAGS) $(ETCLIB)/libsxetc5.0.a $(SXLIB)/libsx5.0.a || (rm $@;exit 1)'

$(LIB)/hard_bylev_t.o:	$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/hard_bylev_t.c
	$(CC) $(SRC)/hard_bylev_t.c		\
		-DPARSACT=sxndligbylev		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/hard_post2.out:	$(NEEDS5)	$(BIBS)
	@echo '	cc -o $@'; ${SHELL} -c 'cc -o $@ $(NEEDS5) $(CFLAGS) $(LDFLAGS) $(SXLIB)/libsx5.0.a $(ETCLIB)/libsxetc5.0.a || (rm $@;exit 1)'

$(LIB)/hard_post2_t.o:	$(SXINCL)/sxunix.h 	\
			$(TAG)/force-make 	\
			$(SRC)/hard_post2_t.c
	$(CC) $(SRC)/hard_post2_t.c		\
		-DPARSACT=sxndligpost2		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid
