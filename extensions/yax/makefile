
include $(SX)/etc/make/gccoptions

BNF=$(SX)/bnf
BNL=$(BNF)/lib.$(ARCH)

YAX=$(SX)/../extensions/yax
BIN=$(YAX)/bin.$(ARCH)
LIB=$(YAX)/lib.$(ARCH)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(BNF) $(CFLAGS)

all: $(BIN)/yax.out

NEEDS=	$(LIB)/yax_put.o	\
	$(LIB)/yax.o		\
	$(LIB)/yax_main.o	\
	$(LIB)/dec_act.o	\
	$(LIB)/dec_t.o		\
	$(LIB)/def_act.o	\
	$(LIB)/def_t.o		\
	$(BNL)/bnf_folcon.o	\
	$(BNL)/bnf_lo.o		\
	$(BNL)/bnf_protest.o	\
	$(BNL)/bnf_sact.o	\
	$(BNL)/bnf_smp.o	\
	$(BNL)/bnf_symtab.o	\
	$(BNL)/bnf_t.o

$(BIN)/yax.out:		$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@; exit 1)

$(LIB)/yax_main.o:	$(SXINCL)/sxunix.h		\
	 		$(SXINCL)/util/B_tables.h	\
			yax_vars.h	\
			$(BNF)/bnf_vars.h	\
			yax_main.c
	$(CC) -o $@ -c yax_main.c

$(LIB)/yax.o: 		$(SXINCL)/sxunix.h		\
	 		$(SXINCL)/util/B_tables.h	\
	 		$(SXINCL)/put_edit.h	\
			yax_vars.h	\
			$(BNF)/bnf_vars.h	\
			yax.c
	$(CC) -o $@ -c yax.c

$(LIB)/yax_put.o:	$(SXINCL)/sxunix.h		\
	 		$(SXINCL)/util/B_tables.h	\
			yax_vars.h	\
			$(BNF)/bnf_vars.h	\
			yax_put.c
	$(CC) -o $@ -c yax_put.c

$(LIB)/dec_act.o:	$(SXINCL)/sxunix.h		\
	 		$(SXINCL)/util/B_tables.h	\
			yax_vars.h	\
			$(BNF)/bnf_vars.h	\
			dec_act.c
	$(CC) -o $@ -c dec_act.c

$(LIB)/def_act.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/util/B_tables.h	\
			yax_vars.h	\
			$(BNF)/bnf_vars.h	\
			def_act.c
	$(CC) -o $@ -c def_act.c

$(LIB)/dec_t.o:		$(SXINCL)/sxunix.h		\
			dec_t.c
	$(CC) -o $@ -c dec_t.c	-Dsxtables=dec_tables \
		-DSCANACT=dec_scanact	\
		-DSEMACT=dec_semact

$(LIB)/def_t.o:		$(SXINCL)/sxunix.h		\
			def_t.c
	$(CC) -o $@ -c def_t.c	-Dsxtables=def_tables \
		-DSCANACT=def_scanact	\
		-DSEMACT=def_semact

# debut de code produit par "make-makefile dec"

dec_t.c: dec.bt dec.st dec.pt dec.rt 
	tables_c dec > $@

dec.bt  : dec.bnf
	semact dec.bnf 

dec.st: dec.bt dec.lecl
	lecl dec.lecl || [ $$? = 1 ]

dec.pt: dec.bt
	csynt dec || [ $$? = 1 ]

dec.rt: dec.bt dec.st dec.recor
	recor dec.recor

# fin de code produit par "make-makefile dec"

# debut de code produit par "make-makefile def"

def_t.c: def.bt def.st def.pt def.rt 
	tables_c def > $@

def.bt  : def.bnf
	semact def.bnf 

def.st: def.bt def.lecl
	lecl def.lecl || [ $$? = 1 ]

def.pt: def.bt
	csynt def || [ $$? = 1 ]

def.rt: def.bt def.st def.recor
	recor def.recor

# fin de code produit par "make-makefile def"

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/yax.out
	/bin/rm -f $(YAX)/dec.*.l $(YAX)/def.*.l
	/bin/rm -f $(YAX)/dec.bt $(YAX)/dec.pt $(YAX)/dec.rt $(YAX)/dec.st
	/bin/rm -f $(YAX)/dec_t.c
	/bin/rm -f $(YAX)/def.bt $(YAX)/def.pt $(YAX)/def.rt $(YAX)/def.st
	/bin/rm -f $(YAX)/def_t.c

