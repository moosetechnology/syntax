/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/
#ifndef SS_h
#define SS_h

#include "sxalloc.h"

extern SXINT		*SS_alloc (SXINT size);
/* extern SXINT		SS_sature (SXINT **SS_ptr_ptr); */
#define SS_sature(t)    (SXINT*) sxrealloc ((t)-1, ((t)[-1] *= 2)+2, sizeof (SXINT))+1
#define SS_free(t)	sxfree((t)-1)
#define SS_get(t,x)	((t)[x])
#define SS_top(t)	(*(t))
#define SS_bot(t)	((t)[1])
#define SS_get_top(t)	((t)[SS_top(t)-1])
#define SS_get_bot(t)	((t)[SS_bot(t)])
#define SS_size(t)	((t)[-1])
#define SS_decr(t)	(--SS_top(t))
/* ATTENTION : utilisation de &(t) ... */
/* #define SS_push(t,e)	((t)[SS_top(t)++]=(e),SS_top(t)>SS_size(t)?SS_sature(&(t)):(SXINT)0) */
#define SS_push(t,e)	((t)[SS_top(t)++]=(e),t=(SS_top(t)>SS_size(t))?SS_sature(t):(t))
#define SS_pop(t)	(SS_get(t,SS_decr(t)))
#define SS_open(t)	(SS_push(t,SS_bot(t)),SS_bot(t)=SS_top(t))
#define SS_close(t)	(SS_bot(t)=SS_get(t,SS_top(t)=SS_bot(t)-1))
#define SS_is_empty(t)	(SS_bot(t)==SS_top(t))
#define SS_is_ss_empty(t)	(SS_is_empty(t)&&SS_bot(t)==2)
#define SS_clear(t)	SS_top(t)=SS_bot(t)
#define SS_clear_ss(t)	SS_clear(t)=2

#endif
