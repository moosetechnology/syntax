
include $(SX)/etc/make/gccoptions

EARLEY=$(SX)/../extensions/earley

BIN=$(EARLEY)/bin.$(ARCH)
LIB=$(EARLEY)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS)

all: $(BIN)/earley.out

NEEDS=	$(LIB)/sxearley_main.o

$(BIN)/earley.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/sxearley_main.o:	$(SXINCL)/sxunix.h		\
			sxearley_main.c
	$(CC) -o $@ -c sxearley_main.c

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/earley.out

