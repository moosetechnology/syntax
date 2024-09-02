
include $(SX)/etc/make/gccoptions

TRICO=$(SX)/trico
BIN=$(TRICO)/bin.$(ARCH)
LIB=$(TRICO)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

ALL=$(BIN)/trico.out	

all: $(ALL)

NEEDED=$(LIB)/trico_act.o	\
       $(LIB)/trico_main.o	\
       $(LIB)/trico_t.o

$(BIN)/trico.out:	$(NEEDED)	$(SXBIBS)
	rm -f $@; cc -o $@ `echo $(NEEDED) $(CFLAGS) $(LDFLAGS) $(SXBIBS)` || (rm $@;exit 1)

$(LIB)/trico_act.o:	$(SXINCL)/sxunix.h	\
			trico_env.h	\
			trico_act.c	
	$(CC) -o $@ -c trico_act.c

$(LIB)/trico_main.o:	$(SXINCL)/sxunix.h	\
			trico_env.h	\
			trico_main.c
	$(CC) -o $@ -c trico_main.c

$(LIB)/trico_t.o:	$(SXINCL)/sxunix.h 	\
			trico_t.c
	$(CC) -o $@ -c trico_t.c			\
		-DSCANACT=trico_scan_act	\
		-DSEMACT=trico_sem_act

# debut de code produit par "make-makefile trico"

trico_t.c: trico.bt trico.st trico.pt trico.rt 
	tables_c trico > $@

trico.bt  : trico.bnf
	semact trico.bnf 

trico.st: trico.bt trico.lecl
	lecl trico.lecl || [ $$? = 1 ]

trico.pt: trico.bt
	csynt trico || [ $$? = 1 ]

trico.rt: trico.bt trico.st trico.recor
	recor trico.recor

# fin de code produit par "make-makefile trico"

clean:
	rm -f $(ALL) $(NEEDED)
	rm -f trico_t.c trico.bt trico.st trico.pt trico.rt

