
/* |------------------------------------------------------|
   |                                                      |
   |                                                      |
   | Copyright (c) 1987 by Institut National de Recherche |
   |                    en Informatique et en Automatique |
   |                                                      |
   |     Produit de l'Equipe Langages et Traducteurs.     |
   |                                                      |
   |------------------------------------------------------| */

/* --------------------------------------------------------
   |							  |
   |                   T  A  B  C		          |
   |					        	  |
   |          Constructeur pour les Grammaires	          |
   |        Attribuees avec Attributs Synthetises	  |
   |					          	  |
   |   Les Definitions et Calculs d'Attributs sont en C   |
   |					                  |
   |	       Codage par  Bernard Lorho                  |
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

/* 

   |----------------------------------------------------|
   |							|
   |               H I S T O R I Q U E                  |
   |         D E S   M O D I F I C A T I O N S          |
   |					         	|
   |----------------------------------------------------| */

/*--------------------------------------------------------------*/
/*   03/03/87: adaptation a partir de la version 2 de tabact	*/
/*   17/03/87: ajout de tabc_lo					*/
/*   24/03/87: correction de sature				*/
/*   13/05/87: correction de tabc_lo				*/
/*   20/05/87: terminal, non-terminal et attribut de meme nom	*/
/*   26/05/87: ajout de $ptoken, $pste				*/
/*   27/05/87: debut de la version bootstrapee			*/
/*   01/07/87: transport sur sun				*/
/*   13-08-87:	Ajout par PhD de la macro PhDadded1		*/
/*		Nettoyages et optimisations divers		*/
/*   20-08-87:  Modification par PhD du debut de la procedure	*/
/*		"sature", pour contourner une bogue du compilo	*/
/*		C Apollo...  					*/
/*		Modification du type de "type_attr"		*/
/*   21-08-87: Correction de deux bogues dans l'ecriture sur	*/
/*		la liste (notamment ecriture sur un fichier non	*/
/*		ouvert...), et autres joyeusetes. (phd)		*/
/*   01-09-87: Correction bugs dans dec et def quand mauvaise   */
/*		 orthographe de non-terminaux			*/
/*   02-09-87: Reorganisation du traitement des erreurs         */
/*   09-09-87: Suppression limitation dans ste_to               */
/*   16-09-87: Correction bug put_ste_to                        */
/*   07/10/87: detachement de tabc_lo et creation module	*/
/*   28/10/87: ajout de l'initialisation de M_at		*/
/*   19/07/88: mise a jour numero version			*/
/*   24/08/88: ajout #ifdef MSDOS				*/
/*--------------------------------------------------------------*/

#define PhDadded1 1 /* added to some allocations */
#include "sxunix.h"
#include "B_tables.h"
char WHAT_TABC[] = "@(#)SYNTAX - $Id: tabc.c 1186 2008-03-11 10:27:13Z rlacroix $" WHAT_DEBUG;

#ifdef	MSDOS
#include "tbc_vars.h"
#else
#include "tabc_vars.h"
#endif

#include "put_edit.h"
#include "sem_by.h"

FILE	*listing;
static char	ME [] = "tabc";
static char	release [] = "Tabc Release 6.0, January 17, 2007";
static char	quotes [] = "''''''''''''''''''''''''''''''";
static SXINT	size_ste_to, nb_tnt;
extern struct sxtables	bnf_tables, dec_tables, def_tables;
extern VOID	put_identite (SXINT nat1, SXINT nat2, SXINT pos);
extern VOID	put_release (char *);




#define	        NBTERM	        6
/* nombre d'attributs de terminaux */
#define		LG_MOD		256

#define	   UNUSED	   0
#define	   TERMINAL	   1
#define	   NON_TERMINAL	   2
#define	   ATTRIBUTE	   3

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
	|            max_attr     attr_to_ste, type_attr, defini        |
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



/* */
/* ------------------------------------------------------------------------
   |  Procedures de placement ,  de recherche de symboles dans les tables |
   |               et de sortie du C	                                  |
   ------------------------------------------------------------------------ */

