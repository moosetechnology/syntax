
include $(SX)/etc/make/gccoptions

TAG=$(SX)/../outdated/oldies/TAG/hard
SRC=$(TAG)
LIB=$(TAG)
INCL=$(TAG)

CC=cc -I$(INCL) -I$(SXINCL) $(CFLAGS) -DEBUG

all: $(TAG)/hard.out			\
     $(TAG)/hard_mixt.out		\
     $(TAG)/hard_bylev.out		\
     $(TAG)/hard_post.out		\
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

$(TAG)/hard.out:	$(NEEDS)	$(SXBIBS)
	cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/hard_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/hard_t.c
	$(CC) -o $@ -c $(SRC)/hard_t.c		\
		-DPARSACT=sxndligparsact	\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/hard_post.out:	$(NEEDS2)	$(SXBIBS)
	cc -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/hard_post_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/hard_post_t.c
	$(CC) -o $@ -c $(SRC)/hard_post_t.c	\
		-DPARSACT=sxndligpost		\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/hard_mixt.out:	$(NEEDS3)	$(SXBIBS)
	cc -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/hard_mixt_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/hard_mixt_t.c
	$(CC) -o $@ -c $(SRC)/hard_mixt_t.c	\
		-DPARSACT=sxndligmixt		\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/hard_bylev.out:	$(NEEDS4)	$(SXBIBS)
	cc -o $@ $(NEEDS4) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/hard_bylev_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/hard_bylev_t.c
	$(CC) -o $@ -c $(SRC)/hard_bylev_t.c	\
		-DPARSACT=sxndligbylev		\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(TAG)/hard_post2.out:	$(NEEDS5)	$(SXBIBS)
	cc -o $@ $(NEEDS5) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/hard_post2_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/hard_post2_t.c
	$(CC) -o $@ -c $(SRC)/hard_post2_t.c	\
		-DPARSACT=sxndligpost2		\
		-DSEMACT=sxwvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

