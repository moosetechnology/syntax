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
/* 20030502 18:05 (phd):	Modif pour calmer le compilo SGI	*/
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

# include "sxunix.h"

char WHAT_DBGACTION[] = "@(#)SYNTAX - $Id: dbgaction.c 1214 2008-03-13 12:49:51Z rlacroix $" WHAT_DEBUG;




#if	BUG

static VOID	outstack (void)
{
    SXINT	outstack_index;
    struct sxparstack	*item_ptr;
    struct sxtoken	*atok;

    for (outstack_index = STACKtop (); outstack_index >= STACKnewtop (); outstack_index--) {
	item_ptr = &(sxpglobals.parse_stack [outstack_index]);
	atok = &(item_ptr->token);
	printf ("\tstack [%ld] = {\n", outstack_index);
	printf ("\t\ttoken.lahead = %ld (\"%s\")\n", atok->lahead, sxttext (sxplocals.sxtables, atok->lahead));
	printf ("\t\t      string_table_entry = %ld (\"%s\")\n", atok->string_table_entry, sxstrget (atok->string_table_entry));
	printf ("\t\t      source_index = {\"%s\", %lu, %lu}\n", atok->source_index.file_name, atok->source_index.line, atok->source_index.column);

	if (atok->comment == NULL)
	    puts ("\t\t      comment = NULL");
	else
	    printf ("\t\t      comment = \"%s\"\n", atok->comment);

	printf ("\t\tstate = %ld\n", item_ptr->state);
	puts ("\t}");
    }
}




#else
#define outstack()
#endif


static SXVOID	gripe (char *whom)
{
    fprintf (sxstderr, "The function \"%s\" is out of date with respect to its specification.\n", whom);
    sxexit (1);
}



SXINT	dbgsemact (SXINT what_to_do, SXINT arg)
{
    switch (what_to_do) {
    case ACTION:
	printf ("semact: action = %ld, reduce = %ld, top = %ld, newtop = %ld\n", (long)arg, (long)STACKreduce (), (long)STACKtop (), (long)STACKnewtop ());
	outstack ();
	break;

    case SEMPASS:
	puts ("semact: SEMPASS");
	break;

    case ERROR:
	printf ("semact: error, top = %ld, newtop = %ld\n", (long)STACKtop (), (long)STACKnewtop ());
	outstack ();
	break;

    case OPEN:
	puts ("semact: OPEN");
	break;

    case CLOSE:
	puts ("semact: CLOSE");
	break;

    case INIT:
	puts ("semact: INIT");
	break;

    case FINAL:
	puts ("semact: FINAL");
	break;

    default:
	gripe ("semact");
    }

    return 0;
}



static VOID	print_scanner_state (void)
{
  /* LINTED this cast from long to int is needed by printf() */
  printf ("token string = \"%.*s\", state = %ld,\n", (int)sxsvar.lv.ts_lgth, sxsvar.lv_s.token_string, (long)sxsvar.lv.current_state_no);
    printf ("terminal token = {lahead = %ld, entry =%ld,\n", (long)sxsvar.lv.terminal_token.lahead, (long)sxsvar.lv.terminal_token.string_table_entry);
    printf ("\tsource_index = \"%s\" [%lu, %lu],\n", sxsvar.lv.terminal_token.source_index.file_name, (SXUINT)sxsvar.lv.terminal_token.source_index.line, (SXUINT)sxsvar.lv.terminal_token.source_index.column);

    if (sxsvar.lv.terminal_token.comment == NULL)
	puts ("\tno comment},");
    else
	printf ("\tcomment = \"%s\"},\n", sxsvar.lv.terminal_token.comment);

    printf ("source_class = %d.\n", sxsvar.lv.source_class);
}



SXINT	dbgscanact (SXINT what_to_do, SXINT arg)
{
    switch (what_to_do) {
    case ACTION:
	printf ("scanact: action = %ld, ", (long)arg);
	print_scanner_state ();
	break;

    case PREDICATE:
	printf ("scanact: predicate = %ld, ", (long)arg);
	print_scanner_state ();
	return 1;

    case OPEN:
	puts ("scanact: OPEN");
	break;

    case CLOSE:
	puts ("scanact: CLOSE");
	break;

    case INIT:
	puts ("scanact: INIT");
	break;

    case FINAL:
	puts ("scanact: FINAL");
	break;

    default:
	gripe ("scanact");
    }

    return 0;
}