static VOID	sature (void)
{
    /* appelee lors de la saturation des tables */
/* on multiplie par deux leur taille et on continue */

    SXINT	i, old_size, new_size;

    fprintf (sxtty, "%s: Tables of size %ld overflowed: reallocation with size %ld.\n", ME, (long)tbl_lgth, (long)(new_size = tbl_lgth *
	 2));
    old_size = tbl_lgth;
    tbl_lgth = new_size;
    attr_to_ste = (SXINT*) sxrealloc (attr_to_ste - NBTERM, new_size + NBTERM, sizeof (SXINT)) + NBTERM;
    defini = (SXINT*) sxrealloc (defini, new_size + 1, sizeof (SXINT));
    type_attr = (SXINT*) sxrealloc (type_attr, new_size + 1, sizeof (SXINT));

    for (i = old_size + 1; i <= new_size; i++) {
	type_attr [i] = 0;
    }

    attr_nt = (char**) sxrealloc (attr_nt, new_size + 1, sizeof (SXINT));

    for (i = old_size + 1; i <= new_size; i++) {
	attr_nt [i] = sxcalloc (W.ntmax + PhDadded1, sizeof (char));
    }
}




/* sature */


/* --------------------------------------------------------- */

static VOID	put_ste_to (SXINT ste, short code, short nature)
{
    struct ste_elem	*p1, *p2;
    SXINT	x;

    x = (ste % size_ste_to) + 1;


/* ste+1 ou le reste de la division par size_ste_to */

    if (ste_to [x].nature == UNUSED) {
	ste_to [x].code = code;
	ste_to [x].nature = nature;
	ste_to [x].stentry = ste;
    }
    else {
	/* allocation d'un nouvel element */
	p1 = (struct ste_elem*) sxalloc (1, sizeof (struct ste_elem));
	p1->code = code;
	p1->nature = nature;
	p1->stentry = ste;
	p1->next_elem = NULL;

	for (p2 = ste_to + x; (p2->next_elem) != NULL; p2 = (p2->next_elem))
	    ;


/* p2 pointe sur le dernier element de la liste */

	p2->next_elem = p1;
    }
}




/* put_ste_to */

/* --------------------------------------------------------- */

static short	get_ste_to (SXINT ste, short nature)
{
    struct ste_elem	*ptr;
    SXINT	x;

    x = (ste % size_ste_to) + 1;


/* ste+1 ou le reste de la division par size_ste_to */

    if (ste != ERROR_STE) {
	if (ste_to [x].nature != UNUSED)
	    for (ptr = ste_to + x; ptr != NULL; ptr = (ptr->next_elem)) {
		/* ptr pointe sur le prochain element de la liste */
		if ((ptr->nature == nature) && (ptr->stentry == ste))
		    return ptr->code; /* car trouve */
	    }
    }

    return 0; /* car pas trouve */
}




/* get_ste_to */

/* --------------------------------------------------------- */

SXINT	placer_attr (struct sxtoken ttok)
{

/*
   place un attribut dans attr_to_ste pendant les declarations
   si il y est deja, c'est une double declaration ==> warning
   et positionnement d'un flag d'erreur
*/

    SXINT	nt, ste;

    ste = ttok.string_table_entry;

    if (get_ste_to (ste, ATTRIBUTE) != 0) {
	sxput_error (ttok.source_index, "%sThis attribute has already been declared.", bnf_tables.err_titles [1]);
	is_err = TRUE;
	return 0;
    }

    max_attr++;

    if ((SXINT)max_attr >= tbl_lgth)
	sature ();

    attr_to_ste [max_attr] = ste;
    put_ste_to (ste, max_attr, ATTRIBUTE);

    for (nt = 1; nt <= W.ntmax; nt++)
	attr_nt [max_attr] [nt] = ' ';

    return max_attr;
}




/* placer_attr */

/* --------------------------------------------------------- */

SXINT	chercher_attr (SXINT ste)
{
    SXINT		code;

    code = get_ste_to (ste, ATTRIBUTE);

    if (code == 0) {
	sxput_error (terminal_token.source_index, "%sUndeclared attribute.", bnf_tables.err_titles [2]);
	is_err = TRUE;
	return 0;
    }

    return code;
}




/* chercher_attr */
    
/* --------------------------------------------------------- */

