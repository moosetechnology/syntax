/* ********************************************************
 *                                                      *
 *                                                      *
 * Copyright (c) 1985 by Institut National de Recherche *
 *                    en Informatique et en Automatique *
 *                                                      *
 *                                                      *
 ******************************************************** */



/* ********************************************************
 *                                                      *
 *              Produit de l'equipe ChLoE.		  *
 *                	     (PB & FT)			  *
 *                                                      *
 ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 09-07-92 10:07 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)sxindparser.c\t- SYNTAX [unix] - 31 Juillet 1992";

    static char	ME [] = "INDPARSER";
    
    
    
    /************************************************************************/
    /* It is the incremental nondeterministic version of the SYNTAX parser	*/
    /* This algorithm is borrowed from Jan Rekers' thesis			*/
    /************************************************************************/
    
    /*
      The parse_stack is a directed graph whose nodes are called parse_stack_elem
      Each parse_stack_elem is a couple (state, token_nb) where state is an element
      of the underlying PDA and token_nb the indexes of the current_token at the
      moment where this parse_stack_elem was built.
      The active_parsers are a set of parse_stack_elem.
      The parse_stack is a set of couples (parse_stack_elem, parse_stack_elem).
      */
    
# include <stdio.h>
# include "sxindparser.h"
    
#define state_occur_maxnb 50 /* nombre maximal d'occurences d'un etat LR dans la pile */
    
    
    extern bool	bnf_read ();
    extern void	bnf_free ();
    
    
    
    struct common_struct
{
  SXBA		*worthy_red;
  int		max_red_nb;
} common_struct;



struct rhs {
  int		symbol, next;
};

struct  parse_stack_elem 
{
  int		state, red_ref, shift_ref, son, level, symbol;
};

struct G {
  int		N, T, P, G;
};

struct red_parsers {
  int parser, lnk;
};


static struct parse_stack {
  struct parse_stack_elem	*graph;
  
  struct red_parsers		*red_parsers_list;
  
  XxY_header			sons,
                                suffixes,
                                ranges,
                                symbols;
  
  XH_header			output_grammar;
  
  SXBA				*parsers_set;
  
  int	                        *parsers_rhs0,
                                *parsers_rhs1;
  
  SXBA				current_links,
                                used_links,
                                free_parsers_set,
                                erasable_sons;
  
  int				*active_states,
                                *active_parsers,
                                *GC_area,
                                *red_stack,
                                *red_parsers_hd,
                                *for_shifter,
                                *for_scanner,
                                *for_actor;
  
  int				start_symbol,
                                graph_size,
                                graph_top,
                                rhs_size,
                                rhs_top,
                                red_parsers_list_top,
                                red_parsers_list_size,
                                free_parser,
                                current_level,
                                current_token;
  
  int			        *transitions,
                                red_no,
                                current_symbol;
  
  struct rhs			*rhs;
  
  struct G			G;
  
  bool			is_new_links;
} parse_stack;


struct ltd_parse_stack_elem {
  int		state, red_ref, shift_ref, son, symbol;
};

struct occurence {int number, last};


static struct ltd_parse_stack {
  struct ltd_parse_stack_elem 	*graph;
  
  XxY_header			sons;
  
  SXBA				*parsers_set,
                                family_Mb,
                                *families;
  
  SXBA				current_links,
                                free_parsers_set,
                                erasable_sons,
                                used_links;
  
  int				*active_states,
                                *active_parsers,
                                *GC_area,
                                *for_shifter,
                                *for_scanner,
                                *for_actor;
  
  struct occurence		*lr_state_occ;
  
  int				start_symbol,
                                free_parser,
                                current_token;
  
  int			        *transitions,
                                red_no,
                                graph_top,
                                graph_size;
  
  bool			is_new_links, 
                                parse_time_built;
  
} ltd_parse_stack;


static  void sons_oflw (old_size, new_size)
    int		old_size, new_size;
{
  parse_stack.erasable_sons = sxba_resize (parse_stack.erasable_sons, new_size + 1);
  parse_stack.current_links = sxba_resize (parse_stack.current_links, new_size + 1);
  parse_stack.used_links = sxba_resize (parse_stack.used_links, new_size + 1);
  parse_stack.transitions = (int*) sxrealloc (parse_stack.transitions, 
					      new_size + 1, 
					      sizeof (int));
}


static void GC (p)
    int p;
{
  register int	x, father, son, lim;
  register SXBA	free_parsers_set, erasable_sons;
  bool		is_in_sons = false;
  
  sxba_fill (erasable_sons = parse_stack.erasable_sons);
  sxba_fill (free_parsers_set = parse_stack.free_parsers_set);
  
  SS_push (parse_stack.GC_area, p);
  SXBA_0_bit (free_parsers_set, p);
  
  for (lim = SS_top (parse_stack.for_scanner), x = SS_bot (parse_stack.for_scanner);
       x < lim;
       x++) 
    {
      father = SS_get (parse_stack.for_scanner, x);
      
      if (SXBA_bit_is_set_reset (free_parsers_set, father))
	  SS_push (parse_stack.GC_area, father);
    }
  
  /* Not usefull, for_actor is always a subset of active_states. 
     for (lim = SS_top (parse_stack.for_actor), x = SS_bot (parse_stack.for_actor);
     x < lim;
     x++) 
     {
     father = SS_get (parse_stack.for_actor, x);
     
     if (SXBA_bit_is_set_reset (free_parsers_set, father))
     SS_push (parse_stack.GC_area, father);
     }
     */
  
  x = 0;
  
  while ((x = parse_stack.for_shifter [x]) > 0) 
    {
      father = parse_stack.active_parsers [x];
      
      if (SXBA_bit_is_set_reset (free_parsers_set, father))
	  SS_push (parse_stack.GC_area, father);
    }
  
  x = 0;
  
  while ((x = parse_stack.active_states [x]) > 0) 
    {
      father = parse_stack.active_parsers [x];
      
      if (SXBA_bit_is_set_reset (free_parsers_set, father))
	  SS_push (parse_stack.GC_area, father);
    }
  
  do 
    {
      son = SS_pop (parse_stack.GC_area);
      
      while ((son = parse_stack.graph [father = son].son) > 0) 
	{
	  if (!SXBA_bit_is_set_reset (free_parsers_set, son))
	      break;
	}
      
      if (son < 0) 
	{
	  is_in_sons = true;
	  son = -son;
	  
	  if (SXBA_bit_is_set_reset (free_parsers_set, son))
	      SS_push (parse_stack.GC_area, son);
	  
	  XxY_Xforeach (parse_stack.sons, father, x) 
	    {
	      SXBA_0_bit (erasable_sons, x);
	      son = XxY_Y (parse_stack.sons, x);
	      
	      if (SXBA_bit_is_set_reset (free_parsers_set, son))
		  SS_push (parse_stack.GC_area, son);
	    }
	}
    } while (!SS_is_empty (parse_stack.GC_area));
  
  if (is_in_sons) 
    {
      x = XxY_top (parse_stack.sons) + 1;
      
      while ((x = sxba_1_rlscan (erasable_sons, x)) > 0)
	  XxY_erase (parse_stack.sons, x);
    }
  
  parse_stack.free_parser =
      parse_stack.graph_top =
	  ((x = sxba_0_rlscan (free_parsers_set, 
			       parse_stack.graph_size + 1)) < 0) ? 0 : x;
}



static bool new_symbol (p, k, i, j, symbol)
    int p, k, i, j, *symbol;
{
  int kind, range;
  
  if (k > 0) 
    {
      /* Suffixe sharing. */
      XxY_set (&parse_stack.suffixes, p, k, &kind);
      kind += sxplocals.SXP_tables.P_nbpro;
    }
  else
      kind = p;
  
  if (p >= 0)
      XxY_set (&parse_stack.ranges, i, j, &range);
  else
      range = j;
  
  return XxY_set (&parse_stack.symbols, kind, range, symbol);
}


static void output_symbol (symbol)
    int symbol;
{
  /* Assume symbol > 0 => not a terminal string. */
  int kind, range, i, j;
  
  if (symbol < 0) 
    {
      symbol = -symbol;
      
      if (symbol <= sxplocals.SXP_tables.P_tmax)
	  printf ("\"%s\" ", sxttext (sxsvar.sxtables, symbol));
      else 
	  printf ("\"%s\" ", sxstrget (symbol - sxplocals.SXP_tables.P_tmax));
      
    }
  else 
    {
      kind = XxY_X (parse_stack.symbols, symbol);
      range = XxY_Y (parse_stack.symbols, symbol);
      
      if (kind < 0)
	  printf("<%i, %i> ", kind, range);
      else 
	{
	  i = XxY_X (parse_stack.ranges, range);
	  j = XxY_Y (parse_stack.ranges, range);
	  
	  if (kind > sxplocals.SXP_tables.P_nbpro) 
	    {
	      kind -= sxplocals.SXP_tables.P_nbpro;
	      printf ("<%i, %i, %i, %i> ", XxY_X (parse_stack.suffixes, kind),
		      XxY_Y (parse_stack.suffixes, kind), i, j);
	    }
	  else
	      printf ("<%i, %i, %i> ", kind, i, j);
	}
    }
}


static void output_grammar_rule (rule_no)
    int rule_no;
{
  int x, lim;
  
  lim = XH_X (parse_stack.output_grammar, rule_no + 1);
  x = XH_X (parse_stack.output_grammar, rule_no);
  output_symbol (XH_list_elem (parse_stack.output_grammar, x));
  printf ("\t= ");
  
  while (++x < lim) 
    {
      output_symbol (XH_list_elem (parse_stack.output_grammar, x));
    }
  
  printf (";\n");
}


static void  set_t_rule (symbol)
    int	symbol;
{
  int		kind, range, t, rule_no;
  
  kind = XxY_X (parse_stack.symbols, symbol);
  range = XxY_Y (parse_stack.symbols, symbol);
  t = (sxgenericp (sxsvar.sxtables, -kind)) ?
      -(SXGET_TOKEN (range).string_table_entry + sxplocals.SXP_tables.P_tmax) :
	  kind;
  XH_push (parse_stack.output_grammar, symbol);
  XH_push (parse_stack.output_grammar, t);
  XH_set (&parse_stack.output_grammar, &rule_no);
}  


static void  set_unit_rule (lhs, rhs)
    int		lhs, rhs;
{
  int rule_no;
  
  XH_push (parse_stack.output_grammar, lhs);
  XH_push (parse_stack.output_grammar, rhs);
  XH_set (&parse_stack.output_grammar, &rule_no);
}  


static void  set_rule (lhs, rhs)
    int			lhs, rhs;
{
  int				rule_no;
  
  XH_push (parse_stack.output_grammar, lhs);
  
  if (rhs < 0)
      XH_push (parse_stack.output_grammar, -rhs);
  else 
    {
      while (rhs > 0) 
	{
	  XH_push (parse_stack.output_grammar, parse_stack.rhs [rhs].symbol);
	  rhs = parse_stack.rhs [rhs].next;
	}
    }
  
  XH_set (&parse_stack.output_grammar, &rule_no);
}



