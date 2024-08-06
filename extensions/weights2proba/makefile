
include $(SX)/etc/make/gccoptions

W2P=$(SX)/../extensions/weights2proba

BIN=$(W2P)/bin.$(ARCH)
LIB=$(W2P)/lib.$(ARCH)

EXTINCL=$(SX)/../extensions/incl

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(EXTINCL) $(CFLAGS)

all: $(BIN)/weights2proba.out

NEEDS=	$(LIB)/weights2proba.o

$(BIN)/weights2proba.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) -lm || (rm $@;exit 1)
	# l'option -lm sert pour log10()

$(LIB)/weights2proba.o:	$(SXINCL)/sxunix.h weights2proba.c
	$(CC) -o $@ -c -DLC_TABLES_H=\"dummy_lc_tables.h\" weights2proba.c

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/weights2proba.out

