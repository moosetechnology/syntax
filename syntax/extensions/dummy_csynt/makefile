
include $(SX)/etc/make/gccoptions

DUMMY_CSYNT=$(SX)/../extensions/dummy_csynt
SRC=$(DUMMY_CSYNT)/src
LIB=$(DUMMY_CSYNT)/lib

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SX)/bin/dummy_csynt.out

$(SX)/bin/dummy_csynt.out:	$(DUMMY_CSYNT)/dummy_csynt.out
	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

NEEDS=	$(LIB)/dummy_csynt.o

$(DUMMY_CSYNT)/dummy_csynt.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/dummy_csynt.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/B_tables.h		\
			$(SXINCL)/util/P_tables.h		\
			$(SRC)/dummy_csynt.c
	$(CC) -o $@ -c $(SRC)/dummy_csynt.c

clean:
	/bin/rm -f $(LIB)/*.o $(DUMMY_CSYNT)/dummy_csynt.out