static void push_rp (ref, p)
    int ref, p;
{
  int		q;
  struct red_parsers	*ared_parsers;
  
  if (parse_stack.red_stack [ref] == 0) 
    {
      PUSH (parse_stack.red_stack, ref);
      parse_stack.red_parsers_hd [ref] = -p;
    }
  else 
    {
      if (++parse_stack.red_parsers_list_top >= parse_stack.red_parsers_list_size)
	  parse_stack.red_parsers_list = (struct red_parsers*)
	      sxrealloc (parse_stack.red_parsers_list,
			 (parse_stack.red_parsers_list_size *= 2) + 1,
			 sizeof (struct red_parsers));
      
      ared_parsers = parse_stack.red_parsers_list + parse_stack.red_parsers_list_top;
      
      if ((q = parse_stack.red_parsers_hd [ref]) < 0) 
	{
	  ared_parsers->parser = -q;
	  ared_parsers->lnk = 0;
	  q = parse_stack.red_parsers_hd [ref] = parse_stack.red_parsers_list_top;
	  ared_parsers++, parse_stack.red_parsers_list_top++;
	}
      
      ared_parsers->parser = p;
      ared_parsers->lnk = q;
      parse_stack.red_parsers_hd [ref] = parse_stack.red_parsers_list_top;
    }
}



static void  set_start_symbol (symbol)
    int		symbol;
{
  int		symb;
  /* Halt */
  /* Several "final" parse_stack elem could occur since they are not
     set in "active_parsers". */
  
  if (parse_stack.start_symbol == 0) 
    {
      new_symbol (0, 0, 0, parse_stack.current_level, &symb);
      parse_stack.start_symbol = symb;
    }
  
  set_unit_rule (parse_stack.start_symbol, symbol);
}



static void ltd_GC (p)
    int p;
{
  register int	x, father, son, lim;
  register SXBA	free_parsers_set, erasable_sons;
  bool		is_in_sons = false;
  
  sxba_fill (erasable_sons = ltd_parse_stack.erasable_sons);
  sxba_fill (free_parsers_set = ltd_parse_stack.free_parsers_set);
  
  SS_push (ltd_parse_stack.GC_area, p);
  SXBA_0_bit (free_parsers_set, p);
  
  for (lim = SS_top (ltd_parse_stack.for_scanner), x = SS_bot (ltd_parse_stack.for_scanner);
       x < lim;
       x++) 
    {
      father = SS_get (ltd_parse_stack.for_scanner, x);
      
      if (SXBA_bit_is_set_reset (free_parsers_set, father))
	  SS_push (ltd_parse_stack.GC_area, father);
    }
  
  /* Not usefull, for_actor is always a subset of active_states. 
     for (lim = SS_top (ltd_parse_stack.for_actor), x = SS_bot (ltd_parse_stack.for_actor);
     x < lim;
     x++) 
     {
     father = SS_get (ltd_parse_stack.for_actor, x);
     if (SXBA_bit_is_set_reset (free_parsers_set, father))
     SS_push (ltd_parse_stack.GC_area, father);
     }
     */
  
  x = 0;
  
  while ((x = ltd_parse_stack.for_shifter [x]) > 0) 
    {
      father = ltd_parse_stack.active_parsers [x];
      
      if (SXBA_bit_is_set_reset (free_parsers_set, father))
	  SS_push (ltd_parse_stack.GC_area, father);
    }
  
  x = 0;
  
  while ((x = ltd_parse_stack.active_states [x]) > 0) 
    {
      father = ltd_parse_stack.active_parsers [x];
      
      if (SXBA_bit_is_set_reset (free_parsers_set, father))
	  SS_push (ltd_parse_stack.GC_area, father);
    }
  
  do 
    {
      son = SS_pop (ltd_parse_stack.GC_area);
      
      while ((son = ltd_parse_stack.graph [father = son].son) > 0) 
	{
	  if (!SXBA_bit_is_set_reset (free_parsers_set, son))
	      break;
	}
      
      if (son < 0) 
	{
	  is_in_sons = true;
	  son = -son;
	  
	  if (SXBA_bit_is_set_reset (free_parsers_set, son))
	      SS_push (ltd_parse_stack.GC_area, son);
	  
	  XxY_Xforeach (ltd_parse_stack.sons, father, x) 
	    {
	      SXBA_0_bit (erasable_sons, x);
	      son = XxY_Y (ltd_parse_stack.sons, x);
	      
	      if (SXBA_bit_is_set_reset (free_parsers_set, son))
		  SS_push (ltd_parse_stack.GC_area, son);
	    }
	}
    } while (!SS_is_empty (ltd_parse_stack.GC_area));
  
  if (is_in_sons) 
    {
      x = XxY_top (ltd_parse_stack.sons) + 1;
      
      while ((x = sxba_1_rlscan (erasable_sons, x)) > 0)
	  XxY_erase (ltd_parse_stack.sons, x);
    }
  
  ltd_parse_stack.free_parser =
      ltd_parse_stack.graph_top =
	  ((x = sxba_0_rlscan (free_parsers_set, 
			       ltd_parse_stack.graph_size + 1)) < 0) ? 0 : x;
}



static int ltd_add_active_parser (family, son, state, symbol)
    int			son, state, symbol;
    SXBA		family;
{
  
  register int			x, father, ref;
  int					link, t_state, lim, red_no;
  register struct SXP_prdct		*aprdct;
  register struct ltd_parse_stack_elem	*agraph;
  
  /* Already known as an active parser or as a for_shifter? */
  if (ltd_parse_stack.active_states [state] != 0  ||
      ltd_parse_stack.for_shifter [state] != 0) 
    {
      agraph = &(ltd_parse_stack.graph [father = ltd_parse_stack.active_parsers [state]]);
      
      /* Add a link from father to son if it does not already exists. */
      
      if ((x = agraph->son) == son || -x == son)
	  return 0;
      
      if (x > 0)
	  agraph->son = -x;
      
      if (XxY_set (&ltd_parse_stack.sons, father, son, &link))
	  return 0;
      
      /* complete the family */
      sxba_or (ltd_parse_stack.families [father], family);
      
      ltd_parse_stack.transitions [link] = symbol;
      return ltd_parse_stack.active_states [state] != 0 ? link : 0;
    }
  
  /* create a stack node father with state "state" */
  
  if (ltd_parse_stack.lr_state_occ [state].number == state_occur_maxnb)
    {
      father = ltd_parse_stack.lr_state_occ [state].last;
      agraph = &(ltd_parse_stack.graph [father]);
      
      /* complete the family */
      sxba_or (ltd_parse_stack.families [father], family);
      
      /* add a link from father to son */
      if ((x = agraph->son) != son && -x != son)
	{
	  if (x > 0)
	      agraph->son = -x;
	  
	  if (!XxY_set (&ltd_parse_stack.sons, father, son, &link))
	      ltd_parse_stack.transitions [link] = symbol;
	}
    }
  else
    {
      /* Seeking a new parse graph elem whose id is father. */
      if (ltd_parse_stack.graph_top < ltd_parse_stack.graph_size)
	  father = ++ltd_parse_stack.graph_top;
      else    
	  if (ltd_parse_stack.free_parser > 0 &&
	      (ltd_parse_stack.free_parser = sxba_1_rlscan (ltd_parse_stack.free_parsers_set, 
							    ltd_parse_stack.free_parser)) > 0)
	      father =  ltd_parse_stack.free_parser;
	  else 
	    {
	      ltd_GC (son);
	      
	      if (ltd_parse_stack.graph_top < ltd_parse_stack.graph_size)
		  father = ++ltd_parse_stack.graph_top;
	      else    
		  if (ltd_parse_stack.free_parser > 0 &&
		      (ltd_parse_stack.free_parser = sxba_1_rlscan (ltd_parse_stack.free_parsers_set, 
								    ltd_parse_stack.free_parser)) > 0)
		      father =  ltd_parse_stack.free_parser;
		  else 
		    {
		      ltd_parse_stack.graph = (struct ltd_parse_stack_elem*)
			  sxrealloc (ltd_parse_stack.graph,
				     (ltd_parse_stack.graph_size *= 2) + 1,
				     sizeof (struct ltd_parse_stack_elem));
		      
		      ltd_parse_stack.free_parsers_set = sxba_resize (ltd_parse_stack.free_parsers_set,
								      ltd_parse_stack.graph_size + 1);
		      ltd_parse_stack.parsers_set = sxbm_resize (ltd_parse_stack.parsers_set,
								 4,
								 4,
								 ltd_parse_stack.graph_size + 1);
		      ltd_parse_stack.parsers_rhs0 = (int*) sxrealloc (ltd_parse_stack.parsers_rhs0,
								       ltd_parse_stack.graph_size + 1,
								       sizeof (int));
		      ltd_parse_stack.parsers_rhs1 = (int*) sxrealloc (ltd_parse_stack.parsers_rhs1,
								       ltd_parse_stack.graph_size + 1,
								       sizeof (int));
		      father =  ++ltd_parse_stack.graph_top;
		    }
	    }
      
      ltd_parse_stack.lr_state_occ [state].number++;
      ltd_parse_stack.lr_state_occ [state].last = father;
      
      agraph = &(ltd_parse_stack.graph [father]);
      
      /* add a link from father to son */
      agraph->son = son;
      agraph->symbol = symbol;
      
      ltd_parse_stack.families [father] = family;
      
    }
  
  agraph->state = state;
  agraph->parse_time_built = true;
  agraph->red_ref = 0;
  agraph->shift_ref = 0;
  
  if (state > lr0_automaton_state_nb) 
    {
      /*	state -= lr0_automaton_state_nb;*/
      PUSH (ltd_parse_stack.active_states, state);
      ltd_parse_stack.active_parsers [state] = father;
      SS_push (ltd_parse_stack.for_actor, father);
    }
  else 
    {
      register struct Q0	*aQ0 = Q0 + state;
      
      if (aQ0->t_trans_nb != 0 &&
	  (ref = XxY_is_set (&Q0xV_hd, state, -ltd_parse_stack.current_token)) > 0) 
	{
	  if (ltd_parse_stack.current_token != sxplocals.SXP_tables.P_tmax) 
	    {
	      agraph->shift_ref = Q0xV_to_Q0 [ref];
	      PUSH (ltd_parse_stack.for_shifter, state);
	      ltd_parse_stack.active_parsers [state] = father;
	    }
	}
      
      if ((lim = aQ0->red_trans_nb) != 0) 
	{
	  ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb;
	  red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
	  PUSH (ltd_parse_stack.active_states, state);
	  ltd_parse_stack.active_parsers [state] = father;
	  SS_push (ltd_parse_stack.for_actor, father);
	  
	  if (lim == 1) 
	      agraph->red_ref = red_no;
	  else 
	      agraph->red_ref = -state;
	}
    }
  
  return 0;
  
}


