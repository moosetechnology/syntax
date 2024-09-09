
include $(SX)/etc/make/gccoptions

TAB=$(SX)/semc
LIB=$(TAB)/lib.$(ARCH)
BOOT=$(TAB)/boot

CC=$(COMPILER) -I$(TAB) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS)

all: $(SXBIN)/semc.out

NEEDS= \
 $(LIB)/semc_put.o	$(LIB)/semc.o		$(LIB)/semc_main.o	\
 $(LIB)/dec.o		$(LIB)/def.o		$(LIB)/def_sact.o	\
 $(LIB)/dec_t.o		$(LIB)/def_t.o		$(LIB)/semc_lo.o

$(SXBIN)/semc.out:	$(NEEDS) $(SXBIBS) $(SXLIB)/libsxbnf.a
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXLIB)/libsxbnf.a $(SXBIBS) || (rm $@;exit 1)

$(LIB)/semc_main.o:		\
		semc_main.c 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		semc_vars.h	\
		$(SXINCL)/util/bnf_vars.h
	$(CC) -o $@ -c semc_main.c

$(LIB)/semc_lo.o:		\
		semc_lo.c 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		semc_vars.h	\
		$(SXINCL)/util/bnf_vars.h
	$(CC) -o $@ -c semc_lo.c

$(LIB)/semc.o: \
		semc.c		\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		semc_vars.h	\
		$(SXINCL)/util/bnf_vars.h
	$(CC) -o $@ -c semc.c

$(LIB)/semc_put.o:	\
		semc_put.c 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		semc_vars.h	\
		$(SXINCL)/util/bnf_vars.h
	$(CC) -o $@ -c semc_put.c

$(LIB)/dec.o:		\
		$(BOOT)/dec.c	 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		semc_vars.h	\
		$(SXINCL)/util/bnf_vars.h
	$(CC) -o $@ -c $(BOOT)/dec.c

$(LIB)/def.o:		\
		$(BOOT)/def.c	 	\
		$(SXINCL)/sxunix.h		\
		$(SXINCL)/util/B_tables.h	\
		semc_vars.h	\
		$(SXINCL)/util/bnf_vars.h
	$(CC) -o $@ -c $(BOOT)/def.c

$(LIB)/def_sact.o:	\
		def_sact.c 	\
		$(SXINCL)/sxunix.h
	$(CC) -o $@ -c def_sact.c

$(LIB)/dec_t.o:	\
		$(BOOT)/dec_t.c	\
		$(SXINCL)/sxunix.h
	$(CC) -o $@ -c $(BOOT)/dec_t.c	\
		-DSEMACT=dec_act

$(LIB)/def_t.o:		\
		$(BOOT)/def_t.c		\
		$(SXINCL)/sxunix.h
	$(CC) -o $@ -c $(BOOT)/def_t.c	\
		-DSEMACT=def_act

# debut de code produit par "make-makefile dec"

bootstrap: dec_t.c dec.c boostrap2

dec_t.c: dec.bt dec.st dec.pt dec.rt 
	tables_c -name dec_tables dec > $@

dec.bt  dec.c: dec.semc
	semc dec.semc > dec.c

dec.st: dec.bt dec.lecl
	lecl dec.lecl || [ $$? = 1 ]

dec.pt: dec.bt
	csynt dec || [ $$? = 1 ]

dec.rt: dec.bt dec.st dec.recor
	recor dec.recor

# fin de code produit par "make-makefile dec"

# debut de code produit par "make-makefile def"

boostrap2: def_t.c def.c 

def_t.c: def.bt def.st def.pt def.rt 
	tables_c -name def_tables def > $@

def.bt  def.c: def.semc
	semc def.semc > def.c

def.st: def.bt def.lecl
	lecl def.lecl || [ $$? = 1 ]

def.pt: def.bt
	csynt def || [ $$? = 1 ]

def.rt: def.bt def.st def.recor
	recor def.recor

# fin de code produit par "make-makefile def"

clean:
	/bin/rm -f $(LIB)/*.o
	/bin/rm -f dec.c dec_t.c
	/bin/rm -f dec.bt dec.pt dec.rt dec.st 
# pas de dec.dt dec_td.h 
	/bin/rm -f dec.bn.l dec.lc.l dec.pr.l dec.rc.l 
# pas de dec.la.l dec.td.l 
	/bin/rm -f def.c def_t.c
	/bin/rm -f def.bt def.pt def.rt def.st 
# pas de dec.dt dec_td.h 
	/bin/rm -f def.bn.l def.lc.l def.pr.l def.rc.l 
# pas de def.la.l def.td.l 

