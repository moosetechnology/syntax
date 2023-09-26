/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */


/* --------------------------------------------------------
   |							  |
   |                     Y  A  X		          |
   |					        	  |
   |          Constructeur pour les Grammaires	          |
   |               Attribuees a la yacc		   	  |
   |					          	  |
   |	       Codage par  Bernard Lorho                  |
   |           Reecritures de Philippe Deschamp           |
   |					                  |
   |------------------------------------------------------|
   |					        	  |
   |	Le traitement se decompose en 2 etapes :          |
   |					        	  |
   |   1) passage sur la grammaire attribuee de BNF       |
   |	qui traite les regles de grammaire uniquement     |
   |					          	  |
   |   2) passage de ce programme qui traite le	          |
   |      complementaire, c'est a dire, les attributs     |
   |					         	  |
   |------------------------------------------------------|
   |        Analyseur et Attributs pour C	          |
   -------------------------------------------------------- */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030520 14:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 20020502 (phd)	Correction d'un petit pb d'allocation dynamique	*/
/************************************************************************/
/* 23-06-87 (BL&pb):	Les NT ont deja les "<>".			*/
/************************************************************************/
/* 30-04-87 (phd):	Amelioration (finale?) de yax_lo ().		*/
/*			Creation de la procedure mise_en_place		*/
/************************************************************************/
/* 28-04-87 (phd):	Correction de l'initialisation de max_attr!	*/
/*			Debut d'amelioration de yax_lo ().		*/
/************************************************************************/
/* 22-04-87 (phd):	Definition de l'attribut terminal "sxtoken"	*/
/************************************************************************/
/* 1*-04-87 (phd):	Reecritures, ameliorations et optimisations	*/
/************************************************************************/
/* 01/04/87 (bl):	Adaptation a partir de la version 1 de tabc	*/
/************************************************************************/

#include "sxunix.h"
#include "B_tables.h"
#include "yax_vars.h"
#include "put_edit.h"
char WHAT_YAX[] = "@(#)SYNTAX - $Id: yax.c 580 2007-05-24 12:45:54Z rlacroix $" WHAT_DEBUG;


static char	ME [] = "yax";
static int	nb_tnt;
static int	ste_max_attr;
extern struct sxtables	bnf_tables;
extern struct sxtables	dec_tables;
extern struct sxtables	def_tables;

#include "sem_by.h"

extern VOID	put_release ();




#define	        NBTERM	        1/* nombre d'attributs de terminaux */
/*
 	|---------------------------------------------------------------|
	|								|
	|   TABLES UTILISEES						|
	|								|
	|	    Index        Nom des tables	   			|
	|								|
	|            xprod        reduc, prolon, lgprod, tnbt		|
	|            xnt          nt_to_ste, npg, npd			|
	|            xt           t_to_ste				|
	|            indpro       lispro				|
	|            max_attr     attr_to_ste			        |
	|								|
	|---------------------------------------------------------------| */

/* 	|---------------------------------------------------------------|
	|								|
	|  S T R U C T U R E   D E    D O N N E E S   U T I L I S E E   |
	|								|
	|   reduc   prolon   lgprod   tnbt      			|
	|   -----   -----     -----   -----	  			|
	|   |   |   |   |     |   |   |   |      			|
	|   |   |   |   |     |   |   |   |      			|
	|   | *-|-| | * |     | * |   |   |      			|
	|   |   | | | | |     | | |   |   |      			|
	|   |   | | | | |     | | |   |   |      			|
	|   ----- | --|--     --|--   -----	  			|
	|         |   |         |					|
	|         |   |         |					|
	|         |   |         |					|
	|         |   |         |---------------------|      lispro	|
	|         |   |-----------------------------| |      ------	|
	|         |<------------------------------| |-|----->|    |	|
	|nt_to_ste| npg   npd       t_to_ste      |   |    { |    |	|
	|   ----- | ---   ---         -----       |   |--->{ |    |	|
	|   |   | | | |   | |         |   |       |        { |    |	|
	|   |   <-| | |   | |         |   |       |----------| >0 |	|
	|   |   |   | |   | |         |   |                  |    |	|
	|   | * |   | |   | |         | * |<-----------------| <0 |	|
	|   --|--   ---   ---         --|--                  ------	|
	|     |-> string_table <--------|				|
	|								|
	|	     attr_to_ste type_attr   defini   			|
	|	        ------    -------   -------			|
	|		|    |    |     |   |     |			|
	|		|    |<-| |     |   |     |			|
	|		| *  |  | |     |   |     |			|
	|		| |  |  | |     |   |     |			|
	|		| |  |  | |     |   |     |			|
	|		--|---  | -------   -------			|
	|		  |->string_table				|
	|								|
	|---------------------------------------------------------------| */
