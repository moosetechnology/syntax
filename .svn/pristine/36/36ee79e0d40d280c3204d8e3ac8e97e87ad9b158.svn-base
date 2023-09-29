/* Variables globales des analyseurs DRCG. */
#include "rcg_glbl.h";


struct drcg_struct {
    struct {
	int max_var, max_gvar, rhs_maxnt, max_atom, max_term, max_list, has_integer_op, maxpppprod, is_dynam_atom,
	    logvar, has_prolog, max_gvarppp, max_varppp; 
    } cst;

    int		**param_nb, *var_nb, *gvar_disp, *gvar_name, *term_disp, *term_list, *list_list, ***term;
    int		*prod2da_disp, *prod2da, *ppp_prolon, *ppp_lispro, *ppp_term_hd, *ppp_term;
    char	**atom_str, **var_str, **t2str, **ppp2str;
    SXBA_ELT	**var2ppp_item_set; 
};

struct drcg_struct	drcg [];

void		DRCGsem_fun_init(), DRCGsem_fun_final();
BOOLEAN		DRCGsem_fun(), DRCGsem_fun_post();

/* Pour le traitement d'erreur depuis RCG_parser.c */
struct drcg_tree2attr {
    int		prod, pid, Aij, next, node_list, var_hd;
    SXBA	tree_set;
#if DRCG_is_cyclic==1
    int		cyclic_tree_id;
#endif
};

struct drcg_tree2attr	*drcg_tree2attr;
int			*Aij2tree;
SXBA			root_set;

void		DRCGsubstitute ();

#include "XxY.h"
XxY_header	dynam_list;

