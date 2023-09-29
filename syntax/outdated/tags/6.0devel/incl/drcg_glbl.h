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
/* Variables globales des analyseurs DRCG. */
#include "rcg_glbl.h";


struct drcg_struct {
    struct {
	SXINT max_var, max_gvar, rhs_maxnt, max_atom, max_term, max_list, has_integer_op, maxpppprod, is_dynam_atom,
	    logvar, has_prolog, max_gvarppp, max_varppp; 
    } cst;

    SXINT		**param_nb, *var_nb, *gvar_disp, *gvar_name, *term_disp, *term_list, *list_list, ***term;
    SXINT		*prod2da_disp, *prod2da, *ppp_prolon, *ppp_lispro, *ppp_term_hd, *ppp_term;
    char	**atom_str, **var_str, **t2str, **ppp2str;
    SXBA_ELT	**var2ppp_item_set; 
};

struct drcg_struct	drcg [];

void		DRCGsem_fun_init(), DRCGsem_fun_final();
SXBOOLEAN		DRCGsem_fun(), DRCGsem_fun_post();

/* Pour le traitement d'erreur depuis RCG_parser.c */
struct drcg_tree2attr {
    SXINT		prod, pid, Aij, next, node_list, var_hd;
    SXBA	tree_set;
#if DRCG_is_cyclic==1
    SXINT		cyclic_tree_id;
#endif
};

struct drcg_tree2attr	*drcg_tree2attr;
SXINT			*Aij2tree;
SXBA			root_set;

void		DRCGsubstitute ();

#include "XxY.h"
XxY_header	dynam_list;

