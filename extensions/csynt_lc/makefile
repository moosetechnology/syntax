
include $(SX)/etc/make/gccoptions

LC=$(SX)/../extensions/csynt_lc
BIN=$(LC)/bin.$(ARCH)
LIB=$(LC)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(LC) -I$(SXINCL) -I$(SXINCL)/util -I$(SX)/../extensions/incl $(CFLAGS)

# all: $(BIN)/csynt_lc.out
all: $(BIN)/lc1.out

NEEDS=	$(LIB)/lc_main.o		\
	$(LIB)/LC1.o

$(BIN)/lc1.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

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
	/bin/rm -f $(LIB)/*.o $(BIN)/lc1.out

