
include $(SX)/etc/make/gccoptions

EXTINCL=$(SX)/../extensions/incl

BIB=$(SXLIB)/libsxext.a

CC=$(COMPILER) -I$(SXINCL) -I$(EXTINCL) $(CFLAGS)

FORCE=$(SXINCL)/sxunix.h

all: $(BIB)

NBEST=                  $(BIB)(nbest_semact.o) \
                        $(BIB)(exact_nbest_semact.o)
OUTPUT=                 $(BIB)(output_semact.o)
READ_A_RE=		$(BIB)(read_a_re.o)
READ_A_SDAG=		$(BIB)(read_a_sdag.o)
READ_A_UDAG=		$(BIB)(read_a_udag.o)
SX_BAG=			$(BIB)(sxbag_mngr.o)
SX_MATRIX=		$(BIB)(sxmatrix2vector.o)
SX_ND=			$(BIB)(sxndtreewalker.o)\
			$(BIB)(sxndtoken_mngr.o)\
			$(BIB)(sxndscanner.o)	\
			$(BIB)(sxnds_rcvr.o)	\
			$(BIB)(sxndparser.o)	\
			$(BIB)(sxndp_rcvr.o)	\
			$(BIB)(sxndparser32.o)	\
			$(BIB)(sxndp_rcvr32.o)	\
			$(BIB)(sxndscanner32.o)\
			$(BIB)(sxnds_rcvr32.o)
SX_PART=		$(BIB)(sxpart_mngr.o)
SX_SPELL_MANAGER=	$(BIB)(sxspell_mngr.o)
SX_SPF_MANAGER=		$(BIB)(sxspf_mngr.o)
SYMB_TBL_MNGR=		$(BIB)(sxsymbol_table_mngr.o)
SX_DYNAM_PARSER=	$(BIB)(sxdynam_parser.o)
SX_DYNAM_SCANNER=	$(BIB)(sxdynam_scanner.o)	\
			$(BIB)(sxreg_exp_mngr.o)
SXLIST_MNGR=		$(BIB)(XT_mngr.o)	\
			$(BIB)(sxSL_mngr.o)	\
			$(BIB)(sxlist_mngr.o)	\
			$(BIB)(sxset_mngr.o)
SXSEM_MNGR=		$(BIB)(sxsem_mngr.o)
SXBA_BAG_MNGR=		$(BIB)(sxba_bag_mngr.o)
SXSUBSET_MNGR=		$(BIB)(sxsubset_mngr.o)
SXVARS2=		$(BIB)(sxvars2.o)
SXWORD_MNGR=		$(BIB)(sxword_mngr.o)

$(BIB):		$(NBEST)		\
		$(OUTPUT)		\
		$(READ_A_RE)		\
		$(READ_A_SDAG)		\
		$(READ_A_UDAG)		\
		$(SX_BAG)		\
		$(SX_MATRIX)		\
		$(SX_ND)		\
		$(SX_PART)		\
		$(SX_SPELL_MANAGER)	\
		$(SX_SPF_MANAGER)	\
		$(SYMB_TBL_MNGR)	\
		$(SX_DYNAM_PARSER)	\
		$(SX_DYNAM_SCANNER)	\
		$(SXLIST_MNGR)		\
		$(SXSEM_MNGR)		\
		$(SXBA_BAG_MNGR)	\
		$(SXSUBSET_MNGR)	\
		$(SXVARS2)		\
		$(SXWORD_MNGR)
	$(RANLIB) $@

$(NBEST)		: $(FORCE) $(EXTINCL)/earley.h	$(EXTINCL)/nbest.h
$(OUTPUT)		: $(FORCE) $(EXTINCL)/earley.h
$(READ_A_RE)		: $(FORCE) $(SXINCL)/fsa.h	$(EXTINCL)/re_t.h
$(READ_A_SDAG)		: $(FORCE) $(EXTINCL)/sdag_t.h
$(READ_A_UDAG)		: $(FORCE) $(EXTINCL)/udag_t.h
$(SX_BAG)		: $(FORCE) $(EXTINCL)/bag.h
$(SX_MATRIX)		: $(FORCE)
$(SX_ND)		: $(FORCE) $(EXTINCL)/sxnd.h
$(SX_PART)		: $(FORCE) $(EXTINCL)/sxpart.h
$(SX_SPELL_MANAGER)	: $(FORCE) $(EXTINCL)/sxspell.h
$(SX_SPF_MANAGER)	: $(FORCE) $(EXTINCL)/earley.h $(EXTINCL)/sxspf.h
$(SYMB_TBL_MNGR)	: $(FORCE)
$(SX_DYNAM_PARSER)	: $(FORCE) $(EXTINCL)/sxdynam_parser.h
$(SX_DYNAM_SCANNER)	: $(FORCE) $(EXTINCL)/sxdynam_scanner.h $(EXTINCL)/sxreg_exp_mngr.h
$(SXLIST_MNGR)		: $(FORCE) $(EXTINCL)/sxlist.h \
				$(EXTINCL)/XT.h	$(EXTINCL)/sxSL.h \
				$(EXTINCL)/sxset.h
$(SXSEM_MNGR)		: $(FORCE) \
				$(EXTINCL)/earley.h \
				$(EXTINCL)/sxspf.h \
				$(SXINCL)/varstr.h
$(SXBA_BAG_MNGR)	: $(FORCE) \
				$(EXTINCL)/sxpart.h
$(SXSUBSET_MNGR)	: $(FORCE) \
				$(EXTINCL)/sxsubset.h
$(SXVARS2)		: $(FORCE)
$(SXWORD_MNGR)		: $(FORCE) \
				$(EXTINCL)/sxword.h

# ----------------------------------------------------------------------------

sxndp_rcvr32.c	:	sxndp_rcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxndp_rcvr.c"') > $@

sxndparser32.c	:	sxndparser.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxndparser.c"') > $@

sxndscanner32.c	:	sxndscanner.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxndscanner.c"') > $@

sxnds_rcvr32.c	:	sxnds_rcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxnds_rcvr.c"') > $@


# ----------------------------------------------------------------------------

clean:
	rm -f $(BIB)
	rm -f sxndp_rcvr32.c sxndparser32.c sxndscanner32.c sxnds_rcvr32.c
	rm -f *.o

