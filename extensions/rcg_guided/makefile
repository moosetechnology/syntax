
include $(SX)/etc/make/gccoptions

RCG=$(SX)/../extensions/rcg_guided

BIN=$(RCG)/bin.$(ARCH)
LIB=$(RCG)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS)

all: $(BIN)/rcg_guided.out

NEEDS= $(LIB)/RCG_guided_main.o

$(BIN)/rcg_guided.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/RCG_guided_main.o:	$(SXINCL)/sxunix.h		\
			RCG_guided_main.c
	$(CC) -o $@ -c RCG_guided_main.c

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/rcg_guided.out

