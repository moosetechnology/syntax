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

/* Actions bidon pour le debug de la nouvelle version de SYNTAX (TM) */

#include "sxversion.h"
#include "sxunix.h"

char WHAT_DBGACTION[] = "@(#)SYNTAX - $Id: dbgaction.c 4139 2024-07-31 16:02:45Z garavel $" WHAT_DEBUG;

#if BUG

static void	outstack (void)
{
    SXINT	outstack_index;
    struct sxparstack	*item_ptr;
    struct sxtoken	*atok;

    for (outstack_index = SXSTACKtop (); outstack_index >= SXSTACKnewtop (); outstack_index--) {
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


static void	gripe (char *whom)
{
    fprintf (sxstderr, "The function \"%s\" is out of date with respect to its specification.\n", whom);
    sxexit (1);
}



SXINT	dbgsemact (SXINT what_to_do, SXINT arg)
{
    switch (what_to_do) {
    case SXACTION:
	printf ("semact: action = %ld, reduce = %ld, top = %ld, newtop = %ld\n", (SXINT) arg, (SXINT) SXSTACKreduce (), (SXINT) SXSTACKtop (), (SXINT) SXSTACKnewtop ());
	outstack ();
	break;

    case SXSEMPASS:
	puts ("semact: SXSEMPASS");
	break;

    case SXERROR:
	printf ("semact: error, top = %ld, newtop = %ld\n", (SXINT) SXSTACKtop (), (SXINT) SXSTACKnewtop ());
	outstack ();
	break;

    case SXOPEN:
	puts ("semact: SXOPEN");
	break;

    case SXCLOSE:
	puts ("semact: SXCLOSE");
	break;

    case SXINIT:
	puts ("semact: SXINIT");
	break;

    case SXFINAL:
	puts ("semact: SXFINAL");
	break;

    default:
	gripe ("semact");
    }

    return 0;
}



static void	print_scanner_state (void)
{
  /* LINTED this cast from long to int is needed by printf() */
  printf ("token string = \"%.*s\", state = %ld,\n", (int)sxsvar.sxlv.ts_lgth, sxsvar.sxlv_s.token_string, (SXINT) sxsvar.sxlv.current_state_no);
    printf ("terminal token = {lahead = %ld, entry =%ld,\n", (SXINT) sxsvar.sxlv.terminal_token.lahead, (SXINT) sxsvar.sxlv.terminal_token.string_table_entry);
    printf ("\tsource_index = \"%s\" [%lu, %lu],\n", sxsvar.sxlv.terminal_token.source_index.file_name, (SXUINT)sxsvar.sxlv.terminal_token.source_index.line, (SXUINT)sxsvar.sxlv.terminal_token.source_index.column);

    if (sxsvar.sxlv.terminal_token.comment == NULL)
	puts ("\tno comment},");
    else
	printf ("\tcomment = \"%s\"},\n", sxsvar.sxlv.terminal_token.comment);

    printf ("source_class = %d.\n", sxsvar.sxlv.source_class);
}



bool dbgscanact (SXINT what_to_do, SXINT arg)
{
    switch (what_to_do) {
    case SXACTION:
	printf ("scanact: action = %ld, ", (SXINT) arg);
	print_scanner_state ();
	break;

    case SXPREDICATE:
	printf ("scanact: predicate = %ld, ", (SXINT) arg);
	print_scanner_state ();
	return true;

    case SXOPEN:
	puts ("scanact: SXOPEN");
	break;

    case SXCLOSE:
	puts ("scanact: SXCLOSE");
	break;

    case SXINIT:
	puts ("scanact: SXINIT");
	break;

    case SXFINAL:
	puts ("scanact: SXFINAL");
	break;

    default:
	gripe ("scanact");
    }

    return SXANY_BOOL;
}