/* ------------------------------------------------------------------------
   |  Procedures de placement ,  de recherche de symboles dans les tables |
   |               et de sortie du C	                                  |
   ------------------------------------------------------------------------ */

static VOID	sature ()
{
    /* appelee lors de la saturation des tables */
/* on multiplie par deux leur taille et on continue */

    register int	i, old_size;

    tbl_lgth = 2 * (old_size = tbl_lgth);

    if (sxverbosep) {
	fprintf (sxtty, "%s: Tables of size %d overflowed: reallocation with size %d.\n", ME, old_size, tbl_lgth);
    }

    attr_to_ste = (int*) sxrealloc (attr_to_ste - NBTERM, tbl_lgth + NBTERM, sizeof (int)) + NBTERM;
    attr_nt = (char**) sxrealloc (attr_nt, tbl_lgth + 1, sizeof (int));

    for (i = old_size + 1; i <= tbl_lgth; i++) {
	attr_nt [i] = (char*) sxcalloc (W.ntmax + 1, sizeof (char));
    }

    ste_to_attr = (int*) sxrealloc (ste_to_attr, 2 * tbl_lgth + 1, sizeof (int));

    for (i = 2 * old_size + 1; i <= 2 * tbl_lgth; i++) {
	ste_to_attr [i] = 0;
    }
}




/* --------------------------------------------------------- */

int	placer_attr (ste)
    register int	ste;
{

/* place un attribut dans attr_to_ste pendant les declarations
   si il y est deja, c'est une double declaration ==> erreur */

    if (ste <= nb_tnt || ste <= ste_max_attr) {
	if (ste != 0) {
	    sxput_error (terminal_token->source_index, "%s%s.", bnf_tables.err_titles [2], ste <= nb_tnt ?
			 "An attribute name must be different from the name of a terminal symbol" :
			 "This attribute has already been declared");
	    is_err = TRUE;
	}

	return 0;		/* attr bidon */
    }

    if (++max_attr >= tbl_lgth)
	sature ();

    return ste_to_attr [(attr_to_ste [max_attr] = ste_max_attr = ste) - nb_tnt] = max_attr;
}




/* --------------------------------------------------------- */

int	chercher_attr (ste)
    int		ste;
{
    register int	entree;

    if (ste <= nb_tnt) {
	return placer_attr (ste);
    }

    if ((entree = ste_to_attr [ste - nb_tnt]) == 0) {
	sxput_error (terminal_token->source_index, "%sUndeclared attribute.", bnf_tables.err_titles [2]);
	is_err = TRUE;
    }

    return entree;
}




/* --------------------------------------------------------- */

int	chercher_nt (ste)
    int		ste;
{
    register int	entree;

    if (ste > nb_tnt || ste == 0 || (entree = ste_to_tnt [ste]) < 0) {
	if (ste != 0) {
	    sxput_error (terminal_token->source_index, "%sUnknown non-terminal symbol.", bnf_tables.err_titles [2]);
	    is_err = TRUE;
	}

	return W.ntmax;
    }

    return entree;
}




/* --------------------------------------------------------- */

