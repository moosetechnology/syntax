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
/*------------------------------*/
/*          sxdico.h          	*/
/*------------------------------*/

#ifndef sxdico_h
#define sxdico_h

#include "sxba.h"

struct dico {
    SXUINT      max;
    SXBA_INDEX          init_base;
    SXUINT       class_mask, stop_mask, is_suffix_mask;
    SXUINT 	base_shift, partiel;
    unsigned char	*char2class;
    SXBA_INDEX          *comb_vector;
};

struct mot2 {
    SXINT 	nb, size, optim_kind, process_kind, total_lgth, print_on_sxtty;
    char 	**string;
    SXINT 	*lgth, *code, *min, *max;
};

extern SXBA_INDEX  sxdico_get         (struct dico *dico, unsigned char **kw, SXINT *kwl);
extern SXBA_INDEX  sxdico_get_bounded (struct dico *dico, unsigned char **kw, SXINT *kwl, SXINT kind_bound);
extern void sxdico_process            (struct mot2 input, struct dico *output);
extern void sxdico2c                  (struct dico *sxdico2c_dico, 
				       FILE *file, 
				       char *dico_name, 
				       SXBOOLEAN is_static);

#define TOTAL        0
#define PARTIEL      1

#define TIME         0
#define SPACE        1
#define PREFIX_BOUND 2
#define SUFFIX_BOUND 3


#endif /* #ifndef sxdico_h */




