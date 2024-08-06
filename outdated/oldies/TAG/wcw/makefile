
include $(SX)/etc/make/gccoptions

TAG=$(SX)/../outdated/oldies/TAG/wcw
SRC=$(TAG)
LIB=$(TAG)
INCL=$(TAG)

CC=cc -I$(INCL) -I$(SXINCL) $(CFLAGS) -DEBUG

all: $(TAG)/wcw.out		\
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

$(TAG)/wcw.out:	$(NEEDS)	$(SXBIBS)
	cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/wcw_t.o:		$(SXINCL)/sxunix.h 	\
			$(SRC)/wcw_t.c
	$(CC) -o $@ -c $(SRC)/wcw_t.c		\
		-DPARSACT=sxndligparsact	\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/wcw_post.out:	$(NEEDS2)	$(SXBIBS)
	cc -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/wcw_post_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/wcw_post_t.c
	$(CC) -o $@ -c $(SRC)/wcw_post_t.c	\
		-DPARSACT=sxndligpost		\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/wcw_mixt.out:	$(NEEDS3)	$(SXBIBS)
	cc -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/wcw_mixt_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/wcw_mixt_t.c
	$(CC) -o $@ -c $(SRC)/wcw_mixt_t.c	\
		-DPARSACT=sxndligmixt		\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/wcw_bylev.out:	$(NEEDS4)	$(SXBIBS)
	cc -o $@ $(NEEDS4) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/wcw_bylev_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/wcw_bylev_t.c
	$(CC) -o $@ -c $(SRC)/wcw_bylev_t.c	\
		-DPARSACT=sxndligbylev		\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/wcw_post2.out:	$(NEEDS5)	$(SXBIBS)
	cc -o $@ $(NEEDS5) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/wcw_post2_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/wcw_post2_t.c
	$(CC) -o $@ -c $(SRC)/wcw_post2_t.c	\
		-DPARSACT=sxndligpost2		\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

