
include $(SX)/etc/make/gccoptions

DAG=$(SX)/../extensions/DAG

BIN=$(DAG)/bin.$(ARCH)
LIB=$(DAG)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS)

all: text_t.c

# debut de code produit par "make-makefile text -dummy_csynt"

text_t.c: text.bt text.st text.pt text.rt 
	tables_c text > $@

text.bt  : text.bnf
	semact text.bnf 

text.st: text.bt text.lecl
	lecl text.lecl || [ $$? = 1 ]

text.pt: text.bt
	sh -x $(SXEXTCOM)/dummy_csynt text || [ $$? = 1 ]

text.rt: text.bt text.st text.recor
	recor text.recor

# fin de code produit par "make-makefile text -dummy_csynt"

clean:
	/bin/rm -f $(LIB)/*.o
	/bin/rm -f text_t.c