SXINT	chercher_nt (SXINT ste)
{
    SXINT		code;

    code = get_ste_to (ste, NON_TERMINAL);

    if (code == 0) {
	sxput_error (terminal_token.source_index, "%sUnknown non-terminal symbol.", bnf_tables.err_titles [2]);
	is_err = TRUE;
	return 0;
    }

    return code;
}




/* chercher_nt */

/* --------------------------------------------------------- */

SXINT	chercher_t (SXINT ste)
{
    SXINT		code;

    code = get_ste_to (ste, TERMINAL);

    if (code == 0) {
	/* pas trouve ou non-terminal */
	sxput_error (terminal_token.source_index, "%sUnknown terminal symbol.", bnf_tables.err_titles [2]);
	is_err = TRUE;
	return 0;
    }

    return code;
}




/* chercher_t */

/*   -------------------------------------------------------- */

SXINT	posprod (SXINT prodnum /* numero d'une production */,
		 SXINT tntno /* indice d'un nt ou d'un t */, 
		 SXINT quote /* nombre de quotes */ )

{
    SXINT	i, quote_bis, code, lgth;


/* rend la position dans la partie droite du truc decrit
   Erreur et message s'il n'existe pas et rend -1 */

    if ((quote == 0) && (WN [prodnum].reduc == tntno))
	return 0;

    quote_bis = (tntno == WN [prodnum].reduc) ? quote - 1 : quote;
    lgth = LGPROD (prodnum);

    for (i = 1; i <= lgth; i++) {
	code = WI [WN [prodnum].prolon + i - 1].lispro;
	code = (code < 0) ? XT_TO_T (code) : XNT_TO_NT (code);

	if (code == tntno) {
	    if (quote_bis == 0)
		return i;

	    quote_bis--;
	}
    }


/* pas trouve */

    sxput_error (terminal_token.source_index, "%sThis symbol does not exist in this rule.", bnf_tables.err_titles [2]);
    is_err = TRUE;
    return -1;
}




/* posprod */

/* --------------------------------------------------------- */

static VOID	print_ident (SXINT attrg, SXINT attrd, SXINT posd)
{
    SXINT	pg;
    char	*nt1, *nt2, *quotes_ptr;

    quotes_ptr = quotes + sizeof (quotes) / sizeof (quotes [0]) - 1;
    pg = WN [xprod].reduc;
    /* partie gauche */
    nt1 = sxstrget (nt_to_ste [pg]);

    if (posd == 0)
	nt2 = nt1;
    else {
	SXINT	i, nt, s;

	nt = WI [maxlis - posd].lispro;
	nt2 = sxstrget (nt_to_ste [nt = XNT_TO_NT (nt)]);

	if (nt == pg)
	    quotes_ptr--;

	for (i = maxlis - posd - 1; i >= prolnb; i--) {
	    if ((s = WI [i].lispro) > 0 && XNT_TO_NT (s) == nt)
		quotes_ptr--;
	}
    }

    sxput_error (rule_token.source_index, "%s%s(%s) = %s(%s%s)", bnf_tables.err_titles [0], sxstrget (attr_to_ste [attrg])
	 , nt1, sxstrget (attr_to_ste [attrd]), nt2, quotes_ptr);
}




/* print_ident */

/* --------------------------------------------------------- */

VOID	creer (SXINT attrg, SXINT attrd, SXINT posd)
    /* cree une identite */
{
    put_identite (attrg, attrd, posd);

    if (is_default)
	print_ident (attrg, attrd, posd);


/* ecriture sur le listing */

    nb_defaults++;
    nb_sem_rules++;
}




/* creer */

/* --------------------------------------------------------- */

VOID	creervide (SXINT attrg, SXINT attrd, SXINT posd)
    /* cree une identite vide */
{
    if (is_default)
	print_ident (attrg, attrd, posd);


/* ecriture sur le listing */

    nb_defaults++;
    nb_sem_rules++;
    nb_identities++;
}




/* creervide */

/* --------------------------------------------------------- */

VOID	creermilieu (SXINT attrg, SXINT attrd, SXINT posd)
{
    /* cree une identite avec un symbole situe au milieu de la regle */

    if (attr_nt [attrg] [nt_pg] == 's')
	creer (attrg, attrd, posd);
    else
	creervide (attrg, attrd, posd);
}




