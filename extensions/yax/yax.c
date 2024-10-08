/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

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

#include "sxunix.h"
#include "B_tables.h"
#include "yax_vars.h"
#include "put_edit.h"

char WHAT_YAX[] = "@(#)SYNTAX - $Id: yax.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

static char	ME [] = "yax";
static SXINT	nb_tnt;
static SXINT	ste_max_attr;
extern SXTABLES	bnf_tables;
extern SXTABLES	dec_tables;
extern SXTABLES	def_tables;

#include "sem_by.h"

extern void	put_release (void);




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

static void	sature (void)
{
    /* appelee lors de la saturation des tables */
/* on multiplie par deux leur taille et on continue */

    SXINT	i, old_size;

    tbl_lgth = 2 * (old_size = tbl_lgth);

    if (sxverbosep) {
	fprintf (sxtty, "%s: Tables of size %ld overflowed: reallocation with size %ld.\n", ME, (SXINT) old_size, (SXINT) tbl_lgth);
    }

    attr_to_ste = (SXINT*) sxrealloc (attr_to_ste - NBTERM, tbl_lgth + NBTERM, sizeof (SXINT)) + NBTERM;
    attr_nt = (char**) sxrealloc (attr_nt, tbl_lgth + 1, sizeof (SXINT));

    for (i = old_size + 1; i <= tbl_lgth; i++) {
	attr_nt [i] = (char*) sxcalloc (W.ntmax + 1, sizeof (char));
    }

    ste_to_attr = (SXINT*) sxrealloc (ste_to_attr, 2 * tbl_lgth + 1, sizeof (SXINT));

    for (i = 2 * old_size + 1; i <= 2 * tbl_lgth; i++) {
	ste_to_attr [i] = 0;
    }
}




/* --------------------------------------------------------- */

SXINT	placer_attr (SXINT ste)
{

/* place un attribut dans attr_to_ste pendant les declarations
   si il y est deja, c'est une double declaration ==> erreur */

    if (ste <= nb_tnt || ste <= ste_max_attr) {
	if (ste != 0) {
	    sxerror (terminal_token->source_index,
		     bnf_tables.err_titles [2][0],
		     "%s%s.",
		     bnf_tables.err_titles [2]+1,
		     ste <= nb_tnt ?
		     "An attribute name must be different from the name of a terminal symbol" :
		     "This attribute has already been declared");
	    is_err = true;
	}

	return 0;		/* attr bidon */
    }

    if (++max_attr >= tbl_lgth)
	sature ();

    return ste_to_attr [(attr_to_ste [max_attr] = ste_max_attr = ste) - nb_tnt] = max_attr;
}




/* --------------------------------------------------------- */

SXINT	chercher_attr (SXINT ste)
{
    SXINT	entree;

    if (ste <= nb_tnt) {
	return placer_attr (ste);
    }

    if ((entree = ste_to_attr [ste - nb_tnt]) == 0) {
	sxerror (terminal_token->source_index,
		 bnf_tables.err_titles [2][0],
		 "%sUndeclared attribute.",
		 bnf_tables.err_titles [2]+1);
	is_err = true;
    }

    return entree;
}




/* --------------------------------------------------------- */

SXINT	chercher_nt (SXINT ste)
{
    SXINT	entree;

    if (ste > nb_tnt || ste == 0 || (entree = ste_to_tnt [ste]) < 0) {
	if (ste != 0) {
	    sxerror (terminal_token->source_index,
		     bnf_tables.err_titles [2][0],
		     "%sUnknown non-terminal symbol.",
		     bnf_tables.err_titles [2]+1);
	    is_err = true;
	}

	return W.ntmax;
    }

    return entree;
}




/* --------------------------------------------------------- */

static bool	less_nt (SXINT i, SXINT j)
{
    char	*k1, *k2;

    k1 = sxstrget (nt_to_ste [i]);
    k2 = sxstrget (nt_to_ste [j]);

    while (*k1 == *k2)
	k1++, k2++;

    return *k1 < *k2;
}



