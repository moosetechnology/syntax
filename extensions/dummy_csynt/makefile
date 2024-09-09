
include $(SX)/etc/make/gccoptions

DUMMY_CSYNT=$(SX)/../extensions/dummy_csynt

LIB=$(DUMMY_CSYNT)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXEXTBIN)/dummy_csynt.out

NEEDS=	$(LIB)/dummy_csynt.o

$(SXEXTBIN)/dummy_csynt.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/dummy_csynt.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			dummy_csynt.c
	$(CC) -o $@ -c dummy_csynt.c

clean:
	/bin/rm -f $(LIB)/*.o $(SXEXTBIN)/dummy_csynt.out
