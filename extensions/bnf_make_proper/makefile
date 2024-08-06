
include $(SX)/etc/make/gccoptions

BMP=$(SX)/../extensions/bnf_make_proper

BIN=$(BMP)/bin.$(ARCH)
LIB=$(BMP)/lib.$(ARCH)

EXTINCL=$(SX)/../extensions/incl

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(EXTINCL) $(CFLAGS)

all: $(LIB)/make_proper_main.o

#NEEDS=	$(LIB)/make_proper_main.o

#$(BIN)/make_proper_main.out:	$(NEEDS)	$(SXEXTBIBS)
#	@rm -f $@
#	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/make_proper_main.o:	$(SXINCL)/sxunix.h		\
			make_proper_main.c
	$(CC) -o $@ -c make_proper_main.c		\
			-DLC_TABLES_H=\"dummy_lc_tables.h\"

# debut de code produit par "make-makefile make_proper"

make_proper_t.c: make_proper.bt make_proper.st make_proper.pt make_proper.rt 
	tables_c make_proper > $@

make_proper.bt  : make_proper.bnf
	semact make_proper.bnf 

make_proper.st: make_proper.bt make_proper.lecl
	lecl make_proper.lecl || [ $$? = 1 ]

make_proper.pt: make_proper.bt
	csynt make_proper || [ $$? = 1 ]

make_proper.rt: make_proper.bt make_proper.st make_proper.recor
	recor make_proper.recor

# fin de code produit par "make-makefile make_proper"

clean:
	# to be provided
