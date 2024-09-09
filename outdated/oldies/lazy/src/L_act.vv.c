/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1993 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     	Produit de l'equipe ChLoe		  *
   *                	     (PB)			  *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 16-09-93 15:35 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)L_act.c\t- SYNTAX [unix] -  Ven 21 Jan 1994 16:12:55";

static char	ME [] = "L_act";

/*
  Il serait sans doute possible de se passer de table des symboles et de (presque) tout
  traiter par grammaire dynamique.

  1: int i, f [5], f (int);
  2: {
  3:	bool i;
  4:	int f [4], f (int, bool);
  5:	...
  6: }
  7: ...

  1: On genere :
     r1:	<INT_LHS> = "i" ;
     v1':	<DECLARATOR> = "i" @n ;
     a1':	<DECLARATOR> = "i" [ <INT_EXPR+> ] @n ;
     f1':	<DECLARATOR> = "i" ( <TYPE_NAME*> ) @n ;

     r2:	<INT_LHS> = "f" [ <INT_EXPR> ] ;
     v2':	<DECLARATOR> = "f" @n ;
     a2':	<DECLARATOR> = "f"&n [ <INT_EXPR> ] @n ;

     r3:	<INT_FUN> = "f" ( int ) ;
     v3':	<DECLARATOR> = "f" @n ;     
     f3':	<DECLARATOR> = "f"&n ( int ) @n ;     

     Les regles primes "interdisent" l'utilisation du meme nom au meme niveau de
     bloc, conformement a la specif du langage : une variable simple ne peut pas etre
     redeclaree (en tant que variable, tableau ou fonction) et un tableau ou fonction
     ne peut pas etre redeclaree en tant que variable ou tableau et fonction identique.
     Pb, si les profiles sont identiques, mais le type du resultat est different, c'est
     correct. Ce probleme est regle' avec les predicats "&n" qui re'pondent true ssi
     le type de base est "int".

     @n produit le message d'erreur :
         "Already declared at the same block level"

     
  2: On supprime les regles primees (on peut redeclarer des variables de meme nom et
     meme profile ds des blocs interieurs). Elles sont supprimees (et pas desactivees)
     car on entre ds un bloc, on est donc ds la partie "stmt" de l'englobant, a la sortie
     de ce bloc (ligne 6), on ne risque pas de retrouver des declarations.

     Il faut pouvoir "desactiver" les regles non primees, lorsque de nouvelles variables
     vont cacher les precedentes. On genere donc :
     r1'':	<DECLARATOR> = "i" @m1 ;
     r2'':	<DECLARATOR> = "f"&n [ <INT_EXPR> ] @m2 ;
     r3'':	<DECLARATOR> = "f"&n ( int ) @m3 ;     

     Ou @mi = {desactiver ri, detruire ri'', generation normale}

     On ne peut detruire ri'' depuis @mi puisque ri'' est en cours de reconnaissance,
     il faut trouver une astuce (par ex differer cette suppression et l'effectuer
     apres la reconnaissance de <DECLARATOR>).


  Difficultes :
  - Provoque des problemes avec SXDO/SXUNDO/SXACTION, puisque maintenant il peut y avoir
    du look-ahead non borne' dans les declarations.

  - Traiter des predicats "multiples" comme "f"&n.
*/


#include "sxdynam_parser.h"
#include "sxsymbol_table.h"
#include "L_tdef.h"


static int	INT_LHS, BOOL_LHS, INT_EXPR, BOOL_EXPR, BASIC_TYPE_NAME, STMT, INT_CONST,
                BOOL_CONST, INT_FUN, BOOL_FUN, INT_VAR, BOOL_VAR, INT_PRIMARY, BOOL_PRIMARY,
                LABEL;

#define SCALAR			1
#define ARRAY_or_FUNCTION	2

struct st_attr {
    int lhs_case, prev_xname, xt, listof_level;
};

#define SUBTYPE		1
#define RECORD		2
struct tt_attr {
    int kind, seed_type_name, father_type_name, xt, lhs, fun, expr, primary,
        var, listof_level, prev_recordtype_lhs;
};

struct listof_attr {
    int prev, lhs, fun, expr, var;
    bool	is_activated;
};

struct sem_stack {
    int rule;
};

struct rule_to_attr {
    int action, priority, associativity;
};

struct type_steXblock_nb_to_attr {
    int var, lhs, fun, expr, primary;
};

static struct sxsymbol_table_hd		symbol_table,
                                        type_table;
static struct st_attr			*st_to_attr;
static struct tt_attr			*tt_to_attr;
static struct sxtoken			*ptok;
static int	*rule_stack, *rule_attr_stack, *block_stack, *reactivate_stack, *deactivate_stack,
    *xtnt_stack;
static int	*symbols, symbols_size, symbols_top;
static int	symbs [7];
static int	block_level, block_nb, current_block_nb, basic_type_name, current_listof_level,
                listof_level, prev_listof, prev_recordtype_lhs, sem_stack_size, record_name;
static int	action_20, action_21, cur_nt;
static bool	is_record_body;

static XxY_header		listof_hd, labels_hd, recordtype_lhsXrule, var_nameXrule,
                                type_nameXrule, type_steXblock_nb;
static struct listof_attr	*listof_to_attr;
static struct sem_stack		*sem_stack;
static struct rule_to_attr	*rule_to_attr;
static struct type_steXblock_nb_to_attr
                                *type_steXblock_nb_to_attr;

static int	_xnt, _xt;
#define new_nt()	(sxdp_new_xnt (++cur_nt, -1, &_xnt), _xnt)
#define new_xt(t,p)	(sxdp_new_xt (t, p, &_xt, 0, 0), -_xt)

static void
listof_oflw (old_size, new_size)
    int old_size, new_size;
{
    listof_to_attr = (struct listof_attr*) sxrealloc (listof_to_attr,
						      new_size + 1,
						      sizeof (struct listof_attr));
}

static void
symbol_table_oflw (old_size, new_size)
    int old_size, new_size;
{
    st_to_attr = (struct st_attr*) sxrealloc (st_to_attr, new_size + 1, sizeof (struct st_attr));
}

static void
type_table_oflw (old_size, new_size)
    int old_size, new_size;
{
    tt_to_attr = (struct tt_attr*) sxrealloc (tt_to_attr, new_size + 1, sizeof (struct tt_attr));
}

static void
type_steXblock_nb_oflw (old_size, new_size)
    int old_size, new_size;
{
    type_steXblock_nb_to_attr = (struct type_steXblock_nb_to_attr*)
	sxrealloc (type_steXblock_nb_to_attr,
		   new_size + 1,
		   sizeof (struct type_steXblock_nb_to_attr));
}

static void
symbols_oflw (size)
    int size;
{
    do {
	symbols_size *= 2;
    } while (size > symbols_size);

    symbols = (int*) sxrealloc (symbols, symbols_size + 1, sizeof (int));
}

static int
desambig ()
{
    /* Appele' depuis sxdynam_parser lorsqu'une [pseudo-]ambiguite' a ete detectee.
       Les conflits se trouvent ds G.ambig_stack [1:G.ambig_stack_top].
       si G.ambig_stack [x].rule est positif, c'est un numero de regle (reduce)
       sinon c'est un shift sur sxplocals.ptok_no.
       La [pseudo-]ambiguite a ete detectee sur le token G.Mtok_no.
       Si une action est ecartee, mettre 0 ds G.ambig_stack [x].rule, sinon
       ne pas y toucher. La resolution est complete si un et seul element n'est
       pas mis a zero. Sinon, la resolution systeme sera employee. */

    /* Sur L, on suppose que les ambiguites sont dues au typage des (construction)
       de liste vide, on choisit une action qqc. */

    int i;

    sxput_error (SXGET_TOKEN (sxplocals.ptok_no).source_index,
		 "%sNot uniquely identified.",
		 sxplocals.sxtables->err_titles [2]);

    for (i = SXDG.ambig_stack_top; i > 1; i--)
	SXDG.ambig_stack [i].rule = 0;

    return 0;
}



/* Processus de l'ajout d'une regle r :

   1) On desactive les regles associees a r (par ex. l'ensemble des regles
      definissant le meme id ds ds blocs englobants).

   2) On ajoute r :
      - si elle n'existe pas, on la cree (desactive'e) et on la met eventuellement
        ds la pile des regles a detruire/desactiver
      - si elle existe:
           - inactive, => pile a desactiver
           - avec les memes attributs => rien
	   - on change les attributs en se souvenant des anciens

      - on l'active

   Sortie de bloc :
   
   1) On detruit les regles
   2) On desactive
   3) On remet les attributs
*/


static void
desactivate_var_rules (name)
    int name;
{
/* desactiver toutes les regles associees a la variable name. */
    int x, rule;

    XxY_Xforeach (var_nameXrule, name, x)
    {
	rule = XxY_Y (var_nameXrule, x);

	if (sxdp_rule_is_activated (rule))
	{
	    SS_push (reactivate_stack, rule);
	    sxdp_rule_deactivate (rule);	    
	}
    }
}

