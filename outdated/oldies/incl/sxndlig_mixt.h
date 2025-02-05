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

struct unfold {
/* address = (rule_no, pile) */
  XxY_header		addresses;
  struct rule		*new_rule;

/* new_symbol = (symbol, pile) */
  XxY_header		new_symbols;
  bool		is_new_symbols;

/* hooks = (rhs_symb, rhs_pile) */
  XxY_header		hooks;
  struct uf_hook2attr {
    int		item;
  }			*hook2attr;

  struct grammar	*new_grammar;

  int			hook_rule, rule_top, new_grammar_top, new_grammar_size;
};


/* Structure "globale" partagee par tous les "ligparsact". */
struct sxndlig_common {
    struct sxligparsact	code;
    SXTABLES	*sxtables;
};


struct sxndlig {
    /* Structures persistantes */
    /* main_trans = (son, father) : son est un parser, si father > 0, c'est un parser
       sinon c'est un xtriple. */
    XxY_header		main_trans; /* (son, father) */
    struct main_trans2attr {
	int		main_symbol;
	int		pile; /* Si pile & LIG_80 != 0 , il y a des objets */
	int		temp;
    }			*main_trans2attr;
    /* Stocke les trans dont le 2eme champ est <0 et qui devra
       etre detruit en quittant le niveau courant (dans sxndlig_action_final). */
    int			*main_trans_to_be_erased;

    /* pile = (pile, elem) | 0 */
    XxY_header		piles;

    struct symb2attr {
	int		pile;
    }			*symb2attr;
    SXBA		symbol_set;

    XxY_header		symbXpile;
    SXBA		symbXpile_set;
    /* aux_trans = (main_trans, aux_symbol) */
    /* XxY_header		aux_trans; */

    /* object = (main_trans, pile) */
    /* Contient l'union des piles associees aux symboles de main_trans. Si recognizer
       c'est la seule structure remplie. */
    XxY_header		object;
    struct object2attr {
	int		temp;
    }			*object2attr;

    /* dependancy = (aux_trans, main_trans)  Si aux_trans&LIG_80 == 0, c'est une main_trans */
    XxY_header		dependancies;
    struct dependancies2attr {
	int		act_no,
	                prdct_no;
    }			*dependancies2attr;

    XxY_header		paths;

    XxYxZ_header	failed;

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
                        main_symbol,
                        cur_symbol,
                        cur_main_trans,
                        cur_trans;

    bool		is_paths,
                        is_secondary,
                        is_old_main_trans;

    SXBA		already_computed;
    int			*to_be_processed, tbp_top;

    struct rule2attr
    {
	int		count, act_no, symbol;
    }			*rule2attr;

    struct grammar2attr
    {
	int		prdct_no;		
    }			*grammar2attr;

    struct hook2attr {
	int		pile;
    }			*hook2attr;
    XxY_header		hookXpile;

    struct unfold 	uf;

    struct for_postparsact
	                for_postparsact;
};
