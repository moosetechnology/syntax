
include $(SX)/etc/make/gccoptions

MIX=$(SX)/../outdated/oldies/mix

LIB=$(MIX)/lib.$(ARCH)

CC=cc -I$(SXINCL) $(CFLAGS)

all: $(MIX)/mix.out

NEEDS=	$(LIB)/mix_main.o

$(MIX)/mix.out:	$(NEEDS)	$(SXBIBS)
	rm -f $@; cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/mix_main.o:	$(SXINCL)/sxunix.h	\
			mix_main.c
	$(CC) -o $@ -c mix_main.c

clean:
	/bin/rm -f $(MIX)/mix.out $(LIB)/mix_main.o