static void ltd_do_reductions (complete_item_father, son, is_in_set)
    int		complete_item_father, son;
    bool	is_in_set;
{
  /* We want to perform a  reduce_action A -> alpha on p. */
  /* l = length(alpha). */
  /* complete_item_father is a ltd_parse_stack_elem and link an index in ltd_parse_stack.sons
     which is characteristic of a given transition. */
  /* FORALL q for which a path of length l from p to q through
     link exists DO
     REDUCER (q, GOTO (state (q), A)). */
  
  int					symb, link, next; 
  struct ltd_parse_stack_elem		*agraph;
  bool				is_first_time;
  SXBA				family;
  
  is_first_time = true;
  
  do 
    {
      agraph = &(ltd_parse_stack.graph [son]);
      
      if (ltd_parse_stack.graph [son].parse_time_built)
	  /* si on n'est en train de construire la pile d'analyse,
	     ou alors, tombe dans un fils construit lors de l'analyse */
	  family = ltd_parse_stack.families [complete_item_father];
      else
	  family = ltd_parse_stack.families [son];
      
      /* in-lining of reducer () */
      
      if ( (next = Q0xV_to_Q0 
	    [XxY_is_set (&Q0xV_hd, 
			 agraph->state, 
			 symb = bnf_ag.WS_NBPRO [ltd_parse_stack.red_no].reduc)]
	    ) < 0) 
	{
	  /* Shift and Reduce */
	  next = lr0_automaton_state_nb + bnf_ag.WS_INDPRO [-next].prolis;
	}
      
      /* We create an active parser even when next designates a
	 shift (on the LHS non-terminal) and reduce
	 (i.e next > lr0_automaton_state_nb.) 
	 If exists q in active_parsers with state(q)==state then */
      
      if ((link = ltd_add_active_parser (family, son, next, symb)) > 0) 
	{
	  /* if there is no direct link from father to son yet then */
	  /* add a link from father to son */
	  /* forall r in (active_parsers - for_actor) do */
	  ltd_parse_stack.is_new_links = true;
	  SXBA_1_bit (ltd_parse_stack.current_links, link);
	}
      
      if (is_first_time) 
	{
	  if (!is_in_set)
	      break;
	  
	  is_first_time = false;
	  son = 0;
	}
    } while ((son = sxba_scan_reset (ltd_parse_stack.parsers_set [0], son)) > 0);
}



static void ltd_do_pops (p, l)
    int		p, l;
{
  /* We want to perform a  reduce_action A -> alpha on p. */
  /* l = length(alpha). */
  /* p is a ltd_parse_stack_elem. */
  /* FORALL q for which a path of length l from p to q DO
     REDUCER (q, GOTO (state (q), A)). */
  int			x, son, father, next_son, first_son;
  SXBA		fathers_set, sons_set;
  bool		is_in_set;
  
  is_in_set = false;
  son = p;
  
  while (l-- > 0) 
    {
      father = son;
      
      if (!is_in_set) 
	{
	  if ((son = ltd_parse_stack.graph [father].son) < 0) 
	    {
	      sons_set = (l & 01) ?
		  ltd_parse_stack.parsers_set [1] : ltd_parse_stack.parsers_set [0];
	      
	      XxY_Xforeach (ltd_parse_stack.sons, father, x) 
		{
		  next_son = XxY_Y (ltd_parse_stack.sons, x);
		  SXBA_1_bit (sons_set, next_son);
		}
	      
	      son = -son;
	      is_in_set = true;
	    }
	}
      else 
	{
	  is_in_set = false;
	  
	  if (l & 01) 
	    {
	      fathers_set = ltd_parse_stack.parsers_set [0];
	      sons_set = ltd_parse_stack.parsers_set [1];
	    }
	  else 
	    {
	      fathers_set = ltd_parse_stack.parsers_set [1];
	      sons_set = ltd_parse_stack.parsers_set [0];
	    }
	  
	  if ((son = ltd_parse_stack.graph [father].son) < 0) 
	    {
	      is_in_set = true;
	      son = -son;
	      
	      XxY_Xforeach (ltd_parse_stack.sons, father, x) 
		{
		  next_son = XxY_Y (ltd_parse_stack.sons, x);
		  SXBA_1_bit (sons_set, next_son);
		}
	    }
	  
	  father = 0;
	  
	  while ((father = sxba_scan_reset (fathers_set, father)) > 0) 
	    {
	      if ((first_son = ltd_parse_stack.graph [father].son) < 0) 
		{
		  is_in_set = true;
		  first_son = -first_son;
		  
		  XxY_Xforeach (ltd_parse_stack.sons, father, x) 
		    {
		      if ((next_son = XxY_Y (ltd_parse_stack.sons, x)) != son)
			  SXBA_1_bit (sons_set, next_son);
		    }
		}
	      
	      if (first_son != son) 
		{
		  SXBA_1_bit (sons_set, first_son);
		  is_in_set = true;
		}
	    }
	}
    }
  
  ltd_do_reductions (p, son, is_in_set);
}



static void ltd_do_limited_reductions (p, l)
    int 	p, l;
{
  /* We want to perform a  reduce_action A -> alpha on p  */
  /* l = length(alpha). */
  /* p is a ltd_parse_stack_elem*/
  /* FORALL q for which a path of length l from p to q DO
     REDUCER (q, GOTO (state (q), A)). */
  /* called with l > 0 */
  register int	x, father, son, first_father;
  bool		is_no_link, is_sons;
  register SXBA	fathers_set, sons_set, fathers_no_link_set, sons_no_link_set;
  SXBA		s;
  
  x = l & 01;
  fathers_set = ltd_parse_stack.parsers_set [x];
  fathers_no_link_set = ltd_parse_stack.parsers_set [x+2];
  x = (x + 1) & 01;
  sons_set = ltd_parse_stack.parsers_set [x];
  sons_no_link_set = ltd_parse_stack.parsers_set [x+2];
  SXBA_1_bit (fathers_set, p);
  SXBA_1_bit (fathers_no_link_set, p);
  
  for (;;) 
    {
      father = 0;
      
      while ((father = sxba_scan_reset (fathers_set, father)) > 0) 
	{
	  is_no_link = SXBA_bit_is_set_reset (fathers_no_link_set, father);
	  
	  if ((is_sons = (son = ltd_parse_stack.graph [father].son) < 0))
	      son = -son;
	  
	  SXBA_1_bit (sons_set, son);
	  
	  if (is_no_link)
	      SXBA_1_bit (sons_no_link_set, son);
	  
	  if (is_sons) 
	    {
	      XxY_Xforeach (ltd_parse_stack.sons, father, x) 
		{
		  son = XxY_Y (ltd_parse_stack.sons, x);
		  SXBA_1_bit (sons_set, son);
		  
		  if (is_no_link && !SXBA_bit_is_set (ltd_parse_stack.used_links, x))
		      SXBA_1_bit (sons_no_link_set, son);
		}
	    }
	}
      
      if (--l == 0)
	  break;
      
      s = fathers_set, fathers_set = sons_set, sons_set = s;
      s = fathers_no_link_set, fathers_no_link_set = sons_no_link_set, sons_no_link_set = s;
    }
  
  sxba_minus (sons_set, sons_no_link_set);
  
  if ((son = sxba_scan_reset (sons_set, 0)) > 0) 
    {
      ltd_do_reductions (p, son, sxba_scan (sons_set, son) > 0);
    }
}


