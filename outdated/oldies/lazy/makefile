
include $(SX)/etc/make/gccoptions

L=$(SX)/../outdated/oldies/lazy
LIB=$(L)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS) -DEBUG

ALL=$(L)/SL.out			\
     $(L)/DL.out

all: $(ALL)

SL_NEEDS=$(LIB)/L_main.o		\
	 $(LIB)/L_act.o			\
	 $(LIB)/SL_t.o

DL_NEEDS=$(LIB)/L_main.o		\
	 $(LIB)/L_act.o			\
	 $(LIB)/DL_t.o

$(L)/SL.out:	$(SL_NEEDS)	$(SXBIBS)
	cc -o $@ $(SL_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/SL_t.o:	$(SXINCL)/sxunix.h 		\
			SL_t.c
	$(CC) -o $@ -c SL_t.c		\
		-Dsxparser=sxdynam_parser	\
		-DSEMACT=sxwvoid

$(L)/DL.out:	$(DL_NEEDS)	$(SXBIBS)
	cc -o $@ $(DL_NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm -f $@; exit 1)

$(LIB)/DL_t.o:	$(SXINCL)/sxunix.h 		\
			DL_t.c
	$(CC) -o $@ -c DL_t.c		\
		-Dsxparser=sxdynam_parser	\
		-DSEMACT=sxwvoid			\

$(LIB)/L_act.o:	$(SXINCL)/sxunix.h 		\
		$(SXINCL)/sxba.h 		\
		$(SXINCL)/XxY.h 		\
		$(SXEXTINCL)/sxdynam_parser.h 	\
		L_tdef.h 		\
		L_act.c
	$(CC) -o $@ -c L_act.c

$(LIB)/L_main.o:	$(SXINCL)/sxunix.h 	\
			L_main.c
	$(CC) -o $@ -c L_main.c

clean:
	rm -f $(ALL) $(SL_NEEDS) $(DL_NEEDS)



