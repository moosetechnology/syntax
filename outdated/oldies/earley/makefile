
include $(SX)/etc/make/gccoptions

EARLEY=$(SX)/../outdated/oldies/earley
SRC=$(EARLEY)/src
LIB=$(EARLEY)/lib
INCL=$(EARLEY)/incl

CC=cc -I$(INCL) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(EARLEY)/earley.out	\
     $(EARLEY)/earley_LC_OK.out \
     $(EARLEY)/earley_LC.out

NEEDS=$(LIB)/earley.o	

NEEDS_LC=$(LIB)/earley_LC.o

N_LC_OK=$(LIB)/earley_LC_OK.o

$(EARLEY)/earley.out:	$(NEEDS)	$(SXBIBS)
	cc -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/earley.o:	$(SXINCL)/XxY.h	\
			$(SRC)/earley.c
	$(CC) -o $@ -c $(SRC)/earley.c

$(EARLEY)/earley_LC.out:	$(NEEDS_LC)	$(SXBIBS)
	cc -o $@ $(NEEDS_LC) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/earley_LC.o:	$(SXINCL)/XxY.h	\
			$(SRC)/earley_LC.c
	$(CC) -o $@ -c $(SRC)/earley_LC.c

$(EARLEY)/earley_LC_OK.out:	$(N_LC_OK)	$(SXBIBS)
	cc -o $@ $(N_LC_OK) $(CFLAGS) $(LDFLAGS) $(SXBIBS) /usr/lib/libm.a || (rm $@; exit 1)

$(LIB)/earley_LC_OK.o:	$(SXINCL)/XxY.h	\
			$(SRC)/earley_LC_OK.c
	$(CC) -o $@ -c $(SRC)/earley_LC_OK.c

clean:
	@echo "clean target for earley should be provided"