static bool ltd_shifter ()
{
  int		p, q, ref, n, son, lgth;
  
  /* active_parsers == phi */
  /* for_actor == phi */
  
  if (ltd_parse_stack.for_shifter [0] < 0)
      return false;
  
  do 
    {
      POP (ltd_parse_stack.for_shifter, p);
      SS_push (ltd_parse_stack.for_scanner, ltd_parse_stack.active_parsers [p]);
    } while (ltd_parse_stack.for_shifter [0] > 0);
  
  ltd_parse_stack.current_level = sxplocals.ptok_no;
  
  /* read next token */
  n = ++sxplocals.ptok_no;
  
  while (n > sxplocals.Mtok_no)
      (*(sxplocals.SXP_tables.P_scan_it)) ();
  
  ltd_parse_stack.current_token = SXGET_TOKEN (n).lahead;
  /* for_scanner = for_shifter */
  /* for_shifter = phi */
  
  do 
    {
      p = SS_pop (ltd_parse_stack.for_scanner);
      ref = ltd_parse_stack.graph [p].shift_ref;
      ltd_parse_stack.graph [p].shift_ref = 0;
      
      if (ref < 0 /* scan and reduce */) 
	{
	  /* in-lining */
	  ltd_do_pops (p,
		       lgth = -ref - bnf_ag.WS_NBPRO [ltd_parse_stack.red_no = 
						      bnf_ag.WS_INDPRO [-ref].prolis].prolon);
	}
      else
	  ltd_add_active_parser (ltd_parse_stack.families [p], 
				 p, 
				 ref, 
				 ltd_parse_stack.current_symbol);
    } while (!SS_is_empty (ltd_parse_stack.for_scanner));
      
  return true;
}
  
  
  
  static void discrimine (shift_ref, parser)
      int	shift_ref, parser;
{
  int				i, x, p, ref, lgth, lim, shift_ambiguity, red_no;
  struct ltd_parse_stack_elem	*agraph = ltd_parse_stack.graph + parser;
  int 				state = agraph->state;
  register struct Q0		*aQ0 = Q0 + state;
  
  
  shift_ambiguity = (shift_ref > 0) ? 0 : -1;
  ltd_parse_stack.families_birth = true;
  
  /* Initialisation des familles avec leur premier membre */
  
  if (shift_ref > 0)
    {
      agraph->shift_ref = Q0xV_to_Q0 [shift_ref];
      PUSH (ltd_parse_stack.for_shifter, state);
      ltd_parse_stack.active_parsers [state] = parser;
      sxba_1_bit (ltd_parse_stack.family_Mb, 0);
    }
  
  lim = aQ0->red_trans_nb;
  red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
  sxba_empty (ltd_parse_stack.families [parser]);
  
  if (lim == 1) 
    {
      sxba_1_bit (ltd_parse_stack.families [parser], 1);
      ltd_parse_stack.red_no = red_no;
      ltd_do_pops (true, parser, LGPROD (red_no));
    }
  else
    {
      register struct Q0	*aQ0	    = Q0 + state;
      SXBA     			aworthy_red = common_struct.worthy_red [state];
      
      ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb;
      x = 0;
      lim = aQ0->red_trans_nb + ref;
      i = 0;
      
      do
	{
	  sxba_0_bit (ltd_parse_stack.families [parser], i++);
	  sxba_1_bit (ltd_parse_stack.families [parser], i);
	  ltd_parse_stack.red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
	  ltd_do_pops (false, parser, LGPROD (ltd_parse_stack.red_no));
	} while (++ref < lim);
    }
  
  /* Boucle de discrimine : tant 
     qu'on n'est pas au bout, 
     qu'il reste des conflit et 
     qu'il y a qqc a shifter         */
  do
    {
      if (!SS_is_empty (ltd_parse_stack.for_actor))
 	{
	  /* while for_actor <> phi  do */
	  do 
	    {
	      /* remove a parser p from for_actor */
	      p = SS_pop (ltd_parse_stack.for_actor);
	      
	      if ((ref = ltd_parse_stack.graph [p].red_ref) == 0)
		{
		  /* Shift and reduce parser */
		  ref = ltd_parse_stack.graph [p].state - lr0_automaton_state_nb;
		}
	      
	      if (ref > 0)
		{
		  if (ltd_parse_stack.families_birth)
		    {
		      sxba_empty (ltd_parse_stack.families [p]);
		      sxba_1_bit (ltd_parse_stack.families [p], 1);
		    }
		  ltd_parse_stack.red_no = ref;
		  ltd_do_pops (true, p, LGPROD (ltd_parse_stack.red_no));
		}
	      else
		{
		  register struct Q0	*aQ0	    = Q0 - ref;
		  SXBA     		aworthy_red = common_struct.worthy_red [-ref];
		  int      		x;
		  
		  ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb;
		  x = 0;
		  lim = aQ0->red_trans_nb + ref;
		  if (ltd_parse_stack.families_birth)
		    {
		      i = 0;
		      sxba_empty (ltd_parse_stack.families [p]);
		    }
		  
		  do
		    {
		      if (ltd_parse_stack.families_birth)
			{
			  sxba_0_bit (ltd_parse_stack.families [p], i++);
			  sxba_1_bit (ltd_parse_stack.families [p], i);
			}
		      ltd_parse_stack.red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
		      if ((lgth = LGPROD (ltd_parse_stack.red_no)) > 0)
			  ltd_do_pops (false, p, lgth);
		    } while (++ref < lim);
		}
	      ltd_parse_stack.families_birth = false;
	    } while (!SS_is_empty (ltd_parse_stack.for_actor));
	  
	  while (ltd_parse_stack.is_new_links)
	    {
	      ltd_parse_stack.is_new_links = false;
	      sxba_copy (ltd_parse_stack.used_links, ltd_parse_stack.current_links);
	      sxba_empty (ltd_parse_stack.current_links);
	      
	      /* for_actor == active_parsers */
	      p = 0;
	      
	      while ((p = ltd_parse_stack.active_states [p]) > 0)
		  SS_push (ltd_parse_stack.for_actor, ltd_parse_stack.active_parsers [p]);
	      
	      do 
		{
		  /* remove a parser p from for_actor */
		  p = SS_pop (ltd_parse_stack.for_actor);
		  
		  if ((ref = ltd_parse_stack.graph [p].red_ref) == 0)
		    {
		      /* Shift and reduce parser */
		      ref = ltd_parse_stack.graph [p].state - lr0_automaton_state_nb;;
		    }
		  
		  if (ref > 0)
		    {
		      if ((lgth = LGPROD (ref)) > 0)
			{
			  ltd_parse_stack.red_no = ref;
			  ltd_do_limited_reductions (true, p, lgth);
			}
		    }
		  else
		    {
		      register struct Q0	*aQ0 = Q0 - ref;
		      
		      lim = aQ0->red_trans_nb +
			  (ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);
		      
		      do
			{
			  ltd_parse_stack.red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
			  
			  if ((lgth = LGPROD (ltd_parse_stack.red_no)) > 0)
			      ltd_do_limited_reductions (true, p, lgth);
			} while (++ref < lim);
		    }
		  
		} while (!SS_is_empty (ltd_parse_stack.for_actor));
	      
	    }
	  
	}
      
      sxba_empty (ltd_parse_stack.family_Mb);
      x = 0;
      while ((x = ltd_parse_stack.active_states [x]) > 0) 
	  sxba_or (ltd_parse_stack.family_Mb, ltd_parse_stack.families [parse_stack.active_parsers [x]]);
      
    } while (parse_stack.current_token != sxplocals.SXP_tables.P_tmax &&
	     sxba_cardinal (ltd_parse_stack.family_Mb) + shift_ambiguity > 0 && 
	     ltd_shifter(true));
  
  common_struct.worthy_red [state] = ltd_parse_stack.family_Mb;
  
}



static int add_active_parser (son, ltd_son, state, symbol, ltd_link)
    int			son, ltd_son, state, symbol, ltd_link;
{
  /* Create a parser named "father" whose state is "state"
     and link it to "son". */
  /* The transition from "son" to "father" is performed on "symbol".
     "symbol" is a pointer to a 4-tuple (p, k, i, j) s.t.
     - p>0 is an input grammar rule number
     p<0 is the opposite of the code of aterminal symbol
     - k is non negative integer. The couple (p,k) designate an item
     if p = A -> X0 X1   Xk  Xl, (p,k) is the item A -> X0 X1   .Xk  Xl
     - i and j are positive integers which are token numbers s.t.
     i <= j and Xk...Xl =>* ai+1 ... aj */
  
  
  register int				x, father, ltd_father, ref;
  int					link, t_state, lim, red_no;
  register struct parse_stack_elem	*agraph;
  register struct ltd_parse_stack_elem	*ltd_agraph;
  
  /* Already known as an active parser or as a for_shifter? */
  if (parse_stack.active_states [state] != 0  ||
      parse_stack.for_shifter [state] != 0) 
    {
      agraph = &(parse_stack.graph [father = parse_stack.active_parsers [state]]);
      ltd_agraph = &(ltd_parse_stack.graph [ltd_father = ltd_parse_stack.active_parsers [state]]);
      
      /* Add a link from father to son if it does not already exists. */
      if ((x = agraph->son) == son || -x == son) 
	{
	  if (agraph->symbol != symbol)
	      set_unit_rule (agraph->symbol, symbol);
	  
	  return 0;
	}
      
      if (x > 0)
	{
	  agraph->son = -x;
	  ltd_agraph->son = -ltd_agraph->son;
	}
      
      XxY_set (&ltd_parse_stack.sons, ltd_father, ltd_son, &ltd_link);
      if (XxY_set (&parse_stack.sons, father, son, &link))
	{
	  if (parse_stack.transitions [link] != symbol)
	      set_unit_rule (parse_stack.transitions [link], symbol);
	  
	  return 0;
	}
      
      ltd_parse_stack.transitions [ltd_link] = symbol;
      parse_stack.transitions [link] = symbol;
      if (ltd_parse_stack.active_states [state] == 0) ltd_link = 0;
      return parse_stack.active_states [state] != 0 ? link : 0;
    }
  
  /* create a stack node father with state "state" */
  
  /* Seeking a new parse graph elem whose id is father. */
  if (ltd_parse_stack.graph_top < ltd_parse_stack.graph_size)
      ltd_father = ++ltd_parse_stack.graph_top;
  else    
      if (ltd_parse_stack.free_parser > 0 &&
	  (ltd_parse_stack.free_parser = sxba_1_rlscan (ltd_parse_stack.free_parsers_set, 
							ltd_parse_stack.free_parser)) > 0)
	  ltd_father =  ltd_parse_stack.free_parser;
      else 
	{
	  ltd_GC (ltd_son);
	      
	  if (ltd_parse_stack.graph_top < ltd_parse_stack.graph_size)
	      ltd_father = ++ltd_parse_stack.graph_top;
	  else    
	      if (ltd_parse_stack.free_parser > 0 &&
		  (ltd_parse_stack.free_parser = sxba_1_rlscan (ltd_parse_stack.free_parsers_set, 
								ltd_parse_stack.free_parser)) > 0)
		  ltd_father =  ltd_parse_stack.free_parser;
	      else 
		{
		  ltd_parse_stack.graph = (struct ltd_parse_stack_elem*)
		      sxrealloc (ltd_parse_stack.graph,
				 (ltd_parse_stack.graph_size *= 2) + 1,
				 sizeof (struct ltd_parse_stack_elem));
		  
		  ltd_parse_stack.free_parsers_set = sxba_resize (ltd_parse_stack.free_parsers_set,
								  ltd_parse_stack.graph_size + 1);
		  ltd_parse_stack.parsers_set = sxbm_resize (ltd_parse_stack.parsers_set,
							     4,
							     4,
							     ltd_parse_stack.graph_size + 1);
		  ltd_parse_stack.parsers_rhs0 = (int*) sxrealloc (ltd_parse_stack.parsers_rhs0,
								   ltd_parse_stack.graph_size + 1,
								   sizeof (int));
		  ltd_parse_stack.parsers_rhs1 = (int*) sxrealloc (ltd_parse_stack.parsers_rhs1,
								   ltd_parse_stack.graph_size + 1,
								   sizeof (int));
		  ltd_father =  ++ltd_parse_stack.graph_top;
		}
	}
      
  if (parse_stack.graph_top < parse_stack.graph_size)
      father = ++parse_stack.graph_top;
  else    
      if (parse_stack.free_parser > 0 &&
	  (parse_stack.free_parser = sxba_1_rlscan (parse_stack.free_parsers_set, 
						    parse_stack.free_parser)) > 0)
	  father =  parse_stack.free_parser;
      else 
	{
	  GC (son);
	  
	  if (parse_stack.graph_top < parse_stack.graph_size)
	      father = ++parse_stack.graph_top;
	  else    
	      if (parse_stack.free_parser > 0 &&
		  (parse_stack.free_parser = sxba_1_rlscan (parse_stack.free_parsers_set, 
							    parse_stack.free_parser)) > 0)
		  father =  parse_stack.free_parser;
	      else 
		{
		  parse_stack.graph = (struct parse_stack_elem*)
		      sxrealloc (parse_stack.graph,
				 (parse_stack.graph_size *= 2) + 1,
				 sizeof (struct parse_stack_elem));
		  
		  parse_stack.free_parsers_set = sxba_resize (parse_stack.free_parsers_set,
							      parse_stack.graph_size + 1);
		  parse_stack.parsers_set = sxbm_resize (parse_stack.parsers_set,
							 4,
							 4,
							 parse_stack.graph_size + 1);
		  
		  ltd_parse_stack.families = sxbm_resize (ltd_parse_stack.families,
							  common_struct.max_red_nb,
							  common_struct.max_red_nb,
							  parse_stack.graph_size + 1);
		  
		  parse_stack.parsers_rhs0 = (int*) sxrealloc (parse_stack.parsers_rhs0,
							       parse_stack.graph_size + 1,
							       sizeof (int));
		  
		  parse_stack.parsers_rhs1 = (int*) sxrealloc (parse_stack.parsers_rhs1,
							       parse_stack.graph_size + 1,
							       sizeof (int));
		  father =  ++parse_stack.graph_top;
		}
	}
  
  agraph = &(parse_stack.graph [father]);
  ltd_agraph = &(ltd_parse_stack.graph [ltd_father]);

  agraph->state = state;
  ltd_agraph->state = state;

  agraph->red_ref = 0;
  ltd_agraph->red_ref = 0;

  agraph->shift_ref = 0;
  ltd_agraph->shift_ref = 0;

  /* add a link from father to son */
  agraph->son = son;
  ltd_agraph->son = ltd_son;

  agraph->symbol = symbol;
  ltd_agraph->symbol = symbol;

  agraph->level = parse_stack.current_level;
  ltd_agraph->parse_time_built = true;
  
  if (state > lr0_automaton_state_nb) 
    {
      /*   state -= lr0_automaton_state_nb;   */
      PUSH (parse_stack.active_states, state);

      parse_stack.active_parsers [state] = father;
      ltd_parse_stack.active_parsers [state] = ltd_father;

      SS_push (parse_stack.for_actor, father);
      SS_push (ltd_parse_stack.for_actor, ltd_father);
    }
  else 
    {
      register struct Q0	*aQ0 = Q0 + state;
      int			red_nb;
      
      if ((ref = XxY_is_set (&Q0xV_hd, state, -parse_stack.current_token)) > 0 
	  && (red_nb = lim = aQ0->red_trans_nb) > 0) 
	{
	  /* conflit shit-reduce */
	  discrimine (ref, father);
	}
      else if (lim > 1)
	{
	  /* conflit reduce-reduce */
	  discrimine (0, father);
	}
      
      if (!sxba_bit_is_set (ltd_parse_stack.family_Mb, 0)) ref = 0;
      sxba_0_bit (ltd_parse_stack.family_Mb, 0);
      red_nb = sxba_cardinal (ltd_parse_stack.family_Mb);
      
      if (aQ0->t_trans_nb != 0 && ref > 0)   /* POURQUOI  "aQ0->t_trans_nb != 0"  ??? */
	{
	  if (parse_stack.current_token == sxplocals.SXP_tables.P_tmax) 
	    {
	      /* Halt */
	      set_start_symbol (symbol);
	    }
	  else 
	    {
	      agraph->shift_ref = Q0xV_to_Q0 [ref];
	      PUSH (parse_stack.for_shifter, state);
	      parse_stack.active_parsers [state] = father;

	      ltd_agraph->shift_ref = agraph->shift_ref;
	      PUSH (ltd_parse_stack.for_shifter, state);
	      ltd_parse_stack.active_parsers [state] = ltd_father;
	    }
	}
      
      if (red_nb != 0) 
	{
	  ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb;
	  red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
	  PUSH (parse_stack.active_states, state);
	  parse_stack.active_parsers [state] = father;
	  ltd_parse_stack.active_parsers [state] = ltd_father;
	  SS_push (parse_stack.for_actor, father);
	  SS_push (ltd_parse_stack.for_actor, ltd_father);
	  push_rp (red_no, father); /* for parse_forest () */
	  
	  if (red_nb == 1)
	      agraph->red_ref = (lim == 1) ? red_no : sxba_1_lrscan (common_struct.worthy_red [father], 1);
	  else 
	    {
	      agraph->red_ref = -state;
	      lim += ref;
	      
	      while (++ref < lim) 
		{
		  red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis; 
		  push_rp (red_no, father); /* for parse_forest () */
		}
	    }
	  ltd_agraph->red_ref = agraph->red_ref;
	}
    }
  return 0;
}


