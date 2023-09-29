
include $(SX)/etc/make/gccoptions

MIX=$(SX)/../outdated/oldies/mix
SRC=$(MIX)/src
LIB=$(MIX)/lib

CC=cc -I$(SXINCL) $(CFLAGS)

all: $(SX)/bin/mix.out

$(SX)/bin/mix.out:	$(MIX)/mix.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/mix_main.o

$(MIX)/mix.out:	$(NEEDS)	$(SXBIBS)
	rm -f $@; cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/mix_main.o:	$(SXINCL)/sxunix.h	\
			$(SRC)/mix_main.c
	$(CC) -o $@ -c $(SRC)/mix_main.c

clean:
	/bin/rm -f $(SX)/bin/mix.out $(MIX)/mix.out $(LIB)/mix_main.o

