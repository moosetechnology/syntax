pkglib_LIBRARIES =	libsx.a			\
			libsx.debug.a		\
			libsx.O2.a		\
			libsxdet.a		\
			libsxdet.debug.a	\
			libsxdet.O2.a

SXSRC =		$(srcdir)/../src
SXINCL =	$(srcdir)/../incl

AM_CPPFLAGS =	-I$(SXINCL)

# ... et pour quelques warnings de plus
#CC=cc -Wcomment -Wformat -Wimplicit -Wreturn-type -Wmain -Wswitch -I$(INCL) # -Wunused -Wuninitialized -Wtrigraphs -Wparentheses -Wtraditional  -Wredundant-decls  -Wchar-subscripts

DEBUG_ACTION =		$(SXSRC)/dbgaction.c
SORT_BY_TREE =		$(SXSRC)/sort_by_tree.c
SX_ARGS_MANAGER =	$(SXSRC)/sxnumarg.c
SX_AT_MANAGER =		$(SXSRC)/sxatc.c		\
			$(SXSRC)/sxatcpp.c	\
			$(SXSRC)/sxatedit.c	\
			$(SXSRC)/sxat_mngr.c	\
			$(SXSRC)/sxsmp.c
SX_BAG =		$(SXSRC)/sxbag_mngr.c
SX_BITS_ARRAY =		$(SXSRC)/sxbitsarray.c	\
			$(SXSRC)/sxba_print.c	\
			$(SXSRC)/sxba_write.c	\
			$(SXSRC)/sxba_read.c	\
			$(SXSRC)/sxba_ops.c	\
			$(SXSRC)/sxba_ops2.c	\
			$(SXSRC)/sxba_ops3.c	\
			$(SXSRC)/sxba_ops4.c	\
			$(SXSRC)/sxba_2op.c	\
			$(SXSRC)/sxba_bag_mngr.c	\
			$(SXSRC)/sxbatrap.c	\
			$(SXSRC)/sxbitsmatrix.c
SX_CHAR_TO_STRING =	$(SXSRC)/sxc_to_str.c
SX_DICO =		$(SXSRC)/sxdico_get.c	\
			$(SXSRC)/sxdico_process.c
SX_DYNAM_PARSER =	$(SXSRC)/sxdynam_parser.c
SX_DYNAM_SCANNER =	$(SXSRC)/sxdynam_scanner.c	\
			$(SXSRC)/sxreg_exp_mngr.c
SX_ERROR_MANAGER =	$(SXSRC)/sxcheckmagic.c	\
			$(SXSRC)/sxexit.c	\
			$(SXSRC)/sxerr_mngr.c	\
			$(SXSRC)/sxperror.c
SX_ERROR_RECOVERY =	$(SXSRC)/sxp_rcvr.c	\
			$(SXSRC)/sxp_rcvr32.c	\
			$(SXSRC)/sxp_srcvr.c	\
			$(SXSRC)/sxp_srcvr32.c	\
			$(SXSRC)/sxs_rcvr.c	\
			$(SXSRC)/sxs_rcvr32.c	\
			$(SXSRC)/sxs_srcvr.c	\
			$(SXSRC)/sxs_srcvr32.c
SX_FILE_MANAGER =	$(SXSRC)/sxfilecopy.c	\
			$(SXSRC)/sxtmpfile.c
SX_GET_BIT =		$(SXSRC)/sxgetbit.c	\
			$(SXSRC)/sxgetprdct.c
SX_INDEX =		$(SXSRC)/sxindex_mngr.c
SX_LISTING_OUTPUT =	$(SXSRC)/sxlistout.c
SX_MATRIX =		$(SXSRC)/sxmatrix2vector.c
SX_MEMORY_MANAGER =	$(SXSRC)/sxmem_mngr.c	\
			$(SXSRC)/sxgc.c
SX_NEXT_PRIME =		$(SXSRC)/sxnext_prime.c
SX_ND =			$(SXSRC)/sxndtreewalker.c\
			$(SXSRC)/sxndtoken_mngr.c\
			$(SXSRC)/sxndscanner.c	\
			$(SXSRC)/sxnds_rcvr.c	\
			$(SXSRC)/sxndparser.c	\
			$(SXSRC)/sxndp_rcvr.c	\
			$(SXSRC)/sxndp_rcvr.c	\
			$(SXSRC)/sxndparser32.c	\
			$(SXSRC)/sxndp_rcvr32.c	\
			$(SXSRC)/sxndscanner32.c	\
			$(SXSRC)/sxnds_rcvr32.c
SX_PARAGRAPHEURS =	$(SXSRC)/sxppp.c		\
			$(SXSRC)/sxpptree.c
SX_PART =		$(SXSRC)/sxpart_mngr.c
SX_PARSER =		$(SXSRC)/sxparser.c	\
			$(SXSRC)/sxparser32.c
