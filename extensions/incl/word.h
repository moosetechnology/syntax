/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/
/* This include file to provide a much agreeable interface to sxword_mngr */

#ifndef word_h
#define word_h

#define SXEMPTY_STE		SXEMPTY_STE	
#define ERROR_STE		SXERROR_STE
#define word_alloc		sxword_alloc
#define word_clear		sxword_clear
#define word_free		sxword_free
#define WORD_get		SXWORD_get
#define word_get		sxword_get
#define word_header		sxword_header
#define WORD_len		SXWORD_len
#define word_len		sxword_len
#define WORD_retrieve		SXWORD_retrieve
#define word_retrieve		sxword_retrieve
#define word_2retrieve		sxword_2retrieve
#define WORD_save		SXWORD_save
#define word_save		sxword_save
#define word_2save		sxword_2save
#define WORD_size		SXWORD_size
#define word_stat		sxword_stat
#define WORD_test_and_save	SXWORD_test_and_save
#define WORD_test_and_2save	SXWORD_test_and_2save
#define WORD_top		SXWORD_top

#include "sxword.h"

#endif