static int push_rhs (symbol, list)
    int symbol, list;
{
  /* Adds symbol before list. */
  
  if ((++parse_stack.rhs_top + 1) > parse_stack.rhs_size)
      parse_stack.rhs = (struct rhs*) sxrealloc 
	  (parse_stack.rhs, (parse_stack.rhs_size *= 2) + 1, sizeof (struct rhs));
  
  if (list < 0) 
    {
      parse_stack.rhs [parse_stack.rhs_top].symbol = -list;
      parse_stack.rhs [list = parse_stack.rhs_top].next = 0;
      ++parse_stack.rhs_top;
    }
  
  parse_stack.rhs [parse_stack.rhs_top].symbol = symbol;
  parse_stack.rhs [parse_stack.rhs_top].next = list;
  return parse_stack.rhs_top;
}


static void do_reductions (son, is_in_set, ltd_son, ltd_is_in_set)
    int		son, ltd_son, ltd_link;
    bool	is_in_set, ltd_is_in_set;
{
  /* We want to perform a  reduce_action A -> alpha on p. 
     l = length(alpha).
     p is a parse_stack_elem and link an index in parse_stack.sons
     which is characteristic of a given transition. 
     FORALL q for which a path of length l from p to q through
     link exists DO
     REDUCER (q, GOTO (state (q), A)). 
  */

  int					symb, link, next; 
  struct parse_stack_elem		*agraph;
  bool				is_first_time;
  
  is_first_time = true;
  
  do 
    {
      agraph = &(parse_stack.graph [son]);
      new_symbol (parse_stack.red_no,
		  0,
		  agraph->level,
		  parse_stack.current_level,
		  &symb);
      
      /* in-lining of reducer () */
      if ( (next = Q0xV_to_Q0 
	    [XxY_is_set (&Q0xV_hd, 
			 agraph->state, 
			 symb = bnf_ag.WS_NBPRO [parse_stack.red_no].reduc)]
	    ) < 0) 
	{
	  /* Shift and Reduce */
	  next = lr0_automaton_state_nb + bnf_ag.WS_INDPRO [-next].prolis;
	}
      
      /* We create an active parser even when next designates a
	 shift (on the LHS non-terminal) and reduce
	 (i.e next > lr0_automaton_state_nb.
         If exists q in active_parsers with state(q)==state then 
      */
      if ((link = add_active_parser (son, ltd_son, next, symb, &ltd_link)) > 0) 
	{
	  /* if there is no direct link from father to son yet then */
	  /* add a link from father to son                          */
	  /* forall r in (active_parsers - for_actor) do            */
	  parse_stack.is_new_links = true;
	  ltd_parse_stack.is_new_links = true;
	  SXBA_1_bit (parse_stack.current_links, link);
	  SXBA_1_bit (ltd_parse_stack.current_links, ltd_link);
	}
      
      if (is_first_time) 
	{
	  if (!is_in_set)
	      break;
	  
	  is_first_time = false;
	  son = 0;
	}
    } while ((son = sxba_scan_reset (parse_stack.parsers_set [0], son)) > 0);
}



static void do_pops (p, ltd_p, l)
    int		p, ltd_p, l;
{
  /* We want to perform a  reduce_action A -> alpha on p. */
  /* l = length(alpha). */
  /* p is a parse_stack_elem and link an index in parse_stack.sons
     which is characteristic of a given transition. */
  /* FORALL q for which a path of length l from p to q through
     link exists DO
     REDUCER (q, GOTO (state (q), A)). */
  int			x, son, ltd_son, father, next_son, first_son;
  SXBA		fathers_set, sons_set;
  bool		is_in_set, ltd_is_in_set;
  
  is_in_set = false;
  son = p;
  
  while (l-- > 0) 
    {
      father = son;
      
      if (!is_in_set) 
	{
	  if ((son = ltd_parse_stack.graph [father].son) < 0) 
	    {
	      sons_set = (l & 01) ?
		  ltd_parse_stack.parsers_set [1] : ltd_parse_stack.parsers_set [0];
	      
	      XxY_Xforeach (ltd_parse_stack.sons, father, x) 
		{
		  next_son = XxY_Y (ltd_parse_stack.sons, x);
		  SXBA_1_bit (sons_set, next_son);
		}
	      
	      son = -son;
	      is_in_set = true;
	    }
	}
      else 
	{
	  is_in_set = false;
	  
	  if (l & 01) 
	    {
	      fathers_set = ltd_parse_stack.parsers_set [0];
	      sons_set = ltd_parse_stack.parsers_set [1];
	    }
	  else 
	    {
	      fathers_set = ltd_parse_stack.parsers_set [1];
	      sons_set = ltd_parse_stack.parsers_set [0];
	    }
	  
	  if ((son = ltd_parse_stack.graph [father].son) < 0) 
	    {
	      is_in_set = true;
	      son = -son;
	      
	      XxY_Xforeach (ltd_parse_stack.sons, father, x) 
		{
		  next_son = XxY_Y (ltd_parse_stack.sons, x);
		  SXBA_1_bit (sons_set, next_son);
		}
	    }
	  
	  father = 0;
	  
	  while ((father = sxba_scan_reset (fathers_set, father)) > 0) 
	    {
	      if ((first_son = ltd_parse_stack.graph [father].son) < 0) 
		{
		  is_in_set = true;
		  first_son = -first_son;
		  
		  XxY_Xforeach (ltd_parse_stack.sons, father, x) 
		    {
		      if ((next_son = XxY_Y (ltd_parse_stack.sons, x)) != son)
			  SXBA_1_bit (sons_set, next_son);
		    }
		}
	      
	      if (first_son != son) 
		{
		  SXBA_1_bit (sons_set, first_son);
		  is_in_set = true;
		}
	    }
	}
    }

  ltd_is_in_set = is_in_set;
  ltd_son = son;

  is_in_set = false;
  son = ltd_p;
  
  while (l-- > 0) 
    {
      father = son;
      
      if (!is_in_set) 
	{
	  if ((son = parse_stack.graph [father].son) < 0) 
	    {
	      sons_set = (l & 01) ?
		  parse_stack.parsers_set [1] : parse_stack.parsers_set [0];
	      
	      XxY_Xforeach (parse_stack.sons, father, x) 
		{
		  next_son = XxY_Y (parse_stack.sons, x);
		  SXBA_1_bit (sons_set, next_son);
		}
	      
	      son = -son;
	      is_in_set = true;
	    }
	}
      else 
	{
	  is_in_set = false;
	  
	  if (l & 01) 
	    {
	      fathers_set = parse_stack.parsers_set [0];
	      sons_set = parse_stack.parsers_set [1];
	    }
	  else 
	    {
	      fathers_set = parse_stack.parsers_set [1];
	      sons_set = parse_stack.parsers_set [0];
	    }
	  
	  if ((son = parse_stack.graph [father].son) < 0) 
	    {
	      is_in_set = true;
	      son = -son;
	      
	      XxY_Xforeach (parse_stack.sons, father, x) 
		{
		  next_son = XxY_Y (parse_stack.sons, x);
		  SXBA_1_bit (sons_set, next_son);
		}
	    }
	  
	  father = 0;
	  
	  while ((father = sxba_scan_reset (fathers_set, father)) > 0) 
	    {
	      if ((first_son = parse_stack.graph [father].son) < 0) 
		{
		  is_in_set = true;
		  first_son = -first_son;
		  
		  XxY_Xforeach (parse_stack.sons, father, x) 
		    {
		      if ((next_son = XxY_Y (parse_stack.sons, x)) != son)
			  SXBA_1_bit (sons_set, next_son);
		    }
		}
	      
	      if (first_son != son) 
		{
		  SXBA_1_bit (sons_set, first_son);
		  is_in_set = true;
		}
	    }
	}
    }
  
  do_reductions (son, is_in_set, ltd_son, ltd_is_in_set);
}



