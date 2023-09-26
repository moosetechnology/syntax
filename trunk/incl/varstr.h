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

#ifndef varstr_h
#define varstr_h

#include "sxcommon.h"

typedef struct varstr_hd {
   char *first, *current, *last;
   } *VARSTR;

extern VARSTR varstr_alloc	 	(SXINT size);
extern VARSTR varstr_realloc	 	(VARSTR varstr_ptr);
extern void   varstr_free	 	(VARSTR varstr_ptr);
extern VARSTR varstr_raz	 	(VARSTR varstr_ptr);
extern VARSTR varstr_rcatenate	 	(VARSTR varstr_ptr, char *s, SXINT l);
extern VARSTR varstr_lcatenate	 	(VARSTR varstr_ptr, char *s, SXUINT l);
extern VARSTR varstr_catenate	 	(VARSTR varstr_ptr, char *s);
extern VARSTR varstr_quote	 	(VARSTR varstr_ptr, char *s);
extern VARSTR varstr_cat_cstring 	(VARSTR varstr_ptr, char *s);
extern VARSTR varstr_lcat_cstring	(VARSTR varstr_ptr, char *s, SXUINT l);
extern VARSTR varstr_modify		(VARSTR varstr_ptr, SXINT i, SXINT l, char *s, SXINT sl);
extern void   vncpy			(char *s1, char *s2, SXINT l2);
extern VARSTR varstr_lcatenate_with_escape (VARSTR varstr_ptr, char *string, SXUINT string_lgth, char *escape_string);
extern VARSTR varstr_lcatenate_with_escape_except_for_comments (VARSTR varstr_ptr, char *string, SXUINT string_lgth, char *escape_string);

#define varstr_catchar(v,c)	((*((v)->current++) = c),\
                                ((v)->current == (v)->last) ? varstr_realloc (v) : (v))

#define varstr_complete(v)	((*((v)->current) = '\0'), v)
   /*
    * '\0' et pas SXNUL pour pouvoir l'inclure avant (ou sans) sxunix.h,
    * par exemple pour profiter du mecanisme de controle sur sxba2c
    */

#define varstr_maxlength(v)	((v)->last - (v)->first)
#define varstr_length(v)	((v)->current - (v)->first)
#define varstr_length_not_null(v)       ((v)->current != (v)->first)
#define varstr_tostr(v)		((v)->first)
#define varstr_pop(v,n)		((v)->current -= n)

#define varstr_catenate_with_escape(v,c,s)  (varstr_lcatenate_with_escape (v, c, (SXINT) strlen (c), s))

#endif /* varstr_h */

