
include $(SX)/etc/make/gccoptions

LC=$(SX)/../extensions/csynt_lc
SRC=$(LC)/src
LIB=$(LC)/lib
INCL=$(LC)/incl

#CFLAGS= -O2
#CFLAGS= -g -DEBUG_ALLOC
#CFLAGS= -g -DEBUG_ALLOC -DEBUG
#CFLAGS= -g -DEBUG_ALLOC -p
CFLAGS= -g

CC=$(COMPILER) -I$(INCL) -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../extensions/incl $(CFLAGS)

all: $(SX)/bin/csynt_lc.out

$(SX)/bin/csynt_lc.out:	$(LC)/lc1.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/lc_main.o		\
	$(LIB)/LC1.o

$(LC)/lc1.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/lc_main.o:	$(INCL)/lc.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SRC)/lc_main.c
	$(CC) -o $@ -c $(SRC)/lc_main.c

$(LIB)/LC1.o:		$(INCL)/lc.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SRC)/LC1.c
	$(CC) -o $@ -c $(SRC)/LC1.c

clean:
	/bin/rm -f $(LIB)/*.o $(LC)/lc1.out

