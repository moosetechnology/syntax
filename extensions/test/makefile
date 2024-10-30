
include $(SX)/etc/make/gccoptions

TEST=$(SX)/../extensions/test

BIN=$(TEST)/bin.$(ARCH)

LIB=$(TEST)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=cc -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS)

ALL=$(BIN)/primes_nb.out 	\
    $(BIN)/test_sxlist.out	\
    $(BIN)/test_sxindex.out	\
    $(BIN)/test_sxword.out	\
    $(BIN)/test_partition.out

all: $(ALL)

NEEDS1= $(LIB)/primes_nb.o

$(BIN)/primes_nb.out:	$(NEEDS1)	$(SXBIBS)
	rm -f $@; cc -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/primes_nb.o:	$(SXINCL)/sxunix.h		\
			primes_nb.c
	$(CC) -o $@ -c primes_nb.c

TEST1=$(LIB)/test_sxlist.o

$(BIN)/test_sxlist.out:	$(TEST1)	$(SXEXTBIBS)
	cc -o $@ $(TEST1) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@; exit 1)

$(LIB)/test_sxlist.o:	$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxalloc.h 	\
			$(SXINCL)/sxba.h 	\
			$(SXINCL)/X.h 		\
			$(SXINCL)/XxY.h 	\
			$(SXINCL)/XH.h 		\
			$(SXEXTINCL)/XT.h 		\
			$(SXEXTINCL)/sxlist.h 	\
			$(SXEXTINCL)/sxSL.h 	\
			$(SXEXTINCL)/sxsubset.h 	\
			$(SXEXTINCL)/sxset.h 	\
			test_sxlist.c
	$(CC) -o $@ -c test_sxlist.c

TEST2=$(LIB)/test_sxword.o	

$(BIN)/test_sxword.out:	$(TEST2)	$(SXEXTBIBS)
	cc -o $@ $(TEST2) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@; exit 1)

$(LIB)/test_sxword.o:	$(SXINCL)/sxunix.h 	\
			$(SXEXTINCL)/sxword.h 	\
			$(SXINCL)/varstr.h 	\
			test_sxword.c
	$(CC) -o $@ -c test_sxword.c


TEST3=$(LIB)/test_partition.o	

$(BIN)/test_partition.out:	$(TEST3)	$(SXEXTBIBS)
	cc -o $@ $(TEST3) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm -f $@; exit 1)

$(LIB)/test_partition.o:$(SXEXTINCL)/sxpart.h 	\
			$(SXINCL)/sxba.h 	\
			test_partition.c
	$(CC) -o $@ -c test_partition.c

TEST4=$(LIB)/test_sxindex.o	

$(BIN)/test_sxindex.out:	$(TEST4)	$(SXBIBS)
	cc -o $@ $(TEST4) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/test_sxindex.o:	$(SXINCL)/sxunix.h 	\
			$(SXINCL)/sxindex.h 	\
			$(TEST)/sxcell.h 	\
			test_sxindex.c
	$(CC) -o $@ -c test_sxindex.c

clean:
	rm -f $(ALL) $(NEEDS1)
	rm -f $(TEST1) $(TEST2) $(TEST3) $(TEST4)

