
include $(SX)/etc/make/gccoptions

MAKE_A_DICO=$(SX)/../extensions/make_a_dico

BIN=$(MAKE_A_DICO)/bin.$(ARCH)
LIB=$(MAKE_A_DICO)/lib.$(ARCH)

EXTINCL=$(SX)/../extensions/incl
SXEXTBIBS=$(SXLIB)/libsxext.a $(SXBIBS)

CC=$(COMPILER) -I$(SXINCL) -I$(SXINCL)/util -I$(EXTINCL) $(CFLAGS)

all: $(BIN)/make_a_dico.out	\
     $(BIN)/amlgm_cmpd.out

NEEDS=	$(LIB)/make_a_dico.o	\
	$(LIB)/make_a_dico_t.o

$(BIN)/make_a_dico.out:	$(NEEDS)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/make_a_dico.o:	$(SXINCL)/sxunix.h		\
			$(SXINCL)/XxY.h			\
			$(SXINCL)/fsa.h			\
			$(EXTINCL)/sxword.h		\
			make_a_dico.c
	$(CC) -o $@ -c make_a_dico.c

$(LIB)/make_a_dico_t.o:	$(SXINCL)/sxunix.h 	\
			make_a_dico_t.c
	$(CC) -o $@ -c make_a_dico_t.c		\
		-DSCANACT=make_a_dico_scanact	\
		-DSEMACT=make_a_dico_action

NEEDS2=	$(LIB)/amlgm_cmpd_main.o	\
	$(LIB)/amlgm_cmpd_smp.o		\
	$(LIB)/amlgm_cmpd_t.o

$(BIN)/amlgm_cmpd.out:	$(NEEDS2)	$(SXEXTBIBS)
	@rm -f $@
	$(COMPILER) -o $@ $(NEEDS2) $(CFLAGS) $(LDFLAGS) $(SXEXTBIBS) || (rm $@;exit 1)

$(LIB)/amlgm_cmpd_smp.o:$(SXINCL)/sxunix.h	\
			$(SXINCL)/XH.h		\
			$(SXINCL)/XxY.h		\
			$(SXINCL)/fsa.h		\
			$(SXINCL)/varstr.h	\
			amlgm_cmpd_smp.c
	$(CC) -o $@ -c amlgm_cmpd_smp.c

$(LIB)/amlgm_cmpd_t.o:	$(SXINCL)/sxunix.h 	\
			amlgm_cmpd_t.c
	$(CC) -o $@ -c amlgm_cmpd_t.c		\
		-DSEMPASS=amlgm_cmpd_smp

$(LIB)/amlgm_cmpd_main.o:	$(SXINCL)/sxunix.h	\
			$(SXINCL)/fsa.h			\
			amlgm_cmpd_main.c
	$(CC) -o $@ -c amlgm_cmpd_main.c

# debut de code produit par "make-makefile make_a_dico"

make_a_dico_t.c: make_a_dico.bt make_a_dico.st make_a_dico.pt make_a_dico.rt 
	tables_c make_a_dico > $@

make_a_dico.bt  : make_a_dico.bnf
	semact make_a_dico.bnf 

make_a_dico.st: make_a_dico.bt make_a_dico.lecl
	lecl make_a_dico.lecl || [ $$? = 1 ]

make_a_dico.pt: make_a_dico.bt
	csynt make_a_dico || [ $$? = 1 ]

make_a_dico.rt: make_a_dico.bt make_a_dico.st make_a_dico.recor
	recor make_a_dico.recor

# fin de code produit par "make-makefile make_a_dico"

# debut de code produit par "make-makefile amlgm_cmpd"

amlgm_cmpd_t.c: amlgm_cmpd.bt amlgm_cmpd.st amlgm_cmpd.pt amlgm_cmpd.rt amlgm_cmpd.att
	tables_c amlgm_cmpd > $@

amlgm_cmpd.bt amlgm_cmpd.att amlgm_cmpd_mix.c: amlgm_cmpd.at
	semat amlgm_cmpd.at  > amlgm_cmpd_mix.c

amlgm_cmpd.st: amlgm_cmpd.bt amlgm_cmpd.lecl
	lecl amlgm_cmpd.lecl || [ $$? = 1 ]

amlgm_cmpd.pt: amlgm_cmpd.bt
	csynt amlgm_cmpd || [ $$? = 1 ]

amlgm_cmpd.rt: amlgm_cmpd.bt amlgm_cmpd.st amlgm_cmpd.recor
	recor amlgm_cmpd.recor

# fin de code produit par "make-makefile amlgm_cmpd"

clean:
	/bin/rm -f $(LIB)/*.o $(BIN)/make_a_dico.out $(BIN)/amlgm_cmpd.out
	/bin/rm -f $(MAKE_A_DICO)/make_a_dico.*.l $(MAKE_A_DICO)/amlgm_cmpd.*.l
	/bin/rm -f $(MAKE_A_DICO)/make_a_dico.bt $(MAKE_A_DICO)/make_a_dico.pt
	/bin/rm -f $(MAKE_A_DICO)/make_a_dico.rt $(MAKE_A_DICO)/make_a_dico.st
	/bin/rm -f $(MAKE_A_DICO)/make_a_dico.att $(MAKE_A_DICO)/make_a_dico_t.c
	/bin/rm -f $(MAKE_A_DICO)/amlgm_cmpd.bt $(MAKE_A_DICO)/amlgm_cmpd.pt
	/bin/rm -f $(MAKE_A_DICO)/amlgm_cmpd.rt $(MAKE_A_DICO)/amlgm_cmpd.st
	/bin/rm -f $(MAKE_A_DICO)/amlgm_cmpd.att $(MAKE_A_DICO)/amlgm_cmpd_mix.c
	/bin/rm -f $(MAKE_A_DICO)/amlgm_cmpd_t.c

