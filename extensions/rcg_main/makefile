
include $(SX)/etc/make/gccoptions

RCG=$(SX)/../extensions/rcg_main

BIN=$(RCG)/bin.$(ARCH)
LIB=$(RCG)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS)

all: $(BIN)/RCG.out

NEEDS= $(LIB)/RCG_main.o

$(BIN)/RCG.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/RCG_main.o:	$(SXINCL)/sxunix.h		\
			RCG_main.c
	$(CC) -o $@ -c RCG_main.c

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/RCG.out

