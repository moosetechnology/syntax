/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1994 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ChLoE.			  *
   *                                                      *
   ******************************************************** */

/* Execute les actions et predicats syntaxiques produits a partir d'une
   Linear Indexed Grammar par le traducteur lig2bnf dans le cas d'une
   analyse deterministe. */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030506 10:21 (phd):	Modif pour calmer un peu le compilo SGI	*/
/************************************************************************/
/* 23-01-95 11:47 (pb):		Nouvelle tables des LIG			*/
/************************************************************************/
/* 10-08-94 14:29 (pb):		Prise en compte des post-actions de	*/
/*				l'utilisateur				*/
/************************************************************************/
/* 10-08-94 14:29 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)sxdligparsact.c	- SYNTAX [unix] - Lundi 23 Janvier 1995";

#if 0
static char	ME [] = "sxdligparsact";
#endif

#include "sxunix.h"
#include "sxindex.h"
#include "SS.h"

#define SECONDARY	0
#define PRIMARY		1


/* SXDO travaille sur where [1]
   SXACTION travaille sur where [0] (apres swap eventuel)
   SXUNDO travaille sur where [1]
*/



static void tank_oflw (ptank, old_line_nb, old_size)
    sxindex_header	*ptank;
    int			old_line_nb, old_size;
{
    int **p, **lim;

    sxliglocals.stacks = (int**) sxrealloc (sxliglocals.stacks,
					    sxindex_size (*ptank) + 1,
					    sizeof (int*));

    lim = sxliglocals.stacks + sxindex_size (*ptank);

    for (p = sxliglocals.stacks + old_size; p <= lim; p++)
	*p = NULL;
}


