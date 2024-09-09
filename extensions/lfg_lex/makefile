
include $(SX)/etc/make/gccoptions

LFGLEX=$(SX)/../extensions/lfg_lex

LIB=$(LFGLEX)/lib.$(ARCH)

SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(SXEXTINCL) $(CFLAGS) -g

all: $(SXEXTBIN)/lfg_lex.out

###############################################################################

NEEDS1=\
	$(LIB)/lfg_lex_main.o		\
	$(LIB)/lfg_lex_scanact.o	\
	$(LIB)/lfg_lex_smp.o		\
	$(LIB)/lfg_lex_t.o

#	$(LIB)/sxdico_process.d.o	\

$(SXEXTBIN)/lfg_lex.out:	$(NEEDS1)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/lfg_lex_main.o:	$(SXINCL)/sxunix.h 	\
			lfg_lex_main.c
	$(CC) -o $@ -c lfg_lex_main.c

$(LIB)/lfg_lex_scanact.o:		$(SXINCL)/sxunix.h 	\
			lfg_lex_scanact.c
	$(CC) -o $@ -c lfg_lex_scanact.c

$(LIB)/lfg_lex_smp.o:	$(SXINCL)/sxunix.h 	\
			lfg_lex_smp.c
	$(CC) -o $@ -c lfg_lex_smp.c

$(LIB)/lfg_lex_t.o:	$(SXINCL)/sxunix.h 	\
			lfg_lex_t.c
	$(CC) -o $@ -c lfg_lex_t.c 		\
		-DSEMPASS=lfg_lex_smp

# debut de code produit par "make-makefile lfg_lex"

lfg_lex_t.c: lfg_lex.bt lfg_lex.st lfg_lex.pt lfg_lex.rt lfg_lex.att
	tables_c lfg_lex > $@

lfg_lex.bt lfg_lex.att lfg_lex_mix.c: lfg_lex.at
	semat lfg_lex.at > lfg_lex_mix.c

lfg_lex.st: lfg_lex.bt lfg_lex.lecl
	lecl lfg_lex.lecl || [ $$? = 1 ]

lfg_lex.pt: lfg_lex.bt
	csynt lfg_lex || [ $$? = 1 ]

lfg_lex.rt: lfg_lex.bt lfg_lex.st lfg_lex.recor
	recor lfg_lex.recor

# fin de code produit par "make-makefile lfg_lex"

clean:
	/bin/rm -f $(SXEXTBIN)/lfg_lex.out
	/bin/rm -f $(LIB)/*.o $(LFGLEX)/lfg_lex.*.l
	/bin/rm -f $(LFGLEX)/lfg_lex.att $(LFGLEX)/lfg_lex.bt $(LFGLEX)/lfg_lex.pt
	/bin/rm -f $(LFGLEX)/lfg_lex.rt $(LFGLEX)/lfg_lex.st $(LFGLEX)/lfg_lex_mix.c $(LFGLEX)/lfg_lex_t.c