static void
desactivate_type_rules (name)
    int name;
{
/* desactiver toutes les regles associees au type name. */
    int x, rule;

    XxY_Xforeach (type_nameXrule, name, x)
    {
	rule = XxY_Y (type_nameXrule, x);

	if (sxdp_rule_is_activated (rule))
	{
	    SS_push (reactivate_stack, rule);
	    sxdp_rule_deactivate (rule);	    
	}
    }
}

static void
push_rule_attr (rule, action, priority, associativity)
    int rule, action, priority, associativity;
{
    int x;
    bool oflw;

    x = SS_top (rule_attr_stack);
    oflw = x == SS_size (rule_attr_stack);
    SS_push (rule_attr_stack, rule);

    if (oflw)
	rule_to_attr = (struct rule_to_attr*) sxrealloc (rule_to_attr,
							 SS_size (rule_attr_stack) + 1,
							 sizeof (struct rule_to_attr));

    rule_to_attr [x].action = action;
    rule_to_attr [x].priority = priority;
    rule_to_attr [x].associativity = associativity;
}



static int
add_rule (symbols, rhs_lgth, clean, act, prio, assoc)
    int *symbols, rhs_lgth, act, prio, assoc;
    bool clean;
{
    /* Traite la plupart des cas de creation de regles. */
    /* Si clean, on remet en etat en sortie de bloc. */
    /* Si on veut la conserver active en sortie de bloc on appelle
       add_rule avec clean==false */
    /* Si on veut la conserver inactive en sortie de bloc on appelle
       add_rule avec clean==false et dans tous les cas, au retour,
       on la met a la main ds deactivate_stack. */
    int rule, action, priority, associativity;

    if (sxdp_rule_set (rhs_lgth, symbols, &rule, 0, prio, assoc))
    {
	/* La regle est nouvelle */
	sxdp_rule_activate (rule);

	if (clean)
	    /* sera detruite en sortie de bloc */
	    SS_push (rule_stack, rule);
    }
    else
    {
	if (!sxdp_rule_is_activated (rule))
	{
	    /* Existe deja mais est inactive */
	    /* On l'active */
	    sxdp_rule_activate (rule);

	    if (clean)
		/* sera desactivee en sortie de bloc */
		SS_push (deactivate_stack, rule);
	}
	else
	    if (clean)
		/* sera reactivee en sortie de bloc (elle pourra etre desactivee
		 manuellement, au cours du bloc --- cas des etiquettes) */
		SS_push (reactivate_stack, rule);

	sxdp_rule_get_attr (rule, &action, &priority, &associativity);

	if (action != act || priority != prio || associativity != assoc)
	{
	    push_rule_attr (rule, action, priority, associativity);
	    sxdp_rule_set_attr (rule, 0, prio, assoc);
	}
    }

    return rule;
}


static int build_listof ();

static int
get_LHS_or_FUN (current_listof_level, basic_type_name, kind)
    int current_listof_level, basic_type_name, kind;
{
    int			couple, nt;
    struct listof_attr  *pl;
    struct tt_attr	*pattr;
    /* Retourne le code du non-terminal <[listof_]basic_type_name[n]_[LHS|FUN|EXPR|VAR]> */

    if (basic_type_name > 0)
	pattr = &(tt_to_attr [basic_type_name]);

    if (current_listof_level == 0)
    {
	switch (kind)
	{
	case 1:			/* LHS */
	    nt = (basic_type_name < 0)
		? (basic_type_name == -KW_INT
		      ? INT_LHS
		      : BOOL_LHS)
		: pattr->lhs;
	    break;

	case 2:			/* FUN */
	    nt = (basic_type_name < 0)
		? (basic_type_name == -KW_INT
		      ? INT_FUN
		      : BOOL_FUN)
		    : pattr->fun;
	    break;

	case 3:			/* EXPR */
	    nt = (basic_type_name < 0)
		? (basic_type_name == -KW_INT
		      ? INT_EXPR
		      : BOOL_EXPR)
		    : pattr->expr;
	    break;

	case 4:			/* VAR */
	    nt = (basic_type_name < 0)
		? (basic_type_name == -KW_INT
		      ? INT_VAR
		      : BOOL_VAR)
		    : pattr->var;
	    break;
	}
    }
    else
    {
	couple = build_listof (current_listof_level, basic_type_name);
	pl = &(listof_to_attr [couple]);

	switch (kind)
	{
	case 1:
	    nt = pl->lhs;
	    break;

	case 2:
	    nt = pl->fun;
	    break;

	case 3:
	    nt = pl->expr;	
	    break;

	case 4:
	    nt = pl->var;	
	    break;
	}
    }

    return nt;
}


static int
build_listof (current_listof_level, basic_type_name)
    int current_listof_level, basic_type_name;
{
    int		couple, LISTOFnm1_TYPE, LISTOFn_TYPE, LISTOFn_TYPE_LHS,
                LISTOFn_TYPE_FUN, LISTOFn_TYPE_VAR, rule;
    bool	is_new;
    struct listof_attr  *pl;

    is_new = !XxY_set (&listof_hd, current_listof_level, basic_type_name, &couple);
    pl = &(listof_to_attr [couple]);

    if (is_new || !pl->is_activated)
    {
	LISTOFnm1_TYPE = (current_listof_level > 1)
	    ? listof_to_attr [build_listof (current_listof_level - 1, basic_type_name)].expr
		: get_LHS_or_FUN (0, basic_type_name, 3 /* EXPR */);

	if (is_new)
	{
	    /* Nouveau, on cree (recursivement):
	       <LISTOFn_TYPE>	= ( ) ;
	       <LISTOFn_TYPE>	= cons ( <LISTOFn-1_TYPE> , <LISTOFn_TYPE> ) ;
	       <LISTOFn_TYPE>	= tail ( <LISTOFn_TYPE> ) ;
	       <LISTOFn_TYPE>	= <LISTOFn_TYPE_VAR> ;
	       
	       <LISTOFn_TYPE_VAR>	= <LISTOFn_TYPE_LHS> ;
	       <LISTOFn_TYPE_VAR>	= <LISTOFn_TYPE_FUN> ;
	       
	       <STMT>		= <LISTOFn_TYPE_LHS>  "="  <LISTOFn_TYPE> ";" ;
	       
	       <BOOL_EXPR>		= <LISTOFn_TYPE>  comp_op  <LISTOFn_TYPE> ;
	       */

	    pl->var = LISTOFn_TYPE_VAR = new_nt ();
	    pl->lhs = LISTOFn_TYPE_LHS = new_nt ();
	    pl->fun = LISTOFn_TYPE_FUN = new_nt ();
	    pl->expr = LISTOFn_TYPE = new_nt ();
	}
	else
	{
	    LISTOFn_TYPE_VAR = pl->var;
	    LISTOFn_TYPE_LHS = pl->lhs;
	    LISTOFn_TYPE_FUN = pl->fun;
	    LISTOFn_TYPE = pl->expr;
	}
	
	pl->prev = prev_listof;
	prev_listof = couple;
	pl->is_activated = true;

	/* Les regles sont uniquement desactivees en sortie de bloc. */
	/* Les codes des non-terminaux sont gardes. */

	symbs [0] = LISTOFn_TYPE;

	symbs [1] = -PAR_G;
	symbs [2] = -PAR_D;
	rule = add_rule (symbs, 2, false, 0, 0, UNDEF_ASSOC);
	SS_push (deactivate_stack, rule);

	symbs [1] = -KW_CONS;
	symbs [2] = -PAR_G;
	symbs [3] = LISTOFnm1_TYPE ;
	symbs [4] = -VIRG;
	symbs [5] = LISTOFn_TYPE;
	symbs [6] = -PAR_D;
	rule = add_rule (symbs, 6, false, 0, 0, UNDEF_ASSOC);
	SS_push (deactivate_stack, rule);

	symbs [1] = -KW_TAIL;
	symbs [2] = -PAR_G;
	symbs [3] = LISTOFn_TYPE;
	symbs [4] = -PAR_D;
	rule = add_rule (symbs, 4, false, 0, 0, UNDEF_ASSOC);
	SS_push (deactivate_stack, rule);

	symbs [1] = LISTOFn_TYPE_VAR;
	rule = add_rule (symbs, 1, false, 0, 0, UNDEF_ASSOC);
	SS_push (deactivate_stack, rule);

	symbs [0] = LISTOFn_TYPE_VAR;

	symbs [1] = LISTOFn_TYPE_LHS;
	rule = add_rule (symbs, 1, false, 0, 0, UNDEF_ASSOC);
	SS_push (deactivate_stack, rule);

	symbs [1] = LISTOFn_TYPE_FUN;
	rule = add_rule (symbs, 1, false, 0, 0, UNDEF_ASSOC);
	SS_push (deactivate_stack, rule);

	symbs [0] = STMT;
	symbs [1] = LISTOFn_TYPE_LHS;
	symbs [2] = -EGAL;
	symbs [3] = LISTOFn_TYPE ;
	symbs [4] = -POINT_VIRG;
	rule = add_rule (symbs, 4, false, 0, 0, UNDEF_ASSOC);
	SS_push (deactivate_stack, rule);

	symbs [0] = BOOL_EXPR;

	symbs [1] = LISTOFn_TYPE;
	symbs [2] = -COMP_OP;
	symbs [3] = LISTOFn_TYPE ;
	rule = add_rule (symbs, 3, false, 0, 400, NON_ASSOC);
	SS_push (deactivate_stack, rule);
    }

    return couple;
}