static BOOLEAN	less_nt (i, j)
    int		i, j;
{
    register char	*k1, *k2;

    k1 = sxstrget (nt_to_ste [i]);
    k2 = sxstrget (nt_to_ste [j]);

    while (*k1 == *k2)
	k1++, k2++;

    return *k1 < *k2;
}




VOID	yax_lo ()
{
    register int	i, j, k;
    register int	*sort_symbols;

    if (is_sem_stats) {
	if (is_source && !IS_ERROR) {
	    put_edit_ff ();
	    put_edit_ap ("Some information about your attribute grammar:");
	    put_edit_ap ("----------------------------------------------");
	    put_edit_nl (1);
	    put_edit_apnnl ("# of productions");
	    put_edit_nb (30, W.nbpro);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of non-terminals");
	    put_edit_nb (30, W.ntmax);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of terminals");
	    put_edit_nb (30, -W.tmax);
	    put_edit_nl (1);
	    put_edit_apnnl ("Size of the grammar");
	    put_edit_nb (30, W.indpro - W.nbpro);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of attributes");
	    put_edit_nb (30, max_attr);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of attribute definitions");
	    put_edit_nb (30, nb_definitions);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of attribute occurrences");
	    put_edit_nb (30, nb_occurrences);
	    put_edit_nl (1);
	    put_edit_apnnl ("# of semantic rules");
	    put_edit_nb (30, nb_sem_rules);
	    put_edit_nl (3);
	    put_edit_apnnl ("ATTRIBUTES");

	    for (i = 1; i <= max_attr; i++) {
		put_edit_fnb (1, 4, i);
		put_edit_nnl (8, "$<");
		put_edit_ap (sxstrget (attr_to_ste [i]) + 1);
	    }

	    put_edit_nl (2);

/* tri non-terminaux */

	    sort_symbols = (int*) sxalloc (W.ntmax + 1, sizeof (int));

	    for (i = W.ntmax; i >= 1; i--)
		sort_symbols [i] = i;

	    sort_by_tree (sort_symbols, 1, W.ntmax, less_nt);
	    put_edit_ap ("NON-TERMINAL symbols associated with their ATTRIBUTES");

	    for (j = 1; j <= W.ntmax; j++) {
		k = sort_symbols [j];
		put_edit_nnl (1, sxstrget (nt_to_ste [k]));

		for (i = 1; i <= max_attr; i++) {
		    if (attr_nt [i] [k]) {
			put_edit_nnl (M_nt + 010 * (i - 1), "$<");
			put_edit_ap (sxstrget (attr_to_ste [i]) + 1);
			break;
		    }
		}
	    }

	    sxfree (sort_symbols);
	}
    }
}




/*  ---------------------------------------- */

VOID	yax_free ()
{
    register int	i;


/* liberation dans l'ordre inverse d'allocation */

    sxfree (ste_to_attr);
    sxfree (ste_to_tnt);

    for (i = tbl_lgth; i >= 0; i--)
	sxfree (attr_nt [i]);

    sxfree (attr_nt);
    sxfree (attr_to_ste - NBTERM);
    sxfree (t_to_ste);
    sxfree (nt_to_ste);
}




/* ---------------------------------------- */

