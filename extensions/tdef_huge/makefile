
include $(SX)/etc/make/gccoptions

TDEF_HUGE=$(SX)/../extensions/tdef_huge

BIN=$(TDEF_HUGE)/bin.$(ARCH)
LIB=$(TDEF_HUGE)/lib.$(ARCH)

EXTINCL=$(SX)/../extensions/incl

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(EXTINCL) $(CFLAGS)

all: $(BIN)/tdef_huge.out

NEEDS=	$(LIB)/make_tdef_main.o

$(BIN)/tdef_huge.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/make_tdef_main.o: $(SXINCL)/sxunix.h make_tdef_main.c
	$(CC) -o $@ -c -DLC_TABLES_H=\"dummy_lc_tables.h\" make_tdef_main.c

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/tdef_huge.out