static void
gen_labels ()
{
    int couple, rule;

    XxY_Xforeach (labels_hd, current_block_nb, couple)
    {
	
	/* <STMT> = goto %label ";" */
	symbols [0] = STMT ;
	symbols [1] = -KW_GOTO ;
	symbols [2] = new_xt (-IDENTIFIER, -XxY_Y (labels_hd, couple));
	symbols [3] = -POINT_VIRG;
	rule = add_rule (symbols, 3, true, 0, 0, UNDEF_ASSOC);
	/* SS_push (xtnt_stack, symbols [2]); */

	/* <LABEL> = %label : ; 22 */
	symbols [0] = LABEL ;
	symbols [1] = symbols [2] ;
	symbols [2] = -DEUX_POINTS ;
	/* Cette regle sera supprimee de l'automate par @14 apres sa premiere utilisation. */
	rule = add_rule (symbols, 2, true, 22, 0, UNDEF_ASSOC);
	/* l'action 22 mettra ds la sem_stack le numero de rule qui sera utilise par @14. */
    }
}


static void
ACTION_1 ()
{
    prev_listof = -prev_listof;
    prev_recordtype_lhs = -prev_recordtype_lhs;
    block_level++;
    SS_push (block_stack, current_block_nb);
    current_block_nb = ++block_nb;
    sxsymbol_table_open (symbol_table, block_level);
    sxsymbol_table_open (type_table, block_level);
    SS_open (rule_stack);
    SS_open (rule_attr_stack);
    SS_open (deactivate_stack);
    SS_open (reactivate_stack);
    SS_open (xtnt_stack);

    /* On genere la reconnaissance des instructions etiquettees ainsi que les goto
       vers ces etiquettes */
    gen_labels () ;
}


static void
rules_at_block_exit (delete)
    bool delete;
{
    int rule, x;

    /* Si on est dans SXUNDO, les regles ne doivent pas etre detruites mais
       simplement desactivees. Ca permet de les refabriquer avec le meme
       numero, donc les items associes ainsi que les etats. */
    while (!SS_is_empty (rule_stack))
    {
	/* On supprime/desactive la regle rule */
	rule = SS_pop (rule_stack);

	if (delete)
	    sxdp_rule_delete (rule);
	else
	    sxdp_rule_deactivate (rule);
    }

    while (!SS_is_empty (deactivate_stack))
    {
	/* On desactive la regle rule */
	rule = SS_pop (deactivate_stack);
	sxdp_rule_deactivate (rule);
    }

    while (!SS_is_empty (reactivate_stack))
    {
	/* On reactive les regles "cachees". */
	rule = SS_pop (reactivate_stack);
	sxdp_rule_activate (rule);
    }

    while (!SS_is_empty (rule_attr_stack))
    {
	/* On repositionne les act, prio, assoc de rule */
	rule = SS_pop (rule_attr_stack);
	x = SS_top (rule_attr_stack);
	sxdp_rule_set_attr (rule,
			    rule_to_attr [x].action,
			    rule_to_attr [x].priority,
			    rule_to_attr [x].associativity);
    }

    SS_close (rule_stack);
    SS_close (rule_attr_stack);
    SS_close (deactivate_stack);
    SS_close (reactivate_stack);
}

static void
ACTION_2 (delete)
    bool delete;
{
    int tnt, prdct_no, couple, name, x;

    rules_at_block_exit (delete);

    if (delete)
	while (!SS_is_empty (xtnt_stack))
	{
	    /* On recupere les symboles */
	    tnt = SS_pop (xtnt_stack);

	    if (tnt < 0)
		sxdp_delete_xt (-tnt);
	    else
		if (tnt > 0)
		    sxdp_delete_xnt (tnt);
	}

    sxsymbol_table_foreach_scope (symbol_table, block_level, name)
    {
	XxY_Xforeach (var_nameXrule, name, couple)
	    XxY_erase (var_nameXrule, couple);
    }

    sxsymbol_table_foreach_scope (type_table, block_level, name)
    {
	XxY_Xforeach (type_nameXrule, name, couple)
	    XxY_erase (type_nameXrule, couple);
    }

    if (delete)
    {
	/* On recupere les doublets de type_steXblock_nb */
	XxY_Yforeach (type_steXblock_nb, current_block_nb, couple)
	    XxY_erase (type_steXblock_nb, couple);
    }
	   
    while (prev_listof > 0)
    {
	listof_to_attr [prev_listof].is_activated = false;
	/* On ne recupere pas les elements de "listof_hd" car "listof_to_attr"
	   contient des codes de non-terminaux qui doivent etre reutilises
	   pour produire les memes regles. */
	/* XxY_erase (listof_hd, prev_listof); */
	prev_listof = listof_to_attr [prev_listof].prev; 
    }

    prev_listof = -prev_listof;

    /* On recupere la structure dans laquelle sont stockees les regles generees
       au cours du traitement du corps d'un record. */
    while (prev_recordtype_lhs > 0)
    {
	XxY_Xforeach (recordtype_lhsXrule, prev_recordtype_lhs, x)
	    XxY_erase (recordtype_lhsXrule, x);

	prev_recordtype_lhs = tt_to_attr [prev_recordtype_lhs].prev_recordtype_lhs; 
    }

    prev_recordtype_lhs = -prev_recordtype_lhs;

    SS_close (xtnt_stack);

    /* Si on est en "SXUNDO", on conserve les entrees des tables de symboles
       qui ont pu etre construites au cours du "SXDO" afin de retrouver les
       memes valeurs sur "SXACTION" */
    sxsymbol_table_close (&symbol_table, block_level, delete);
    sxsymbol_table_close (&type_table, block_level, delete);

    block_level--;
    current_block_nb = SS_pop (block_stack);
}


/* Il n'est pas possible d'associer a un id un entier (predicat) qui soit
   caracteristique de l'ensemble des significations Ex
   int f();
   { int f[];
   int f ();
   apres la ligne 2 "f"&n designe la ft de la 1ere ligne et le tableau.
   comme le f de la 3eme ligne cache la 1ere, il ne peut avoir le meme
   numero (p != n), or apres la 3eme ligne "f"&p doit designer les 2 derniers
   "f", or le f tableau s'appelle deja "f"&n... */