/* creermilieu */

/* --------------------------------------------------------- */

VOID	tilt (SXINT attrno, SXINT ntno, SXINT posp)
{
    /* pour crier quand il y a une erreur */

    SXINT	i, nbq = 0, s;

    for (i = 0; i <= posp - 1; i++)
	if ((s = WI [prolnb + i].lispro) > 0 && ntno == XNT_TO_NT (s))
	    nbq++;

    if (nbq > 0 && ntno != WN [xprod].reduc)
	nbq--;

    sxput_error (rule_token.source_index, "%s%s cannot determine the definition by default of %s(%s%s).", bnf_tables.
	 err_titles [2], ME, sxstrget (attr_to_ste [attrno]), sxstrget (nt_to_ste [ntno]), quotes + sizeof (quotes) /
	 sizeof (quotes [0]) - 1 - nbq);
    bident = FALSE;
    is_err = TRUE;
}




/* tilt */

/* ---------------------------------------- */

VOID	tabc_free (void)
{
    SXINT	i;
    struct ste_elem	*ptr;


/* liberation dans l'ordre inverse d'allocation */


    for (i = 0; i <= tbl_lgth; i++)
	sxfree (attr_nt [i]);

    sxfree (attr_nt);
    sxfree (type_attr);
    sxfree (defini);
    sxfree (attr_to_ste - NBTERM);

    for (i = 1; i <= size_ste_to; i++) {
	if (ste_to [i].next_elem != NULL) {
	    /* liberation d'un element */
	    for (ptr = ste_to [i].next_elem; ptr != NULL; ptr = (ptr->next_elem))
		sxfree (ptr);
	}
    }

    sxfree (ste_to);
    sxfree (t_to_ste);
    sxfree (nt_to_ste);
    sxfree (mod_ident);
    sxfree (modele);
}




/* tabc_free */

/* ---------------------------------------- */

static VOID	tabc_lo_init (void)
{
    char	lst_name [32];

    if (is_source && is_list) {
	if ((listing = sxfopen (strcat (strcpy (lst_name, prgentname), ".bn.l"), "a")) == NULL) {
	    fprintf (sxstderr, "%s: cannot open (append) ", ME);
	    sxperror (lst_name);
	    is_list = FALSE;
	}
	else
	    put_edit_initialize (listing);
    }
}




/* tabc_lo_init */

/* --------------------------------------------------------- */

