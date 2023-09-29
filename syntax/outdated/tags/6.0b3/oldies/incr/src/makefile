# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
SXLIB=$(SX)/lib
ETC=$(SX)/etc
ETCLIB=$(ETC)/lib

IND=/proj2/syntax/5.0/sun4/incr
INDSRC=$(IND)/src
INDLIB=$(IND)/lib
INDINCL=$(IND)/incl

INDETC=$(IND)/etc
INDETCLIB=$(INDETC)/lib

BIBS= $(ETCLIB)/libsxetc$(sxV).a $(SXLIB)/libsx$(sxV).a

CC=cd $(INDLIB); umask 7; exec cc -I$(INDINCL) -I$(INDETC)/incl -I$(SX)/incl -I$(ETC)/incl $(CFLAGS) -c

$(INDLIB)/ALL .PRECIOUS:	$(INDLIB)/sxindparser.o	\
                                $(INDLIB)/LR0.o

$(INDLIB)/sxindparser.o:	$(SX)/incl/sxunix.h		\
				$(SX)/incl/sxba.h		\
				$(ETC)/incl/XxY.h		\
				$(ETC)/incl/XH.h		\
				$(ETC)/incl/SS.h		\
				$(INDINCL)/sxindparser.h	\
				$(INDSRC)/sxindparser.c
	$(CC) $(INDSRC)/sxindparser.c

$(INDLIB)/LR0.o:		$(SX)/incl/sxunix.h		\
				$(SX)/incl/sxba.h		\
				$(ETC)/incl/XxY.h		\
				$(ETC)/incl/XH.h		\
				$(ETC)/incl/SS.h		\
				$(INDINCL)/sxindparser.h	\
				$(INDSRC)/LR0.c
	$(CC) $(INDSRC)/LR0.c

