pkglib_LIBRARIES =		libsx.a

if LIBSXDEBUG
pkglib_LIBRARIES +=	libsx.debug.a
endif

if LIBSXDET
pkglib_LIBRARIES +=	libsxdet.a
if LIBSXDEBUG
pkglib_LIBRARIES +=	libsxdet.debug.a
endif
endif

SXSRC =		$(srcdir)/../src
SXINCL =	$(srcdir)/../incl

BUILT_SOURCES = $(SXINCL)/sxversion.h 
CLEAN_FILES = $(SXINCL)/sxversion.h 

AM_CPPFLAGS =	-I$(SXINCL) $(CFLAGS)

SX_VARS =		$(SXSRC)/sxvars.c	\
			$(SXSRC)/sxvars2.c
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
SX_COMMENT_MANAGER=	$(SXSRC)/sxcomment_mngr.c
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
			$(SXSRC)/sxtm_mngr.c
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
OUTPUT =                $(SXSRC)/output_semact.c
FSA_MNGR =		$(SXSRC)/fsa_mngr.c
PUT_EDIT =		$(SXSRC)/puteditcut.c	\
			$(SXSRC)/puteditfnb.c	\
			$(SXSRC)/puteditnl.c	\
			$(SXSRC)/puteditpos.c
READ_A_UDAG =		$(SXSRC)/read_a_udag.c
READ_A_SDAG =		$(SXSRC)/read_a_sdag.c
READ_A_RE =		$(SXSRC)/read_a_re.c
SYMB_TBL_MNGR =		$(SXSRC)/sxsymbol_table_mngr.c
EQ_SORT =		$(SXSRC)/eq_sort.c	\
			$(SXSRC)/fermer.c
SXEARLEY_MAIN =		$(SXSRC)/sxearley_main.c
SXVOID =		$(SXSRC)/sxvoid.c

LIBSX =	$(SX_VARS)		\
	$(DEBUG_ACTION)		\
	$(SORT_BY_TREE)		\
	$(SX_ARGS_MANAGER)	\
	$(SX_AT_MANAGER)	\
	$(SX_BAG)		\
	$(SX_BITS_ARRAY)	\
	$(SX_CHAR_TO_STRING)	\
	$(SX_COMMENT_MANAGER)	\
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
	$(OUTPUT)		\
	$(FSA_MNGR)		\
	$(PUT_EDIT)		\
	$(READ_A_UDAG)		\
	$(READ_A_SDAG)		\
	$(READ_A_RE)		\
	$(SYMB_TBL_MNGR)	\
	$(EQ_SORT)		\
	$(SXEARLEY_MAIN)	\
	$(SX_SYNTAX_MANAGER)	\
	$(SXVOID)

LIBSXDET =	$(SX_VARS)		\
		$(SORT_BY_TREE) 	\
		$(SX_AT_MANAGER)	\
		$(SX_BITS_ARRAY)	\
		$(SX_CHAR_TO_STRING)	\
		$(SX_COMMENT_MANAGER)	\
		$(SX_ERROR_MANAGER)	\
		$(SX_ERROR_RECOVERY)	\
		$(SX_GET_BIT)		\
		$(SX_INDEX)		\
		$(SX_NEXT_PRIME)	\
		$(SX_PARAGRAPHEURS)	\
		$(SX_MEMORY_MANAGER)	\
		$(SX_PARSER)		\
		$(SX_SOURCE_MANAGER)	\
		$(SX_STRINGS_MANAGER)	\
		$(SX_SCANNER)		\
		$(SX_TOKEN_MANAGER)	\
		$(SX_TRAP)		\
		$(SS_MNGR)		\
		$(PUT_EDIT)		\
		$(VARSTR)		\
		$(SX_SYNTAX_MANAGER)	\
		$(SXVOID)




libsx_a_SOURCES = $(LIBSX)
libsx_a_CFLAGS = -O2 -Wuninitialized

libsx_debug_a_SOURCES = $(LIBSX)
libsx_debug_a_CFLAGS = -ggdb -DEBUG #-DLOG -DLLOG -DLLLOG

libsxdet_a_SOURCES = $(LIBSXDET)
libsxdet_a_CFLAGS = -O2 -Wuninitialized

libsxdet_debug_a_SOURCES = $(LIBSXDET)
libsxdet_debug_a_CFLAGS = -ggdb -DEBUG


install-data-hook:
	@list='$(pkglib_LIBRARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    f=`echo $$p | sed -e 's|^.*/||'`; \
	    l=`echo $$f | sed -e 's|.a$$|$(PACKAGE_VERSION).a|'`; \
	    if test -e "$(DESTDIR)$(pkglibdir)/$$l"; then rm $(DESTDIR)$(pkglibdir)/$$l; else :; fi; \
	    echo " mv '$(DESTDIR)$(pkglibdir)/$$f' '$(DESTDIR)$(pkglibdir)/$$l'"; \
	    mv "$(DESTDIR)$(pkglibdir)/$$f" "$(DESTDIR)$(pkglibdir)/$$l"; \
	    echo " ln -s '$(DESTDIR)$(pkglibdir)/$$l' '$(DESTDIR)$(pkglibdir)/$$f'"; \
	    ln -s "$(DESTDIR)$(pkglibdir)/$$l" "$(DESTDIR)$(pkglibdir)/$$f"; \
	  else :; fi; \
	done

dist-hook:
	if test -e $(SXINCL)/sxversion.h; then rm $(SXINCL)/sxversion.h; fi
	$(MAKE) $(SXINCL)/sxversion.h



%32.c	:	%.c
	(echo '#define SXP_MAX (SHRT_MAX+1)';echo '#define SXS_MAX (SHRT_MAX+1)';echo '#define VARIANT_32';echo '#include "$*.c"') > $@





libsxdebug :	sxmem_mngr.d.o	\
		dbgaction.d.o	\
		sxmain.o	\
		sxscanner.d.o	\
		sxsrc_mngr.d.o

sxmem_mngr.d.o:	$(SXSRC)/sxmem_mngr.c
	$(CC)  -ggdb -DBUG -DEBUG_ALLOC $(AM_CPPFLAGS) -c $(SXSRC)/sxmem_mngr.c;mv sxmem_mngr.o $@

dbgaction.d.o:	$(SXSRC)/dbgaction.c
	$(CC)  -ggdb -DBUG -DEBUG_ALLOC $(AM_CPPFLAGS) -c $(SXSRC)/dbgaction.c;mv dbgaction.o $@

sxmain.o:	$(SXSRC)/sxmain.c
	$(CC)  $(AM_CPPFLAGS) -c $(SXSRC)/sxmain.c

sxscanner.d.o:	$(SXSRC)/sxscanner.c
	$(CC)  -ggdb -DBUG -DEBUG_ALLOC $(AM_CPPFLAGS) -c $(SXSRC)/sxscanner.c;mv sxscanner.o $@

sxsrc_mngr.d.o:	$(SXSRC)/sxsrc_mngr.c
	$(CC)  -ggdb -DBUG -DEBUG_ALLOC $(AM_CPPFLAGS) -c $(SXSRC)/sxsrc_mngr.c;mv sxsrc_mngr.o $@

sxspf_mngr.d.o:	$(SXSRC)/sxspf_mngr.c
	$(CC)  -ggdb -DBUG -DEBUG -DLOG -DLLOG $(AM_CPPFLAGS) -c $(SXSRC)/sxspf_mngr.c;mv sxspf_mngr.o $@


$(SXINCL)/sxversion.h:
	$(top_srcdir)/etc/bin/make-sxversion $(top_srcdir)
