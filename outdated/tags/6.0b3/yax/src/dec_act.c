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
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030520 14:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/

#include "sxunix.h"
#include "B_tables.h"
#include "yax_vars.h"
char WHAT_YAXDECACT[] = "@(#)SYNTAX - $Id: dec_act.c 1123 2008-02-29 11:27:32Z rlacroix $" WHAT_DEBUG;

extern struct sxtables	bnf_tables;


extern SXVOID    bnf_found_bad_beginning_of_rule (void);


/* dans yax_put.c */

extern VOID	put_prelude (void);
extern VOID	put_deb_act (void);

/* dans yax.c */

extern SXINT	chercher_nt (SXINT ste);
extern SXINT	placer_attr (SXINT ste);



SXINLINE static VOID prepro_line (SXUINT lineno, char *filename)
{
    fprintf (sxstdout, "\n#line %lu \"%s\"\n", lineno, filename);
}


VOID	dec_act (SXINT code, SXINT numact)
{
    switch (code) {
	static BOOLEAN must_init;

    case OPEN:
    case CLOSE:
    case FINAL:
    case SEMPASS:
	break;

    case INIT:
	must_init = TRUE;
	break;

    case ERROR:
	is_err = TRUE;
	return;

    case ACTION:
	switch (numact) {
	    static SXINT	xattr;

	case /*STOP = "<"&Is_First_Col @*/ 0:
	    bnf_found_bad_beginning_of_rule ();
	    return;

	case /*<axiome>		= <decl_list>	  	 	;*/ 1:
	    terminal_token = &sxsvar.lv.terminal_token /* EOF */ ;

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
		if (xt != 2 && STACKtoken (STACKtop () - xt).comment != NULL) {
		  sxfree (STACKtoken (STACKtop () - xt).comment);
		}
	      }
	    }

	    prepro_line (STACKtoken (STACKtop ()).source_index.line, STACKtoken (STACKtop ()).source_index.file_name);
	    return;

	case /*<attr>		= %ATTR				;*/ 5:
	    if (must_init) {
		put_prelude ();
		prepro_line (1l, sxsrcmngr.source_coord.file_name);
		must_init = FALSE;
	    }

	    terminal_token = &STACKtoken (STACKtop ());

	    if (terminal_token->comment != NULL) {
		fputs (terminal_token->comment, sxstdout);
		sxfree (terminal_token->comment);
	    }

	    xattr = placer_attr (terminal_token->string_table_entry);
	    /* si double declaration is_err est positionne */
	    return;

	case /*<nt_list>	= <nt_list> "," %NT		;*/ 7:
		if (STACKtoken (STACKtop () - 1).comment != NULL) {
		    sxfree (STACKtoken (STACKtop () - 1).comment);
		}

	case /*<nt_list>	= %NT				;*/ 6:

	    {
		/* enregistrement d'un non terminal */
		SXINT	xnt;

		terminal_token = &STACKtoken (STACKtop ());

		if (terminal_token->comment != NULL) {
		    sxfree (terminal_token->comment);
		}

		if ((xnt = chercher_nt (terminal_token->string_table_entry)) != 0) {
		    /* non terminal connu */
		    if (attr_nt [xattr] [xnt]) {
			sxput_error (terminal_token->source_index, "%sMultiple occurrence of this non-terminal symbol.",
			     bnf_tables.err_titles [2]);
			is_err = TRUE;
		    }
		    else
			attr_nt [xattr] [xnt] = TRUE;
		}
	    }

	    return;
	default:
#if EBUG
	  sxtrap("dec_act","unknown switch case #1");
#endif
	  break;
	}

    default:
	fputs ("The function \"dec_act\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