static void do_limited_reductions (p, ltd_p, l)
    int p, ltd_p, l;
{
  /* We want to perform a  reduce_action A -> alpha on p which uses link.
     l = length(alpha). 
     p is a parse_stack_elem and link an index in parse_stack.sons
     which is characteristic of a given transition.
     FORALL q for which a path of length l from p to q through
     link exists DO
     REDUCER (q, GOTO (state (q), A)).
     called with l > 0
  */

  register int	x, father, son, first_father;
  bool		is_no_link, is_sons;
  register SXBA	fathers_set, sons_set, fathers_no_link_set, sons_no_link_set;
  SXBA		s;
  
  /* pops sur le graphe non limite' */

  x = l & 01;
  fathers_set = parse_stack.parsers_set [x];
  fathers_no_link_set = parse_stack.parsers_set [x+2];
  x = (x + 1) & 01;
  sons_set = parse_stack.parsers_set [x];
  sons_no_link_set = parse_stack.parsers_set [x+2];
  SXBA_1_bit (fathers_set, p);
  SXBA_1_bit (fathers_no_link_set, p);
  
  for (;;) 
    {
      father = 0;
      
      while ((father = sxba_scan_reset (fathers_set, father)) > 0) 
	{
	  is_no_link = SXBA_bit_is_set_reset (fathers_no_link_set, father);
	  
	  if ((is_sons = (son = parse_stack.graph [father].son) < 0))
	      son = -son;
	  
	  SXBA_1_bit (sons_set, son);
	  
	  if (is_no_link)
	      SXBA_1_bit (sons_no_link_set, son);
	  
	  if (is_sons) 
	    {
	      XxY_Xforeach (parse_stack.sons, father, x) 
		{
		  son = XxY_Y (parse_stack.sons, x);
		  SXBA_1_bit (sons_set, son);
		  
		  if (is_no_link && !SXBA_bit_is_set (parse_stack.used_links, x))
		      SXBA_1_bit (sons_no_link_set, son);
		}
	    }
	}
      
      if (--l == 0)
	  break;
      
      s 	    = fathers_set, 
      fathers_set = sons_set, 
      sons_set    = s;
      
      s                   = fathers_no_link_set, 
      fathers_no_link_set = sons_no_link_set, 
      sons_no_link_set    = s;
    }
  
  sxba_minus (sons_set, sons_no_link_set);
  
  if ((SON = sxba_scan_reset (sons_set, 0)) > 0)
    {
      is_in_set = sxba_scan (sons_set, SON);
      
      x = l & 01;
      fathers_set = ltd_parse_stack.parsers_set [x];
      fathers_no_link_set = ltd_parse_stack.parsers_set [x+2];
      x = (x + 1) & 01;
      sons_set = ltd_parse_stack.parsers_set [x];
      sons_no_link_set = ltd_parse_stack.parsers_set [x+2];
      SXBA_1_bit (fathers_set, ltd_p);
      SXBA_1_bit (fathers_no_link_set, ltd_p);
      
      for (;;) 
	{
	  father = 0;
	  
	  while ((father = sxba_scan_reset (fathers_set, father)) > 0) 
	    {
	      is_no_link = SXBA_bit_is_set_reset (fathers_no_link_set, father);
	      
	      if ((is_sons = (son = ltd_parse_stack.graph [father].son) < 0))
		  son = -son;
	      
	      SXBA_1_bit (sons_set, son);
	      
	      if (is_no_link)
		  SXBA_1_bit (sons_no_link_set, son);
	      
	      if (is_sons) 
		{
		  XxY_Xforeach (ltd_parse_stack.sons, father, x) 
		    {
		      son = XxY_Y (ltd_parse_stack.sons, x);
		      SXBA_1_bit (sons_set, son);
		      
		      if (is_no_link && !SXBA_bit_is_set (ltd_parse_stack.used_links, x))
			  SXBA_1_bit (sons_no_link_set, son);
		    }
		}
	    }
	  
	  if (--l == 0)
	      break;
	  
	  s = fathers_set, fathers_set = sons_set, sons_set = s;
	  s = fathers_no_link_set, fathers_no_link_set = sons_no_link_set, sons_no_link_set = s;
	}
      sxba_minus (sons_set, sons_no_link_set);
      do_reductions (SON,  is_in_set, son, sxba_scan (sons_set, son));
    }
}


static void seek_paths (p, p_trans, is_in_set, l)
    int		p, p_trans, l;
    bool	is_in_set;
{
  int		x, son, father, symb, first_son;
  SXBA	fathers_set, sons_set, s;
  int		*fathers_rhs_hd, *sons_rhs_hd, *ptr, *son_rhs_ptr;
  int		father_rhs_hd, rhs, first_son_rhs_hd, son_rhs_hd;
  bool	is_sons, is_first_time, is_in_sons_set;
  
  if (l == 0 && !is_in_set) 
    {
      new_symbol (parse_stack.red_no,
		  0,
		  parse_stack.graph [p].level,
		  parse_stack.current_level,
		  &symb);
      set_rule (symb, p_trans);
      return;
    }
  
  fathers_set = parse_stack.parsers_set [1];
  fathers_rhs_hd = parse_stack.parsers_rhs1;
  sons_set = parse_stack.parsers_set [0];
  sons_rhs_hd = parse_stack.parsers_rhs0;
  
  first_son = p;
  first_son_rhs_hd = p_trans;
  
  while (l-- > 0) 
    {
      father = first_son;
      father_rhs_hd = first_son_rhs_hd;
      first_son_rhs_hd = 0;
      is_first_time = true;
      
      for (;;) 
	{
	  if ((is_sons = (son = parse_stack.graph [father].son) < 0))
	      son = -son;
	  
	  if (is_first_time)
	      first_son = son;
	  
	  rhs = push_rhs (parse_stack.graph [father].symbol, father_rhs_hd);
	  
	  if (first_son == son)
	      son_rhs_ptr = &first_son_rhs_hd;
	  else 
	    {
	      is_in_sons_set = true;
	      son_rhs_ptr = sons_rhs_hd + son;
	      
	      if (SXBA_bit_is_reset_set (sons_set, son))
		  *son_rhs_ptr = 0;
	    }
	  
	  if (*son_rhs_ptr == 0)
	      *son_rhs_ptr = rhs;	
	  else {
	    if (*son_rhs_ptr > 0) 
	      {
		new_symbol (parse_stack.red_no,
			    l,
			    parse_stack.graph [son].level,
			    parse_stack.current_level,
			    &symb);
		set_rule (symb, *son_rhs_ptr);
		*son_rhs_ptr = -symb;
	      }
	    
	    set_rule (-*son_rhs_ptr, rhs);
	  }
	  
	  if (is_sons) 
	    {
	      XxY_Xforeach (parse_stack.sons, father, x) 
		{
		  son = XxY_Y (parse_stack.sons, x);
		  son_rhs_ptr = sons_rhs_hd + son;
		  rhs = push_rhs (parse_stack.transitions [x], father_rhs_hd);
		  
		  if (first_son == son)
		      son_rhs_ptr = &first_son_rhs_hd;
		  else {
		    is_in_sons_set = true;
		    son_rhs_ptr = sons_rhs_hd + son;
		    
		    if (SXBA_bit_is_reset_set (sons_set, son))
			*son_rhs_ptr = 0;
		  }
		  
		  if (*son_rhs_ptr == 0)
		      *son_rhs_ptr = rhs;	
		  else {
		    if (*son_rhs_ptr > 0) 
		      {
			new_symbol (parse_stack.red_no,
				    l,
				    parse_stack.graph [son].level,
				    parse_stack.current_level,
				    &symb);
			set_rule (symb, *son_rhs_ptr);
			*son_rhs_ptr = -symb;
		      }
		    
		    set_rule (-*son_rhs_ptr, rhs);
		  }
		}
	    }
	  
	  if (is_first_time) 
	    {
	      is_first_time = false;
	      father = 0;
	    }
	  
	  if (is_in_set && (father = sxba_scan_reset (fathers_set, father)) > 0) 
	    {
	      father_rhs_hd = fathers_rhs_hd [father];
	    }
	  else
	      break;
	}
      
      is_in_set = is_in_sons_set;
      
      if (l > 0) 
	{
	  s = fathers_set, fathers_set = sons_set, sons_set = s;
	  ptr = fathers_rhs_hd, fathers_rhs_hd = sons_rhs_hd, sons_rhs_hd = ptr;
	}
    }
  
  if (first_son_rhs_hd > 0) 
    {
      new_symbol (parse_stack.red_no,
		  0,
		  parse_stack.graph [first_son].level,
		  parse_stack.current_level,
		  &symb);
      set_rule (symb, first_son_rhs_hd);
    }
  
  if (is_in_set) 
    {
      son = 0;
      
      while ((son = sxba_scan_reset (sons_set, son)) > 0) 
	{
	  if ((son_rhs_hd = sons_rhs_hd [son]) > 0) 
	    {
	      new_symbol (parse_stack.red_no,
			  0,
			  parse_stack.graph [son].level,
			  parse_stack.current_level,
			  &symb);
	      set_rule (symb, son_rhs_hd);
	    }
	}
    }
  
  parse_stack.rhs_top = 0;
}


static void parse_forest ()
{
  /* The construction of the parse forest is not performed within the
     recognizer in order to avoid the multiple definition of the same
     sub-string with different productions. This may arise when a new
     link is added between previous parsers, triggering a future call
     to do_limited_reductions and adds a path such that a node with a
     single father becomes multi_fathered and hence produces an inter
     mediate non-terminal which describes a path which has already be
     en processed previously. One solution is to have a two step proc
     ess: first, the recognizer which set all the pathes and the non-
     terminals which are the names of the transitions and second, the
     creation of the parse forest (ie the grammar rules) which uses
     the previous transitions and pathes. */
  int			p, ref, x;
  
  while (parse_stack.active_states [0] > 0) 
    {
      POP (parse_stack.active_states, p);
      p = parse_stack.active_parsers [p];
      
      if ((ref = parse_stack.graph [p].red_ref) == 0) 
	{
	  /* Shift and reduce parser */
	  /* Always a one to one mapping between parsers and ref. */
	  parse_stack.red_no = bnf_ag.WS_INDPRO [parse_stack.graph [p].state].prolis;
	  seek_paths (p, 0, false, LGPROD (parse_stack.red_no));
	  
	}
      else  {
	parse_stack.graph [p].red_ref = 0;
      }
    }
  
  while (parse_stack.red_stack [0] > 0) 
    {
      POP (parse_stack.red_stack, ref);
      
      if ((x = parse_stack.red_parsers_hd [ref]) < 0) 
	{
	  parse_stack.red_no = ref;
	  seek_paths (-x, 0, false, LGPROD (parse_stack.red_no));
	}
      else 
	{
	  do 
	    {
	      p = parse_stack.red_parsers_list [x].parser;
	      
	      if ((x = parse_stack.red_parsers_list [x].lnk) != 0)
		  SXBA_1_bit (parse_stack.parsers_set [1], p);
	    } while (x != 0);
	  
	  parse_stack.red_no = ref;
	  seek_paths (p, 0, true, LGPROD (parse_stack.red_no));
	}
    }
  
  parse_stack.red_parsers_list_top = 0;
}


