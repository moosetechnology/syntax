
include $(SX)/etc/make/gccoptions

L=$(SX)/../outdated/oldies/lazy
LIB=$(L)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS) -DEBUG

ALL=$(L)/SL.out			\
     $(L)/DL.out			\
     $(L)/test_sxindex.out		\
     $(L)/test_sxlist.out		\
     $(L)/test_sxword.out		\
     $(L)/test_partition.out

all: $(ALL)

SL_NEEDS=$(LIB)/L_main.o		\
	 $(LIB)/L_act.o			\
	 $(LIB)/SL_t.o

DL_NEEDS=$(LIB)/L_main.o		\
	 $(LIB)/L_act.o			\
	 $(LIB)/DL_t.o

TEST=$(LIB)/test_sxlist.o	

TEST2=$(LIB)/test_sxword.o	

TEST3=$(LIB)/test_partition.o	

TEST4=$(LIB)/test_sxindex.o	

$(L)/SL.out:	$(SL_NEEDS)	$(SXBIBS)
	cc -o $@ $(SL_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/SL_t.o:	$(SXINCL)/sxunix.h 		\
			SL_t.c
	$(CC) -o $@ -c SL_t.c		\
		-Dsxparser=sxdynam_parser	\
		-DSEMACT=sxwvoid

$(L)/DL.out:	$(DL_NEEDS)	$(SXBIBS)
	cc -o $@ $(DL_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/DL_t.o:	$(SXINCL)/sxunix.h 		\
			DL_t.c
	$(CC) -o $@ -c DL_t.c		\
		-Dsxparser=sxdynam_parser	\
		-DSEMACT=sxwvoid			\

$(LIB)/L_act.o:	$(SXINCL)/sxunix.h 		\
		$(SXINCL)/sxba.h 		\
		$(SXINCL)/XxY.h 		\
		$(SXEXTINCL)/sxdynam_parser.h 	\
		L_tdef.h 		\
		L_act.c
	$(CC) -o $@ -c L_act.c

$(LIB)/L_main.o:	$(SXINCL)/sxunix.h 	\
			L_main.c
	$(CC) -o $@ -c L_main.c

$(L)/test_sxindex.out:	$(TEST4)	$(SXBIBS)
	cc -o $@ $(TEST4) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/test_sxindex.o:	$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxindex.h 	\
			$(SXINCL)/sxcell.h 	\
			test_sxindex.c
	$(CC) -o $@ -c test_sxindex.c

$(L)/test_sxlist.out:	$(TEST)	$(SXBIBS)
	cc -o $@ $(TEST) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/test_sxlist.o:	$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxalloc.h 	\
			$(SXINCL)/sxba.h 	\
			$(SXINCL)/X.h 		\
			$(SXINCL)/XxY.h 	\
			$(SXINCL)/XH.h 		\
			$(SXINCL)/XT.h 		\
			$(SXINCL)/sxlist.h 	\
			$(SXINCL)/sxSL.h 	\
			$(SXINCL)/sxsubset.h 	\
			$(SXINCL)/sxset.h 	\
			test_sxlist.c
	$(CC) -o $@ -c test_sxlist.c

$(L)/test_sxword.out:	$(TEST2)	$(SXBIBS)
	cc -o $@ $(TEST2) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/test_sxword.o:	$(SXINCL)/sxunix.h 	\
			$(SXINCL)/word.h 	\
			$(SXINCL)/varstr.h 	\
			test_sxword.c
	$(CC) -o $@ -c test_sxword.c

$(L)/test_partition.out:	$(TEST3)	$(SXBIBS)
	cc -o $@ $(TEST3) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/test_partition.o:$(SXINCL)/sxpart.h 	\
			$(SXINCL)/sxba.h 	\
			test_partition.c
	$(CC) -o $@ -c test_partition.c

clean:
	rm -f $(ALL) $(SL_NEEDS) $(DL_NEEDS)
	rm -f $(TEST) $(TEST2) $(TEST3) $(TEST4)


