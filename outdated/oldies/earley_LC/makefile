
include $(SX)/etc/make/gccoptions

EARLEY=$(SX)/../outdated/oldies/earley_LC
LIB=$(EARLEY)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -DEBUG

ALL=$(EARLEY)/earley.out	\
     $(EARLEY)/earley_LC_OK.out \
     $(EARLEY)/earley_LC.out

all: $(ALL)

NEEDS=$(LIB)/earley.o	

NEEDS_LC=$(LIB)/earley_LC.o

N_LC_OK=$(LIB)/earley_LC_OK.o

$(EARLEY)/earley.out:	$(NEEDS)	$(SXBIBS)
	cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/earley.o:	$(SXINCL)/XxY.h	\
			earley.c
	$(CC) -o $@ -c earley.c

$(EARLEY)/earley_LC.out:	$(NEEDS_LC)	$(SXBIBS)
	cc -o $@ $(NEEDS_LC) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/earley_LC.o:	$(SXINCL)/XxY.h	\
			earley_LC.c
	$(CC) -o $@ -c earley_LC.c

$(EARLEY)/earley_LC_OK.out:	$(N_LC_OK)	$(SXBIBS)
	cc -o $@ $(N_LC_OK) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/earley_LC_OK.o:	$(SXINCL)/XxY.h	\
			earley_LC_OK.c
	$(CC) -o $@ -c earley_LC_OK.c

clean:
	rm -f $(ALL) $(NEEDS) $(NEEDS1)