static bool shifter ()
{
  int		p, q, ref, n, son, lgth;
  
  /* active_parsers == phi */
  /* for_actor == phi */
  
  if (parse_stack.for_shifter [0] < 0)
      return false;
  
  do 
    {
      POP (parse_stack.for_shifter, p);
      SS_push (parse_stack.for_scanner, parse_stack.active_parsers [p]);
      POP (ltd_parse_stack.for_shifter, p);
      SS_push (ltd_parse_stack.for_scanner, parse_stack.active_parsers [p]);
    } while (parse_stack.for_shifter [0] > 0);
  
  parse_stack.current_level = sxplocals.ptok_no;
  
  new_symbol (-parse_stack.current_token,
	      0,
	      parse_stack.current_level - 1,
	      parse_stack.current_level,
	      &parse_stack.current_symbol);
  
  if (parse_stack.current_token != sxplocals.SXP_tables.P_tmax)
      set_t_rule (parse_stack.current_symbol);
  
  /* read next token */
  n = ++sxplocals.ptok_no;
  
  while (n > sxplocals.Mtok_no)
      (*(sxplocals.SXP_tables.P_scan_it)) ();
  
  parse_stack.current_token = SXGET_TOKEN (n).lahead;
  /* for_scanner = for_shifter */
  /* for_shifter = phi */
  
  do 
    {
      p = SS_pop (parse_stack.for_scanner);
      ref = parse_stack.graph [p].shift_ref;
      parse_stack.graph [p].shift_ref = 0;
      ltd_p = SS_pop (ltd_parse_stack.for_scanner);
      parse_stack.graph [ltd_p].shift_ref = 0;
      
      if (ref < 0 /* scan and reduce */) 
	{
	  /* in-lining */
	  do_pops (p,
		   ltd_p,
		   lgth = -ref - bnf_ag.WS_NBPRO [parse_stack.red_no = bnf_ag.WS_INDPRO [-ref].prolis].prolon);
	  seek_paths (p, push_rhs (parse_stack.current_symbol, 0), false, lgth);
	}
      else
	  add_active_parser (p, ltd_p, ref, parse_stack.current_symbol);
    } while (!SS_is_empty (parse_stack.for_scanner));
  
  return true;
}



static void unused_rule_elimination (start_symbol)
    int start_symbol;
{
  /* Let R be the relation : x R y <=> x -> alpha y beta in P.
     A rule p is kept iff p = A -> alpha and A in {y | s R+ y}. */
  
  SXBA	*R_plus, line;
  int		size, lim, rule_no, lim1, x, symbol;
  
  size = XxY_top (parse_stack.symbols) + 1;
  R_plus = sxbm_calloc (size, size);
  
  for (lim = XH_top (parse_stack.output_grammar),
       rule_no = 1;
       rule_no < lim;
       rule_no++) 
    {
      lim1 = XH_X (parse_stack.output_grammar, rule_no + 1);
      x  = XH_X (parse_stack.output_grammar, rule_no);
      line = R_plus [XH_list_elem (parse_stack.output_grammar, x)];
      
      while (++x < lim1) 
	{
	  if ((symbol = XH_list_elem (parse_stack.output_grammar, x)) > 0)
	      SXBA_1_bit (line, symbol);
	}
    }
  
  fermer (R_plus, size);
  
  line = R_plus [start_symbol];
  SXBA_1_bit (line, start_symbol);
  
  parse_stack.G.N = sxba_cardinal (line);
  parse_stack.G.T = parse_stack.current_level;
  parse_stack.G.G = parse_stack.G.P = 0;
  
  for (rule_no = 1;
       rule_no < lim;
       rule_no++) 
    {
      x = XH_X (parse_stack.output_grammar, rule_no);
      symbol = XH_list_elem (parse_stack.output_grammar, x);
      
      if (SXBA_bit_is_set (line, symbol)) 
	{
	  ++parse_stack.G.P;
	  parse_stack.G.G += XH_X (parse_stack.output_grammar, rule_no + 1) - x;
	}
      else
	  XH_erase (parse_stack.output_grammar, rule_no);
    }
  
  sxbm_free (R_plus);
}



static void output_grammar (start_symbol)
    int start_symbol;
{
  int lim, rule_no;
  
  printf ("*\t(|N| = %i, |T| = %i, |P| = %i, |G| = %i)\n\n",
	  parse_stack.G.N,  
	  parse_stack.G.T,  
	  parse_stack.G.P,  
	  parse_stack.G.G);
  
  if (start_symbol > 0) 
    {
      fputs ("<start_symbol> \t= ", stdout);
      output_symbol (start_symbol);
      putchar (';');
      putchar ('\n');
    }
  else
      fputs ("*\tThere is no start symbol in this parse forest.\n\n", stdout);
  
  for (lim = XH_top (parse_stack.output_grammar),
       rule_no = 1;
       rule_no < lim;
       rule_no++) 
    {
      if (!XH_is_erased (parse_stack.output_grammar, rule_no)) 
	{
	  output_grammar_rule (rule_no); 
	}
    }
}



static bool sxparse_it ()
{
  int				x, y, p, ref, lgth, lim;
  struct parse_stack_elem	*agraph;
  struct SXP_prdct		*aprdct;
  
  parse_stack.start_symbol = 0;
  parse_stack.current_token = sxplocals.SXP_tables.P_tmax;
  
  /* create a stack node p with state START_STATE(grammar) */
  p = parse_stack.graph_top = 1;
  agraph = &(parse_stack.graph [p]);
  agraph->state = 1; /* why not! */
  agraph->red_ref = 0;
  agraph->shift_ref = 1;
  
  /* no son */
  agraph->son = 0;
  agraph->level = 0;
  /* agraph->symbol = ... */
  PUSH (parse_stack.for_shifter, agraph->state);
  parse_stack.active_parsers [agraph->state] = p;
  parse_stack.rhs_top = 0;
  shifter ();
  
  /* in_lining of parseword () */
  do
    {
      /* for_actor == active_parsers */
      /* for_shifter == phi */
      if (!SS_is_empty (parse_stack.for_actor)) 
	{
	  /* while for_actor <> phi  do */
	  do
	    {
	      /* remove a parser p from for_actor */
	      p = SS_pop (parse_stack.for_actor);
	      ltd_p = SS_pop (ltd_parse_stack.for_actor);
	      
	      if ((ref = parse_stack.graph [p].red_ref) == 0) 
		{
		  /* Shift and reduce parser */
		  ref = parse_stack.graph [p].state - lr0_automaton_state_nb;
		}
	      
	      if (ref > 0) 
		{
		  parse_stack.red_no = ref;
		  ltd_parse_stack.red_no = ref;
		  do_pops (p, ltd_p, LGPROD (parse_stack.red_no));
		}
	      else
		{
		  register struct Q0	*aQ0 = Q0 - ref;
		  SXBA     aworthy_red    = common_struct.worthy_red [-ref];
		  
		  ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb;
		  x = 0;
		  while ((x = sxba_1_lrscan (x, aworthy_red)) > 0)
		    {
		      parse_stack.red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref + x]].prolis;
		      ltd_parse_stack.red_no = parse_stack.red_no;
		      do_pops (p, ltd_p, LGPROD (parse_stack.red_no));
		    }
		}
	    } while (!SS_is_empty (parse_stack.for_actor));
	  
	  while (parse_stack.is_new_links) 
	    {
	      parse_stack.is_new_links = false;
	      sxba_copy (parse_stack.used_links, parse_stack.current_links);
	      sxba_empty (parse_stack.current_links);
	      sxba_copy (ltd_parse_stack.used_links, ltd_parse_stack.current_links);
	      sxba_empty (ltd_parse_stack.current_links);
	      
	      /* for_actor == active_parsers */
	      p = 0;
	      while ((p = parse_stack.active_states [p]) > 0)
		  SS_push (parse_stack.for_actor, parse_stack.active_parsers [p]);
	      p = 0;
	      while ((p = ltd_parse_stack.active_states [p]) > 0)
		  SS_push (ltd_parse_stack.for_actor, ltd_parse_stack.active_parsers [p]);
	      
	      do 
		{
		  /* remove a parser p from for_actor */
		  p = SS_pop (parse_stack.for_actor);
		  ltd_p = SS_pop (ltd_parse_stack.for_actor);
		  
		  if ((ref = parse_stack.graph [p].red_ref) == 0) 
		    {
		      /* Shift and reduce parser */
		      ref = parse_stack.graph [p].state - lr0_automaton_state_nb;;
		    }
		  
		  if (ref > 0)
		    {
		      if ((lgth = LGPROD (ref)) > 0) 
			{
			  parse_stack.red_no = ref;
			  ltd_parse_stack.red_no = ref;
			  do_limited_reductions (p, ltd_p, lgth);
			}
		    }
		  else
		    {
		      register struct Q0	*aQ0 = Q0 - ref;
		      SXBA     aworthy_red    = common_struct.worthy_red [-ref];
		      
		      ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb;
		      x = 0;
		      while ((x = sxba_1_lrscan (x, aworthy_red)) > 0)
			{
			  parse_stack.red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref + x]].prolis;
			  ltd_parse_stack.red_no = parse_stack.red_no;
			  if ((lgth = LGPROD (parse_stack.red_no)) > 0)
			      do_limited_reductions (p, ltd_p, lgth);
			}
		    }
		} while (!SS_is_empty (parse_stack.for_actor));
	    }
	  
	  parse_forest ();
	}
      
    } while (shifter (true));
  
  if (parse_stack.start_symbol == 0)
      sxput_error (SXGET_TOKEN (sxplocals.ptok_no).source_index,
		   "%sSyntax error.",
		   sxplocals.sxtables->err_titles [2]);
  else
      unused_rule_elimination (parse_stack.start_symbol);
  
  output_grammar (parse_stack.start_symbol);
  
  return parse_stack.start_symbol > 0;
}


