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
#define pt_num_version 6
#include "sxcommon.h"
struct P_constants {
	SXINT tmax;
	SXINT xtmax;
	SXINT nbpro;
	SXINT xnbpro;
	SXINT ntmax;
	SXINT Mtstate;
    	SXINT Mntstate;
    	SXINT Mvec;
    	SXINT mgerme;
    	SXINT Mgerme;
    	SXINT deltavec;
    	SXINT init_state;
    	SXINT final_state;
    	SXINT mMprdct;
    	SXINT mMred;
    	SXINT mMrednt;
    	SXINT Mprdct;
    	SXINT Mfe;
    	SXINT M0ref;
    	SXINT Mref;
    	SXINT nd_degree;
    	SXINT Mprdct_list;
	SXINT sem_kind;
   };


struct P_reduction {
    SXINT		action,
                reduce,
                lgth,
                lhs;
    };

struct P_item {
    SXINT		check,
                action;
    };

struct P_prdct {
    SXINT		prdct,
                action;
    };

struct P_base {
    SXINT		defaut,
                commun,
                propre;
    };

struct P_prdct_list {
    SXINT		red_no,
                pos,
                prdct;
    };

struct parser_ag_item {
	long			bnf_modif_time;
	struct P_constants 	constants; 
	struct P_reduction 	*reductions 	/* 1:Mred */ ;
	struct P_base		*t_bases	/* 1:Mtstate */ ,
	                        *nt_bases	/* 1:Mntstate */ ;
	struct P_item	        *vector	   	/* deltavec+1:deltavec+Mvec */ ;
	struct P_prdct          *prdcts         /* Mred+1:Mprdct */ ;
	SXINT			*germe_to_gr_act/* mgerme+1:Mgerme */;
	struct P_prdct_list	*prdct_list	/* 1:Mprdct_list */;
   };
