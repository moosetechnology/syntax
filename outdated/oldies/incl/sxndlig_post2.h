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


struct R_attr {
  int		item, kind;
};

struct R {
  SXBA		left_item_set, right_item_set;
  struct R_attr	*left_attr, *right_attr;
  XxYxZ_header	graph;
};

struct memo2attr {
  int		kind;
  SXBOOLEAN	result;
};

/* Structure "globale" partagee par tous les "ligparsact". */
struct sxndlig_common {
    struct sxligparsact	code;
    struct sxtables	*sxtables;
};


struct sxndlig {
  SXBA			*working_sets;

  struct rule2attr {
    int		act_no;
  }			*rule2attr;

  struct grammar2attr {
    int		prdct_no;		
  }			*grammar2attr;

  int			R0_size;

  SXBA			orig_set, final_set, inf_orig_set, sup_final_set, valid_set;
  struct R		R1, R2, R3, *Rkm1, *Rk;

  int			fill_trans_k_called_nb;

  XxY_header		memo;
  struct memo2attr	*memo2attr;

  struct for_postparsact
	                for_postparsact;

  SXBOOLEAN		is_cyclic;
};