bool sxindparser (what_to_do, arg)
    int		what_to_do;
    struct sxtables	*arg;
{
  switch (what_to_do) 
    {
    case SXBEGIN:
      return true;
      
    case SXOPEN:
      /* new language: new tables, new local parser variables */
      /* the global variable "sxplocals" must have been saved in case */
      /* of recursive invocation */
      
      /* test arg->magic for consistency */
      sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, ME);
      
      /* internal grammar form reading. */
      /* Assume that the name of the language has been given in the "record" spec!! */
      if (!bnf_read (&bnf_ag, arg->err_titles [0] + 1))
	  sxexit (2);
      
      /* LR (0) automaton construction. */
      LR0 ();
      XH_lock (&nucleus_hd);
      
      lr0_automaton_state_nb = xac2 - 1;
      
      parse_stack.graph = (struct parse_stack_elem*)
	  sxalloc ((parse_stack.graph_size = 128) + 1,
		   sizeof (struct parse_stack_elem));
      ltd_parse_stack.graph = (struct ltd_parse_stack_elem*) 
	  sxalloc ((ltd_parse_stack.graph_size = 128) + 1,
		   sizeof (struct ltd_parse_stack_elem));
      
      parse_stack.graph_top = 0;
      ltd_parse_stack.graph_top = 0;
      
      parse_stack.free_parsers_set = sxba_calloc (parse_stack.graph_size + 1);
      ltd_parse_stack.free_parsers_set = sxba_calloc (ltd_parse_stack.graph_size + 1);
      
      sxba_fill (parse_stack.free_parsers_set);
      sxba_fill (ltd_parse_stack.free_parsers_set);
      
      parse_stack.free_parser = 0;
      ltd_parse_stack.free_parser = 0;
      
      ltd_parse_stack.lr_state_occ = (struct occurence*) 
	  sxcalloc (lr0_automaton_state_nb + 1, sizeof (struct occurence));
    {
      register int i;
      
      for (i=0; i<=lr0_automaton_state_nb + 1; i++) 
	{
	  ltd_parse_stack.lr_state_occ [i].number = 0;
	  ltd_parse_stack.lr_state_occ [i].last = 0;
	}
      
    }
      
      parse_stack.parsers_set = sxbm_calloc (4, parse_stack.graph_size + 1);
      ltd_parse_stack.parsers_set = sxbm_calloc (4, ltd_parse_stack.graph_size + 1);
      
      parse_stack.parsers_rhs0 = (int*) sxalloc (parse_stack.graph_size + 1, sizeof (int));
      parse_stack.parsers_rhs1 = (int*) sxalloc (parse_stack.graph_size + 1, sizeof (int));
      
    {
      register int SIZE = lr0_automaton_state_nb + bnf_ag.WS_TBL_SIZE.xnbpro + 1;
      
      parse_stack.active_states = (int*) sxcalloc (SIZE, sizeof (int));
      parse_stack.active_states [0] = -1;
      parse_stack.for_shifter = (int*) sxcalloc (SIZE, sizeof (int));
      parse_stack.for_shifter [0] = -1;
      parse_stack.active_parsers = (int*) sxalloc (SIZE, sizeof (int));
      
      ltd_parse_stack.active_states = (int*) sxcalloc (SIZE, sizeof (int));
      ltd_parse_stack.active_states [0] = -1;
      ltd_parse_stack.for_shifter = (int*) sxcalloc (SIZE, sizeof (int));
      ltd_parse_stack.for_shifter [0] = -1;
      ltd_parse_stack.active_parsers = (int*) sxalloc (SIZE, sizeof (int));
    }
      
    {
      register int SIZE = arg->SXP_tables.Mred + 1;
      
      common_struct.max_red_nb = SIZE;
      ltd_parse_stack.family_Mb = sxba_calloc (SIZE);
      common_struct.worthy_red = sxbm_calloc (lr0_automaton_state_nb + 1, SIZE);
      
      ltd_parse_stack.families = sxbm_calloc (parse_stack.graph_size + 1, SIZE);
      
      parse_stack.red_stack = (int*) sxcalloc (SIZE, sizeof (int));
      parse_stack.red_stack [0] = -1;
      parse_stack.red_parsers_hd = (int*) sxalloc (SIZE, sizeof (int));
      parse_stack.red_parsers_list = (struct red_parsers*) 
	  sxalloc (parse_stack.red_parsers_list_size = SIZE, sizeof (struct red_parsers));
      parse_stack.red_parsers_list_top = 0;
    }
      
      parse_stack.for_actor = SS_alloc (20);
      ltd_parse_stack.for_actor = SS_alloc (20);
      
      parse_stack.for_scanner = SS_alloc (20);
      ltd_parse_stack.for_scanner = SS_alloc (20);
      
      parse_stack.GC_area = SS_alloc (20);
      ltd_parse_stack.GC_area = SS_alloc (20);
      
      XxY_alloc (&parse_stack.sons, "sons", 128, 1, 1, 0, sons_oflw, NULL);
      XxY_alloc (&ltd_parse_stack.sons, "ltd_sons", 128, 1, 1, 0, sons_oflw, NULL);
      
      parse_stack.erasable_sons = sxba_calloc (XxY_size (parse_stack.sons) + 1);
      ltd_parse_stack.erasable_sons = sxba_calloc (XxY_size (ltd_parse_stack.sons) + 1);
      
      parse_stack.current_links = sxba_calloc (XxY_size (parse_stack.sons) + 1);
      ltd_parse_stack.current_links = sxba_calloc (XxY_size (ltd_parse_stack.sons) + 1);
      
      parse_stack.used_links = sxba_calloc (XxY_size (parse_stack.sons) + 1);
      ltd_parse_stack.used_links = sxba_calloc (XxY_size (ltd_parse_stack.sons) + 1);
      
      parse_stack.transitions = (int*) sxalloc (XxY_size (parse_stack.sons) + 1, sizeof (int));
      ltd_parse_stack.transitions = (int*) sxalloc (XxY_size (parse_stack.sons) + 1, sizeof (int));
      
      
      XxY_alloc (&parse_stack.suffixes, "suffixes", 128, 1, 0, 0, NULL, NULL);
      XxY_alloc (&parse_stack.ranges, "ranges", 128, 1, 0, 0, NULL, NULL);
      XxY_alloc (&parse_stack.symbols, "symbols", 128, 1, 0, 0, NULL, NULL);
      
      XH_alloc (&parse_stack.output_grammar, "output_grammar", 128, 1, 4, NULL, NULL);
      
      parse_stack.rhs = (struct rhs*) 
	  sxalloc ((parse_stack.rhs_size = 50) + 1, sizeof (struct rhs));
      
      /* prepare new local variables */
      
      sxplocals.sxtables = arg;
      sxplocals.SXP_tables = arg->SXP_tables;
      sxtkn_mngr (SXOPEN, NULL, sxplocals.SXP_tables.P_sizofpts * 8);
      (*sxplocals.SXP_tables.P_recovery) (SXOPEN, NULL /* dummy */, (SXINT) 0 /* dummy */);
      return true;
      
    case SXINIT:
      /* on initialise toks_buf avec "EOF" */
      
    {
      struct sxtoken tok;
      
      /* terminal_token EOF */
      tok.lahead = sxplocals.SXP_tables.P_tmax;
      tok.string_table_entry = SXEMPTY_STE;
      tok.source_index = sxsrcmngr.source_coord;
      tok.comment = NULL;
      sxtkn_mngr (SXINIT, &tok, 0);
    }
      
      /* analyse normale */
      /* valeurs par defaut qui peut etre changee ds les
	 scan_act ou pars_act. */
      sxplocals.mode.look_back = 1; 
      sxplocals.mode.kind = SXWITH_RECOVERY;
      sxplocals.mode.errors_nb = 0;
      sxplocals.mode.is_prefixe = false;
      sxplocals.mode.is_silent = false;
      sxplocals.mode.with_semact = true;
      sxplocals.mode.with_parsact = true;
      sxplocals.mode.with_parsprdct = true;
      return true;
      
    case SXACTION:
      return sxparse_it ();
      
    case SXFINAL:
      sxtkn_mngr (SXFINAL, NULL, 0);
      return true;
      
    case SXCLOSE:
      /* end of language: free the local arrays */
      
      sxfree (ltd_parse_stack.lr_state_occ);
      sxfree (common_struct.worthy_red);
      sxfree (ltd_parse_stack.family_Mb);
      
      sxfree (parse_stack.graph);
      sxfree (ltd_parse_stack.graph);
      
      sxfree (parse_stack.free_parsers_set);
      sxfree (ltd_parse_stack.free_parsers_set);
      
      sxfree (parse_stack.parsers_set);
      sxfree (ltd_parse_stack.parsers_set);
      sxfree (ltd_parse_stack.families);
      
      sxfree (parse_stack.active_states);
      sxfree (ltd_parse_stack.active_states);
      
      sxfree (parse_stack.for_shifter);
      sxfree (ltd_parse_stack.for_shifter);
      
      sxfree (parse_stack.active_parsers);
      sxfree (ltd_parse_stack.active_parsers);
      
      SS_free (parse_stack.for_actor);
      SS_free (ltd_parse_stack.for_actor);
      
      SS_free (parse_stack.for_scanner);
      SS_free (ltd_parse_stack.for_scanner);
      
      SS_free (parse_stack.GC_area);
      SS_free (ltd_parse_stack.GC_area);
      
      sxfree (parse_stack.red_stack);
      sxfree (parse_stack.red_parsers_hd);
      sxfree (parse_stack.red_parsers_list);
      
      sxfree (parse_stack.transitions);
      sxfree (ltd_parse_stack.transitions);
      
      XxY_free (&parse_stack.sons);
      XxY_free (&ltd_parse_stack.sons);
      
      XxY_free (&parse_stack.suffixes);
      XxY_free (&parse_stack.ranges);
      XxY_free (&parse_stack.symbols);
      XH_free (&parse_stack.output_grammar);
      
      sxfree (parse_stack.erasable_sons);
      sxfree (ltd_parse_stack.erasable_sons);
      
      sxfree (parse_stack.current_links);
      sxfree (ltd_parse_stack.current_links);
      
      sxfree (parse_stack.used_links);
      sxfree (ltd_parse_stack.used_links);
      
      sxfree (parse_stack.rhs);
      sxfree (parse_stack.parsers_rhs0);
      sxfree (parse_stack.parsers_rhs1);
      
      sxtkn_mngr (SXCLOSE, NULL, 0);
      bnf_free (&bnf_ag);
      return true;
      
    case SXEND:
      (*sxplocals.SXP_tables.P_recovery) (SXCLOSE, NULL /* dummy */, (SXINT) 0 /* dummy */);
      return true;
      
    default:
      fprintf (sxstderr, "The function \"sxindparser\" is out of date with respect to its specification.\n");
      abort ();
    }
}
