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

#ifndef sxalloc_h
#define sxalloc_h
#include "SXante.h"
#include "sxcommon.h"

SX_GLOBAL_VAR SXBOOLEAN SYNTAX_is_in_critical_zone;
SX_GLOBAL_VAR void *sxcont_malloc (size_t size), *sxcont_alloc (size_t n, size_t size), *sxcont_realloc (void *table, size_t size), *sxcont_recalloc (void *table, size_t old_size, size_t new_size);
SX_GLOBAL_VAR void sxcont_free (void *zone);
SX_GLOBAL_VAR SXBOOLEAN sxpush_in_tobereset_signature (void *ptr, size_t size);

#define sxprepare_for_possible_reset(data) (mem_signature_mode ? sxpush_in_tobereset_signature ((void*) &(data), sizeof (data)) : SXFALSE)
#define sxprepare_ptr_for_possible_reset(ptr,size) (mem_signature_mode ? sxpush_in_tobereset_signature ((void*) (ptr), (size)) : SXFALSE)

#ifdef lint

/* the definitions of malloc(), calloc(), realloc() ... are here -> */
#include <stdlib.h>


#define sxalloc(NB, SZ)		malloc ((size_t)((NB)*(SZ)))
#define sxcalloc(NB, SZ)		calloc ((size_t)(NB), (size_t)(SZ))
#define sxrealloc(PTR, NB, SZ)	realloc ((char*)(PTR), (size_t)((NB)*(SZ)))
#define sxrecalloc(PTR, NB1, NB2, SZ)	realloc ((char*)(PTR), (size_t)((NB1)*(SZ)+(NB2)*(SZ)))
#define sxfree(PTR)			free ((char*)(PTR))

#else

#define sxalloc(NB, SZ)		sxcont_malloc ((size_t)((NB)*(SZ)))
#define sxcalloc(NB, SZ)		sxcont_alloc ((size_t)(NB), (size_t)(SZ))
#define sxrealloc(PTR, NB, SZ)	sxcont_realloc ((void*)(PTR), (size_t)((NB)*(SZ)))
#define sxrecalloc(PTR, NB1, NB2, SZ)	sxcont_recalloc ((void*)(PTR), (size_t)((NB1)*(SZ)), (size_t)((NB2)*(SZ)))
#define sxfree(PTR)			sxcont_free ((void*)(PTR))

#endif /* lint */

extern void * sx_alloc_and_copy (size_t nbemb, size_t size, void *original_content, size_t copy_size);

SX_GLOBAL_VAR SXBOOLEAN mem_signature_mode;

struct mem_signature {
  SXINT top, size;
  void* *ptr_stack;
};

SX_GLOBAL_VAR struct mem_signatures {
  struct mem_signature alloc_signature, free_signature, tobereset_signature;
  size_t *tobereset_sizes;
} *mem_signatures;

SX_GLOBAL_VAR void sxmem_signatures_allocate (SXINT size), sxmem_signatures_raz (void), sxmem_signatures_free (void);

SX_GLOBAL_VAR void (*sxgc_recovery) (void);

#include "SXpost.h"
#endif /* sxalloc_h */

