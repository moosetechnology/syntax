/* ********************************************************************
   *  This program has been generated from lig.at                     *
   *  on Tue Aug  9 11:16:58 1994                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */


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

/* Genere a partir d'une [x]LIG
   - la [x]BNF correspondante
   - les tables des lig sont produites en  C ds L_ligt.c */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Ven 15 Nov 1996 10:14 (pb):	Nouvelle generation pour SXLIGparsact.h	*/
/************************************************************************/
/* 19-01-95 15:31 (pb):		Les actions et predicats primaires sont */
/*                              reversibles.                            */
/************************************************************************/
/* 06-01-95 16:02 (pb):		La specif de pile devient optionnelle.  */
/************************************************************************/
/* 21-09-94 11:32 (pb):		On accepte des post_action utilisateur	*/
/*				Verification que la LIG est un ensemble	*/
/*				et on force, par des actions en fin de	*/
/*				regle, la bnf a etre un ensemble.	*/
/************************************************************************/
/* 09-09-94 16:45 (pb):		Generation de "post-actions" ds la bnf	*/
/************************************************************************/
/* 23-08-94 13:05 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/
#define WHAT	"@(#)lig_smp.c	- SYNTAX [unix] - Ven 15 Nov 1996 10:14:21"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "lig_smp";


/*   I N C L U D E S   */
#define SXNODE struct lig_node
#include "sxunix.h";

/* Pour "SXLIGparsact.h" */
#define SXLIGis_normal_form     0
#define SXLIGis_post_act        0
#define SXLIGis_post_prdct      0
#define SXLIGis_leveln_complete 	1
#define SXLIGis_reduced	 	1
#define is_initial_LIG		1
/* Y-a-t'il des nt secondaires a gauche de primaires, a priori oui en "statique" */
#define RDGis_left_part_empty	0

#include "sxba.h"

static int		*SXLIGaction, *SXLIGpost_action, *SXLIGprdct, *SXLIGpost_prdct, *SXLIGprod2dum_nb;
static int		*SXLIGap_disp, *SXLIGap_list;
static char		**SXLIGssymbstring;
static SXBA		*SXLIGEQUALn_set, *SXLIGPUSHPOPn_set, *SXLIGPOPn_set;
static int		**SXLIGAxs, *SXLIGAxs_area;
static SXBA		RDGt_set;
static int		*SXLIGprod2left_secnb;

#include "SXLIGparsact.h"

#include "lig.h"
#include "varstr.h"
#include "XH.h"

struct lig_node {
    SXNODE_HEADER_S SXVOID_NAME;

/*
your attribute declarations...
*/
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ACTION_n 2
#define GENERIC_TERMINAL_n 3
#define LHS_NON_TERMINAL_n 4
#define LHS_OBJECT_n 5
#define LIG_n 6
#define NON_TERMINAL_n 7
#define PREDICATE_n 8
#define P_OBJECT_n 9
#define RHS_OBJECT_n 10
#define RULE_n 11
#define RULE_S_n 12
#define S_OBJECT_n 13
#define TERMINAL_n 14
#define TERMINAL_S_n 15
#define VOCABULARY_S_n 16
#define VOID_n 17
#define X_TERMINAL_n 18

#define SECONDARY	0
#define PRIMARY		1

extern char             *lig_comment_get ();

static VARSTR		vstr;
static int		xprod, action_nb, prdct_nb, prdct_val, action_val,
                        action_lgth, rhs_symb_nb, parsact_nb;
static XH_header	predicates_or_actions, bnf_rules, lig_rules;
static int		*lig_rule_to_line_no;
static SXBA             non_void_set;
static SXBA             void_set;
static int		*ste2ssymb;
static int		*ste2nt;

static int		BNFmaxnt, BNFmaxrhsnt, BNFprod, BNFmaxitem;
static int		*BNFlhs, *BNFprolon, *BNFlispro, *BNFprod2nbnt;
static char		**BNFntstring;

static SXBOOLEAN		is_LIG_in_normal_form, is_post_act, is_post_prdct, is_useless, is_reduced, sxttycol1p;
static int		maxssymb, max_stack_lgth, bnf_dum_post_act;
static int		*temp_stack;
static SXBA		dum_bnf_rule;
static int		SXLIGitem, SXLIGprod;

static int		is_left_part_empty = SXTRUE;
static SXBOOLEAN		is_left_part;


static void print_comment (comment)
    char	*comment;
{
  if (comment != NULL)
    {
      printf ("%s", comment);
      sxfree (comment);
    }
}


/* On verifie sur chaque regle que les specifs de pile sont correctes :
   - Si LHS est un S_OBJECT  <==> pas de P_OBJECT ds RHS
   - Si LHS est un P_OBJECT  <==> Existe 1 et 1 seul P_OBJECT ds RHS
   - Si is_normal_form la somme des longueurs des suffixes des primaires en LHS et RHS
     est au plus 1 et les secondaires sont de longueur nulle.
   - Si aucune pile est associee avec un NT, aucune occurrence de ce NT n'en possede
   - Si une pile est associee avec un NT, toutes les occurrences de ce NT en possedent
*/

static void
lig_check_nt (visited)
    struct lig_node	*visited;
{
    /* visited->name == LHS_OBJECT_n || visited->name == RHS_OBJECT_n */
    /* On verifie la coherence de la definition des piles sur toute la grammaire. */
    struct lig_node	*son = visited->son, *brother = son->brother;
    unsigned int        ste = son->token.string_table_entry;
    
    if (brother->name == VOID_n) {
	if (SXBA_bit_is_set (non_void_set, ste))
	    sxput_error (son->token.source_index,
			 "%sA stack specification is mandatory.",
			 sxtab_ptr->err_titles [2]);
	else
	    SXBA_1_bit (void_set, ste);
    }
    else {
	if (SXBA_bit_is_set (void_set, ste))
	    sxput_error (son->token.source_index,
			 "%sIllegal stack specification.",
			 sxtab_ptr->err_titles [2]);
	else
	    SXBA_1_bit (non_void_set, ste);
    }
}

static void
lig_check_productions (visited)
    struct lig_node	*visited;
{
    struct lig_node	*rule, *lhs, *rhs, *lhs_son, *rhs_son;
    int			P_object_nb, degree, prod, rhs_nt_nb, pop_nb, push_nb;

    /* visited->name == RULE_S_n */

    action_nb = visited->degree;

