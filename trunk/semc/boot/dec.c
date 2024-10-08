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

#include "sxversion.h"
#include "sxunix.h"

static SXINT sx_stack_size;

char WHAT_SEMC_DEC[] = "@(#)SYNTAX - $Id: dec.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

#include "B_tables.h"
#include "semc_vars.h"

extern SXTABLES bnf_tables;

/* dans semc_put.c */
extern void put_avant_decl (void);
extern void put_entre_decl_open (void);
extern void put_entre_open_close (void);
extern void put_apres_close (void);

/* dans semc.c */
extern SXINT chercher_nt (SXINT ste);
extern SXINT placer_attr (struct sxtoken ttok);

static SXINT i, xnt, xattr;
static char* ptr;

static bool is_error;

#define skx(x) (x)
#define pcode(x) (SXSTACKtoken(x).lahead)
#define ptoken(x) (SXSTACKtoken(x))
#define ptext(x) (sxstrget(SXSTACKtoken(x).string_table_entry))
#define pste(x) (SXSTACKtoken(x).string_table_entry)
#define pcomment(x) (SXSTACKtoken(x).comment)

/* A C T I O N */
void dec_act(SXINT code, SXINT numact, SXTABLES *arg)
{
(void) arg;
switch (code)
{
case SXOPEN:
break;

case SXCLOSE:
break;

case SXERROR:is_error=true;break;
case SXINIT:is_error=false;break;
case SXFINAL:break;
case SXACTION:
if(is_error)return;
if(sxP_get_parse_stack_size()>sx_stack_size){
};
switch (numact){
case 0:break;
case 1:

break;
case 2:

break;
case 3:
    terminal_token = ptoken(SXSTACKtop());
    sxerror (terminal_token.source_index,
             bnf_tables.err_titles[2][0],
	     "%s You must provide at least one attribute declaration.",
             bnf_tables.err_titles[2]+1
	 );
break;
case 4:
    terminal_token = SXSTACKtoken(SXSTACKtop());
    sxerror (terminal_token.source_index,
             bnf_tables.err_titles[2][0],
             "%s You must provide at least one attribute declaration.",
	      bnf_tables.err_titles[2]+1
	 );
break;
case 5:
    put_apres_close();
break;
case 6:
{
    put_avant_decl ();
    ptr = pcomment(SXSTACKtop());
    if (ptr != NULL) {
	fputs (ptr, sxstdout);
	/* sxfree (ptr); */
    }
}
break;
case 7:
     put_avant_decl ();
break;
case 8:
{
    put_entre_decl_open ();
    ptr = pcomment(SXSTACKtop());
    if (ptr != NULL) {
	fputs (ptr, sxstdout);
	/* sxfree (ptr); */
    }
    fputs ("break;\n", sxstdout);
}
break;
case 9:
{
    put_entre_decl_open ();
    fputs ("break;\n", sxstdout);
}
break;
case 10:
{
    put_entre_open_close ();
    ptr = pcomment(SXSTACKtop());
    if (ptr != NULL) {
	fputs (ptr, sxstdout);
	/* sxfree (ptr); */
    }
    fputs ("break;\n", sxstdout);
}
break;
case 11:
{
    put_entre_open_close ();
    fputs ("break;\n", sxstdout);
}
break;
case 14:
{
    type_attr[xattr] = pste(SXSTACKtop());
}
break;
case 15:
{
    for (xnt = 1; xnt <= W.ntmax; xnt++) {
	if (attr_nt[xattr][xnt] == 's')
	    attr_nt[xattr][xnt] = 'v';
    }
    type_attr[xattr] = 0;
}
break;
case 17:
    xattr = placer_attr (ptoken(SXSTACKtop()));
    i = (SXINT)strlen(sxstrget(attr_to_ste [xattr]));
    if (i > M_at ) M_at = i;
break;
case 18:
{
    terminal_token = ptoken(SXSTACKtop());
    xnt = chercher_nt (pste(SXSTACKtop()));
    if (xnt != 0) attr_nt[xattr][xnt] = 's';
}
break;
case 19:
{
    terminal_token = ptoken(SXSTACKtop());
    xnt = chercher_nt (pste(SXSTACKtop()));
    if (xnt != 0) {			/* non terminal connu */
	if (attr_nt[xattr][xnt] != ' ') {
		sxerror (terminal_token.source_index
		,bnf_tables.err_titles[1][0]
		,"%sMultiple occurrence of this non-terminal symbol."
		,bnf_tables.err_titles[1]+1
		);
		is_err = true;
	}
	else attr_nt[xattr][xnt] = 's';
    }
}
break;
default:;
}

default:break;
} /* switch (code) */
} /* end dec_act */