static VOID	mise_en_place ()
{
    register int	ste;
    register int	i;

/* terminal et non-terminal bidons pour les cas d'erreur */

    register int	bt_ste, bnt_ste;

    bnt_ste = sxstrsave ("<>>"), W.ntmax++;
    bt_ste = sxstrsave ("\377"), W.tmax--;

/*--------------------------------*/
/* allocation des tableaux de YAX */
/*--------------------------------*/

/* longueur constante : pas de reallocation a craindre */

    nt_to_ste = (int*) sxalloc (W.ntmax + 1, sizeof (int));
    t_to_ste = (int*) sxalloc (-W.tmax + 1, sizeof (int));

/* string table entry du dernier element entre, donc du terminal bidon */

    nb_tnt = bt_ste;

/* longueur variable */

    attr_to_ste = (int*) sxalloc (tbl_lgth + NBTERM, sizeof (int)) + NBTERM;
    /* ou NBTERM donne le nombre d'attributs terminaux (sxtoken ...) */

    attr_nt = (char**) sxalloc (tbl_lgth + 1, sizeof (char*));

    for (i = 0; i <= tbl_lgth; i++)
	attr_nt [i] = (char*) sxcalloc (W.ntmax + 1, sizeof (char));

    ste_to_tnt = (int*) sxcalloc (nb_tnt + 1, sizeof (int));
    ste_to_attr = (int*) sxcalloc (2 * tbl_lgth + 1, sizeof (int));


/*-----------------------------*/
/* fin allocation des tableaux */
/*-----------------------------*/

/* Construction des tableaux nt_to_ste et ste_to_tnt */

    /* non-terminal bidon pour les cas d'erreur */
    ste_to_tnt [nt_to_ste [W.ntmax] = bnt_ste] = W.ntmax;

    /* On recherche dans la string_table les non-terminaux */

    {
	register int	xnt;

	for (xnt = W.ntmax - 1; xnt >= 1; xnt--) {
	    ste_to_tnt [nt_to_ste [xnt] = sxstrretrieve (nt_string + adr [xnt])] = xnt;
	}
    }


/* Construction des tableaux t_to_ste et ste_to_tnt */

    /* terminal bidon pour les cas d'erreur */
    ste_to_tnt [t_to_ste [-W.tmax] = bt_ste] = W.tmax;

    /* On recherche dans la string_table les terminaux */

    {
	register int	xt;

	for (xt = -W.tmax - 1; xt >= 1; xt--) {
	    ste_to_tnt [t_to_ste [xt] = sxstrretrieve (t_string + adr [-xt])] = -xt;
	}
    }


/* Construction des tableaux ste_to_attr et attr_to_ste */

    /* on cree les attributs de terminaux */
    ste_to_attr [(attr_to_ste [-1] = ste = sxstrsave ("$sxtoken>")) - nb_tnt] = -1;

    /* on cree un attribut non-terminal bidon pour le traitement d'erreur */
    ste_to_attr [(attr_to_ste [0] = ste = sxstrsave ("$$")) - nb_tnt] = 0;

    /* C'est tout... */
    ste_max_attr = ste;
    max_attr = 0;
}




/* ---------------------------------------- */

BOOLEAN		yax_sem ()
{
    struct {
	struct sxsvar	sxsvar;
	struct sxplocals	sxplocals;
    }	bnf /* pour sauver les tables de bnf */ ;

    mise_en_place ();

/* Reinitialisation du source   */

    rewind (sxsrcmngr.infile);
    sxsrc_mngr (INIT, sxsrcmngr.infile, sxsrcmngr.source_coord.file_name);
    /* On reutilise la string table de bnf */

/* on sauve ... */
    bnf.sxsvar = sxsvar;
    bnf.sxplocals = sxplocals;
    W.sem_kind = sem_by_action;
    is_err = FALSE;
    put_edit_initialize (sxstdout);
    put_release ();

    if (sxverbosep)
	fputs ("   Attributes Declaration\n", sxtty);

    syntax (OPEN, &dec_tables);
    syntax (ACTION, &dec_tables);
    syntax (CLOSE, &dec_tables);

/* transition entre dec et def pour le source_manager */

    sxsrcpush (EOF, "\n", sxsrcmngr.source_coord);

/* initialisations pour def */

    nb_sem_rules = 0;
    nb_definitions = 0;
    nb_occurrences = 0;

    if (sxverbosep)
	fputs ("   Attributes Definition\n", sxtty);

    syntax (OPEN, &def_tables);
    syntax (ACTION, &def_tables);
    syntax (CLOSE, &def_tables);

/* on elimine les non-terminal et terminal bidons crees pour les cas d'erreur */

    W.ntmax--, W.tmax++;

/* on restaure ...*/

    sxsvar = bnf.sxsvar;
    sxplocals = bnf.sxplocals;
    sxsrc_mngr (FINAL);
    return !is_err;
}