void	yax_lo (void)
{
    SXINT	i, j, k;
    SXINT	*sort_symbols;

    if (is_sem_stats) {
	if (is_source && !IS_ERROR) {
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

	    sort_symbols = (SXINT*) sxalloc (W.ntmax + 1, sizeof (SXINT));

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

void	yax_free (void)
{
    SXINT	i;


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

static void	mise_en_place (void)
{
    SXINT	ste;
    SXINT	i;

/* terminal et non-terminal bidons pour les cas d'erreur */

    SXINT	bt_ste, bnt_ste;

    bnt_ste = sxstrsave ("<>>"), W.ntmax++;
    bt_ste = sxstrsave ("\377"), W.tmax--;

/*--------------------------------*/
/* allocation des tableaux de YAX */
/*--------------------------------*/

/* longueur constante : pas de reallocation a craindre */

    nt_to_ste = (SXINT*) sxalloc (W.ntmax + 1, sizeof (SXINT));
    t_to_ste = (SXINT*) sxalloc (-W.tmax + 1, sizeof (SXINT));

/* string table entry du dernier element entre, donc du terminal bidon */

    nb_tnt = bt_ste;

/* longueur variable */

    attr_to_ste = (SXINT*) sxalloc (tbl_lgth + NBTERM, sizeof (SXINT)) + NBTERM;
    /* ou NBTERM donne le nombre d'attributs terminaux (sxtoken ...) */

    attr_nt = (char**) sxalloc (tbl_lgth + 1, sizeof (char*));

    for (i = 0; i <= tbl_lgth; i++)
	attr_nt [i] = (char*) sxcalloc (W.ntmax + 1, sizeof (char));

    ste_to_tnt = (SXINT*) sxcalloc (nb_tnt + 1, sizeof (SXINT));
    ste_to_attr = (SXINT*) sxcalloc (2 * tbl_lgth + 1, sizeof (SXINT));


/*-----------------------------*/
/* fin allocation des tableaux */
/*-----------------------------*/

/* Construction des tableaux nt_to_ste et ste_to_tnt */

    /* non-terminal bidon pour les cas d'erreur */
    ste_to_tnt [nt_to_ste [W.ntmax] = bnt_ste] = W.ntmax;

    /* On recherche dans la string_table les non-terminaux */

    {
	SXINT	xnt;

	for (xnt = W.ntmax - 1; xnt >= 1; xnt--) {
	    ste_to_tnt [nt_to_ste [xnt] = sxstrretrieve (nt_string + adr [xnt])] = xnt;
	}
    }


/* Construction des tableaux t_to_ste et ste_to_tnt */

    /* terminal bidon pour les cas d'erreur */
    ste_to_tnt [t_to_ste [-W.tmax] = bt_ste] = W.tmax;

    /* On recherche dans la string_table les terminaux */

    {
	SXINT	xt;

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

bool		yax_sem (void)
{
    struct {
	struct sxsvar	sxsvar;
	struct sxplocals	sxplocals;
    }	bnf /* pour sauver les tables de bnf */ ;

    mise_en_place ();

/* Reinitialisation du source   */

    rewind (sxsrcmngr.infile);
    sxsrc_mngr (SXINIT, sxsrcmngr.infile, sxsrcmngr.source_coord.file_name);
    /* On reutilise la string table de bnf */

/* on sauve ... */
    bnf.sxsvar = sxsvar;
    bnf.sxplocals = sxplocals;
    W.sem_kind = sem_by_action;
    is_err = false;
    put_edit_initialize (sxstdout);
    put_release ();

    if (sxverbosep)
	fputs ("   Attributes Declaration\n", sxtty);

    syntax (SXOPEN, &dec_tables);
    syntax (SXACTION, &dec_tables);
    syntax (SXCLOSE, &dec_tables);

/* transition entre dec et def pour le source_manager */

    sxsrcpush (EOF, "\n", sxsrcmngr.source_coord);

/* initialisations pour def */

    nb_sem_rules = 0;
    nb_definitions = 0;
    nb_occurrences = 0;

    if (sxverbosep)
	fputs ("   Attributes Definition\n", sxtty);

    syntax (SXOPEN, &def_tables);
    syntax (SXACTION, &def_tables);
    syntax (SXCLOSE, &def_tables);

/* on elimine les non-terminal et terminal bidons crees pour les cas d'erreur */

    W.ntmax--, W.tmax++;

/* on restaure ...*/

    sxsvar = bnf.sxsvar;
    sxplocals = bnf.sxplocals;
    sxsrc_mngr (SXFINAL);
    return !is_err;
}
