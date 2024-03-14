
include $(SX)/etc/make/gccoptions

LC=$(SX)/../extensions/csynt_lc
LIB=$(LC)/lib

#CFLAGS= -O2
#CFLAGS= -g -DEBUG_ALLOC
#CFLAGS= -g -DEBUG_ALLOC -DEBUG
#CFLAGS= -g -DEBUG_ALLOC -p
CFLAGS= -g

CC=$(COMPILER) -I$(LC) -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../extensions/incl $(CFLAGS)

all: $(SXBIN)/csynt_lc.out

$(SXBIN)/csynt_lc.out:	$(LC)/lc1.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/lc_main.o		\
	$(LIB)/LC1.o

$(LC)/lc1.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/lc_main.o:	lc.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h		\
			lc_main.c
	$(CC) -o $@ -c lc_main.c

$(LIB)/LC1.o:		lc.h			\
			$(SXINCL)/sxunix.h		\
			$(SXINCL)/sxba.h		\
			$(SXINCL)/util/bstr.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			LC1.c
	$(CC) -o $@ -c LC1.c

clean:
	/bin/rm -f $(LIB)/*.o $(LC)/lc1.out