BOOLEAN		tab_sem (void)
{
    SXINT	i, ste;
    SXINT       xnt, xt;

/* pour sauver les tables de bnf */

    struct {
	struct sxsvar	sxsvar;
	struct sxplocals	sxplocals;
    }	bnf;


/*---------------------------------*/
/* allocation des tableaux de tabc */
/*---------------------------------*/

/* longueur constante : pas de reallocation a craindre */

    modele = (char*) sxalloc (LG_MOD + 1, sizeof (char));
    mod_ident = (char*) sxalloc (LG_MOD + 1, sizeof (char));
    nt_to_ste = (SXINT*) sxalloc (W.ntmax + 1, sizeof (SXINT));
    t_to_ste = (SXINT*) sxalloc (-W.tmax + 1, sizeof (SXINT));

/* string table entry du dernier element entre */

    nb_tnt = sxstrmngr.top - 1;

/* longueur variable */

    size_ste_to = nb_tnt + tbl_lgth + NBTERM;
    ste_to = (struct ste_elem*) sxalloc (size_ste_to + 1, sizeof (struct ste_elem));

    for (i = 1; i <= size_ste_to; i++) {
	ste_to [i].nature = UNUSED;
	ste_to [i].next_elem = NULL;
    }

    attr_to_ste = (SXINT*) sxalloc (tbl_lgth + NBTERM, sizeof (SXINT)) + NBTERM;
    /* ou NBTERM donne le nombre d'attributs terminaux (ptext, ...) */

    defini = (SXINT*) sxalloc (tbl_lgth + 1, sizeof (SXINT));
    type_attr = (SXINT*) sxcalloc (tbl_lgth + 1, sizeof (SXINT));
    attr_nt = (char**) sxalloc (tbl_lgth + 1, sizeof (char*));

    for (i = 0; i <= tbl_lgth; i++)
	attr_nt [i] = (char*) sxalloc (W.ntmax + PhDadded1, sizeof (char));


/*-------------------------------------*/
/* fin allocation des tableaux de tabc */
/*-------------------------------------*/

/* Construction des tableaux nt_to_ste et ste_to */

/* On recherche dans la string_table les non-terminaux */

    for (xnt = 1; (SXINT)xnt <= W.ntmax; xnt++) {
	ste = sxstrretrieve (nt_string + adr [xnt]);
	nt_to_ste [xnt] = ste;
	put_ste_to (ste, xnt, NON_TERMINAL);
    }


/* On recherche dans la string_table les terminaux */

    for (xt = 1; (SXINT)xt <= -W.tmax; xt++) {
	ste = sxstrretrieve (t_string + adr [-xt]);
	t_to_ste [xt] = ste;
	put_ste_to (ste, -xt, TERMINAL);
    }


/* on cree les attributs de terminaux */

    ste = sxstrsave ("$ptext");
    attr_to_ste [-1] = ste;
    put_ste_to (ste, -1, ATTRIBUTE);
    ste = sxstrsave ("$pcode");
    attr_to_ste [-2] = ste;
    put_ste_to (ste, -2, ATTRIBUTE);
    ste = sxstrsave ("$skx");
    attr_to_ste [-3] = ste;
    put_ste_to (ste, -3, ATTRIBUTE);
    ste = sxstrsave ("$pcomment");
    attr_to_ste [-4] = ste;
    put_ste_to (ste, -4, ATTRIBUTE);
    ste = sxstrsave ("$pste");
    attr_to_ste [-5] = ste;
    put_ste_to (ste, -5, ATTRIBUTE);
    ste = sxstrsave ("$ptoken");
    attr_to_ste [-6] = ste;
    put_ste_to (ste, -6, ATTRIBUTE);

/* on cree un attribut bidon pour le traitement d'erreur */

    ste = sxstrsave ("$$");
    attr_to_ste [0] = ste;
    put_ste_to (ste, 0, ATTRIBUTE);

    for (i = 1; i <= W.ntmax; i++)
	attr_nt [1] [i] = ' ';


/*----------------------------------------------------------------*/

/*     I N I T I A L I S A T I O N S    */

/* Reinitialisation du source   */

    rewind (sxsrcmngr.infile);
    sxsrc_mngr (INIT, sxsrcmngr.infile, sxsrcmngr.source_coord.file_name);
    /* On reutilise la string table de bnf */

/* on sauve ... */
    bnf.sxsvar = sxsvar;
    bnf.sxplocals = sxplocals;
    W.sem_kind = sem_by_action;
    max_attr = 0;
    M_at = 0;
    /* longueur maximum d'un attribut */
    is_err = FALSE;
    tabc_lo_init ();
    put_release (release);

    if (sxverbosep)
	fputs ("\n   Attributes Declaration\n", sxtty);

    syntax (OPEN, &dec_tables);
    syntax (ACTION, &dec_tables);
    syntax (CLOSE, &dec_tables);

/* transition entre dec et def pour le source_manager */

    sxsrcpush (EOF, "\n", sxsrcmngr.source_coord);

/* initialisations pour def */

    nb_sem_rules = 0;
    nb_defaults = 0;
    nb_identities = 0;
    xprod = 0;
    nb_definitions = 0;
    nt_pg = ws_nbpro [1].reduc;
    is_empty = TRUE;
    /* a priori pas de semantique */
    is_ident = TRUE;
    /* initialisation bidon */
    mod_ident [0] = NUL;


/* et pas d'identite */

    if (sxverbosep)
	fputs ("   Attributes Definition\n", sxtty);

    syntax (OPEN, &def_tables);
    syntax (ACTION, &def_tables);
    syntax (CLOSE, &def_tables);

/* on restaure ...*/

    sxsvar = bnf.sxsvar;
    sxplocals = bnf.sxplocals;
    sxsrc_mngr (FINAL);
    return !is_err;
}
/* tab_sem */