bool sxdligparsact (int	which, SXTABLES *arg)
{
    int *p, *plim, i, code, *pc, stack_id, *cur_stack, *pclim, xs, si, newtop, top, act_no, pop_nb, push_nb, elem;

    switch (which) {
    case SXOPEN:
	sxliglocals.sxtables = arg;
	sxliglocals.code = *(arg->sxligparsact);

	sxliglocals.DO_stack = (int *) sxalloc ((sxliglocals.DO_stack_size = 100) + 1,
						sizeof (int));

	plim = sxliglocals.DO_stack + sxliglocals.DO_stack_size;

	for (p = sxliglocals.DO_stack; p <= plim; p++)
	    *p = -1;

	sxindex_alloc (&sxliglocals.tank, 0, 10, tank_oflw);
	sxliglocals.stacks = (int **) sxcalloc ((SXUINT) (sxindex_size (sxliglocals.tank) + 1),
						sizeof (int*));

	sxliglocals.store1.act.ss = SS_alloc (50);
	sxliglocals.store1.act.cur_bot = SS_bot (sxliglocals.store1.act.ss);
	sxliglocals.store1.pop.ss = SS_alloc (50);
	sxliglocals.store1.pop.cur_bot = SS_bot (sxliglocals.store1.pop.ss);

	sxliglocals.store2.act.ss = SS_alloc (50);
	sxliglocals.store2.act.cur_bot = SS_bot (sxliglocals.store2.act.ss);
	sxliglocals.store2.pop.ss = SS_alloc (50);
	sxliglocals.store2.pop.cur_bot = SS_bot (sxliglocals.store2.pop.ss);

	sxliglocals.where [0] = &sxliglocals.store1;
	sxliglocals.where [1] = &sxliglocals.store2;

	(*sxliglocals.code.parsact) (which, arg);

	return SXANY_BOOL;

    case SXCLOSE:
	sxfree (sxliglocals.DO_stack);
	sxindex_free (sxliglocals.tank);

	for (i = sxindex_size (sxliglocals.tank); i >= 0; i--)
	    if (sxliglocals.stacks [i] != NULL)
		SS_free (sxliglocals.stacks [i]);

	sxfree (sxliglocals.stacks);

	SS_free (sxliglocals.store1.act.ss);
	SS_free (sxliglocals.store1.pop.ss);
	SS_free (sxliglocals.store2.act.ss);
	SS_free (sxliglocals.store2.pop.ss);

	(*sxliglocals.code.parsact) (which, arg);

	return SXANY_BOOL;

    case SXINIT:
	sxplocals.mode.with_do_undo = true;
	(*sxliglocals.code.parsact) (which, arg);

	return SXANY_BOOL;

    case SXFINAL:
	(*sxliglocals.code.parsact) (which, arg);

	return SXANY_BOOL;

    case SXACTION:
	act_no = (long) arg;

	if (act_no < 10000)
	{
	    (*sxliglocals.code.parsact) (which, act_no);
	    return SXANY_BOOL; 
	}
	
	act_no -= 10000;

	if (act_no == 0)
	    /* empty action */
	    return SXANY_BOOL;

	if (SS_top (sxliglocals.where [0]->act.ss) == sxliglocals.where [0]->act.cur_bot)
	{
	    /* sxliglocals.where [0] est vide, on swappe */
	    struct store	*p =  sxliglocals.where [0];

	    sxliglocals.where [0] = sxliglocals.where [1];
	    sxliglocals.where [1] = p;
	}

	newtop = SXSTACKnewtop ();

#ifdef EBUG
	if (act_no != SS_get (sxliglocals.where [0]->act.ss, sxliglocals.where [0]->act.cur_bot++))
	    sxtrap (ME, "parsing action process");

	if (newtop != SS_get (sxliglocals.where [0]->act.ss, sxliglocals.where [0]->act.cur_bot++))
	    sxtrap (ME, "parsing action process");   
#endif

	stack_id = SS_get (sxliglocals.where [0]->act.ss, sxliglocals.where [0]->act.cur_bot++);
	pop_nb = SS_get (sxliglocals.where [0]->act.ss, sxliglocals.where [0]->act.cur_bot++);

	sxliglocals.where [0]->pop.cur_bot += pop_nb;

#ifdef EBUG
	if (SS_top (sxliglocals.where [0]->pop.ss) < sxliglocals.where [0]->pop.cur_bot)
	    sxtrap (ME, "parsing action process"); 
#endif

	top = SXSTACKtop ();

#ifdef EBUG
	if (top != SS_get (sxliglocals.where [0]->act.ss, sxliglocals.where [0]->act.cur_bot++))
	    sxtrap (ME, "parsing action process");
#endif

	/* On recupere les "piles" */
	for (xs = newtop; xs <= top; xs++)
	{
	    si = SS_get (sxliglocals.where [0]->pop.ss, sxliglocals.where [0]->pop.cur_bot++);

	    if (si >= 0 && si != stack_id)
		sxindex_release (sxliglocals.tank, si);
	}

	/* post-action */
	code = sxliglocals.code.prdct_or_act_code [sxliglocals.code.prdct_or_act_disp [act_no + 1] - 1];

	if (code >= 0) {
           (void) ((*sxliglocals.code.parsact) (which, code));
        }
	return SXANY_BOOL;

    case SXDO:
	/*
	   (SECONDARY, {push_elem}*, post_action)
	   <A>() = c ; 		=>  (0, -1)	: creation d'une pile vide
	   <A>(a) = c ; 	=>  (0, a, -1)	: creation d'une pile vide + push (a)

	   (PRIMARY, index_delta, prdct_lgth, common_prefix_lgth, {push_elem}*, post_action) 

	   <A>(..a) = X0()... Xi(..b) Xp() ;
	                        =>  (1,i,1,0,a,-1): Pile primaire a l'indice new_top+i
				                  on pop 1 element
						  on push (a)
	*/
			
	act_no = (long) arg;

	if (act_no < 10000)
	{
	    (*sxliglocals.code.parsact) (which, act_no);
	    return false; 
	}

	act_no -= 10000;

	if (act_no == 0)
	    /* empty action */
	    return false;

	pc = sxliglocals.code.prdct_or_act_code + sxliglocals.code.prdct_or_act_disp [act_no];
	code = *pc++;
	newtop = SXSTACKnewtop ();

#ifdef EBUG
	SS_push (sxliglocals.where [1]->act.ss, act_no);
	SS_push (sxliglocals.where [1]->act.ss, newtop);
#endif

	if (code == SECONDARY)
	{
	    pop_nb = 0;
	    /* creation d'une pile */
	    stack_id = sxindex_seek (&sxliglocals.tank);

	    if ((cur_stack = sxliglocals.stacks [stack_id]) == NULL)
		sxliglocals.stacks [stack_id] = SS_alloc (50);
	    else
		SS_clear (cur_stack);
	}
	else
	{
	  /* pc [1] == longueur du test du predicat
	     pc [2] == longueur du prefixe commun entre action et predicat. */
	    pop_nb = pc [1] - pc [2];
	    stack_id = sxliglocals.DO_stack [newtop + pc [0]];

#ifdef EBUG
	    if (stack_id == -1)
		sxtrap (ME, "parsing action process");
#endif

	    pc += 3 + pc [2];
	}

	SS_push (sxliglocals.where [1]->act.ss, stack_id);
	SS_push (sxliglocals.where [1]->act.ss, pop_nb);

	/* On pop */
	while (pop_nb-- > 0)
	{
	    elem = SS_pop (sxliglocals.stacks [stack_id]);
	    SS_push (sxliglocals.where [1]->pop.ss, elem);
	}

	pclim = sxliglocals.code.prdct_or_act_code + sxliglocals.code.prdct_or_act_disp [act_no + 1] - 1;

	/* on push */
	while (pc < pclim)
	    /* Il peut y avoir reallocation */
	    SS_push (sxliglocals.stacks [stack_id], *pc++);

	if (newtop >= sxliglocals.DO_stack_size)
	{
	    sxliglocals.DO_stack = (int*) sxrealloc (sxliglocals.DO_stack,
						     (sxliglocals.DO_stack_size *= 2) + 1,
						     sizeof (int));
	    plim = sxliglocals.DO_stack + sxliglocals.DO_stack_size;

	    for (p = sxliglocals.DO_stack + (sxliglocals.DO_stack_size >> 1) + 1;
		 p <= plim;
		 p++)
		*p = -1;
	}

	/* On recupere les "piles" */
	top = SXSTACKtop ();

#ifdef EBUG
	SS_push (sxliglocals.where [1]->act.ss, top);
#endif

	for (xs = newtop; xs <= top; xs++)
	{
	    si = sxliglocals.DO_stack [xs];
	    SS_push (sxliglocals.where [1]->pop.ss, si);

	    if (si >= 0)
		sxliglocals.DO_stack [xs] = -1;
	}

	sxliglocals.DO_stack [newtop] = stack_id;

	/* Appel de la post-action utilisateur eventuelle */
	if (*pclim >= 0) {
           (void) ((*sxliglocals.code.parsact) (which, *pclim));
        }
	return SXANY_BOOL;


    case SXUNDO:
	act_no = (long) arg;

	if (act_no < 10000)
	{
	    (*sxliglocals.code.parsact) (which, act_no);
	    return false; 
	}

	act_no -= 10000;

	if (act_no == 0)
	    /* empty action */
	    return false;

	if (SS_top (sxliglocals.where [1]->act.ss) == sxliglocals.where [1]->act.cur_bot)
	{
	    /* sxliglocals.where [1] est vide, on swappe */
	    struct store	*p =  sxliglocals.where [1];

	    sxliglocals.where [1] = sxliglocals.where [0];
	    sxliglocals.where [0] = p;
	}

	pclim = sxliglocals.code.prdct_or_act_code + sxliglocals.code.prdct_or_act_disp [act_no + 1] - 1;

	if ((code = *pclim) >= 0)
	    /* On defait la post-action utilisateur */
	    (*sxliglocals.code.parsact) (which, code);

	top = SXSTACKtop ();

#ifdef EBUG
	if (top != SS_pop (sxliglocals.where [1]->act.ss))
	    sxtrap (ME, "parsing action process");   
#endif
	
	newtop = SXSTACKnewtop ();
	pop_nb = SS_pop (sxliglocals.where [1]->act.ss);
	stack_id = SS_pop (sxliglocals.where [1]->act.ss);

#ifdef EBUG
	if (newtop != SS_pop (sxliglocals.where [1]->act.ss))
	    sxtrap (ME, "parsing action process");   

	if (act_no != SS_pop (sxliglocals.where [1]->act.ss))
	    sxtrap (ME, "parsing action process");    
#endif
	
	/* On refabrique la pile courante. */
	/* On pop */

	pc = sxliglocals.code.prdct_or_act_code + sxliglocals.code.prdct_or_act_disp [act_no];

	if (*pc == SECONDARY)
	  push_nb = pclim - pc - 1;
	else
	  push_nb = pclim - pc - 3 - pc [3];
	    
	while (push_nb-- > 0)
	    SS_decr (sxliglocals.stacks [stack_id]);

	/* Reaffectation en sens inverse du SXDO */
	for (xs = top; xs >= newtop; xs--)
	    sxliglocals.DO_stack [xs] = SS_pop (sxliglocals.where [1]->pop.ss);

	/* On push */
	while (pop_nb-- > 0)
	{
	    elem = SS_pop (sxliglocals.where [1]->pop.ss);
	    SS_push (sxliglocals.stacks [stack_id], elem);
	}

	if (code == SECONDARY)
	    /* On rend la pile */
	    sxindex_release (sxliglocals.tank, stack_id);

	return false;

    case SXPREDICATE:
	act_no = (long) arg;

	if (act_no >= 10000)
	  {
	    act_no -= 10000;
	    stack_id = sxliglocals.DO_stack [SXSTACKtop ()];

#ifdef EBUG
	    if (stack_id == -1)
	      sxtrap (ME, "parsing action process");
#endif

	    cur_stack = sxliglocals.stacks [stack_id];

	    if (act_no == 0)
	      /* ( ) */
	      return SS_is_empty (cur_stack);

	    top = SS_top (cur_stack);

	    pc = sxliglocals.code.prdct_or_act_code + sxliglocals.code.prdct_or_act_disp [act_no];
	    pclim = sxliglocals.code.prdct_or_act_code + sxliglocals.code.prdct_or_act_disp [act_no + 1] - 2;
	    act_no = pclim [1];	/* Predicat utilisateur associe' (ou -1) */

	    if ((code = *pc) == PRIMARY)
	      pc += 3;

	    while (pclim > pc)
	      {
		if (SS_bot (cur_stack) == top)
		  /* La pile est vide */
		  return false;

		if (SS_get (cur_stack, --top) != *pclim--)
		  return false;
	      }

	    if (code == SECONDARY && SS_bot (cur_stack) != top)
	      /* La pile doit etre vide */
		return false;
	  }

	return (act_no >= 0) ? (*sxliglocals.code.parsact) (which, act_no) : true;

    default:
	fputs ("The function \"sxdligparsact\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }

    /* NOTREACHED return SXANY_BOOL; */
}

