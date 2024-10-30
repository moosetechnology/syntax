
include $(SX)/etc/make/gccoptions

IND=$(SX)/../outdated/oldies/incr

INDLIB=$(IND)/lib.$(ARCH)

CC=cc -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(INDLIB)/sxindparser.o		\
     $(INDLIB)/LR0incr.o

$(INDLIB)/sxindparser.o:	$(SXINCL)/sxunix.h		\
				$(SXINCL)/sxba.h		\
				$(SXINCL)/XxY.h		\
				$(SXINCL)/XH.h			\
				$(SXINCL)/SS.h			\
				$(SXINCL)/util/B_tables.h	\
				sxindparser.h	\
				sxindparser.c
	$(CC) -o $@ -c sxindparser.c

$(INDLIB)/LR0incr.o:		$(SXINCL)/sxunix.h		\
				$(SXINCL)/sxba.h		\
				$(SXINCL)/XxY.h		\
				$(SXINCL)/XH.h			\
				$(SXINCL)/SS.h			\
				sxindparser.h	\
				LR0incr.c
	$(CC) -o $@ -c LR0incr.c

clean:
	/bin/rm -f $(INDLIB)/sxindparser.o $(INDLIB)/LR0incr.o
