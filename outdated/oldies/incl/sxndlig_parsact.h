#define SECONDARY	0
#define PRIMARY		1

#define SET_NIL		(((unsigned int) (~0))>>1) /* 7F */
#define LIG_7F		SET_NIL
#define LIG_80		(~SET_NIL)

struct for_postparsact {
    int		(*action_top) (),
    		(*action_pop) (),
    		(*action_bot) (),
    		(*action_new_top) (),
    		(*action_final) (),
    		(*GC) (),
      		(*post) ();
};

/* Structure "globale" partagee par tous les "ligparsact". */
struct sxndlig_common {
    struct sxligparsact	code;
    SXTABLES	*sxtables;
};


struct sxndlig {
/* Structures persistantes */
/* main_trans = (son, father) : son est un parser, si father > 0, c'est un parser
                               sinon c'est une ref. */
    XxY_header		main_trans; /* (son, father) */
    struct main_trans2attr {
	int		main_symbol, /* Si < 0 il y a des nt auxiliaires. */
	                sigma, /* Si < 0 il y a des objets associes. */
	                pile,
	                temp;
    }			*main_trans2attr;
    int			*main_trans_to_be_erased;

/* Structures memorisant les piles des LIGs */
/* aux_trans = (main_trans, symbol) : symbol est auxiliaire (<0). */
#if 0
    XxY_header		aux_trans;
    struct aux_trans2attr {
	int		sigma, /* Si & LIG_80 il y a des objets associes. */
	                pile, /* Si & LIG_80 il y a des piles associees ds recognizer. */
	                temp;
    }			*aux_trans2attr;
#endif
/* object = (aux_trans, sigma) Si aux_trans >0, c'est un main_trans, sinon une aux_trans */
    XxY_header		object;
    struct object2attr {
	int		pile,
	                temp;
    }			*object2attr;

/* sigma = (symbol, sigma) | 0 */
    XxY_header		sigma;

/* pile = (pile, elem) | 0 */
    XxY_header		piles;

/* dependancy = (aux_trans, main_trans)  Si aux_trans >0, c'est un main_trans */
    XxY_header		dependancies;
    struct dependancies2attr {
	int		act_no,
	                prdct_no;
    }			*dependancies2attr;

    XxY_header		paths;

    XxYxZ_header	failed;

    SXBA		GC_sigma_set,
                        sigma_set,
                        symbol_set,
                        GC_pile_set;
    int			*GC_to_be_processed;

/* si on est en reconnaisseur */
    XxY_header		recognizer; /* (main_trans, pile) */
    struct recognizer2attr {
	int		temp;
    }			*recognizer2attr;

    int			bot,
                        xtriple,
                        *pc,
                        *pclim,
                        *pprdct_bot,
                        *pprdct_top,
                        level,
                        act_no,
                        prdct_no,
                        prev_main_trans,
                        cur_symb,
                        cur_trans,
                        last_main_symb;

    bool		is_paths,
                        is_secondary,
                        is_old_main_trans;

    struct for_postparsact
	                for_postparsact;
};

