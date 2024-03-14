
include $(SX)/etc/make/gccoptions

DUMMY_CSYNT=$(SX)/../extensions/dummy_csynt
LIB=$(DUMMY_CSYNT)/lib

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXBIN)/dummy_csynt.out

$(SXBIN)/dummy_csynt.out:	$(DUMMY_CSYNT)/dummy_csynt.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/dummy_csynt.o

$(DUMMY_CSYNT)/dummy_csynt.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/dummy_csynt.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			dummy_csynt.c
	$(CC) -o $@ -c dummy_csynt.c

clean:
	/bin/rm -f $(LIB)/*.o $(DUMMY_CSYNT)/dummy_csynt.out