bool sxparser_action (SXINT what, SXINT act_no)
{
    int			name, rule, x, bot, xt, tnt, lhs_case, prev_xname, prev_lhs_case,
                        prev_block, prdct_no, param_type_name;
    bool		is_new_name;
    struct tt_attr	*ptt_attr;

    switch (what) {
    case SXOPEN:
	sxsymbol_table_alloc (&symbol_table, 500, 750, symbol_table_oflw);
	/* name == 1, bidon pour version avec predicats, ca permet d'identifier
	   le name avec le predicat == -name. */
	st_to_attr = (struct st_attr*) sxalloc (sxsymbol_table_size (symbol_table) + 1,
						sizeof (struct st_attr));
	sxsymbol_table_put (&symbol_table, SXEMPTY_STE, 0, &name);

	sxsymbol_table_alloc (&type_table, 500, 750, type_table_oflw);
	/* name == 1, bidon pour version avec predicats, ca permet d'identifier
	   le name avec le predicat == -name. */
	tt_to_attr = (struct tt_attr*) sxalloc (sxsymbol_table_size (type_table) + 1,
						sizeof (struct tt_attr));
	sxsymbol_table_put (&type_table, SXEMPTY_STE, 0, &name);

	rule_stack = SS_alloc (200);
	rule_attr_stack = SS_alloc (200);
	deactivate_stack = SS_alloc (200);
	reactivate_stack = SS_alloc (200);
	rule_to_attr = (struct rule_to_attr*) sxalloc (SS_size (rule_attr_stack) + 1,
						       sizeof (struct rule_to_attr));
	xtnt_stack = SS_alloc (200);
	block_stack = SS_alloc (20);
	symbols = (int*) sxalloc ((symbols_size = 30) + 1, sizeof (int));
	XxY_alloc (&listof_hd, "listof", 50, 1, 0, 0, listof_oflw, NULL);
	listof_to_attr = (struct listof_attr*) sxalloc (XxY_size (listof_hd) + 1,
							sizeof (struct listof_attr));
	XxY_alloc (&labels_hd, "labels", 75, 1, 1, 0, NULL, NULL);
	XxY_alloc (&recordtype_lhsXrule, "recordtype_lhsXrule", 50, 1, 1, 0, NULL, NULL);

	sem_stack = (struct sem_stack*) sxalloc ((sem_stack_size = SXDG.lgt1) + 1,
						 sizeof (struct sem_stack));
	XxY_alloc (&var_nameXrule, "var_nameXrule", 100, 1, 1, 0, NULL, NULL);
	XxY_alloc (&type_nameXrule, "type_nameXrule", 100, 1, 1, 0, NULL, NULL);
	XxY_alloc (&type_steXblock_nb, "type_steXblock_nb", 100, 1, 0, 1,
		   type_steXblock_nb_oflw, NULL);
	
	type_steXblock_nb_to_attr = (struct type_steXblock_nb_to_attr*)
	    sxalloc (XxY_size (type_steXblock_nb) + 1,
		     sizeof (struct type_steXblock_nb_to_attr));
	
	return SXANY_BOOL;

    case SXINIT:
	SXDG.desambig = desambig;
	sxplocals.mode.with_do_undo = true; /* Pour tester... */

	block_level = 0;
	current_block_nb = block_nb = 0;
	listof_level = 0;
	prev_listof = 0;
	prev_recordtype_lhs = 0;
	sxsymbol_table_open (symbol_table, block_level);
	sxsymbol_table_open (type_table, block_level);
	SS_open (rule_stack);
	SS_open (rule_attr_stack);
	SS_open (deactivate_stack);
	SS_open (reactivate_stack);	
	SS_open (xtnt_stack);
	return SXANY_BOOL;

    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {
	case 0:
	    return SXANY_BOOL;
	    
	case 1:			/* <BLOCK> = { @1 <DECL*> <STMT*> } @2 ; */
	    /* Entree de block */
	    is_record_body = false;
	    ACTION_1 ();

	    return SXANY_BOOL;
	    
	case 2:			/* <BLOCK> = { @1 <DECL*> <STMT*> } @2 ; */
	    /* Sortie de block */
	    ACTION_2 (true);

	    return SXANY_BOOL;

	case 3:	/* <DECL> = <TYPE_NAME> @3 <DECLARATOR+> ";" ;
		   <DECL> = subtype <BASIC_TYPE_NAME> @3 <SUBTYPE_NAME+> ;
		   <OBJECT_DECL> = <TYPE_NAME> @3 <LHS_DECLARATOR+> ";" ;*/
	    current_listof_level = listof_level;
	    listof_level = 0;
	    ptok = &(SXGET_TOKEN (sxplocals.atok_no - 1));
	    basic_type_name = ptok->lahead;

	    if (basic_type_name == IDENTIFIER)
	    {
		if ((basic_type_name = sxsymbol_table_get_nested (type_table,
							    ptok->string_table_entry)) == 0)
		    sxput_error (ptok->source_index,
				 "%snot a subtype name",
				 sxplocals.sxtables->err_titles [2]);
	    }
	    else
		/* basic_type_name == KW_INT || basic_type_name == KW_BOOL)*/
		basic_type_name = -basic_type_name;

	    /* En sequence */
	case 4: /* <DECLARATOR+> = <DECLARATOR+> , @4 <DECLARATOR> ;
		   <LHS_DECLARATOR+> = <LHS_DECLARATOR+> , @4 <LHS_DECLARATOR> ; */
	    /* On reserve la place pour <RECTYPE_LHS> "." */
	    symbols_top = is_record_body ? 3 : 1;

	    return SXANY_BOOL;

	case 5:			/* <INT_EXPR+> = <INT_EXPR+> ,  <INT_EXPR> @5 ;
				   <INT_EXPR+> = <INT_EXPR> @5 ; */

	    if (symbols_top + 3 > symbols_size)
		symbols_oflw (symbols_top + 3);

	    symbols [++symbols_top] = -CROC_G;
	    symbols [++symbols_top] = INT_EXPR;
	    symbols [++symbols_top] = -CROC_D;
	    return SXANY_BOOL;

	case 8:			/* <PROFILE> = %id  @7 ( <TYPE_NAME*> ) @8 ; */
	    symbols [++symbols_top] = -PAR_D;
	    /* On continue en sequence */

	case 6:			/* <DECLARATOR>		= %id @6 ; */
	case 12:		/* <ARRAY_DECLARATOR>	= %id  [ <INT_EXPR+> ] @12 ; */
	    /* sur : "short petit_entier, fun (short), t [2];"
	      on cree :
	      <SHORT_LHS> = petit_entier ;
	      <SHORT_FUN> = fun ( <SHORT_EXPR> ) ;
	      <SHORT_LHS> = t [ <INT_EXPR> ] ;
	    */
	    if (basic_type_name != 0)
	    {
		symbols [0] = get_LHS_or_FUN (current_listof_level, basic_type_name,
					      act_no == 8 ? 2 : 1);
		lhs_case = act_no == 6 ? SCALAR : ARRAY_or_FUNCTION;
		ptok = &(SXSTACKtoken (SXSTACKnewtop () -
				     (act_no == 6 ? 1 : (act_no == 12 ? 4 : 5))));

		prev_xname =
		    sxsymbol_table_get_nested (symbol_table, ptok->string_table_entry);

		if (!is_record_body)
		{
		    if (prev_xname > 0)
		    {
			/* Il existe une variable de meme nom visible */
			prev_lhs_case = st_to_attr [prev_xname].lhs_case;
			prev_block = sxsymbol_table_level (symbol_table, prev_xname);
			symbols [1] = xt = st_to_attr [prev_xname].xt;

			if (prev_block < block_level)
			{
			    /* et definie dans un bloc externe */
			    is_new_name = sxsymbol_table_put (&symbol_table,
							      ptok->string_table_entry,
							      block_level,
							      &name); /* true */
			    if (lhs_case == SCALAR || prev_lhs_case == SCALAR)
			    {
				/* La nouvelle variable cache la precedente */
				/* desactiver toutes les regles associees a prev_xname. */
				desactivate_var_rules (prev_xname);
			    }
			}
			else
			{
			    /* et definie au meme niveau de bloc */
			    name = prev_xname;
			    is_new_name = false;

			    if (lhs_case == SCALAR || prev_lhs_case == SCALAR)
				/* Erreur : illegale redefinition */
				xt = 0;
			    else
			    {
				/* Surcharge ou erreur suivant que la production qui sera generee
				   existe deja ou non et a ete generee au meme niveau de bloc. */
				if ((rule = sxdp_rule_is_set (symbols_top, symbols)) != 0
				    && sxdp_rule_is_activated (rule))
				{
				    /* rule existe deja */
				    /* On regarde si la regle a ete generee a ce niveau de bloc */
				    if (XxY_is_set (&var_nameXrule, name, rule) > 0)
					/* Error */
					xt = 0;
				}
			    }
			}
		    }
		    else
		    {
			/* Aucune variable de ce nom est visible */
			is_new_name = sxsymbol_table_put (&symbol_table, ptok->string_table_entry,
							  block_level, &name); /* true */
			symbols [1] = xt = new_xt (-IDENTIFIER, -ptok->string_table_entry);
		    }
	
		    if (xt != 0)
		    {
			int couple;

			if (is_new_name)
			{
			    struct st_attr	*pattr;

			    pattr = st_to_attr + name;
			    pattr->lhs_case = lhs_case;
			    pattr->prev_xname = prev_xname;
			    pattr->xt = xt;
			    pattr->listof_level = current_listof_level;
			}

			rule = add_rule (symbols, symbols_top, true, 0, 0, UNDEF_ASSOC);
			XxY_set (&var_nameXrule, name, rule, &couple);
		    }
		    else
			sxput_error (ptok->source_index,
				     "%sillegal redefinition",
				     sxplocals.sxtables->err_titles [2]);  

		}
		else if (record_name != 0)
		{
		    /* Declaration ds le corps d'un record. */
		    /* On ne peut pas avoir de fonction. */
		    /* On change les prefixes par :
		       <TYPE_LHS> = <RECTYPE_VAR> "." 
		       La 2eme est necessire car une fonction "f" peut rendre un record
		       et "f().a" ne peut pas etre utilise en partie gauche.
		    */
		    symbols [1] = tt_to_attr [record_name].var;
		    symbols [2] = -POINT;

		    if (prev_xname > 0)
		    {
			/* Il existe une variable de meme nom visible */
			prev_lhs_case = st_to_attr [prev_xname].lhs_case;
			prev_block = sxsymbol_table_level (symbol_table, prev_xname);
			symbols [3] = xt = st_to_attr [prev_xname].xt;

			if (prev_block < block_level)
			{
			    /* et definie dans un bloc externe */
			    is_new_name = sxsymbol_table_put (&symbol_table,
							      ptok->string_table_entry,
							      block_level,
							      &name); /* true */
			    /* Aucun conflit possible, sauf par l'intermediaire des "with"!! */
			}
			else
			{
			    /* et definie au meme niveau de bloc */
			    name = prev_xname;
			    is_new_name = false;

			    if (lhs_case == SCALAR || prev_lhs_case == SCALAR)
				/* Erreur : illegale redefinition */
				xt = 0;
			    else
			    {
				/* Surcharge ou erreur suivant que la production qui sera generee
				   existe deja ou non et a ete generee au meme niveau de bloc. */
				if ((rule = sxdp_rule_is_set (symbols_top, symbols)) != 0
				    && sxdp_rule_is_activated (rule))
				{
				    /* rule existe deja */
				    /* On regarde si la regle a ete generee a ce niveau de bloc */
				    if (XxY_is_set (&var_nameXrule, name, rule) > 0)
					/* Error */
					xt = 0;
				}
			    }
			}
		    }
		    else
		    {
			/* Aucune variable de ce nom est visible */
			is_new_name = sxsymbol_table_put (&symbol_table, ptok->string_table_entry,
							  block_level, &name); /* true */
			symbols [3] = xt = new_xt (-IDENTIFIER, -ptok->string_table_entry);
		    }
	
		    if (xt != 0)
		    {
			if (is_new_name)
			{
			    struct st_attr	*pattr;

			    pattr = st_to_attr + name;
			    pattr->lhs_case = lhs_case;
			    pattr->prev_xname = prev_xname;
			    pattr->xt = xt;
			    pattr->listof_level = current_listof_level;
			}

			rule = add_rule (symbols, symbols_top, true, 0, 0, UNDEF_ASSOC);
			XxY_set (&var_nameXrule, name, rule, &x);
			XxY_set (&recordtype_lhsXrule, tt_to_attr [record_name].lhs, rule, &x);
		    }
		    else
			sxput_error (ptok->source_index,
				     "%sillegal redefinition of a record field",
				     sxplocals.sxtables->err_titles [2]);  
		}
	    }

	    return SXANY_BOOL;

	case 7:			/* <PROFILE> = %id  ( @7 <TYPE_NAME*> @8 ) ; */
	    symbols [++symbols_top] = -PAR_G;
	    return SXANY_BOOL;

	case 9:			/* <TYPE_NAME+> = <TYPE_NAME+> , <TYPE_NAME> @9 ; */
	    symbols [++symbols_top] = -VIRG;
	    /* On continue en sequence */

	case 10:		/* <TYPE_NAME+> =  <TYPE_NAME> @10 ; */ 
	    if (symbols_top + 3 > symbols_size)
		symbols_oflw (symbols_top + 3);

	    ptok = &(SXGET_TOKEN (sxplocals.atok_no - 1));
	    param_type_name = ptok->lahead;

	    if (param_type_name == IDENTIFIER)
	    {
		if ((param_type_name = sxsymbol_table_get_nested (type_table,
								  ptok->string_table_entry)) == 0)
		{
		    sxput_error (ptok->source_index,
				 "%snot a subtype name, int is assumed",
				 sxplocals.sxtables->err_titles [2]);
		    param_type_name = -KW_INT; /* Defaut */
		    x = 3;  /* EXPR */
		}
		else
		    x = tt_to_attr [param_type_name].kind == SUBTYPE
			? 3 /* EXPR */ : /* RECORD */ 4 /* VAR */;
	    }
	    else
	    {
		/* param_type_name == KW_INT || param_type_name == KW_BOOL)*/
		param_type_name = -param_type_name;
		x = 3; /* EXPR */
	    }
	    
	    symbols [++symbols_top] = get_LHS_or_FUN (listof_level,
						      param_type_name,
						      x);
	    listof_level = 0;

	    return SXANY_BOOL;

	case 11: /* <DYNAM_OPER> = ( <DYNOP_PRIO> , <DYNOP_CLASS> , <DYNOP_ASSOC> @11 ) ; */
	    if ((xt = symbols [1]) != 0)
	    {
		int prio, class, assoc;

		if (symbols_top == 4 || symbols_top == 6)
		{
		    prio = SXGET_TOKEN (sxplocals.atok_no - 5).string_table_entry;
		    class = SXGET_TOKEN (sxplocals.atok_no - 3).lahead;
		    assoc = SXGET_TOKEN (sxplocals.atok_no - 1).lahead;

		    if (symbols_top == 4)
		    {
			if (class == KW_PREFIX)
			{	
			    symbols [2] = symbols [symbols_top = 3];
			}
			else
			    if (class == KW_POSTFIX) 
			    {
				symbols [symbols_top = 2] = symbols [1];
				symbols [1] = symbols [3];
			    }
			    else
				symbols [1] = 0;
		    }
		    else
		    {
			if (class == KW_INFIX)
			{
			    symbols [2] = symbols [1];
			    symbols [1] = symbols [3];
			    symbols [symbols_top = 3] = symbols [5];
			}
			else
			    symbols [1] = 0;
		    }
		}
		else
		    symbols [1] = 0;

		if (symbols [1] != 0)
		{
		    prio = atoi (sxstrget (prio));
		    assoc = (assoc == KW_LEFT) ? LEFT_ASSOC
			: (assoc == KW_RIGHT ? RIGHT_ASSOC : NON_ASSOC);
		    rule = add_rule (symbols, symbols_top, true, 0, prio, assoc);

		    SXDG.t_priorities [-xt].value = prio;
		    SXDG.t_priorities [-xt].assoc = assoc;
		}
		else
		    sxput_error (SXGET_TOKEN (sxplocals.atok_no).source_index,
				 "%sillegal definition of a dynamic operator",
				 sxplocals.sxtables->err_titles [2]);
	    }

	    return SXANY_BOOL;

	case 13: /* <SUBTYPE_NAME+> = <SUBTYPE_NAME+> , %id @13 ;
		    <SUBTYPE_NAME+> = %id @13 ; */
	    /* Si on a le texte source :
	     * ... subtype int short; ...
	     * On cree :
	     *<BASIC_TYPE_NAME>	= short ;
	     *
	     *<SHORT_VAR>	= <SHORT_LHS> ;
	     *<SHORT_VAR>	= <SHORT_FUN> ;
	     *
	     *<INT_VAR>		= <SHORT_VAR> ;
	     *
	     *<STMT>		= <SHORT_LHS>  "="  <SHORT_EXPR> ";" ;
	     *
	     *<SHORT_EXPR>	= <SHORT_EXPR> add_op <SHORT_EXPR> ;
	     *<SHORT_EXPR>	= <SHORT_EXPR> mul_op <SHORT_EXPR> ;
	     *<SHORT_EXPR>	= add_op <SHORT_EXPR> ;
	     *<SHORT_EXPR>	= <SHORT_PRIMARY> ;
	     *
	     *<SHORT_PRIMARY>	= <INT_CONST> ;
	     *<SHORT_PRIMARY>	= <SHORT_VAR> ;
	     *<SHORT_PRIMARY>	= ( <SHORT_EXPR> ) ;
	     *<SHORT_PRIMARY>	= ( short ) <INT_PRIMARY> ;
	     *
	     *<INT_PRIMARY>	= ( short ) <INT_PRIMARY> ;
	     */

	    if (basic_type_name != 0)
	    {
		ptok = &(SXGET_TOKEN (sxplocals.atok_no - 1));

		prev_xname =
		    sxsymbol_table_get_nested (type_table, ptok->string_table_entry);

		if (prev_xname > 0)
		{
		    /* Il existe un sous-type de meme nom visible */
		    prev_block = sxsymbol_table_level (type_table, prev_xname);

		    if (prev_block < block_level)
		    {
			/* et declare ds un bloc englobant */
			/* Le nouveau nom cache le precedent, on desactive les regles associees */
			desactivate_type_rules (prev_xname);
			prev_xname = 0;
		    }
		    else
			sxput_error (ptok->source_index,
				     "%sAlready defined as a %s name.",
				     sxplocals.sxtables->err_titles [2],
				     tt_to_attr [prev_xname].kind == SUBTYPE
				     ? "subtype" : "record");
		}

		if (prev_xname == 0)
		{
		    /* Nouveau nom de [sous-]type au niveau courant */
		    /* On code de facon qu'un "SXACTION" qui suit un "SXDO" genere
		       les regles de facon identique. */
		    int subtype_var, subtype_lhs, subtype_fun, subtype_expr, subtype_primary,
		        subtype_name;
		    int seed_var, seed_primary, seed_const, seed_type_name;
		    int father_type_name, couple;
		    bool	already_found;
		    struct type_steXblock_nb_to_attr	*ptb_to_attr;

		    sxsymbol_table_put (&type_table, ptok->string_table_entry,
					block_level, &name); /* true */

		    /* Si le sous-type est vu en look-ahead (plusieurs fois) et en
		       normal, le "name" peut etre different (ou pas) a chaque fois.
		       En revanche, le couple (string_table_entry, current_block_nb)
		       est lui caracteristique. */
		    already_found = XxY_set (type_steXblock_nb, ptok->string_table_entry,
					     current_block_nb, &couple);
		    /* Il faut assurer qu'un bloc visite' en SXDO puis en SXACTION aura
		       le meme current_block_nb. */
		    /* Ces couples seront "recuperes" en sortie de bloc sur "SXACTION" */
		    ptb_to_attr = type_steXblock_nb_to_attr + couple;

		    if (already_found)
		    {
			/* Existe deja */
			subtype_var = ptb_to_attr->var;
			subtype_lhs = ptb_to_attr->lhs;
			subtype_fun = ptb_to_attr->fun;
			subtype_expr = ptb_to_attr->expr;
			subtype_primary = ptb_to_attr->primary;
		    }
		    else
		    {
			ptb_to_attr->var = subtype_var = new_nt ();
			ptb_to_attr->lhs = subtype_lhs = new_nt ();
			ptb_to_attr->fun = subtype_fun = new_nt ();
			ptb_to_attr->expr = subtype_expr = new_nt ();
			ptb_to_attr->primary = subtype_primary = new_nt ();
			/* On "recuperera" ces noms en sortie de bloc "SXACTION" */
			SS_push (xtnt_stack, subtype_var);
			SS_push (xtnt_stack, subtype_lhs);
			SS_push (xtnt_stack, subtype_fun);
			SS_push (xtnt_stack, subtype_expr);
			SS_push (xtnt_stack, subtype_primary);
		    }

		    subtype_name = new_xt (-IDENTIFIER, -ptok->string_table_entry);

		    seed_type_name = (basic_type_name < 0) ? -basic_type_name
			: tt_to_attr [basic_type_name].seed_type_name;

		    seed_var = seed_type_name == KW_INT ? INT_VAR : BOOL_VAR;
		    seed_primary = seed_type_name == KW_INT ? INT_PRIMARY : BOOL_PRIMARY;
		    seed_const = seed_type_name == KW_INT ? INT_CONST : BOOL_CONST;

		    ptt_attr = &(tt_to_attr [name]);
		    ptt_attr->kind = SUBTYPE;
		    ptt_attr->seed_type_name = seed_type_name;
		    ptt_attr->father_type_name = basic_type_name;
		    ptt_attr->xt = subtype_name;
		    ptt_attr->lhs = subtype_lhs;
		    ptt_attr->fun = subtype_fun;
		    ptt_attr->expr = subtype_expr;
		    ptt_attr->primary = subtype_primary;
		    ptt_attr->var = subtype_var;
 
		    /* <BASIC_TYPE_NAME> = subtype_name ; */
		    symbols [0] = BASIC_TYPE_NAME;
		    symbols [1] = subtype_name;
		    rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);

		    /* <SUBTYPE_VAR> = <SUBTYPE_LHS> ; */
		    symbols [0] = subtype_var;
		    symbols [1] = subtype_lhs;
		    rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);

		    SS_push (xtnt_stack, subtype_var);
		    SS_push (xtnt_stack, subtype_lhs);

		    /* <SUBTYPE_VAR> = <SUBTYPE_FUN> ; */
		    symbols [1] = subtype_fun;
		    rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);

		    SS_push (xtnt_stack, subtype_fun);

		    /* la connexion avec le type de base se fait uniquement par
		       l'intemediaire du pere. */

		    /* <FATHER_VAR> = <SUBTYPE_VAR> ; */
		    symbols [0] = basic_type_name < 0
			? seed_var : tt_to_attr [basic_type_name].var;
		    symbols [1] = subtype_var;
		    rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);

		    /* <STMT> = <SUBTYPE_LHS>  "="  <SUBTYPE_EXPR> ";" ; */
		    symbols [0] = STMT;
		    symbols [1] = subtype_lhs;
		    symbols [2] = -EGAL;
		    symbols [3] = subtype_expr;
		    symbols [4] = -POINT_VIRG;
		    rule = add_rule (symbols, 4, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);
		    SS_push (xtnt_stack, subtype_expr);

		    /*	<SUBTYPE_EXPR> = <SUBTYPE_EXPR> add_op <SUBTYPE_EXPR> ; */
		    symbols [0] = subtype_expr;
		    symbols [1] = subtype_expr;
		    symbols [2] = -((seed_type_name == KW_INT) ? ADD_OP : OR_OP); 
		    symbols [3] = subtype_expr;
		    rule = add_rule (symbols, 3, true, 0, 100 /* ADD_PRIO */, LEFT_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);

		    /*	<SUBTYPE_EXPR> = <SUBTYPE_EXPR> mul_op <SUBTYPE_EXPR> ; */
		    symbols [2] = -((seed_type_name == KW_INT) ? MUL_OP : AND_OP);
		    rule = add_rule (symbols, 3, true, 0, 200 /* MUL_PRIO */, LEFT_ASSOC);
		    
		    if (seed_type_name == KW_BOOL)
		    {
			/* <SUBTYPE_EXPR> = <INT_EXPR>  comp_op  <INT_EXPR> ; */
			symbols [1] = symbols [3] = INT_EXPR;
			symbols [2] = -COMP_OP;

			/* Priorite au plus precis */
			rule = add_rule (symbols, 3, true, 0, 400, NON_ASSOC);
			XxY_set (&type_nameXrule, name, rule, &couple);
		    }

		    /*	<SUBTYPE_EXPR> = un_op <SUBTYPE_EXPR> ; */
		    symbols [1] = -((seed_type_name == KW_INT) ? ADD_OP : NOT_OP);
		    symbols [2] = subtype_expr;
		    rule = add_rule (symbols, 2, true, 0, 300 /* UNOP_PRIO */, RIGHT_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);
		    
		    /* <SUBTYPE_EXPR> = <SUBTYPE_PRIMARY> ; */
		    symbols [1] = subtype_primary;
		    rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);
		    SS_push (xtnt_stack, subtype_primary);

		    /* <SUBTYPE_PRIMARY> = <SEED_CONST> ; */
		    symbols [0] = subtype_primary;
		    symbols [1] = seed_const;
		    rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);

		    /* <SUBTYPE_PRIMARY> = <SUBTYPE_VAR> ; */
		    symbols [1] = subtype_var;
		    rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);

		    /* <SUBTYPE_PRIMARY> = ( <SUBTYPE_EXPR> ) ; */
		    symbols [1] = -PAR_G;
		    symbols [2] = subtype_expr;
		    symbols [3] = -PAR_D;
		    rule = add_rule (symbols, 3, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);

		    /* <SUBTYPE_PRIMARY> = ( subtype_name ) <SEED_PRIMARY> ; */
		    symbols [1] = -PAR_G;
		    symbols [2] = subtype_name;
		    symbols [3] = -PAR_D;
		    symbols [4] = seed_primary;
		    rule = add_rule (symbols, 4, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);

		    /* Pour tous les FATHER on genere :
		       <FATHER_PRIMARY> = ( subtype_name ) <SEED_PRIMARY> ; */
		    /* Ca permet de traiter des trucs comme "short = (char) i;" */
		    for (father_type_name = basic_type_name;
			 father_type_name > 0;
			 father_type_name = tt_to_attr [father_type_name].father_type_name)
		    {
			symbols [0] = tt_to_attr [father_type_name].primary;
			rule = add_rule (symbols, 4, true, 0, 0, UNDEF_ASSOC);
			XxY_set (&type_nameXrule, name, rule, &couple);
		    }

		    symbols [0] = (father_type_name == -KW_INT) ? INT_PRIMARY : BOOL_PRIMARY;
		    rule = add_rule (symbols, 4, true, 0, 0, UNDEF_ASSOC);
		    XxY_set (&type_nameXrule, name, rule, &couple);
		}
	    }

	    return SXANY_BOOL;

	case 14: /* <STMT>	= <LABEL> @14 <STMT> ; */
	    /* Supprime la regle definissant <LABEL> */
	    rule = sem_stack [SXSTACKnewtop () - 1].rule;
	    sxdp_rule_deactivate (rule);
	    /* desactivee pour le reste du bloc, elle sera detruite ou reactivee
	       en sortie de bloc par le mecanisme usuel. */

	    return SXANY_BOOL;

	case 15: /* <TYPE_NAME>	= listof @15 <TYPE_NAME> ; */
	    listof_level++;
	    return SXANY_BOOL; 

	case 16: /* <T>		= { @16 ; */
	    SS_push (block_stack, current_block_nb);
	    current_block_nb = ++block_nb;
	    return SXANY_BOOL; 

	case 17: /* <T>		= : @17 ; */
	    ptok = &(SXGET_TOKEN (sxplocals.atok_no - 2));

	    if (ptok->lahead == IDENTIFIER)
	    {
		int label;

		XxY_set (&labels_hd, current_block_nb, ptok->string_table_entry, &label);
	    }

	    return SXANY_BOOL; 

	case 18: /* <T>		= } @18 ; */
	    current_block_nb = SS_pop (block_stack);
	    return SXANY_BOOL; 

	
	case 19: /* <TYPE_DECL> = record @19 %id 
		                     { @23
				        <OBJECT_DECL+>
				     } @24 ";" ; */
	    /* @19 : memorisation de "recordtype-name" et generation immediate (ca permet de le
	     *       referencer recursivement) de :
	     *<BASIC_TYPE_NAME>	= recordtype_name ;
	     *
	     *<RECORDTYPE_VAR>	= <RECORDTYPE_LHS> ;
	     *<RECORDTYPE_VAR>	= <RECORDTYPE_FUN> ;
	     *
	     *<BOOL_EXPR>	= <RECORDTYPE_VAR>  comp_op  <RECORDTYPE_VAR> ;
	     *
	     *<STMT>		= <RECORDTYPE_LHS>  "="  <RECORDTYPE_VAR> ";" ;
	     *<STMT>		= with <RECORDTYPE_LHS> do @20 <STMT> @21 ;
	     *
	     * @20 : Pour toute regle "lhs -> <RECORDTYPE_VAR> . rhs" generee par le traitement
	     *       de <OBJECT_DECL+> generer la re`gle "lhs -> rhs".
	     * @21 : Supprimer les re`gles generees par @20. */
	    ptok = &(SXGET_TOKEN (sxplocals.atok_no));

	    prev_xname =
		sxsymbol_table_get_nested (type_table, ptok->string_table_entry);

	    if (prev_xname > 0)
	    {
		/* Il existe un sous-type de meme nom visible */
		prev_block = sxsymbol_table_level (type_table, prev_xname);

		if (prev_block < block_level)
		{
		    /* et declare ds un bloc englobant */
		    /* Le nouveau nom cache le precedent, on desactive les regles associees */
		    desactivate_type_rules (prev_xname);
		    prev_xname = 0;
		}
		else
		    sxput_error (ptok->source_index,
				 "%sAlready defined as a %s name.",
				 sxplocals.sxtables->err_titles [2],
				 tt_to_attr [prev_xname].kind == SUBTYPE
				 ? "subtype" : "record");
	    }

	    if (prev_xname == 0)
	    {
		/* Nouveau nom de record au niveau courant */
		int recordtype_var, recordtype_lhs, recordtype_fun, recordtype_name, couple;
		bool	already_found;
		struct type_steXblock_nb_to_attr	*ptb_to_attr;

		/* record_name est global (pas de record ds un record) */
		sxsymbol_table_put (&type_table, ptok->string_table_entry,
				    block_level, &record_name); /* true */

		already_found = XxY_set (type_steXblock_nb, ptok->string_table_entry,
					 current_block_nb, &couple);
		ptb_to_attr = type_steXblock_nb_to_attr + couple;

		if (already_found)
		{
		    /* Existe deja */
		    recordtype_var = ptb_to_attr->var;
		    recordtype_lhs = ptb_to_attr->lhs;
		    recordtype_fun = ptb_to_attr->fun;
		}
		else
		{
		    ptb_to_attr->var = recordtype_var = new_nt ();
		    ptb_to_attr->lhs = recordtype_lhs = new_nt ();
		    ptb_to_attr->fun = recordtype_fun = new_nt ();
		    /* On "recuperera" ces noms en sortie de bloc "SXACTION" */
		    SS_push (xtnt_stack, recordtype_var);
		    SS_push (xtnt_stack, recordtype_lhs);
		    SS_push (xtnt_stack, recordtype_fun);
		}

		recordtype_name = new_xt (-IDENTIFIER, -ptok->string_table_entry);

		ptt_attr = &(tt_to_attr [record_name]);
		ptt_attr->kind = RECORD;
		ptt_attr->xt = recordtype_name;
		ptt_attr->lhs = recordtype_lhs;
		ptt_attr->fun = recordtype_fun;
		ptt_attr->var = recordtype_var;
		ptt_attr->prev_recordtype_lhs = prev_recordtype_lhs;
		prev_recordtype_lhs = recordtype_lhs;

		/* <BASIC_TYPE_NAME> = recordtype_name ; */
		symbols [0] = BASIC_TYPE_NAME;
		symbols [1] = recordtype_name;
		rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		XxY_set (&type_nameXrule, name, rule, &couple);

		/* <RECORDTYPE_VAR> = <RECORDTYPE_LHS> ; */
		symbols [0] = recordtype_var;
		symbols [1] = recordtype_lhs;
		rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		XxY_set (&type_nameXrule, name, rule, &couple);

		/* <RECORDTYPE_VAR> = <RECORDTYPE_FUN> ; */
		symbols [1] = recordtype_fun;
		rule = add_rule (symbols, 1, true, 0, 0, UNDEF_ASSOC);
		XxY_set (&type_nameXrule, name, rule, &couple);

		/* <BOOL_EXPR> = <RECORDTYPE_VAR>  comp_op  <RECORDTYPE_VAR> ; */
		symbols [0] = BOOL_EXPR;
		symbols [1] = recordtype_var;
		symbols [2] = -COMP_OP;
		symbols [3] = recordtype_var;
		rule = add_rule (symbols, 3, true, 0, 400, NON_ASSOC);
		XxY_set (&type_nameXrule, name, rule, &couple);
		   
		/* <STMT> = <RECORDTYPE_LHS>  "="  <RECORDTYPE_VAR> ";" ; */
		symbols [0] = STMT;
		symbols [1] = recordtype_lhs;
		symbols [2] = -EGAL;
		symbols [3] = recordtype_var;
		symbols [4] = -POINT_VIRG;
		rule = add_rule (symbols, 4, true, 0, 0, UNDEF_ASSOC);
		XxY_set (&type_nameXrule, name, rule, &couple);

		/* <STMT> = with <RECORDTYPE_VAR> do @20 <STMT> ";" @21 ; */
		if (action_20 == 0)
		{
		    symbols [0] = action_20 = new_nt ();
		    rule = add_rule (symbols, 0, false, 0, 0, UNDEF_ASSOC);
		    SXDG.rule_to_action [rule] = -20;
		}

		if (action_21 == 0)
		{
		    symbols [0] = action_21 = new_nt ();
		    rule = add_rule (symbols, 0, false, 0, 0, UNDEF_ASSOC);
		    SXDG.rule_to_action [rule] = -21;
		}

		symbols [0] = STMT;
		symbols [1] = -KW_WITH;
		symbols [2] = recordtype_lhs;
		symbols [3] = -KW_DO;
		symbols [4] = action_20;
		symbols [5] = STMT;
		symbols [6] = action_21;
		rule = add_rule (symbols, 6, true, 0, 0, UNDEF_ASSOC);
		XxY_set (&type_nameXrule, name, rule, &couple);
	    }

	    return SXANY_BOOL; 

