
include $(SX)/etc/make/gccoptions

IND=$(SX)/../outdated/oldies/incr

INDSRC=$(IND)/src
INDLIB=$(IND)/lib
INDINCL=$(IND)/incl

CC=cc -I$(INDINCL) -I$(SX)/incl -$(CFLAGS)

all: $(INDLIB)/sxindparser.o		\
     $(INDLIB)/LR0.o

$(INDLIB)/sxindparser.o:	$(SX)/incl/sxunix.h		\
				$(SX)/incl/sxba.h		\
				$(SX)/incl/XxY.h		\
				$(SX)/incl/XH.h			\
				$(SX)/incl/SS.h			\
				$(INDINCL)/sxindparser.h	\
				$(INDSRC)/sxindparser.c
	$(CC) -o $@ -c $(INDSRC)/sxindparser.c

$(INDLIB)/LR0.o:		$(SX)/incl/sxunix.h		\
				$(SX)/incl/sxba.h		\
				$(SX)/incl/XxY.h		\
				$(SX)/incl/XH.h			\
				$(SX)/incl/SS.h			\
				$(INDINCL)/sxindparser.h	\
				$(INDSRC)/LR0.c
	$(CC) -o $@ -c $(INDSRC)/LR0.c

