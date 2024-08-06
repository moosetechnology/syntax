
include $(SX)/etc/make/gccoptions

PPTDEF=$(SX)/examples/pptdef
BIN=$(PPTDEF)/bin.$(ARCH)
LIB=$(PPTDEF)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -c


all:	$(BIN)/pptdef.out

NEEDS=	$(LIB)/pptdef_t.o		\
	$(LIB)/pptdef_main.o


$(BIN)/pptdef.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/pptdef_t.o: 	$(SXINCL)/sxunix.h		\
			pptdef_t.c
	$(CC) -o $@ pptdef_t.c		\
			-Dsxtables=pptdef_tables

$(LIB)/pptdef_main.o:	$(SXINCL)/sxunix.h	\
			pptdef_main.c
	$(CC) -o $@ pptdef_main.c

# debut de code produit par "make-makefile pptdef"

pptdef_t.c: pptdef.bt pptdef.st pptdef.pt pptdef.rt pptdef.ppt
	tables_c pptdef > $@

pptdef.bt pptdef.ppt : pptdef.paradis
	paradis pptdef.paradis 

pptdef.st: pptdef.bt pptdef.lecl
	lecl pptdef.lecl || [ $$? = 1 ]

pptdef.pt: pptdef.bt
	csynt pptdef || [ $$? = 1 ]

pptdef.rt: pptdef.bt pptdef.st pptdef.recor
	recor pptdef.recor

# fin de code produit par "make-makefile pptdef"

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/pptdef.out
	rm -f pptdef.bt pptdef.ppt pptdef.pt pptdef.rt pptdef.st pptdef_t.c 
	rm -f pptdef.bn.l pptdef.lc.l pptdef.rc.l