/* @20 et @21 doivent-ils etre mis dans SXDO/SXUNDO ?? */
	case 20: /* <STMT> = with <RECORDTYPE_VAR> do @20 <STMT> ";" @21 ; */
	    /* @20 : Pour toute regle "lhs -> <RECORDTYPE_VAR> . rhs" generee par le traitement
	       de <OBJECT_DECL+> a l'interieur d'un record generer la re`gle "lhs -> rhs". */
	    /* Soit name l'entree de la table des symboles du recordtype associe'
	       a <RECORDTYPE_VAR>. */
	    {
		int rhs, lhs, i;

		SS_open (rule_stack);
		SS_open (rule_attr_stack);
		SS_open (deactivate_stack);
		SS_open (reactivate_stack);

		if (SXDG.lgt1 > sem_stack_size)
		    sem_stack = (struct sem_stack*) sxrealloc (sem_stack,
							       (sem_stack_size = SXDG.lgt1) + 1,
							       sizeof (struct sem_stack));
	    
		rule = sem_stack [SXSTACKnewtop () - 2].rule;
		lhs = XxY_X (SXDG.P, rule); 

		XxY_Xforeach (recordtype_lhsXrule, lhs, x)
		{
		    rule = XxY_Y (recordtype_lhsXrule, x);
		    symbols [0] = XxY_X (SXDG.P, rule); /* lhs */
		    rhs = XxY_Y (SXDG.P, rule);
		    rhs = XxY_Y (SXDG.rhs, rhs); /* <RECORDTYPE_VAR> */
		    rhs = XxY_Y (SXDG.rhs, rhs); /* . */
		    i = 0;

		    while (rhs != 0)
		    {
			/* symbols ne peut pas deborder, rule a deja ete genere. */
			symbols [++i] = XxY_X (SXDG.rhs, rhs);
			rhs = XxY_Y (SXDG.rhs, rhs);
		    }

		    rule = add_rule (symbols, i, true, 0, 0, UNDEF_ASSOC);
		}
	    }

	    return SXANY_BOOL; 

	case 21: /* <STMT> = with <RECORDTYPE_VAR> do @20 <STMT> ";" @21 ; */
	    /* @21 : Supprimer les re`gles generees par @20. */    
	    rules_at_block_exit (true);

	    return SXANY_BOOL; 

	case 22:
	    /* No 22 est utilise' ds semact, pourrait repasser ici. */
	    break;

	case 23: /* <TYPE_DECL> = record @19 %id 
		                     { @23
				        <OBJECT_DECL+>
				     } @24 ";" ; */
	    is_record_body = true;
	    block_level++;
	    SS_push (block_stack, current_block_nb);
	    current_block_nb = ++block_nb;
	    sxsymbol_table_open (symbol_table, block_level);

	    return SXANY_BOOL; 

	case 24: /* <TYPE_DECL> = record @19 %id 
		                     { @23
				        <OBJECT_DECL+>
				     } @24 ";" ; */

	    is_record_body = false;;

	    sxsymbol_table_foreach_scope (symbol_table, block_level, name)
	    {
		XxY_Xforeach (var_nameXrule, name, x)
		    XxY_erase (var_nameXrule, x);
	    }

	    sxsymbol_table_close (&symbol_table, block_level, true);
	    block_level--;
	    current_block_nb = SS_pop (block_stack);

	    return SXANY_BOOL; 

	default:
	    break;
	}

	break;

    case SXDO:
	/* Sur la sous-chaine : "... { goto ..." on se trouve dans l'etat s apres transition
	   sur {. Dans cet etat, une des actions possibles est la reduction du vide vers
	   la parsact @1 (entree de bloc), qui est justement l'action qui genere l'instruction
	   goto ...
	   Meme chose sur : "... { A : ..."
	   On est dans le cas SXDO/SXUNDO. En fait comme c'est la seule action possible, on pourrait
	   se contenter de faire SXDO (sans SXUNDO) et de ne rien faire sur SXACTION (@1).
	   Pour tester, on fait le cas general. */
	/* On pourrait aussi tester si on est ds ce cas 
	   look_ahead (ptok_no) == goto ||
	   look_ahead (ptok_no) == id && look_ahead (ptok_no+1) == :
	   et generer l'instruction correspondante (si il n'y en a pas) qui serait invalidee
	   ds le SXUNDO. */
	if (act_no == 1)
	    ACTION_1 ();

	return SXANY_BOOL;

    case SXUNDO:
	if (act_no == 1)
	{
	    ACTION_2 (false);
	    block_nb--;
	}

	return SXANY_BOOL;

    case SXPREDICATE:
	/* Peut-etre un peu simple !!! */
	if (act_no < -1)
	    SS_push (SXDG.prdct_stack, -SXGET_TOKEN (sxplocals.ptok_no).string_table_entry);

	return act_no < -1;

    case SXCLOSE:
	sxsymbol_table_free (&symbol_table);
	sxfree (st_to_attr);
	sxsymbol_table_free (&type_table);
	sxfree (tt_to_attr);
	SS_free (rule_stack);
	SS_free (rule_attr_stack);
	SS_free (deactivate_stack);
	SS_free (block_stack);
	SS_free (reactivate_stack);
	sxfree (rule_to_attr);
	SS_free (xtnt_stack);
	sxfree (symbols);
	XxY_free (&listof_hd);
	XxY_free (&labels_hd);
	XxY_free (&recordtype_lhsXrule);
	XxY_free (&var_nameXrule);
	XxY_free (&type_nameXrule);
	XxY_free (&type_steXblock_nb);
	sxfree (type_steXblock_nb_to_attr);
	sxfree (listof_to_attr);
	sxfree (sem_stack);
	return SXANY_BOOL;

    default:
	break;
    }

    fputs ("The function \"sxparser_action\" for lazy is out of date with respect to its specification.\n",
	   sxstderr);
    abort ();

}


