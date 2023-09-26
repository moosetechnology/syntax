#define ARN_7F		(((unsigned int) (~0))>>1) /* 7F */
#define ARN_80		(~ARN_7F)

struct for_postparsact {
    int		(*action_top) (),
    		(*action_pop) (),
    		(*action_bot) (),
    		(*action_new_top) (),
    		(*action_final) (),
    		(*GC) (),
      		(*post) ();
};

struct symbol_set2attr {
  int	bot, new_top, local_link, multiple_symbol;
};

struct pXs2attr {
  int	new_top, local_link;
};

struct local_trans2attr {
  int	symb; 
};

struct symb2attr {
  int	trans;
};

struct parser2attr {
  int	val;
};

struct son2attr {
  int	val;
};

struct path2attr {
  int	val;
};

struct level_trans2attr {
  int		val;
  BOOLEAN	to_be_kept;
};

struct ARN {
    struct sxtables	*sxtables;

#if 0
    SXBA		symbol_set;
    int			*symbol_stack;
    struct symb2attr	*symb2attr;
#endif

    SXBA		vanished_parser_set;
    int			*vanished_parser_stack;
    struct parser2attr	*parser2attr;

    struct son2attr	*son2attr;

    XxY_header		paths;
    struct path2attr	*path2attr;

    XxY_header		level_trans;
    struct level_trans2attr
	                *level_trans2attr;

    XxY_header		symbXtrans;

    int			*stack;
    int			stack_size;

    X_header		symbol_set;
    struct symbol_set2attr
	                *symbol_set2attr;

    XxY_header		pXs;
    struct pXs2attr	*pXs2attr;

    XxY_header		local_trans;
    struct local_trans2attr
	                *local_trans2attr;

    X_header		rhs_set;

    int			bot,
                        xtriple,
                        level,
                        act_no,
                        cur_symb,
                        cur_trans,
                        multiple_symbol;

    BOOLEAN		is_old_trans;

    struct for_postparsact
	                for_postparsact;
};

