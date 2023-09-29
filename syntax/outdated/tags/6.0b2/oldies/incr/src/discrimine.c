static SXVOID discrimine (shift_ref, parser)
    int	shift_ref, parser;
{
  int				son, ltd_parser, i, p, ref, lgth, lim, shift_ambiguity, red_no;
  struct parse_stack_elem	*agraph = parse_stack.graph + parser;
  struct ltd_parse_stack_elem	*ltd_agraph;
  int 				state = agraph->state;
  register struct Q0		*aQ0 = Q0 + state;
  int 				y;

  /* Recopie de active_parsers, active_states, for-actor et for-shifter dans un tampon */
  

  y = 0;
  while ((y = ltd_parse_stack.active_states [y]) > 0)
    {
      POP  (ltd_parse_stack.active_states, y);
      PUSH (ltd_parse_stack.tmp_active_states, y);
      ltd_parse_stack.tmp_active_parsers [y] = ltd_parse_stack.active_parsers [y];
    }
  
  while (!SS_is_empty (ltd_parse_stack.for_actor))
      SS_push (ltd_parse_stack.tmp_for_actor, SS_pop (ltd_parse_stack.for_actor));
  
  y = 0;
  while ((y = ltd_parse_stack.for_shifter [y]) > 0)
    {
      POP  (ltd_parse_stack.for_shifter, y);
      PUSH (ltd_parse_stack.tmp_for_shifter, y);
    }
  
  /* Calcul de la constante de correspondance nb-de-red -> nb-d-ambiguites */
  shift_ambiguity = (shift_ref > 0) ? 0 : -1;

  /* Creation du noeud, dans la pile limitee, correspondant a parser */
  give_father
      (&ltd_parser,
       son = ltd_parse_stack.active_parsers [parse_stack.graph [agraph->son].state]);
			 
  ltd_agraph = &(ltd_parse_stack.graph [ltd_parser]);
  
  /* add a link from father to son */
  ltd_agraph->son = son;
  ltd_agraph->symbol = agraph->symbol;
  ltd_agraph->state = state;
  ltd_agraph->parse_time_built = TRUE;
  sxba_empty (ltd_parse_stack.families [ltd_parser]);

  /* Initialisation des familles avec leur premier membre */

  lim = aQ0->red_trans_nb;
  ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb;
  red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
  
  if (lim == 1) 
    {
      sxba_1_bit (ltd_parse_stack.families [ltd_parser], 1);
      ltd_parse_stack.red_no = red_no;
      ltd_do_pops (FALSE, ltd_parser, LGPROD (red_no));
      sxba_0_bit (ltd_parse_stack.families [ltd_parser], 1);
    }
  else
    {
      register struct Q0	*aQ0	    = Q0 + state;
      
      lim = aQ0->red_trans_nb + ref;
      i = 0;
      
      do
	{
	  sxba_0_bit (ltd_parse_stack.families [ltd_parser], i++);
	  sxba_1_bit (ltd_parse_stack.families [ltd_parser], i);
	  ltd_parse_stack.red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
	  ltd_do_pops (FALSE, ltd_parser, LGPROD (ltd_parse_stack.red_no));
	} while (++ref < lim);

      sxba_0_bit (ltd_parse_stack.families [ltd_parser], i);
    }

  /* On traite le cas shift apres, pour que la famille de ltd_parser reste telle quelle */
  if (shift_ref > 0)
    {
      ltd_agraph->shift_ref = Q0xV_to_Q0 [shift_ref];
      PUSH (ltd_parse_stack.for_shifter, state);
      ltd_parse_stack.active_parsers [state] = ltd_parser;
      sxba_1_bit (ltd_parse_stack.families [ltd_parser], 0);
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
	      /* remove a ltd_parser p from for_actor */
	      p = SS_pop (ltd_parse_stack.for_actor);
	      
	      if ((ref = ltd_parse_stack.graph [p].red_ref) == 0)
		{
		  /* Shift and reduce ltd_parser */
		  ref = ltd_parse_stack.graph [p].state - lr0_automaton_state_nb;
		}
	      
	      if (ref > 0)
		{
		  ltd_parse_stack.red_no = ref;
		  ltd_do_pops (FALSE, p, LGPROD (ltd_parse_stack.red_no));
		}
	      else
		{
		  register struct Q0	*aQ0	    = Q0 - ref;
		  
		  ref = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb;
		  lim = aQ0->red_trans_nb + ref;
		  do
		    {
		      ltd_parse_stack.red_no = bnf_ag.WS_INDPRO [-Q0xV_to_Q0 [ref]].prolis;
		      if ((lgth = LGPROD (ltd_parse_stack.red_no)) > 0)
			  ltd_do_pops (FALSE, p, lgth);
		    } while (++ref < lim);
		}
	    } while (!SS_is_empty (ltd_parse_stack.for_actor));
	  
	  while (ltd_parse_stack.is_new_links)
	    {
	      ltd_parse_stack.is_new_links = FALSE;
	      sxba_copy (ltd_parse_stack.used_links, ltd_parse_stack.current_links);
	      sxba_empty (ltd_parse_stack.current_links);
	      
	      /* for_actor == active_parsers */
	      p = 0;
	      
	      while ((p = ltd_parse_stack.active_states [p]) > 0)
		  SS_push (ltd_parse_stack.for_actor, ltd_parse_stack.active_parsers [p]);
	      
	      do {
		/* remove a ltd_parser p from for_actor */
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
			ltd_do_limited_reductions (TRUE, p, lgth);
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
			    ltd_do_limited_reductions (TRUE, p, lgth);
		      } while (++ref < lim);
		  }
		
	      } while (!SS_is_empty (ltd_parse_stack.for_actor));
	      
	    }
	  
	}

      ltd_shifter(FALSE);
      sxba_empty (ltd_parse_stack.family_Mb);
      y = 0;
      while (ltd_parse_stack.active_states [0] > 0)
      {
	POP (ltd_parse_stack.active_states, y);
	ltd_parse_stack.family_Mb = sxba_or (ltd_parse_stack.family_Mb, 
					     ltd_parse_stack.families [ltd_parse_stack.active_parsers [y]]);
      }
  
      y = 0;
      while ((y = ltd_parse_stack.for_shifter [y]) > 0) 
	  ltd_parse_stack.family_Mb = sxba_or (ltd_parse_stack.family_Mb, 
					       ltd_parse_stack.families [ltd_parse_stack.active_parsers [y]]);
      
    } while (parse_stack.current_token != sxplocals.SXP_tables.P_tmax &&
	     sxba_cardinal (ltd_parse_stack.family_Mb) + shift_ambiguity > 0); 

  common_struct.worthy_red [state] = ltd_parse_stack.family_Mb;
  
  /* Recuperation de active_parsers, active_states, for-actor et for-shifter du tampon */
  y = 0;
  while ((y = ltd_parse_stack.tmp_active_states [y]) > 0)
    {
      POP  (ltd_parse_stack.tmp_active_states, y);
      PUSH (ltd_parse_stack.active_states, y);
      ltd_parse_stack.active_parsers [y] = ltd_parse_stack.tmp_active_parsers [y];
    }
  
  while (!SS_is_empty (ltd_parse_stack.tmp_for_actor))
      SS_push (ltd_parse_stack.for_actor, SS_pop (ltd_parse_stack.tmp_for_actor));
  
  y = 0;
  while ((y = ltd_parse_stack.tmp_for_shifter [y]) > 0)
    {
      POP  (ltd_parse_stack.tmp_for_shifter, y);
      PUSH (ltd_parse_stack.for_shifter, y);
    }
  
}