    SXLIGprod2dum_nb = (int*) sxcalloc (visited->degree+1, sizeof (int));

    prdct_nb = 0;
    prod = 0;

    for (rule = visited->son; rule != NULL; rule = rule->brother) {
	/* rule->name == RULE_n */
	prod++;
	lhs = rule->son;

	lig_check_nt (lhs);

	lhs_son = lhs->son->brother;
	
	if (lhs_son->name != VOID_n) {
	    /* lhs_son->name == P_OJECT_n || lhs_son->name == S_OJECT_n */
	    degree = pop_nb = lhs_son->son->degree;

	    P_object_nb = lhs_son->name == P_OBJECT_n ? 1 : 2 /* Pas de primary en RHS */;

	    if (lhs_son->name == P_OBJECT_n && pop_nb > 1 ||
		lhs_son->name == S_OBJECT_n && pop_nb > 0) {
		if (is_normal_form)
		    sxput_error (lhs_son->son->token.source_index,
				 "%sNot in normal form.",
				 sxtab_ptr->err_titles [1]);

		is_LIG_in_normal_form = SXFALSE;
	    }

	    if (pop_nb > max_stack_lgth)
		max_stack_lgth = pop_nb;
	}
	else {
	    degree = pop_nb = 0;
	    P_object_nb = 2; /* Pas de primary en RHS */
	}

	rhs_symb_nb += lhs->brother->brother->degree;
	rhs_nt_nb = 0;

	for (rhs = lhs->brother->brother->son /* On saute = et on prend le fils de VOCABULARY_S */;
	     rhs != NULL;
	     rhs = rhs->brother) {
	    switch (rhs->name) {
	    case RHS_OBJECT_n:
		rhs_nt_nb++;
		prdct_nb++;

		rhs_son = rhs->son->brother;
	
		if (rhs_son->name != VOID_n) {
		    /* rhs_son->name == P_OJECT_n || rhs_son->name == S_OJECT_n */
		    degree += push_nb = rhs_son->son->degree;

		    if (rhs_son->name == P_OBJECT_n) {
			if (++P_object_nb > 2)
			    sxput_error (rhs->son->brother->token.source_index,
				     "%sWrong number of primary constituents.",
				     sxtab_ptr->err_titles [2]);
		    }

		    if ((rhs_son->name == P_OBJECT_n) && (P_object_nb == 2) && (pop_nb + push_nb > 1) ||
			(rhs_son->name == S_OBJECT_n) && push_nb > 0) {
			if (is_normal_form)
			    sxput_error (rhs_son->son->token.source_index,
					 "%sNot in normal form.",
					 sxtab_ptr->err_titles [1]);

			is_LIG_in_normal_form = SXFALSE;
		    }

		    if (push_nb > max_stack_lgth)
			max_stack_lgth = push_nb;
		}

		lig_check_nt (rhs);

	    case ACTION_n:
		parsact_nb++;
		break;

	    case X_TERMINAL_n:
		break;
	    }
	}

	if (rhs_nt_nb > BNFmaxrhsnt)
	    BNFmaxrhsnt = rhs_nt_nb;

	if (degree > 1) {
	    SXLIGprod2dum_nb [prod] = degree-1;
	}
    }
}




static void
action_or_prdct (visited)
    SXNODE	*visited;
{
    /* visited->name == LHS_OBJECT_n ou RHS_OBJECT_n */
    /* On fabrique "action_val ou "prdct_val"" */
    SXNODE	*nt_ptr, *obj_ptr, *ssymb_ptr;
    int         obj_name, i, val, post, ste, ssymb, nt;
    SXBOOLEAN	is_lhs;

    is_lhs = visited->name == LHS_OBJECT_n;

    if (is_lhs)
	is_left_part = SXTRUE;

    nt_ptr = visited->son;

    ste = nt_ptr->token.string_table_entry;

    if ((nt = ste2nt [ste]) == 0) {
	nt = ste2nt [ste] = ++BNFmaxnt;
	BNFntstring [nt] = sxstrget (ste);
    }

    obj_ptr = nt_ptr->brother;
    obj_name = obj_ptr->name;
  
    if (obj_name != VOID_n) {
	if (!is_lhs && obj_name == P_OBJECT_n)
	    /* Primary en rhs */
	    is_left_part = SXFALSE;

	action_lgth = obj_ptr->son->degree;

	XH_push (predicates_or_actions, obj_name == P_OBJECT_n ? PRIMARY : SECONDARY);
	XH_push (predicates_or_actions, action_lgth);

	if (action_lgth > 0) {
	    i = 0;

	    for (ssymb_ptr = obj_ptr->son->son; ssymb_ptr != NULL; ssymb_ptr = ssymb_ptr->brother) {
		ste = ssymb_ptr->token.string_table_entry;

		if ((ssymb = ste2ssymb [ste]) == 0) {
		    ssymb = ste2ssymb [ste] = ++maxssymb;
		    SXLIGssymbstring [maxssymb] = sxstrget (ste);
		}

		if (is_lhs)
		    temp_stack [++i] = ssymb;
		else
		    XH_push (predicates_or_actions, ssymb);
	    }

	    if (is_lhs) {
		/* On les empile en ordre inverse */
		for (i = action_lgth; i > 0; i--)
		    XH_push (predicates_or_actions, temp_stack [i]);
	    }
	}

	XH_set (&predicates_or_actions, &val);
    }
    else
	val = -1;

    post = ((ssymb_ptr = obj_ptr->brother)->name == VOID_n)
	? -1 : atoi (sxstrget (ssymb_ptr->token.string_table_entry) + 1);

    if (is_lhs) {
	SXLIGaction [SXLIGprod] = action_val = val;
	SXLIGpost_action [SXLIGprod] = post;

	if (post != -1)
	    is_post_act = SXTRUE;
    }
    else {
	if (is_left_part)
	    SXLIGprod2left_secnb [SXLIGprod]++;

	SXLIGitem++;
	SXLIGprdct [SXLIGitem] = prdct_val = val;
	SXLIGpost_prdct [SXLIGitem] = post;

	if (post != -1)
	    is_post_prdct = SXTRUE;
    }
}
    

static void
lig_pi ()
{

    /*
       I N H E R I T E D
       */
    SXNODE	*son;

    switch (SXVISITED->father->name) {

    case ERROR_n :
	break;

    case LHS_OBJECT_n :
	break;

    case LIG_n :		/* SXVISITED->name = RULE_S_n */
	break;

    case P_OBJECT_n :		/* SXVISITED->name = TERMINAL_S_n */
	break;

    case RHS_OBJECT_n :
#if 0
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = NON_TERMINAL_n */
	    break;

	case 2 :		/* SXVISITED->name = {P_OBJECT_n, S_OBJECT_n, VOID_n} */
	    break;

	case 3 :		/* SXVISITED->name = {PREDICATE_n, VOID_n} */
	    break;
	}
#endif
	break;

    case RULE_n :
	switch (SXVISITED->position) {
	    int	rule_no, dummy;

	case 1 :		/* SXVISITED->name = LHS_OBJECT_n */
	    action_or_prdct (SXVISITED);
	    break;

	case 2 :		/* SXVISITED->name = VOID_n */
	    break;

	case 3 :		/* SXVISITED->name = VOCABULARY_S_n */
	    break;

	case 4 :		/* SXVISITED->name = VOID_n */
	    if (!is_left_part && SXLIGprod2left_secnb [SXLIGprod] > 0) {
		/*PRIMARY en RHS et SECONDARY a gauche du PRIMARY */
		is_left_part_empty = SXFALSE;
	    }


	    SXLIGitem++; /* Le "0" en fin de lispro */
	    SXLIGprdct [SXLIGitem] = -1;
	    SXLIGpost_prdct [SXLIGitem] = -1;

	    if (XH_set (&lig_rules, &rule_no)) {
		/* La regle existe deja en lig */
		/* La specig LIG n'est pas un ensemble */
		sxput_error (SXVISITED->father->son->token.source_index,
			     "%sIdentical with rule at line #%i.",
			     sxtab_ptr->err_titles [2],
			     lig_rule_to_line_no [rule_no]);
	    }
	    else {
		int	bnf_rule_no;

		lig_rule_to_line_no [rule_no] = SXVISITED->father->son->token.source_index.line;

		if (XH_set (&bnf_rules, &bnf_rule_no)) {
		    /* La regle existe deja en bnf */
		    SXBA_1_bit (dum_bnf_rule, rule_no);
		}
	    }

	    break;

	}

	break;

    case RULE_S_n :		/* SXVISITED->name = RULE_n */
	SXLIGprod++;
	break;

    case S_OBJECT_n :		/* SXVISITED->name = TERMINAL_S_n */
	break;

    case TERMINAL_S_n :break;

    case VOCABULARY_S_n :	/* SXVISITED->name = {ACTION_n, RHS_OBJECT_n, X_TERMINAL_n} */
	if (SXVISITED->name == RHS_OBJECT_n)
	    action_or_prdct (SXVISITED);
	else
	    SXLIGitem++;

	break;

    case X_TERMINAL_n :
#if 0
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {GENERIC_TERMINAL_n, TERMINAL_n} */
	    break;

	case 2 :		/* SXVISITED->name = {PREDICATE_n, VOID_n} */
	    break;
	}
#endif
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end lig_pi */
}



