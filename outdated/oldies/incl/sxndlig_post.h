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
    struct sxtables	*sxtables;
};


struct sxndlig {
  struct ctrl_stack	reached;

  struct rule2attr
  {
    int		count, pile_nb, pile, act_no;
  }			*rule2attr;

  struct grammar2attr
  {
    int		prdct_no;		
  }			*grammar2attr;

/* pile = (pile, elem) | 0 */
  XxY_header		piles;

/* object = (rule_no, pile) */
  XxY_header		objects;
  SXBA			object_set;

  struct unfold 	uf;

  struct for_postparsact
	                for_postparsact;
};
