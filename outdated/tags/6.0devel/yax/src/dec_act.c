/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/








#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "yax_vars.h"
char WHAT_YAXDECACT[] = "@(#)SYNTAX - $Id: dec_act.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

extern struct sxtables	bnf_tables;


extern SXVOID    bnf_found_bad_beginning_of_rule (void);


/* dans yax_put.c */

extern SXVOID	put_prelude (void);
extern SXVOID	put_deb_act (void);

/* dans yax.c */

extern SXINT	chercher_nt (SXINT ste);
extern SXINT	placer_attr (SXINT ste);



SXINLINE static SXVOID prepro_line (SXUINT lineno, char *filename)
{
    fprintf (sxstdout, "\n#line %lu \"%s\"\n", lineno, filename);
}


SXVOID	dec_scanact (SXINT code, SXINT numact)
{
    switch (code) {

    case SXERROR:
      is_err = SXTRUE;
    case SXOPEN:
    case SXCLOSE:
    case SXFINAL:
    case SXSEMPASS:
    case SXINIT:
	return;

    case SXACTION:
	switch (numact) {

	case /*STOP = "<"&Is_First_Col @*/ 0:
	    bnf_found_bad_beginning_of_rule ();
	    return;
	default:
#if EBUG
	  sxtrap("dec_scanact","unknown switch case");
#endif
	  break;
	}

    default:
	fputs ("The function \"dec_scanact\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}


SXVOID	dec_semact (SXINT code, SXINT numact)
{
    switch (code) {
	static SXBOOLEAN must_init;

    case SXOPEN:
    case SXCLOSE:
    case SXFINAL:
    case SXSEMPASS:
	break;

    case SXINIT:
	must_init = SXTRUE;
	break;

    case SXERROR:
	is_err = SXTRUE;
	return;

    case SXACTION:
	switch (numact) {
	    static SXINT	xattr;

	case 0:
	    return;

	case /*<axiome>		= <decl_list>	  	 	;*/ 1:
	    terminal_token = &sxsvar.sxlv.terminal_token /* EOF */ ;

	    if (terminal_token->comment != NULL) {
		fputs (terminal_token->comment, sxstdout);
		sxfree (terminal_token->comment);
	    }

	    put_deb_act ();
	    return;

	case /*<decl_list>	= <decl_list> <decl>		;*/ 2:
	case /*<decl_list>	= <decl>			;*/ 3:
	    return;

	case /*<decl>		= <attr> "(" <nt_list> ")" ";"	;*/ 4:
	    {
	      SXINT xt;

	      for (xt = 0; xt <= 3; xt++) {
		if (xt != 2 && SXSTACKtoken (SXSTACKtop () - xt).comment != NULL) {
		  sxfree (SXSTACKtoken (SXSTACKtop () - xt).comment);
		}
	      }
	    }

	    prepro_line (SXSTACKtoken (SXSTACKtop ()).source_index.line, SXSTACKtoken (SXSTACKtop ()).source_index.file_name);
	    return;

	case /*<attr>		= %ATTR				;*/ 5:
	    if (must_init) {
		put_prelude ();
		prepro_line (1l, sxsrcmngr.source_coord.file_name);
		must_init = SXFALSE;
	    }

	    terminal_token = &SXSTACKtoken (SXSTACKtop ());

	    if (terminal_token->comment != NULL) {
		fputs (terminal_token->comment, sxstdout);
		sxfree (terminal_token->comment);
	    }

	    xattr = placer_attr (terminal_token->string_table_entry);
	    /* si double declaration is_err est positionne */
	    return;

	case /*<nt_list>	= <nt_list> "," %NT		;*/ 7:
		if (SXSTACKtoken (SXSTACKtop () - 1).comment != NULL) {
		    sxfree (SXSTACKtoken (SXSTACKtop () - 1).comment);
		}

	case /*<nt_list>	= %NT				;*/ 6:

	    {
		/* enregistrement d'un non terminal */
		SXINT	xnt;

		terminal_token = &SXSTACKtoken (SXSTACKtop ());

		if (terminal_token->comment != NULL) {
		    sxfree (terminal_token->comment);
		}

		if ((xnt = chercher_nt (terminal_token->string_table_entry)) != 0) {
		    /* non terminal connu */
		    if (attr_nt [xattr] [xnt]) {
			sxerror (terminal_token->source_index,
				 bnf_tables.err_titles [2][0],
				 "%sMultiple occurrence of this non-terminal symbol.",
				 bnf_tables.err_titles [2]+1);
			is_err = SXTRUE;
		    }
		    else
			attr_nt [xattr] [xnt] = SXTRUE;
		}
	    }

	    return;
	default:
#if EBUG
	  sxtrap("dec_semact","unknown switch case");
#endif
	  break;
	}

    default:
	fputs ("The function \"dec_semact\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