bool L_semact (what, act_no)
    int		what, act_no;
{
    int rule, lhs, action_nb;

    switch (what) {
    case SXOPEN:
        sxdp_G0 ();

	cur_nt = XxY_top (SXDG.xnt);

	for (rule = 1; rule <= XxY_top (SXDG.P); rule++)
	{
	    if ((action_nb = SXDG.rule_to_action [rule]) > 0)
	    {
		/* Provisoire, on stocke le code des parties gauches interessantes.
		   Plus tard, obtenu par une extension de tdef aux non-terminaux. */
		lhs = XxY_X (SXDG.P, rule);

		switch (action_nb) {
		case 1:
		    INT_LHS = lhs;
		    break;
	    
		case 2:
		    BOOL_LHS = lhs;
		    break;

		case 3:
		    INT_EXPR = lhs;
		    break;
	    
		case 4:
		    BOOL_EXPR = lhs;
		    break;
	    
		case 5:
		    BASIC_TYPE_NAME = lhs;
		    break;
	    
		case 6:
		    STMT = lhs;
		    break;
	    
		case 7:
		    INT_CONST = lhs;
		    break;
	    
		case 8:
		    BOOL_CONST = lhs;
		    break;
	    
		case 9:
		    INT_FUN = lhs;
		    break;
	    
		case 10:
		    BOOL_FUN = lhs;
		    break;

		case 11:
		    INT_VAR = lhs;
		    break;
	    
		case 12:
		    BOOL_VAR = lhs;
		    break;
	    
		case 13:
		    INT_PRIMARY = lhs;
		    break;
	    
		case 14:
		    BOOL_PRIMARY = lhs;
		    break;
	    
		case 15:
		    LABEL = lhs;
		    break;
	    
		default:
		    break;
		}
	    }
	}
	
	return true;

    case SXFINAL:
	return true;

    case SXSEMPASS:
	return false;

    case SXINIT:

        {
	    /* La premiere passe sur le texte source est lancee d'ici. */
	    int				last_tok_nb;
	    static struct sxparse_mode	
		first_pass_mode = {SXPARSER, SXWITHOUT_ERROR, 0, 0, 0, false, true,
				       false, true, true, true};
/*
  new_mode.mode = SXPARSER; C'est un parser
  new_mode.kind = SXWITHOUT_ERROR; Pas de traitement d'erreur
  new_mode.local_errors_nb = 0; Decompte du nombre de corrections.
  new_mode.global_errors_nb = 0; Decompte du nombre de rattrapage global.
  new_mode.look_back = 0; On conserve tous les tokens
  new_mode.is_prefixe = false; On parse tout le source
  new_mode.is_silent = true; Pas de message d'erreur
  new_mode.with_semact = false; On inhibe les semact
  new_mode.with_parsact = true; On execute les parsact
  new_mode.with_parsprdct = true; On execute les parsprdct
  new_mode.with_do_undo = true; Pourquoi pas
*/

	    sxparse_in_la (FIRST_PASS, 0, &last_tok_nb, &first_pass_mode);

	    current_block_nb = block_nb = 0;
	    is_record_body = false;
	    gen_labels (); /* Du (dummy) bloc externe */
	}

	return true;

    case SXACTION:
	/* On memorise le numero de reduction */
	/* Utilise' ds les "with" pour recuperer, apartir du non-terminal en pg
	   les regles generee lors de la declaration du corps d'un record (voir
	   "recordtype_lhsXrule" et pour l'action 22. */
	if (SXDG.lgt1 > sem_stack_size)
	    sem_stack = (struct sem_stack*) sxrealloc (sem_stack,
						       (sem_stack_size = SXDG.lgt1) + 1,
						       sizeof (struct sem_stack));

	sem_stack [SXSTACKnewtop ()].rule = sxpglobals.reduce;

#if 0
	switch (act_no) {
	case 22: /* <LABEL> = %label : ; 22 */
	    /* On aurait pu en faire une parsact. */
	    if (SXDG.lgt1 > sem_stack_size)
		sem_stack = (struct sem_stack*) sxrealloc (sem_stack,
							   (sem_stack_size = SXDG.lgt1) + 1,
							   sizeof (struct sem_stack));

	    sem_stack [SXSTACKnewtop ()].rule = sxpglobals.reduce;
	    return true; 

	default:
	    /* Ceux qui servent au stockage du nom des LHS. */
	    return true;
	}
#endif

	return true;


    case SXCLOSE:
	return true;

    default:
	break;
    }

    fputs ("The function \"L_semact\" is out of date with respect to its specification.\n", sxstderr);
    abort ();

}