SX_SCANNER =		$(SXSRC)/sxscanner.c	\
			$(SXSRC)/sxscanner32.c
SX_SPELL_MANAGER =	$(SXSRC)/sxspell_mngr.c
SX_SOURCE_MANAGER =	$(SXSRC)/sxsrc_mngr.c	\
			$(SXSRC)/sxincl_mngr.c
SX_SPF_MANAGER =	$(SXSRC)/sxspf_mngr.c
SX_STRINGS_MANAGER =	$(SXSRC)/sxstr_mngr.c	\
			$(SXSRC)/sxword_mngr.c	\
			$(SXSRC)/sxscrmbl.c
SX_SYNTAX_MANAGER =	$(SXSRC)/sxopentty.c	\
			$(SXSRC)/sxsyntax.c	\
			$(SXSRC)/sxtm_mngr.c	\
			$(SXSRC)/sxvars.c
SX_TOKEN_MANAGER =	$(SXSRC)/sxtoken_mngr.c
SX_TRAP =		$(SXSRC)/sxtrap.c
SS_MNGR =		$(SXSRC)/SS_mngr.c
VARSTR =		$(SXSRC)/varstralloc.c	\
			$(SXSRC)/varstrcat.c	\
			$(SXSRC)/varstrfree.c	\
			$(SXSRC)/varstrraz.c
X_MNGR =		$(SXSRC)/X_mngr.c
X_ROOT_MNGR =		$(SXSRC)/X_root_mngr.c
SXLIST_MNGR =		$(SXSRC)/XH_mngr.c	\
			$(SXSRC)/XT_mngr.c	\
			$(SXSRC)/sxSL_mngr.c	\
			$(SXSRC)/sxlist_mngr.c	\
			$(SXSRC)/sxsubset_mngr.c	\
			$(SXSRC)/sxset_mngr.c
XxY_MNGR =		$(SXSRC)/XxY_mngr.c
XxYxZ_MNGR =		$(SXSRC)/XxYxZ_mngr.c
NBEST =                 $(SXSRC)/nbest_semact.c
FSA_TO_RE =		$(SXSRC)/fsa_to_re.c
PUT_EDIT =		$(SXSRC)/puteditcut.c	\
			$(SXSRC)/puteditfnb.c	\
			$(SXSRC)/puteditnl.c	\
			$(SXSRC)/puteditpos.c
READ_A_DAG =		$(SXSRC)/read_a_dag.c
READ_A_UDAG =		$(SXSRC)/read_a_udag.c
SYMB_TBL_MNGR =		$(SXSRC)/sxsymbol_table_mngr.c
EQ_SORT =		$(SXSRC)/eq_sort.c	\
			$(SXSRC)/fermer.c
LFG_FILTERS_MNGR =	$(SXSRC)/lfg_filters_mngr.c

LIBSX =	$(DEBUG_ACTION)		\
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

LIBSXDET =	$(SORT_BY_TREE) 	\
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

libsx_a_SOURCES = $(LIBSX)
libsx_a_CFLAGS = -ggdb

libsx_debug_a_SOURCES = $(LIBSX)
libsx_debug_a_CFLAGS = -ggdb -DEBUG

libsx_O2_a_SOURCES = $(LIBSX)
libsx_O2_a_CFLAGS = -O2

libsxdet_a_SOURCES = $(LIBSXDET)
libsxdet_a_CFLAGS = -ggdb

libsxdet_debug_a_SOURCES = $(LIBSXDET)
libsxdet_debug_a_CFLAGS = -ggdb -DEBUG

libsxdet_O2_a_SOURCES = $(LIBSXDET)
libsxdet_O2_a_CFLAGS = -O2

%32.c	:	%.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#include "$*.c"') > $@

libsxdebug :	sxmem_mngr.d.o	\
		dbgaction.d.o	\
		sxmain.o	\
		sxscanner.d.o	\
		sxsrc_mngr.d.o

sxmem_mngr.d.o:	$(SXSRC)/sxmem_mngr.c
	$(CC)  -DBUG -DEBUG_ALLOC $(SXSRC)/sxmem_mngr.c;mv sxmem_mngr.o $@

dbgaction.d.o:	$(SXSRC)/dbgaction.c
	$(CC)  -DBUG -DEBUG_ALLOC $(SXSRC)/dbgaction.c;mv dbgaction.o $@

sxmain.o:	$(SXSRC)/sxmain.c
	$(CC) $(SXSRC)/sxmain.c

sxscanner.d.o:	$(SXSRC)/sxscanner.c
	$(CC)  -DBUG -DEBUG_ALLOC $(SXSRC)/sxscanner.c;mv sxscanner.o $@

sxsrc_mngr.d.o:	$(SXSRC)/sxsrc_mngr.c
	$(CC)  -DBUG -DEBUG_ALLOC $(SXSRC)/sxsrc_mngr.c;mv sxsrc_mngr.o $@
