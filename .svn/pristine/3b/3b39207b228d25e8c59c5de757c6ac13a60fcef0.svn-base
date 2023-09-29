
include $(SX)/etc/make/gccoptions

LIG=$(SX)/../outdated/oldies/LIG/test

SRC=$(LIG)/src
LIB=$(LIG)/lib
INCL=$(LIG)/incl

CC=cc -I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -DEBUG

all: $(LIG)/wcw.out		\
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

$(LIG)/wcw.out:	$(NEEDS)	$(SXBIBS)
	cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/wcw_t.o:		$(SXINCL)/sxunix.h 	\
			$(SRC)/wcw_t.c
	$(CC) -o $@ -c $(SRC)/wcw_t.c		\
		-DPARSACT=sxdligparsact		\
		-DSEMACT=sxvoid			\
	        -DPARSACT_2=sxvoid


$(LIG)/wcw2.out:	$(NEEDS2)	$(SXBIBS)
	cc -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/wcw2_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/wcw2_t.c
	$(CC) -o $@ -c $(SRC)/wcw2_t.c		\
		-DPARSACT=sxdligparsact		\
		-DSEMACT=sxvoid			\
	        -DPARSACT_2=sxvoid


$(LIG)/wcw3.out:	$(NEEDS3)	$(SXBIBS)
	cc -o $@ $(NEEDS3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/wcw3_parsact.o:	$(SXINCL)/sxunix.h	\
			$(INCL)/wcw3_tdef.h 	\
			$(SRC)/wcw3_parsact.c
	$(CC) -o $@ -c $(SRC)/wcw3_parsact.c

$(LIB)/wcw3_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/wcw3_t.c
	$(CC) -o $@ -c $(SRC)/wcw3_t.c		\
		-DPARSACT=sxdligparsact		\
		-DSEMACT=sxvoid			\
	        -DPARSACT_2=wcw3_parsact


$(LIG)/ndwcw3.out:	$(NDNEEDS3)	$(SXBIBS)
	cc -o $@ $(NDNEEDS3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/ndwcw3_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/ndwcw3_t.c
	$(CC) -o $@ -c $(SRC)/ndwcw3_t.c	\
		-DPARSACT=sxndligparsact	\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid


$(LIG)/ndwcw3_post2.out:	$(NDNEEDS3_POST2)	$(SXBIBS)
	cc -o $@ $(NDNEEDS3_POST2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/ndwcw3_post2_t.o: $(SXINCL)/sxunix.h 	\
			$(SRC)/ndwcw3_post2_t.c
	$(CC) -o $@ -c $(SRC)/ndwcw3_post2_t.c	\
		-DPARSACT=sxndligpost2		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid


$(LIG)/cycle.out:	$(CYCLE)	$(SXBIBS)
	cc -o $@ $(CYCLE) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/cycle_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/cycle_t.c
	$(CC) -o $@ -c $(SRC)/cycle_t.c		\
		-DPARSACT=sxndligparsact	\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(LIG)/cycle_mixt.out:	$(CYCLE_MIXT)	$(SXBIBS)
	cc -o $@ $(CYCLE_MIXT) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/cycle_mixt_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/cycle_mixt_t.c
	$(CC) -o $@ -c $(SRC)/cycle_mixt_t.c	\
		-DPARSACT=sxndligmixt		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(LIG)/cycle_bylev.out:	$(CYCLE_BYLEV)	$(SXBIBS)
	cc -o $@ $(CYCLE_BYLEV) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/cycle_bylev_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/cycle_bylev_t.c
	$(CC) -o $@ -c $(SRC)/cycle_bylev_t.c	\
		-DPARSACT=sxndligbylev		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(LIG)/cycle_post.out:	$(CYCLE_POST)	$(SXBIBS)
	cc -o $@ $(CYCLE_POST) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/cycle_post_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/cycle_post_t.c
	$(CC) -o $@ -c $(SRC)/cycle_post_t.c	\
		-DPARSACT=sxndligpost		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

$(LIG)/cycle_post2.out:	$(CYCLE_POST2)	$(SXBIBS)
	cc -o $@ $(CYCLE_POST2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/cycle_post2_t.o:	$(SXINCL)/sxunix.h 	\
			$(SRC)/cycle_post2_t.c
	$(CC) -o $@ -c $(SRC)/cycle_post2_t.c	\
		-DPARSACT=sxndligpost2		\
		-DSEMACT=sxvoid			\
		-DESAMBIG=sxvoid		\
	        -DPARSACT_2=sxvoid

clean:
	@echo "clean target for LIG/bnf should be provided"

