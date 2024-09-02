
include $(SX)/etc/make/gccoptions

LFG=$(SX)/../extensions/lfg
BIN=$(LFG)/bin.$(ARCH)
LIB=$(LFG)/lib.$(ARCH)
EXTINCL=$(SX)/../extensions/incl
SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(EXTINCL) $(CFLAGS) -g

all: $(BIN)/lfg.out

###############################################################################

NEEDS=\
	$(LIB)/lfg_main.o		\
	$(LIB)/lfg_lex_scanact.o	\
	$(LIB)/lfg_smp.o		\
	$(LIB)/lfg_t.o

$(BIN)/lfg.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/lfg_smp.o:	$(SXINCL)/sxunix.h 	\
			lfg_smp.c
	$(CC) -o $@ -c lfg_smp.c

$(LIB)/lfg_main.o:	$(SXINCL)/sxunix.h 	\
			lfg_main.c
	$(CC) -o $@ -c lfg_main.c

$(LIB)/lfg_t.o:		$(SXINCL)/sxunix.h 	\
			lfg_t.c
	$(CC) -o $@ -c lfg_t.c		\
		-DSEMPASS=lfg_smp	\
		-DSCANACT=lfg_lex_scanact

$(LIB)/lfg_lex_scanact.o:		$(SXINCL)/sxunix.h 	\
			lfg_lex_scanact.c
	$(CC) -o $@ -c lfg_lex_scanact.c

# debut de code produit par "make-makefile lfg"

lfg_t.c: lfg.bt lfg.st lfg.pt lfg.rt lfg.att
	tables_c lfg > $@

lfg.bt lfg.att lfg_mix.c: lfg.at
	semat lfg.at  > lfg_mix.c

lfg.st: lfg.bt lfg.lecl
	lecl lfg.lecl || [ $$? = 1 ]

lfg.pt: lfg.bt
	csynt lfg || [ $$? = 1 ]

lfg.rt: lfg.bt lfg.st lfg.recor
	recor lfg.recor

# fin de code produit par "make-makefile lfg"

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/lfg.out
	/bin/rm -f $(LFG)/lfg.*.l
	/bin/rm -f $(LFG)/lfg.att $(LFG)/lfg.bt $(LFG)/lfg.pt \
		$(LFG)/lfg.rt $(LFG)/lfg.st $(LFG)/lfg_mix.c $(LFG)/lfg_t.c

