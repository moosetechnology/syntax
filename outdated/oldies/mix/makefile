
include $(SX)/etc/make/gccoptions

MIX=$(SX)/../outdated/oldies/mix
SRC=$(MIX)/src
BIN=$(MIX)/bin.$(ARCH)
LIB=$(MIX)/lib.$(ARCH)

CC=cc -I$(SXINCL) $(CFLAGS)

all: $(BIN)/mix.out

NEEDS=	$(LIB)/mix_main.o

$(BIN)/mix.out:	$(NEEDS)	$(SXBIBS)
	rm -f $@; cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/mix_main.o:	$(SXINCL)/sxunix.h	\
			$(SRC)/mix_main.c
	$(CC) -o $@ -c $(SRC)/mix_main.c

clean:
	/bin/rm -f $(BIN)/mix.out $(LIB)/mix_main.o

