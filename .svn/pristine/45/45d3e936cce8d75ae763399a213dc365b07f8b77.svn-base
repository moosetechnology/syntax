
include $(SX)/etc/make/gccoptions

LFG=$(SX)/../extensions/lfg
LIB=$(LFG)/lib

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util $(CFLAGS) -g

all: $(LFG)/lfg.out		\
     $(LFG)/lfg_lex.out

# $(SX)/bin/lfg.out:	$(LFG)/lfg.out
#	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?
#
# $(SX)/bin/lfg_lex.out:	$(LFG)/lfg_lex.out
#	@rm -f $@; mv $? $@ && $(LNorCP) $@ $?

###############################################################################

NEEDS=\
	$(LIB)/lfg_main.o		\
	$(LIB)/lfg_lex_scanact.o	\
	$(LIB)/lfg_smp.o		\
	$(LIB)/lfg_t.o

$(LFG)/lfg.out:	$(NEEDS)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

$(LIB)/lfg_smp.o:	$(SXINCL)/sxunix.h 	\
			lfg_smp.c
	$(CC) -o $@ -c lfg_smp.c

$(LIB)/lfg_main.o:	$(SXINCL)/sxunix.h 	\
			lfg_main.c
	$(CC) -o $@ -c lfg_main.c

$(LIB)/lfg_t.o:		$(SXINCL)/sxunix.h 	\
			lfg_t.c
	$(CC) -o $@ -c lfg_t.c		\
		-Dsxtables=lfg_tables	\
		-Dsempass=lfg_smp	\
		-DSCANACT=lfg_lex_scanact

###############################################################################

NEEDS1=\
	$(LIB)/lfg_lex_main.o		\
	$(LIB)/lfg_lex_scanact.o	\
	$(LIB)/lfg_lex_smp.o		\
	$(LIB)/lfg_lex_t.o

#	$(LIB)/sxdico_process.d.o	\

$(LFG)/lfg_lex.out:	$(NEEDS1)	$(SXBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS1) $(CFLAGS) $(LDFLAGS) $(SXBIBS) || (rm $@;exit 1)

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
		-Dsxtables=lfg_lex_tables	\
		-Dsempass=lfg_lex_smp		\
		-DSCANACT=lfg_lex_scanact

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

# debut de code produit par "make-makefile lfg_lex"

lfg_lex_t.c: lfg_lex.bt lfg_lex.st lfg_lex.pt lfg_lex.rt lfg_lex.att
	tables_c lfg_lex > $@

lfg_lex.bt lfg_lex.att lfg_lex_mix.c: lfg_lex.at
	semat lfg_lex.at  > lfg_lex_mix.c

lfg_lex.st: lfg_lex.bt lfg_lex.lecl
	lecl lfg_lex.lecl || [ $$? = 1 ]

lfg_lex.pt: lfg_lex.bt
	csynt lfg_lex || [ $$? = 1 ]

lfg_lex.rt: lfg_lex.bt lfg_lex.st lfg_lex.recor
	recor lfg_lex.recor

# fin de code produit par "make-makefile lfg_lex"

clean:
	/bin/rm -f $(LIB)/*.o $(LFG)/lfg.out $(LFG)/lfg_lex.out
	/bin/rm -f $(LFG)/lfg.*.l $(LFG)/lfg_lex.*.l
	/bin/rm -f $(LFG)/lfg.att $(LFG)/lfg.bt $(LFG)/lfg.pt \
		$(LFG)/lfg.rt $(LFG)/lfg.st $(LFG)/lfg_mix.c $(LFG)/lfg_t.c
	/bin/rm -f $(LFG)/lfg_lex.att $(LFG)/lfg_lex.bt $(LFG)/lfg_lex.pt \
		$(LFG)/lfg_lex.rt $(LFG)/lfg_lex.st $(LFG)/lfg_lex_mix.c $(LFG)/lfg_lex_t.c

