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
#ifndef bag_h
#define bag_h
/* Gestionnaire de SXBA */

#include "sxba.h"

typedef struct {
    struct bag_disp_hd {
	SXBA_INDEX	size;
	SXBA_ELT	*set;
    }		*hd;
    SXBA_ELT	*pool_top;
    SXBA_INDEX	hd_size, hd_top, hd_high, pool_size, room;
    char	*name;
    SXBA_INDEX	used_size, prev_used_size, total_size; /* #if EBUG */
} bag_header;

extern void bag_alloc (bag_header *pbag, char *name, SXINT size); /* size = nb initial de SXBA_ELT */
extern SXBA bag_get (bag_header *pbag, ssize_t size); /* size = nb de bits */
extern void bag_free (bag_header *pbag);
extern void bag_clear (bag_header *pbag); /* Ne remet pas a zero les zones restituees */
#endif
