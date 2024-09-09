
include $(SX)/etc/make/gccoptions

DAG=$(SX)/../extensions/dag2udag

BIN=$(DAG)/bin.$(ARCH)
LIB=$(DAG)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS)

all: $(BIN)/dag2udag.out

NEEDS=	$(LIB)/dag2udag_main.o

$(BIN)/dag2udag.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/dag2udag_main.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxstack.h		\
			$(SXINCL)/varstr.h		\
			$(SXINCL)/fsa.h			\
			$(SXINCL)/XxYxZ.h		\
			$(SXINCL)/XxY.h			\
			dag2udag_main.c
	$(CC) -o $@ -c dag2udag_main.c

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/dag2udag.out