static void
lig_pd ()
{

    /*
       D E R I V E D
       */
    switch (SXVISITED->name) {

    case ERROR_n :
	break;

    case ACTION_n :
	if (SXVISITED->father->name == VOCABULARY_S_n ||
	    SXVISITED->father->name == LHS_OBJECT_n && SXVISITED->father->son->brother->name == VOID_n)
	{
	    XH_push (bnf_rules, SXVISITED->token.string_table_entry);
	    XH_push (lig_rules, SXVISITED->token.string_table_entry);
	}

	break;

    case GENERIC_TERMINAL_n :
	XH_push (bnf_rules, SXVISITED->token.string_table_entry);
	XH_push (lig_rules, SXVISITED->token.string_table_entry);
	break;

    case LHS_NON_TERMINAL_n :
	XH_push (bnf_rules, SXVISITED->token.string_table_entry);
	XH_push (lig_rules, SXVISITED->token.string_table_entry);
	break;

    case LHS_OBJECT_n :
	XH_push (lig_rules, action_val);
	break;

    case LIG_n :
	break;

    case NON_TERMINAL_n :
	XH_push (bnf_rules, SXVISITED->token.string_table_entry);
	XH_push (lig_rules, SXVISITED->token.string_table_entry);
	break;

    case PREDICATE_n :
	if (SXVISITED->father->name == X_TERMINAL_n ||
	    SXVISITED->father->name == RHS_OBJECT_n && SXVISITED->father->son->brother->name == VOID_n)
	{
	    XH_push (bnf_rules, SXVISITED->token.string_table_entry);
	    XH_push (lig_rules, SXVISITED->token.string_table_entry);
	}
    
	break;

    case P_OBJECT_n :
	break;

    case RHS_OBJECT_n :
	XH_push (lig_rules, prdct_val);
	break;

    case RULE_n :
	break;

    case RULE_S_n :
	break;

    case S_OBJECT_n :
	break;

    case TERMINAL_n :
	XH_push (bnf_rules, SXVISITED->token.string_table_entry);
	XH_push (lig_rules, SXVISITED->token.string_table_entry);
	break;

    case TERMINAL_S_n :
	break;

    case VOCABULARY_S_n :
	break;

    case VOID_n :
	break;

    case X_TERMINAL_n :
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end lig_pd */
}



static void
lig_pi2 ()
{

    /*
       I N H E R I T E D
       */
    SXNODE	*son;

    switch (SXVISITED->father->name) {

    case ERROR_n :
	break;

    case LHS_OBJECT_n :
	break;

    case LIG_n :		/* SXVISITED->name = RULE_S_n */
	break;

    case P_OBJECT_n :		/* SXVISITED->name = TERMINAL_S_n */
	break;

    case RHS_OBJECT_n :
#if 0
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = NON_TERMINAL_n */
	    break;

	case 2 :		/* SXVISITED->name = {P_OBJECT_n, S_OBJECT_n, VOID_n} */
	    break;

	case 3 :		/* SXVISITED->name = {PREDICATE_n, VOID_n} */
	    break;
	}
#endif
	break;

    case RULE_n :
	switch (SXVISITED->position) {
	    int	rule_no, dummy;

	case 1 :		/* SXVISITED->name = LHS_OBJECT_n */
	    break;

	case 2 :		/* SXVISITED->name = VOID_n */
	    print_comment (SXVISITED->token.comment);
	    putchar ('=');
	    break;

	case 3 :		/* SXVISITED->name = VOCABULARY_S_n */
	    break;

	case 4 :		/* SXVISITED->name = VOID_n */
	    print_comment (SXVISITED->token.comment);
	    putchar (';');
	    BNFlispro [++BNFmaxitem] = 0;
	    BNFprolon [++BNFprod] = BNFmaxitem+1;
	    break;

	}

	break;

    case RULE_S_n :		/* SXVISITED->name = RULE_n */
	break;

    case S_OBJECT_n :		/* SXVISITED->name = TERMINAL_S_n */
	break;

    case TERMINAL_S_n :break;

    case VOCABULARY_S_n :	/* SXVISITED->name = {ACTION_n, RHS_OBJECT_n, X_TERMINAL_n} */
	break;

    case X_TERMINAL_n :
#if 0
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {GENERIC_TERMINAL_n, TERMINAL_n} */
	    break;

	case 2 :		/* SXVISITED->name = {PREDICATE_n, VOID_n} */
	    break;
	}
#endif
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end lig_pi */
}


