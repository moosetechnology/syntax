
include $(SX)/etc/make/gccoptions

SRC=$(SX)/src

CC=$(COMPILER) -I$(SXINCL) $(CFLAGS)

FORCE=$(SXINCL)/sxunix.h

all: $(SXLIB)/sxmem_mngr.d.o	\
     $(SXLIB)/dbgaction.d.o	\
     $(SXLIB)/sxmain.o		\
     $(SXLIB)/sxscanner.d.o	\
     $(SXLIB)/sxsrc_mngr.d.o

$(SXLIB)/sxmem_mngr.d.o:	$(SRC)/sxmem_mngr.c	$(FORCE)
	$(CC) -o $@ -c $(SRC)/sxmem_mngr.c

$(SXLIB)/dbgaction.d.o:	$(SRC)/dbgaction.c	$(FORCE)
	$(CC) -o $@ -c $(SRC)/dbgaction.c

$(SXLIB)/sxmain.o:	$(SRC)/sxmain.c		$(FORCE)
	$(CC) -o $@ -c $(SRC)/sxmain.c

$(SXLIB)/sxscanner.d.o:	$(SRC)/sxscanner.c	$(FORCE)
	$(CC) -o $@ -c $(SRC)/sxscanner.c

$(SXLIB)/sxsrc_mngr.d.o:	$(SRC)/sxsrc_mngr.c	$(FORCE)
	$(CC) -o $@ -c $(SRC)/sxsrc_mngr.c

NBEST=                  $(BIB)(nbest_semact.o)
OUTPUT=                 $(BIB)(output_semact.o)
READ_A_RE=		$(BIB)(read_a_re.o)
READ_A_SDAG=		$(BIB)(read_a_sdag.o)
READ_A_UDAG=		$(BIB)(read_a_udag.o)
SXEARLEY_MAIN=		$(BIB)(sxearley_main.o)
SX_BAG=			$(BIB)(sxbag_mngr.o)
SX_MATRIX=		$(BIB)(sxmatrix2vector.o)
SX_ND=			$(BIB)(sxndtreewalker.o)\
			$(BIB)(sxndtoken_mngr.o)\
			$(BIB)(sxndscanner.o)	\
			$(BIB)(sxnds_rcvr.o)	\
			$(BIB)(sxndparser.o)	\
			$(BIB)(sxndp_rcvr.o)	\
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

$(NBEST)		:	$(FORCE)	$(SXINCL)/earley.h	$(SXINCL)/nbest.h

$(OUTPUT)		:	$(FORCE)	$(SXINCL)/earley.h

$(READ_A_RE)		:	$(FORCE)	$(SXINCL)/fsa.h	$(SXINCL)/re_t.h

$(READ_A_SDAG)		:	$(FORCE)	$(SXINCL)/sdag_t.h

$(READ_A_UDAG)		:	$(FORCE)	$(SXINCL)/udag_t.h

$(SXEARLEY_MAIN)	:	$(FORCE)	$(SXINCL)/earley.h

$(SX_BAG)		:	$(FORCE)	$(SXINCL)/bag.h

$(SX_MATRIX)		:       $(FORCE)

$(SX_ND)		:	$(FORCE)	$(SXINCL)/sxnd.h

$(SX_PART)		:	$(FORCE)	$(SXINCL)/sxpart.h

$(SX_SPELL_MANAGER)	:	$(FORCE)	$(SXINCL)/sxspell.h

$(SX_SPF_MANAGER)	:	$(FORCE)	$(SXINCL)/earley.h $(SXINCL)/sxspf.h

$(SYMB_TBL_MNGR)	:	$(FORCE)

$(SX_DYNAM_PARSER)	:	$(FORCE)	$(SXINCL)/sxdynam_parser.h

$(SX_DYNAM_SCANNER)	:	$(FORCE)	$(SXINCL)/sxdynam_scanner.h $(SXINCL)/sxreg_exp_mngr.h

$(SXLIST_MNGR)		:	$(FORCE) 	$(SXINCL)/sxlist.h \
				$(SXINCL)/XT.h	$(SXINCL)/sxSL.h \
				$(SXINCL)/sxset.h

$(BIB):
		$(NBEST)		\
		$(OUTPUT)		\
		$(READ_A_RE)		\
		$(READ_A_SDAG)		\
		$(READ_A_UDAG)		\
		$(SXEARLEY_MAIN)	\
		$(SX_BAG)		\
		$(SX_MATRIX)		\
		$(SX_ND)		\
		$(SX_PART)		\
		$(SX_SPELL_MANAGER)	\
		$(SX_SPF_MANAGER)	\
		$(SYMB_TBL_MNGR)	\
		$(SX_DYNAM_PARSER)	\
		$(SX_DYNAM_SCANNER)	\
		$(SXLIST_MNGR)

sxndp_rcvr32.c	:	sxndp_rcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxndp_rcvr.c"') > $@

sxndparser32.c	:	sxndparser.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxndparser.c"') > $@

sxndscanner32.c	:	sxndscanner.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxndscanner.c"') > $@

sxnds_rcvr32.c	:	sxnds_rcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxnds_rcvr.c"') > $@

clean:
	@echo "clean target for src should be provided"

