sinclude(m4)dnl
define(`smart',ifdef(`archive',`',`skip'))dnl
define(`dummy',ifdef(`archive',`skip'))dnl
define(`archive',ifdef(`archive',$(BIB)($1),$1))dnl
define(`skip',`dnl')dnl
  ########################################################
  #                                                      #
  #                                                      #
  # Copyright (c) 1984 by Institut National de Recherche #
  #                    en Informatique et en Automatique #
  #                                                      #
  #                                                      #
  ########################################################




  ########################################################
  #                                                      #
  #  Produit de l'équipe Langages et Traducteurs. (phd)  #
  #                                                      #
  ########################################################


                 #    #  ######          #####     ##     ####
                 ##   #  #               #    #   #  #   #
                 # #  #  #####           #    #  #    #   ####
                 #  # #  #               #####   ######       #
                 #   ##  #               #       #    #  #    #
                 #    #  ######          #       #    #   ####


         #    #   ####   #####      #    ######     #    ######  #####
         ##  ##  #    #  #    #     #    #          #    #       #    #
         # ## #  #    #  #    #     #    #####      #    #####   #    #
         #    #  #    #  #    #     #    #          #    #       #####
         #    #  #    #  #    #     #    #          #    #       #   #
         #    #   ####   #####      #    #          #    ######  #    #


 #       ######          ######     #     ####   #    #     #    ######  #####
 #       #               #          #    #    #  #    #     #    #       #    #
 #       #####           #####      #    #       ######     #    #####   #    #
 #       #               #          #    #       #    #     #    #       #####
 #       #               #          #    #    #  #    #     #    #       #   #
 ######  ######          #          #     ####   #    #     #    ######  #    #

         #     #
         ##   ##    ##    #    #  ######  ######     #    #       ######
         # # # #   #  #   #   #   #       #          #    #       #
         #  #  #  #    #  ####    #####   #####      #    #       #####
         #     #  ######  #  #    #       #          #    #       #
         #     #  #    #  #   #   #       #          #    #       #
         #     #  #    #  #    #  ######  #          #    ######  ######



#########################################################################
# Historique des modifications, en ordre chronologique inverse:		#
#########################################################################
# 20030521 18:13 (phd):	Ajout de sxvars.c (pour mes 50 ans)		#
#########################################################################
# 20030515 11:35 (phd):	Ajout de cette rubrique "modifications"		#
#########################################################################

# To avoid problems with other shells:
SHELL=/bin/sh

SX=$(sx)
SRC=$(SX)/src
LIB=$(SX)/lib
BIB=$(LIB)/libsx$(sxV).a
BIBDETERMINISTIC=$(LIB)/libsxdet.$(sxV).a
INCL=$(SX)/incl
COMPILER=cc
CC=$(COMPILER) -I$(INCL)
# ... et pour quelques warnings de plus
#CC=cc -Wcomment -Wformat -Wimplicit -Wreturn-type -Wmain -Wswitch -I$(INCL) # -Wunused -Wuninitialized -Wtrigraphs -Wparentheses -Wtraditional  -Wredundant-decls  -Wchar-subscripts

FORCE=$(INCL)/sxunix.h $(SX)/force_make


$(SX)/everything .PRECIOUS:	$(BIB)
libsxdet .PRECIOUS: $(BIBDETERMINISTIC)
libsx    .PRECIOUS: $(BIB)
DEBUG_ACTION=		archive(dbgaction.o)
SORT_BY_TREE=		archive(sort_by_tree.o)
SX_ARGS_MANAGER=	archive(sxnumarg.o)
SX_AT_MANAGER=		archive(sxatc.o)	\
			archive(sxatcpp.o)	\
			archive(sxatedit.o)	\
			archive(sxat_mngr.o)	\
			archive(sxsmp.o)
SX_BAG=			archive(sxbag_mngr.o)
SX_BITS_ARRAY=		archive(sxbitsarray.o)	\
			archive(sxba_print.o)	\
			archive(sxba_write.o)	\
			archive(sxba_read.o)	\
			archive(sxba_ops.o)	\
			archive(sxba_ops2.o)	\
			archive(sxba_ops3.o)	\
			archive(sxba_ops4.o)	\
			archive(sxba_2op.o)	\
			archive(sxba_bag_mngr.o)\
			archive(sxbatrap.o)	\
			archive(sxbitsmatrix.o)
SX_CHAR_TO_STRING=	archive(sxc_to_str.o)
SX_DICO=		archive(sxdico_get.o)	\
			archive(sxdico_process.o)
SX_DYNAM_PARSER=	archive(sxdynam_parser.o)
SX_DYNAM_SCANNER=	archive(sxdynam_scanner.o)	\
			archive(sxreg_exp_mngr.o)
SX_ERROR_MANAGER=	archive(sxcheckmagic.o)	\
			archive(sxexit.o)	\
			archive(sxerr_mngr.o)	\
			archive(sxperror.o)
SX_ERROR_RECOVERY=	archive(sxp_rcvr.o)	\
			archive(sxp_rcvr32.o)	\
			archive(sxp_srcvr.o)	\
			archive(sxp_srcvr32.o)	\
			archive(sxs_rcvr.o)	\
			archive(sxs_rcvr32.o)	\
			archive(sxs_srcvr.o)	\
			archive(sxs_srcvr32.o)
SX_FILE_MANAGER=	archive(sxfilecopy.o)	\
			archive(sxtmpfile.o)
SX_GET_BIT=		archive(sxgetbit.o)	\
			archive(sxgetprdct.o)
SX_INDEX=		archive(sxindex_mngr.o)
SX_LISTING_OUTPUT=	archive(sxlistout.o)
SX_MATRIX=		archive(sxmatrix2vector.o)
SX_MEMORY_MANAGER=	archive(sxmem_mngr.o)	\
			archive(sxgc.o)
SX_NEXT_PRIME=		archive(sxnext_prime.o)
SX_ND=			archive(sxndtreewalker.o)\
			archive(sxndtoken_mngr.o)\
			archive(sxndscanner.o)	\
			archive(sxnds_rcvr.o)	\
			archive(sxndparser.o)	\
			archive(sxndp_rcvr.o)	\
			archive(sxndp_rcvr.o)	\
			archive(sxndparser32.o)	\
			archive(sxndp_rcvr32.o)	\
			archive(sxndscanner32.o)\
			archive(sxnds_rcvr32.o)
SX_PARAGRAPHEURS=	archive(sxppp.o)	\
			archive(sxpptree.o)
SX_PART=		archive(sxpart_mngr.o)
SX_PARSER=		archive(sxparser.o)	\
			archive(sxparser32.o)
SX_SCANNER=		archive(sxscanner.o)	\
			archive(sxscanner32.o)
SX_SPELL_MANAGER=	archive(sxspell_mngr.o)
SX_SOURCE_MANAGER=	archive(sxsrc_mngr.o)	\
			archive(sxincl_mngr.o)
SX_SPF_MANAGER=		archive(sxspf_mngr.o)
SX_STRINGS_MANAGER=	archive(sxstr_mngr.o)	\
			archive(sxword_mngr.o)	\
			archive(sxscrmbl.o)
SX_SYNTAX_MANAGER=	archive(sxopentty.o)	\
			archive(sxsyntax.o)	\
			archive(sxtm_mngr.o)	\
			archive(sxvars.o)
SX_TOKEN_MANAGER=	archive(sxtoken_mngr.o)
SX_TRAP=		archive(sxtrap.o)
SS_MNGR=		archive(SS_mngr.o)
VARSTR=			archive(varstralloc.o)	\
			archive(varstrcat.o)	\
			archive(varstrfree.o)	\
			archive(varstrraz.o)
X_MNGR=			archive(X_mngr.o)
X_ROOT_MNGR=		archive(X_root_mngr.o)
SXLIST_MNGR=		archive(XH_mngr.o)	\
			archive(XT_mngr.o)	\
			archive(sxSL_mngr.o)	\
			archive(sxlist_mngr.o)	\
			archive(sxsubset_mngr.o)\
			archive(sxset_mngr.o)
XxY_MNGR=		archive(XxY_mngr.o)
XxYxZ_MNGR=		archive(XxYxZ_mngr.o)
NBEST=                  archive(nbest_semact.o)
FSA_TO_RE=		archive(fsa_to_re.o)
PUT_EDIT=		archive(puteditcut.o)	\
			archive(puteditfnb.o)	\
			archive(puteditnl.o)	\
			archive(puteditpos.o)
READ_A_DAG=		archive(read_a_dag.o)
READ_A_UDAG=		archive(read_a_udag.o)
SYMB_TBL_MNGR=		archive(sxsymbol_table_mngr.o)
EQ_SORT=		archive(eq_sort.o)	\
			archive(fermer.o)
LFG_FILTERS_MNGR=	archive(lfg_filters_mngr.o)

$(BIB):	smart	$(FORCE)		\
		$(DEBUG_ACTION)		\
		$(SORT_BY_TREE)		\
		$(SX_ARGS_MANAGER)	\
		$(SX_AT_MANAGER)	\
		$(SX_BAG)		\
		$(SX_BITS_ARRAY)	\
		$(SX_CHAR_TO_STRING)	\
		$(SX_DICO)		\
		$(SX_DYNAM_PARSER)	\
		$(SX_DYNAM_SCANNER)	\
		$(SX_ERROR_MANAGER)	\
		$(SX_ERROR_RECOVERY)	\
		$(SX_FILE_MANAGER)	\
		$(SX_GET_BIT)		\
		$(SX_INDEX)		\
		$(SX_LISTING_OUTPUT)	\
		$(SX_MATRIX)		\
		$(SX_MEMORY_MANAGER)	\
		$(SX_NEXT_PRIME)	\
		$(SX_D)			\
		$(SX_ND)		\
		$(SX_PARAGRAPHEURS)	\
		$(SX_PART)		\
		$(SX_PARSER)		\
		$(SX_SCANNER)		\
		$(SX_SPELL_MANAGER)	\
		$(SX_SOURCE_MANAGER)	\
		$(SX_SPF_MANAGER)	\
		$(SX_STRINGS_MANAGER)	\
		$(SX_SYNTAX_MANAGER)	\
		$(SX_TOKEN_MANAGER)	\
		$(SX_TRAP)		\
		$(SS_MNGR)		\
		$(VARSTR)		\
		$(X_MNGR)		\
		$(X_ROOT_MNGR)		\
		$(SXLIST_MNGR)		\
		$(XxY_MNGR)		\
		$(XxYxZ_MNGR)		\
		$(NBEST)		\
		$(FSA_TO_RE)		\
		$(PUT_EDIT)		\
		$(READ_A_DAG)		\
		$(READ_A_UDAG)		\
		$(SYMB_TBL_MNGR)	\
		$(EQ_SORT)		\
		$(LFG_FILTERS_MNGR)
dummy	$(AR) rv $@ $?
	cd $(LIB) ; $(RANLIB) $@

$(BIBDETERMINISTIC): smart	$(SORT_BY_TREE) \
	$(SX_AT_MANAGER)	\
	$(SX_BITS_ARRAY)	\
	$(SX_CHAR_TO_STRING)	\
	$(SX_ERROR_MANAGER)	\
	$(SX_ERROR_RECOVERY)	\
	$(SX_GET_BIT)		\
	$(SX_INDEX)		\
	$(SX_MEMORY_MANAGER)	\
	$(SX_PARSER)		\
	$(SX_SOURCE_MANAGER)	\
	$(SX_STRINGS_MANAGER)	\
	$(SX_SCANNER)		\
	$(SX_SYNTAX_MANAGER)	\
	$(SX_TOKEN_MANAGER)	\
	$(SX_TRAP)		\
	$(SS_MNGR)
dummy $(AR) rv $@ $?
	cd $(LIB) ; $(RANLIB) $(BIB)
	mv $(BIB) $(BIBDETERMINISTIC)

$(DEBUG_ACTION)		:	$(FORCE)
$(SORT_BY_TREE)		:	$(FORCE)
$(SX_AT_MANAGER)	:	$(FORCE)
$(SX_ARGS_MANAGER)	:	$(FORCE)
$(SX_BAG)		:	$(FORCE)	$(INCL)/bag.h
$(SX_BITS_ARRAY)	:	$(FORCE)	$(INCL)/sxba.h
$(SX_CHAR_TO_STRING)	:	$(FORCE)
$(SX_DICO)		:	$(FORCE)	$(INCL)/sxdico.h
$(SX_DYNAM_PARSER)	:	$(FORCE)	$(INCL)/sxdynam_parser.h
$(SX_DYNAM_SCANNER)	:	$(FORCE)	$(INCL)/sxdynam_scanner.h
$(SX_ERROR_MANAGER)	:	$(FORCE)
$(SX_ERROR_RECOVERY)	:	$(FORCE)
$(SX_FILE_MANAGER)	:	$(FORCE)
$(SX_GET_BIT)		:	$(FORCE)
$(SX_INDEX)		:	$(FORCE)
$(SX_LISTING_OUTPUT)	:	$(FORCE)
$(SX_MATRIX)		:       $(FORCE)
$(SX_MEMORY_MANAGER)	:	$(FORCE)
$(SX_NEXT_PRIME)	:	$(FORCE)
$(SX_D)			:	$(FORCE)	$(INCL)/SS.h
$(SX_ND)		:	$(FORCE)	$(INCL)/sxnd.h
$(SX_PARAGRAPHEURS)	:	$(FORCE)
$(SX_PART)		:	$(FORCE)	$(INCL)/sxpart.h
$(SX_PARSER)		:	$(FORCE)
$(SX_SCANNER)		:	$(FORCE)
$(SX_SPELL_MANAGER)	:	$(FORCE)	$(INCL)/sxspell.h
$(SX_SOURCE_MANAGER)	:	$(FORCE)
$(SX_SPF_MANAGER)	:	$(FORCE)	$(INCL)/earley.h $(INCL)/sxspf.h
$(SX_STRINGS_MANAGER)	:	$(FORCE)
$(SX_SYNTAX_MANAGER)	:	$(FORCE)
$(SX_TOKEN_MANAGER)	:	$(FORCE)
$(SX_TRAP)		:	$(FORCE)
$(SS_MNGR)		:	$(FORCE)
$(VARSTR)		:	$(FORCE)
$(X_MNGR)		:	$(FORCE)	$(INCL)/X.h
$(X_ROOT_MNGR)		:	$(FORCE)	$(INCL)/X_root.h
$(SXLIST_MNGR)		:	$(FORCE) 	$(INCL)/sxlist.h 	$(INCL)/XH.h		\
				$(INCL)/XT.h	$(INCL)/sxSL.h 		$(INCL)/sxsubset.h	\
				$(INCL)/sxset.h
$(XxY_MNGR)		:	$(FORCE)	$(INCL)/XxY.h
$(XxYxZ_MNGR)		:	$(FORCE)	$(INCL)/XxYxZ.h
$(NBEST)		:	$(FORCE)	$(INCL)/earley.h	$(INCL)/nbest.h
$(FSA_TO_RE)		:	$(FORCE)
$(PUT_EDIT)		:	$(FORCE)
$(READ_A_DAG)		:	$(FORCE)	$(INCL)/dag_scanner.h	$(INCL)/dag_t.h
$(READ_A_UDAG)		:	$(FORCE)	$(INCL)/dag_scanner.h	$(INCL)/udag_t.h
$(SYMB_TBL_MNGR)	:	$(FORCE)
$(EQ_SORT)		:	$(FORCE)	$(SX)/incl/sxba.h	$(SX)/incl/sxstack.h
$(LFG_FILTERS_MNGR)	:	$(FORCE)	$(INCL)/earley.h

sxp_rcvr32.c	:	sxp_rcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxp_rcvr.c"') > $@
sxp_srcvr32.c	:	sxp_srcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxp_srcvr.c"') > $@
sxs_rcvr32.c	:	sxs_rcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxs_rcvr.c"') > $@
sxs_srcvr32.c	:	sxs_srcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxs_srcvr.c"') > $@
sxparser32.c	:	sxparser.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxparser.c"') > $@
sxscanner32.c	:	sxscanner.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxscanner.c"') > $@
sxndp_rcvr32.c	:	sxndp_rcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxndp_rcvr.c"') > $@
sxndparser32.c	:	sxndparser.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxndparser.c"') > $@
sxndscanner32.c	:	sxndscanner.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxndscanner.c"') > $@
sxnds_rcvr32.c	:	sxnds_rcvr.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32 1';echo '#include "sxnds_rcvr.c"') > $@
