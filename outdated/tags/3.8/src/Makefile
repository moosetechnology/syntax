# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
SRC=$(SX)/src
LIB=$(SX)/lib
BIB=$(LIB)/libsx$(sxV).a
INCL=$(SX)/incl

CC=$(COMPILER) -O -I$(INCL)

FORCE=$(INCL)/sxunix.h $(SX)/force_make


$(SX)/everything .PRECIOUS:	$(BIB)

DEBUG_ACTION=		dbgaction.o
SORT_BY_TREE=		sort_by_tree.o
SX_ARGS_MANAGER=	sxnumarg.o
SX_AT_MANAGER=		sxatc.o	\
			sxatcpp.o	\
			sxatedit.o	\
			sxat_mngr.o	\
			sxsmp.o
SX_BITS_ARRAY=		sxbitsarray.o	\
			sxba_write.o	\
			sxba_read.o	\
			sxba_ops.o	\
			sxba_ops2.o	\
			sxba_ops3.o	\
			sxbitsmatrix.o
SX_CHAR_TO_STRING=	sxc_to_str.o
SX_ERROR_MANAGER=	sxcheckmagic.o	\
			sxexit.o	\
			sxerr_mngr.o	\
			sxperror.o
SX_ERROR_RECOVERY=	sxs_rcvr.o	\
			sxs_srcvr.o	\
			sxp_rcvr.o	\
			sxp_srcvr.o
SX_FILE_MANAGER=	sxtmpfile.o
SX_GET_BIT=		sxgetbit.o
SX_INDEX=		sxindex_mngr.o
SX_LISTING_OUTPUT=	sxlistout.o
SX_MEMORY_MANAGER=	sxmem_mngr.o	\
			sxgc.o
SX_NEXT_PRIME=		sxnext_prime.o
SX_PARAGRAPHEURS=	sxppp.o	\
			sxpptree.o
SX_PARSER=		sxparser.o
SX_SCANNER=		sxscanner.o
SX_SOURCE_MANAGER=	sxsrc_mngr.o	\
			sxincl_mngr.o
SX_STRINGS_MANAGER=	sxstr_mngr.o	\
			sxword_mngr.o	\
			sxscrmbl.o
SX_SYNTAX_MANAGER=	sxopentty.o	\
			sxsyntax.o
SX_TOKEN_MANAGER=	sxtoken_mngr.o
SX_TRAP=		sxtrap.o

$(BIB):			$(DEBUG_ACTION)		\
		$(SORT_BY_TREE)		\
		$(SX_ARGS_MANAGER)	\
		$(SX_AT_MANAGER)	\
		$(SX_BITS_ARRAY)	\
		$(SX_CHAR_TO_STRING)	\
		$(SX_ERROR_MANAGER)	\
		$(SX_ERROR_RECOVERY)	\
		$(SX_FILE_MANAGER)	\
		$(SX_GET_BIT)		\
		$(SX_INDEX)		\
		$(SX_LISTING_OUTPUT)	\
		$(SX_MEMORY_MANAGER)	\
		$(SX_NEXT_PRIME)	\
		$(SX_PARAGRAPHEURS)	\
		$(SX_PARSER)		\
		$(SX_SCANNER)		\
		$(SX_SOURCE_MANAGER)	\
		$(SX_STRINGS_MANAGER)	\
		$(SX_SYNTAX_MANAGER)	\
		$(SX_TOKEN_MANAGER)	\
		$(SX_TRAP)
	ar rv $@ $?
	@ if test "$(STRIP_NAMES)" != ""; then for OBJECT in $? ; do /common/bin.sun5/strip_names $$OBJECT; done; fi
	cd $(LIB) ; $(RANLIB) $@

$(DEBUG_ACTION)		:	$(FORCE)
$(SORT_BY_TREE)		:	$(FORCE)
$(SX_AT_MANAGER)	:	$(FORCE)
$(SX_ARGS_MANAGER)	:	$(FORCE)
$(SX_BITS_ARRAY)	:	$(FORCE)	$(INCL)/sxba.h
$(SX_CHAR_TO_STRING)	:	$(FORCE)
$(SX_ERROR_MANAGER)	:	$(FORCE)
$(SX_ERROR_RECOVERY)	:	$(FORCE)
$(SX_FILE_MANAGER)	:	$(FORCE)
$(SX_GET_BIT)		:	$(FORCE)
$(SX_INDEX)		:	$(FORCE)
$(SX_LISTING_OUTPUT)	:	$(FORCE)
$(SX_MEMORY_MANAGER)	:	$(FORCE)
$(SX_NEXT_PRIME)	:	$(FORCE)
$(SX_PARAGRAPHEURS)	:	$(FORCE)
$(SX_PARSER)		:	$(FORCE)
$(SX_SCANNER)		:	$(FORCE)
$(SX_SOURCE_MANAGER)	:	$(FORCE)
$(SX_STRINGS_MANAGER)	:	$(FORCE)
$(SX_SYNTAX_MANAGER)	:	$(FORCE)
$(SX_TOKEN_MANAGER)	:	$(FORCE)
$(SX_TRAP)		:	$(FORCE)