static void
lig_pd2 ()
{

    /*
       D E R I V E D
       */

    char	s [12];
    int		ate;

    switch (SXVISITED->name) {

    case ERROR_n :
	break;

    case ACTION_n :
	print_comment (SXVISITED->token.comment);

	if (SXVISITED->father->name == VOCABULARY_S_n ||
	    SXVISITED->father->name == LHS_OBJECT_n && SXVISITED->father->son->brother->name == VOID_n)
	{
	    printf ("%s", sxstrget (SXVISITED->token.string_table_entry));
	}

	if (SXVISITED->father->name == VOCABULARY_S_n) {
	    /* Que faire avec les actions qui seront transformees en NT sans pile! */
	    /* Regarder ds BNF la facon dont on leur assigne un code de nt et faire pareil ici! */
	    BNFlispro [++BNFmaxitem] = 0; /* pour l'instant! */
	    BNFprod2nbnt [BNFprod]++;
	}

	break;

    case GENERIC_TERMINAL_n :
	print_comment (SXVISITED->token.comment);
	printf ("%s", sxstrget (SXVISITED->token.string_table_entry));
	/* On ne met pas les terminaux ds BNF... */
	BNFlispro [++BNFmaxitem] = -1; /* On se moque du code reel */
	break;

    case LHS_NON_TERMINAL_n :
	print_comment (lig_comment_get (xprod++));
	printf ("<%s>", sxstrget (SXVISITED->token.string_table_entry));
	BNFlhs [BNFprod] = ste2nt [SXVISITED->token.string_table_entry];
	BNFprod2nbnt [BNFprod] = 0;

	if (SXBA_bit_is_set (dum_bnf_rule, xprod))
	    printf (" @%i", ++bnf_dum_post_act);

	break;

    case LHS_OBJECT_n :
	break;

    case LIG_n :
	break;

    case NON_TERMINAL_n :
	print_comment (SXVISITED->token.comment);
	printf ("<%s>", sxstrget (SXVISITED->token.string_table_entry));
	BNFlispro [++BNFmaxitem] = ste2nt [SXVISITED->token.string_table_entry];
	BNFprod2nbnt [BNFprod]++;
	break;

    case PREDICATE_n :
	print_comment (SXVISITED->token.comment);

	if (SXVISITED->father->name == X_TERMINAL_n ||
	    SXVISITED->father->name == RHS_OBJECT_n && SXVISITED->father->son->brother->name == VOID_n)
	{
	    printf ("%s", sxstrget (SXVISITED->token.string_table_entry));
	}
    
	break;

    case P_OBJECT_n :
	break;

    case RHS_OBJECT_n :
	break;

    case RULE_n :
	break;

    case RULE_S_n :
	break;

    case S_OBJECT_n :
	break;

    case TERMINAL_n :
	print_comment (SXVISITED->token.comment);

	if (SXVISITED->father->name == X_TERMINAL_n)
	{
	    varstr_quote (varstr_raz (vstr), sxstrget (SXVISITED->token.string_table_entry));
	    ate = sxstrsave (varstr_tostr (vstr));
	    printf ("%s", varstr_tostr (vstr));
	}
    
	BNFlispro [++BNFmaxitem] = -1; /* On se moque du code reel */
	break;

    case TERMINAL_S_n :
	break;

    case VOCABULARY_S_n :
	break;

    case VOID_n :
	break;

    case X_TERMINAL_n :
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end lig_pd */
}



#define out_define(str,val)	fprintf(F_lig, "#define %s\t%i\n",str,val)


static SXBOOLEAN
lig_write ()
{
    static char		ME [] = "lig_write";
    long		lt;
    char		*date;

    FILE		*F_lig;
    char		file_name [64];
    VARSTR		vstr;
    int			ap_no, *ap_ptr, prod, nb, i, X, item, ssymb, nt;

    if ((F_lig = fopen (strcat (strcpy (file_name, prgentname), "_ligt.c"), "w")) == NULL) {
	if (sxverbosep && !sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = SXTRUE;
	}

	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	sxperror (file_name);
	return SXFALSE;
    }

    vstr = varstr_alloc (64);

    lt = time (NULL);
    /* *ctime() = "Wed Aug 21 11:02:26 1996\n\0" */
    /* the 25th character is a "\n" */
    fprintf (F_lig, "static char	what [] = \"@(#)Linear Derivation Grammar Generator for the LIG \\\"%s\\\" - SYNTAX [unix] - %.24s\";\n",
	    prgentname, ctime (&lt));
    
    fprintf (F_lig, "static char	ME [] = \"%s_ligt.c\";\n", prgentname);

    fputs ("\n#include \"sxunix.h\"\n\n", F_lig);

    out_define ("SXLIGis_normal_form", is_LIG_in_normal_form ? 1 : 0);
    out_define ("SXLIGis_post_act", is_post_act);
    out_define ("SXLIGis_post_prdct", is_post_prdct);
    out_define ("SXLIGis_leveln_complete", is_useless ? 0 : 1);
    out_define ("SXLIGis_reduced", is_reduced);
    out_define ("is_initial_LIG", 0);
    out_define ("SXLIGmaxssymb", maxssymb);
    out_define ("SXLIGmaxrhsnt", inputG.maxrhsnt);
    out_define ("RDGis_left_part_empty", is_left_part_empty ? 1 : 0);

    fputs ("\n\nstatic int SXLIGap_disp [] = {\n/* 0 */\t1,\n", F_lig);

    for (ap_no = 1; ap_no <= XH_top (predicates_or_actions); ap_no++) {
	fprintf (F_lig, "/* %i */\t%i,\n", ap_no, SXLIGap_disp [ap_no]);
    }

    fputs ("};", F_lig);

    fputs ("\n\nstatic int SXLIGap_list [] = {\n/* 0 */\t0,\n", F_lig);

    for (ap_no = 1; ap_no < XH_top (predicates_or_actions); ap_no++) {
	ap_ptr = &(SXLIGap_list [SXLIGap_disp [ap_no]]);
	fprintf (F_lig, "/* %i */\t%i, %i, ", ap_no, *ap_ptr, nb = ap_ptr [1]);

	if (nb > 0) {
	    for (i = 1; i <= nb; i++)
		fprintf (F_lig, "%i, ", ap_ptr [i+1]);
	}

	fputs ("\n", F_lig);
    }

    fputs ("};", F_lig);

    fputs ("\n\nstatic int SXLIGaction [] = {\n/* 0 */\t-1,\n", F_lig);

    for (prod = 1; prod <= inputG.maxprod; prod++) {
	fprintf (F_lig, "/* %i */\t%i,\n", prod, SXLIGaction [prod]);
    }

    fputs ("};", F_lig);

    if (!is_left_part_empty) {
	fputs ("\n\nstatic int SXLIGprod2left_secnb [] = {\n/* 0 */\t0,\n", F_lig);

	for (prod = 1; prod <= inputG.maxprod; prod++) {
	    fprintf (F_lig, "/* %i */\t%i,\n", prod, SXLIGprod2left_secnb [prod]);
	}

	fputs ("};", F_lig);
    }

    if (is_post_act) {
	fputs ("\n\nstatic int SXLIGpost_action [] = {\n/* 0 */\t-1,\n", F_lig);

	for (prod = 1; prod <= inputG.maxprod; prod++) {
	    fprintf (F_lig, "/* %i */\t%i,\n", prod, SXLIGpost_action [prod]);
	}

	fputs ("};", F_lig);
    }

    if (!is_LIG_in_normal_form) {
	fputs ("\n\nstatic int SXLIGprod2dum_nb [] = {\n/* 0 */\t0,\n", F_lig);

	for (prod = 1; prod <= inputG.maxprod; prod++) {
	    fprintf (F_lig, "/* %i */\t%i,\n", prod, SXLIGprod2dum_nb [prod]);
	}

	fputs ("};", F_lig);
    }

    fputs ("\n\n#ifdef is_sxndparser\n\
/* Allows the access to SXLIGprdct when the parser is sxndparser */\n", F_lig);
    fputs ("\nstatic int SXLIGlhs [] = {\n/* 0 */\t0,\n", F_lig);

    for (prod = 1; prod <= inputG.maxprod; prod++) {
	fprintf (F_lig, "/* %i */\t%i,\n", prod, inputG.lhs [prod]);
    }

    fputs ("};", F_lig);
    fputs ("\nstatic int SXLIGprolon [] = {\n/* 0 */\t0,\n", F_lig);

    for (prod = 1; prod <= inputG.maxprod+1; prod++) {
	fprintf (F_lig, "/* %i */\t%i,\n", prod, inputG.prolon [prod]);
    }

    fputs ("};", F_lig);
    fputs ("\n\nstatic int SXLIGprod2nbnt [] = {\n/* 0 */\t1,\n", F_lig);

    for (prod = 1; prod <= inputG.maxprod; prod++) {
	fprintf (F_lig, "/* %i */\t%i,\n", prod, inputG.prod2nbnt [prod]);
    }

    fputs ("};", F_lig);
    fputs ("\n\nstatic char *SXLIGntstring [] = {\n/* 0 */\t\"\",\n", F_lig);

    for (nt = 1; nt <= inputG.maxnt; nt++) {
	fprintf (F_lig, "/* %i */\t\"%s\", \n", nt, inputG.ntstring [nt]);
    }

    fputs ("};", F_lig);
    
    fputs ("\n#endif\n", F_lig);

    fputs ("\n\nstatic int SXLIGprdct [] = {\n/* 0 S' -> $ S $ 0 */\t-1, -1, -1, -1,\n", F_lig);

    for (prod = 1; prod <= inputG.maxprod; prod++) {
	item = inputG.prolon [prod];

	fprintf (F_lig, "/* %i */\t", prod);

	do {
	    X = inputG.lispro [item];
	    fprintf (F_lig, "%i, ", X > 0 ? SXLIGprdct [item] : -1);
	    item++;
	} while (X != 0);

	fputs ("\n", F_lig);
    }

    fputs ("};", F_lig);

    if (is_post_prdct) {
	fputs ("\n\nstatic int SXLIGpost_prdct [] = {\n/* 0 S' -> $ S $ 0 */\t-1, -1, -1, -1,\n", F_lig);

	for (prod = 1; prod <= inputG.maxprod; prod++) {
	    item = inputG.prolon [prod];

	    fprintf (F_lig, "/* %i */\t", prod);

	    do {
		X = inputG.lispro [item];
		fprintf (F_lig, "%i, ", X > 0 ? SXLIGpost_prdct [item] : -1);
		item++;
	    } while (X != 0);

	    fputs ("\n", F_lig);
	}

	fputs ("};", F_lig);
    }

    fputs ("\n\nstatic char *SXLIGssymbstring [] = {\n/* 0 */\t\"\",\n", F_lig);

    for (ssymb = 1; ssymb <= maxssymb; ssymb++) {
	fprintf (F_lig, "/* %i */\t\"%s\", \n", ssymb, SXLIGssymbstring [ssymb]);
    }

    fputs ("};", F_lig);

    if (!is_reduced)
	sxba2c (RDGt_set, F_lig, "SXLIGt_set", "", SXTRUE /* is_static */, vstr);

    if (is_useless) {
	fputs ("\n\n#if SXLIGuse_reduced==1\n", F_lig);

	if (SXLIGEQUALn_set == NULL)
	    fputs ("\nstatic SXBA *SXLIGEQUALn_set;\n", F_lig);
	else
	    sxbm2c (F_lig, SXLIGEQUALn_set, inputG.maxnt+1, "SXLIGEQUALn_set", "", SXTRUE /* is_static */, vstr);

	if (SXLIGPUSHPOPn_set == NULL)
	    fputs ("\n\nstatic SXBA *SXLIGPUSHPOPn_set;\n", F_lig);
	else
	    sxbm2c (F_lig, SXLIGPUSHPOPn_set, inputG.maxnt+1, "SXLIGPUSHPOPn_set", "", SXTRUE /* is_static */, vstr);

	if (SXLIGPOPn_set == NULL) {
	    fputs ("\n\nstatic int **SXLIGAxs;\n", F_lig);
	    fputs ("\n\nstatic SXBA *SXLIGPOPn_set;\n", F_lig);
	}
	else {
	    fprintf (F_lig, "\n\nstatic int SXLIGAxs [%i] [%i] = {\n", inputG.maxnt+1, maxssymb+1);

	    for (nt = 0; nt <= inputG.maxnt; nt++) {
		fprintf (F_lig, "/* %i */\t{0, ", nt);

		for (ssymb = 1; ssymb <= maxssymb; ssymb++) {
		    fprintf (F_lig, "%i, ", SXLIGAxs [nt] [ssymb]);
		}

		fputs ("},\n", F_lig);
	    }

	    fputs ("};", F_lig);

	    sxbm2c (F_lig, SXLIGPOPn_set, inputG.maxnt+1, "SXLIGPOPn_set", "", SXTRUE /* is_static */, vstr);
	}

	fputs ("\n#endif\n", F_lig);
    }

    fputs ("\n\n#include \"SXLIGparsact.h\"\n", F_lig);

    varstr_free (vstr);

    fclose (F_lig);

    return SXTRUE;
}



static void
lig_reduction ()
{
    int ssymb, prod, act_no, prdct_no, lim, item, i, pop_nb, push_nb, X, nb;
    int type, post_action, post_prdct, dum_nb;
    int	*pca, *pcalim, *pcp, *pcplim;
    int ap_no, new_ap_no, old_xap_list;
    int	ap_list_top;
    unsigned short	prod_core;
    int 		*rhs_stack;
    SXBOOLEAN		is_local;
    static SXBOOLEAN	SXLIG_semact ();
    static SXBOOLEAN 	SXLIG_sem_pass ();

    int			top, bot, AsB, sB, AB, A, B, As, A00, B00;
    int			*p, *plim;
    SXBOOLEAN		EQUALnfull, PUSHPOPnfull, POPnfull;
    char		kind;

    inputG.maxnt = BNFmaxnt;
    inputG.maxrhsnt = BNFmaxrhsnt;
    inputG.maxprod = BNFprod-1;
    inputG.lhs = BNFlhs;
    inputG.prolon = BNFprolon;
    inputG.lispro = BNFlispro;
    inputG.prod2nbnt = BNFprod2nbnt;
    inputG.ntstring = BNFntstring;
    inputG.has_cycles = SXFALSE;

    SXLIGalloc (inputG.maxnt+1);

    dumAij = 0;
    pAij_top = inputG.maxnt;

    rhs_stack = (int*) sxalloc (inputG.maxrhsnt+1, sizeof (int));

    for (prod = 1; prod <= inputG.maxprod; prod++) {
	prod_core = (unsigned short) prod;
	rhs_stack [0] = inputG.lhs [prod];

	item = inputG.prolon [prod];
	i = 0;

	while ((X = inputG.lispro [item++]) != 0) {
	    if (X > 0)
		rhs_stack [++i] = X;
	}

	SXLIG_semact (0, 0, &prod_core, rhs_stack);
    }

    sxfree (rhs_stack);

    /* PROVISOIRE */
    is_print_prod = SXFALSE;

    RDGt_set = sxba_calloc (inputG.maxprod+1);

    SXLIG_sem_pass (1		/* axiome */);

    /* On regarde si la LIG initiale est reduite */
    /* RDGt_set doit etre "complet" */

    is_reduced = (nb = sxba_cardinal (RDGt_set) == inputG.maxprod);

    if (!is_reduced) {
	if (sxverbosep && !sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = SXTRUE;
	}
    
	if (nb == 0)
	    fprintf (sxstderr, "********** The LIG language is empty! **********\n");
	else {
	    prod = sxba_0_lrscan (RDGt_set, 0);
	    fprintf (sxstderr, "********** This LIG is not reduced: the productions at lines\n%i",
		     lig_rule_to_line_no [prod]);
	
	    while ((prod = sxba_0_lrscan (RDGt_set, prod)) > 0)
		fprintf (sxstderr, ", %i", lig_rule_to_line_no [prod]);

	    fprintf (sxstderr, "\nare useless\n");
	}
    }

    /* Si la grammaire est reduite, toutes les relations de niveau 1 sont completes. */
    /* EQUALn et PUSHPOPn */
    bot = EQUALn_ntbot + EQUAL1_top;
    EQUALnfull = PUSHPOPnfull = SXTRUE;

    for (AB = EQUALn_top; (EQUALnfull || PUSHPOPnfull) && (AB > 0); AB--) {
	kind = EQUALn2kind [AB];

	if (!is_LIG_in_normal_form) {
	    A = XxY_X (EQUALn_hd, AB);
	    B = XxY_Y (EQUALn_hd, AB);

	    A00 = (A < 0) ? dumAij2attr [-A].Aij : A;
	    B00 = (B < 0) ? dumAij2attr [-B].Aij : B;

	    /* Comme les nt correspondant a des relations entre les dummy d'un meme nt ne sont pas
	       stockes ds la RDG, ils ne sont pas comptes a priori comme des trous. */
	    is_local = (A00 == B00) && (A != B);
	}

	if (EQUALnfull && (kind & (IS_PUSHPOP_EQ1 | IS_PUSHPOP_EQn | IS_EQ1_EQ1 | IS_EQ1_EQn))) {
	    X = AB+bot;

	    if (!SXBA_bit_is_set (RDGnt_set, X)) {
		if (is_LIG_in_normal_form)
		    EQUALnfull = SXFALSE;
		else {
		    if (!is_local) {
			/* Non local, c'est un trou */
			EQUALnfull = SXFALSE;
		    }
		}
	    }
	}

	if (PUSHPOPnfull && (kind & IS_PUSHPOP)) {
	    X = AB+PUSHPOPn_ntbot;

	    if (!SXBA_bit_is_set (RDGnt_set, X)) {
		if (is_LIG_in_normal_form)
		    PUSHPOPnfull = SXFALSE;
		else {
		    if (!is_local) {
			/* Non local, c'est un trou */
			PUSHPOPnfull = SXFALSE;
		    }
		}
	    }
	}
    }

    if (!EQUALnfull) {
	/* EQUALn */
	is_useless = SXTRUE;
	SXLIGEQUALn_set = sxbm_calloc (inputG.maxnt+1, inputG.maxnt+1);
	X = bot;
	top = bot + EQUALn_top;

	while ((X = sxba_scan (RDGnt_set, X)) > 0 && X <= top) {
	    AB = X-bot;
	    A = XxY_X (EQUALn_hd, AB);
	    B = XxY_Y (EQUALn_hd, AB);

	    if (A < 0) A = dumAij2attr [-A].Aij;
	    if (B < 0) B = dumAij2attr [-B].Aij;

	    SXBA_1_bit (SXLIGEQUALn_set [A], B);
	}
    }

    if (!PUSHPOPnfull) {
	/* PUSHPOPn */
	/* Il y a des "trous" */
	is_useless = SXTRUE;
	SXLIGPUSHPOPn_set = sxbm_calloc (inputG.maxnt+1, inputG.maxnt+1);
	bot = PUSHPOPn_ntbot;
	top = bot + PUSHPOPn_top;

	X = bot;

	while ((X = sxba_scan (RDGnt_set, X)) > 0 && X <= top) {
	    AB = X-bot;
	    A = XxY_X (EQUALn_hd, AB);
	    B = XxY_Y (EQUALn_hd, AB);

	    if (A < 0) A = dumAij2attr [-A].Aij;
	    if (B < 0) B = dumAij2attr [-B].Aij;

	    SXBA_1_bit (SXLIGPUSHPOPn_set [A], B);
	}
    }

    /* POPn */
    POPnfull = SXTRUE;
    bot = POPn_ntbot + POP1_top;

    for (AsB = POPn_top; AsB > 0; AsB--) {
	X = AsB+bot;

	if (!SXBA_bit_is_set (RDGnt_set, X)) {
	    if (is_LIG_in_normal_form) {
		POPnfull = SXFALSE;
		break;
	    }

	    A = XxY_X (POPn_hd, AsB);
	    sB = XxY_Y (POPn_hd, AsB);
	    B = XxY_X (Axs_hd, sB);

	    A00 = (A < 0) ? dumAij2attr [-A].Aij : A;
	    B00 = (B < 0) ? dumAij2attr [-B].Aij : B;

	    /* Comme les nt correspondant a des relations entre les dummy d'un meme nt ne sont pas
	       stockes ds la RDG, ils ne sont pas comptes a priori comme des trous. */
	    if (A00 != B00 || A == B) {
		/* Non local, c'est un trou */
		POPnfull = SXFALSE;
		break;
	    }
	}
    }

    if (!POPnfull) {
	/* Il y a des "trous" */
	is_useless = SXTRUE;
	SXLIGPOPn_set = sxbm_calloc (inputG.maxnt+1, XxY_top (Axs_hd)+1);
	SXLIGAxs = (int**) sxalloc (inputG.maxnt+1, sizeof (int*));
	SXLIGAxs_area = (int*) sxcalloc ((inputG.maxnt+1)*(maxssymb+1), sizeof (int));
	top = bot + POPn_top;
	
	X = 0;

	for (p = SXLIGAxs_area, plim = SXLIGAxs_area + (inputG.maxnt+1)*(maxssymb+1);
	     p < plim; p += maxssymb+1) {
	    SXLIGAxs [X++] = p;
	}

	/* Axs */
	for (As = XxY_top (Axs_hd); As > 0; As--) {
	    A = XxY_X (Axs_hd, As);

	    if (A < 0) A = dumAij2attr [-A].Aij;

	    ssymb = XxY_Y (Axs_hd, As);
	    SXLIGAxs [A] [ssymb] = As;
	}

	X = bot;

	while ((X = sxba_scan (RDGnt_set, X)) > 0 && X <= top) {
	    AB = X-bot;
	    A = XxY_X (POPn_hd, AB);

	    if (A < 0) A = dumAij2attr [-A].Aij;

	    sB = XxY_Y (POPn_hd, AB);

	    SXBA_1_bit (SXLIGPOPn_set [A], sB);
	}
    }

    SXLIG_free2 ();
    SXLIG_free3 ();
}


static void
smpopen (sxtables_ptr)
    struct sxtables *sxtables_ptr;
{
    sxatcvar.atc_lv.node_size = sizeof (struct lig_node);
}

static void
smppass ()
{
    int	rule_nb, old_start_symbol_ste, ml, val, ap_no;

    if (sxverbosep) {
	fputs ("\tSemantic Pass 1, ", sxtty);
	sxttycol1p = SXFALSE;
    }

    non_void_set = sxba_calloc (SXSTRtop () + 1);
    void_set = sxba_calloc (SXSTRtop () + 1);
    is_LIG_in_normal_form = SXTRUE;
    rhs_symb_nb = 0;

    lig_check_productions (sxatcvar.atc_lv.abstract_tree_root->son);
    
    if (!IS_ERROR) {
	if (sxverbosep) {
	    fputs ("2 ... ", sxtty);
	}

	ste2ssymb = (int*) sxcalloc (SXSTRtop () + 1, sizeof (int));
	SXLIGssymbstring = (char**) sxcalloc (SXSTRtop () + 1, sizeof (char*));
	ste2nt = (int*) sxcalloc (SXSTRtop () + 1, sizeof (int));
	BNFntstring = (char**) sxcalloc (SXSTRtop () + 1, sizeof (char*)); 

	SXLIGprod = 0;
	SXLIGitem = 3; /* Repere le "0" de la fin de S' -> $ S $ */
	rule_nb = sxatcvar.atc_lv.abstract_tree_root->son->degree;
	vstr = varstr_alloc (64);
	XH_alloc (&predicates_or_actions, "predicates_or_actions", prdct_nb + action_nb, 1, 4, NULL, NULL);
	XH_push (predicates_or_actions, SECONDARY);
	XH_push (predicates_or_actions, 0);
	XH_set (&predicates_or_actions, &val); /* () */
	XH_push (predicates_or_actions, PRIMARY);
	XH_push (predicates_or_actions, 0);
	XH_set (&predicates_or_actions, &val); /* (..) */

	ml = 1 + (rule_nb + action_nb + rhs_symb_nb + prdct_nb)/rule_nb;

	XH_alloc (&lig_rules, "lig_rules", rule_nb + 1, 1, ml, NULL, NULL);
	/* Ne doit pas deborder! */
	lig_rule_to_line_no = (int*) sxalloc (XH_size (lig_rules) + 1, sizeof (int));
	dum_bnf_rule = sxba_calloc (XH_size (lig_rules) + 1);

	XH_alloc (&bnf_rules, "bnf_rules", rule_nb + 1, 1, ml, NULL, NULL);

	temp_stack = (int*) sxalloc (max_stack_lgth+1, sizeof (int));

	SXLIGaction = (int*) sxalloc (rule_nb+1, sizeof (int));
	SXLIGpost_action = (int*) sxalloc (rule_nb+1, sizeof (int));
	SXLIGprdct = (int*) sxalloc (rule_nb+rhs_symb_nb+1+4, sizeof (int));
	SXLIGpost_prdct = (int*) sxalloc (rule_nb+rhs_symb_nb+1+4, sizeof (int));
	/* Nb de nt secondaires a gauche du nt primaire */
	SXLIGprod2left_secnb = (int*) sxalloc (rule_nb+1, sizeof (int));

	sxsmp (sxatcvar.atc_lv.abstract_tree_root, lig_pi, lig_pd);

	if (sxverbosep) {
	    fputs ("done\n", sxtty);
	    sxttycol1p = SXTRUE;
	}

	if (!IS_ERROR) {
	    /* La LIG est un ensemble */
	    /* On recopie le display des actions et predicats de la LIG. */
	    if (sxverbosep) {
		fprintf (sxtty, "\t%s.bnf on stdout\n", prgentname);
	    }

	    SXLIGap_disp = (int*) sxalloc (XH_top (predicates_or_actions)+2, sizeof (int));
	    SXLIGap_list = &(XH_list_elem (predicates_or_actions, 0));

	    SXLIGap_disp [0] = 0;

	    for (ap_no = 1; ap_no <= XH_top (predicates_or_actions); ap_no++) {
		SXLIGap_disp [ap_no] = XH_X (predicates_or_actions, ap_no);
	    }

	    SXLIGap_disp [ap_no] = XH_X (predicates_or_actions, ap_no);

	    /* on genere la bnf */
	    BNFlhs = (int*) sxalloc (rule_nb+1, sizeof (int));
	    BNFprolon  = (int*) sxalloc (rule_nb+2, sizeof (int));
	    BNFprod2nbnt = (int*) sxalloc (rule_nb+1, sizeof (int));
	    BNFlispro = (int*) sxalloc (rule_nb+rhs_symb_nb+1+4, sizeof (int));
	    BNFprolon [BNFprod = 1] = (BNFmaxitem = 3) + 1;

	    sxsmp (sxatcvar.atc_lv.abstract_tree_root, lig_pi2, lig_pd2);

	    print_comment (lig_comment_get (xprod));

	    if (sxverbosep) {
		fputs ("\tIs this LIG reduced ? ... ", sxtty);
		sxttycol1p = SXFALSE;
	    }

	    lig_reduction ();

	    if (sxverbosep) {
		fputs ("done\n", sxtty);
		sxttycol1p = SXTRUE;
	    }

	    if (sxverbosep) {
		fprintf (sxtty, "\t%s_ligt.c file output ... ", prgentname);
		sxttycol1p = SXFALSE;
	    }

	    lig_write ();

	    if (sxverbosep) {
		fputs ("done\n", sxtty);
		sxttycol1p = SXTRUE;
	    }

	    
	    if (SXLIGEQUALn_set != NULL) sxbm_free (SXLIGEQUALn_set);
	    if (SXLIGPUSHPOPn_set != NULL) sxbm_free (SXLIGPUSHPOPn_set);

	    if (SXLIGPOPn_set != NULL) {
		sxbm_free (SXLIGPOPn_set);
		sxfree (SXLIGAxs_area);
		sxfree (SXLIGAxs);
	    }

	    sxfree (RDGt_set);

	    sxfree (BNFlhs);
	    sxfree (BNFprod2nbnt);
	    sxfree (BNFprolon);
	    sxfree (BNFlispro);

	    sxfree (SXLIGap_disp);
	}


	/* Les structures des instructions primaires action ou predicat sont identiques
	   ca permet de les interpreter de facon opposee (cad les actions deviennent des
	   predicats et les predicats des actions). Ca permet de faire de l'evaluation top-down
	   ou bottom-up sur la foret partagee d'analyse aussi bien que de l'evaluation
	   en // avec l'analyse ascendante. */

	/* Structure :
	   @: PRIMARY prdct_index prdct_lgth common_prefix_lgth elem_list post_action
	   &: PRIMARY action_index action_lgth common_prefix_lgth elem_list post_prdct
	   
	   prdct_index: index ds la rhs du symbole auquel est associe le predicat (primaire)
	   correspondant a l'action. Commence a 0, 1, 2, ...
	   prdct_lgth:  nombre d'element teste par le &
	   common_prefix_lgth: nombre d'element du prefixe commun entre @ et &
	   elem_list:   liste des elements a pusher (de gauche a droite)
	   post_action: post action ou -1
	   
	   action_index: toujours 0, designe toujours l'action de la LHS
	   action_lgth:  nombre d'element empile par l'@
	   post_prdct:  post predicat ou -1
	   */

	sxfree (temp_stack);
	sxfree (SXLIGaction);
	sxfree (SXLIGpost_action);
	sxfree (SXLIGprdct);
	sxfree (SXLIGpost_prdct);
	sxfree (SXLIGprod2left_secnb);
	XH_free (&predicates_or_actions);
	XH_free (&bnf_rules);
	sxfree (lig_rule_to_line_no);
	XH_free (&lig_rules);
	varstr_free (vstr);
	sxfree (ste2ssymb);
	sxfree (SXLIGssymbstring);
	sxfree (ste2nt);
	sxfree (BNFntstring);
    }
    else {
	if (sxverbosep) {
	    fputs ("done\n", sxtty);
	    sxttycol1p = SXTRUE;
	}
    }

    sxfree (SXLIGprod2dum_nb);
    sxfree (non_void_set);
    sxfree (void_set);
}



lig_smp (what, sxtables_ptr)
    int what;
    struct sxtables *sxtables_ptr;
{
    switch (what) {
    case SXOPEN:
	sxtab_ptr = sxtables_ptr;
	smpopen (sxtables_ptr);
	break;
    case SXSEMPASS:		/* For [sub-]tree evaluation during parsing */
	break;
    case SXACTION:
	smppass ();
	break;
    case SXCLOSE:
	break;
    default:
	break;
    }
}
