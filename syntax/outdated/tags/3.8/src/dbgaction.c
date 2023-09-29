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
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */



/* Actions bidon pour le debug de la nouvelle version de SYNTAX (TM) */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 05-02-93 10:00 (pb):		Marche arriere...			*/
/************************************************************************/
/* 27-01-93 15:25 (pb):		Adaptation a la nelle parse_stack	*/
/************************************************************************/
/* 14-12-87 18:10 (pb):		Suppression de l'ecriture de "er_re"   	*/
/************************************************************************/
/* 25-05-87 18:02 (phd):	Ajout de l'entree SEMPASS	   	*/
/************************************************************************/
/* 25-05-87 10:57 (phd):	Ajout de "gripe"			*/
/************************************************************************/
/* 31-03-87 14:10 (phd):	parsact => semact			*/
/************************************************************************/
/* 22-05-86 16:05 (phd):	Trace de la pile du parser		*/
/************************************************************************/
/* 22-05-86 15:22 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 22-05-86 15:22 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#ifndef lint
#ifdef	BUG
char	what_dbgaction [] = "@(#)dbgaction.d.c\t- SYNTAX [unix] - 5 Fevrier 1993";
#else
char	what_dbgaction [] = "@(#)dbgaction.c\t- SYNTAX [unix] - 5 Fevrier 1993";
#endif
#endif


# include "sxunix.h"


#ifdef	BUG

static VOID	outstack ()
{
    register int	index;
    register struct sxparstack	*item_ptr;
    struct sxtoken	*atok;

    for (index = STACKtop (); index >= STACKnewtop (); index--) {
	item_ptr = &(sxpglobals.parse_stack [index]);
	atok = &(item_ptr->token);
	printf ("\tstack [%d] = {\n", index);
	printf ("\t\ttoken.lahead = %d (\"%s\")\n", atok->lahead, sxttext (sxplocals.sxtables, atok->
	     lahead));
	printf ("\t\t      string_table_entry = %d (\"%s\")\n", atok->string_table_entry, sxstrget (atok->
	     string_table_entry));
	printf ("\t\t      source_index = {\"%s\", %lu, %u}\n", atok->source_index.file_name, atok->
	     source_index.line, atok->source_index.column);

	if (atok->comment == NULL)
	    puts ("\t\t      comment = NULL");
	else
	    printf ("\t\t      comment = \"%s\"\n", atok->comment);

	printf ("\t\tstate = %d\n", item_ptr->state);
	puts ("\t}");
    }
}




#else
#define outstack()
#endif


static SXVOID	gripe (whom)
    char	*whom;
{
    fprintf (sxstderr, "The function \"%s\" is out of date with respect to its specification.\n", whom);
    abort ();
}



int	dbgsemact (what_to_do, arg)
    int		what_to_do, arg;
{
    switch (what_to_do) {
    case ACTION:
	printf ("semact: action = %d, reduce = %d, top = %d, newtop = %d\n", arg, STACKreduce (), STACKtop (),
	     STACKnewtop ());
	outstack ();
	return 0;

    case SEMPASS:
	puts ("semact: SEMPASS");
	return 0;

    case ERROR:
	printf ("semact: error, top = %d, newtop = %d\n", STACKtop (), STACKnewtop ());
	outstack ();
	return 0;

    case OPEN:
	puts ("semact: OPEN");
	return 0;

    case CLOSE:
	puts ("semact: CLOSE");
	return 0;

    case INIT:
	puts ("semact: INIT");
	return 0;

    case FINAL:
	puts ("semact: FINAL");
	return 0;

    default:
	gripe ("semact");
	/* NOTREACHED */
    }
}



static VOID	print_scanner_state ()
{
    printf ("token string = \"%.*s\", state = %d,\n", sxsvar.lv.ts_lgth, sxsvar.lv_s.token_string, sxsvar.lv.
	 current_state_no);
    printf ("terminal token = {lahead = %d, entry =%d,\n", sxsvar.lv.terminal_token.lahead, sxsvar.lv.terminal_token.
	 string_table_entry);
    printf ("\tsource_index = \"%s\" [%u, %u],\n", sxsvar.lv.terminal_token.source_index.file_name, sxsvar.lv.
	 terminal_token.source_index.line, sxsvar.lv.terminal_token.source_index.column);

    if (sxsvar.lv.terminal_token.comment == NULL)
	puts ("\tno comment},");
    else
	printf ("\tcomment = \"%s\"},\n", sxsvar.lv.terminal_token.comment);

    printf ("source_class = %d.\n", sxsvar.lv.source_class);
}



int	dbgscanact (what_to_do, arg)
    int		what_to_do, arg;
{
    switch (what_to_do) {
    case ACTION:
	printf ("scanact: action = %d, ", arg);
	print_scanner_state ();
	return 0;

    case PREDICATE:
	printf ("scanact: predicate = %d, ", arg);
	print_scanner_state ();
	return 1;

    case OPEN:
	puts ("scanact: OPEN");
	return 0;

    case CLOSE:
	puts ("scanact: CLOSE");
	return 0;

    case INIT:
	puts ("scanact: INIT");
	return 0;

    case FINAL:
	puts ("scanact: FINAL");
	return 0;

    default:
	gripe ("scanact");
 	/* NOTREACHED */
    }
}
