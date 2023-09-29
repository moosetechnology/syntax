/* Tabc Release 6.0, January 17, 2007 */

#include "sxunix.h"
static SXINT sx_stack_size=10;

#define WHAT	"@(#)dec.tabc\t- SYNTAX [unix] - Mercredi 17 janvier 2007"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};


#include "B_tables.h"

#ifdef	MSDOS
#include "tbc_vars.h"
#else
#include "tabc_vars.h"
#endif

extern struct sxtables bnf_tables;

/* dans tabc_put.c */
extern SXVOID put_avant_decl (void);
extern SXVOID put_entre_decl_open (void);
extern SXVOID put_entre_open_close (void);
extern SXVOID put_apres_close (void);

/* dans tabc.c */
extern SXINT chercher_nt (SXINT ste);
extern SXINT placer_attr (struct sxtoken ttok);

static SXINT i, xnt, xattr;
static char* ptr;

static SXBOOLEAN is_error;

#define skx(x) (x)
#define pcode(x) (SXSTACKtoken(x).lahead)
#define ptoken(x) (SXSTACKtoken(x))
#define ptext(x) (sxstrget(SXSTACKtoken(x).string_table_entry))
#define pste(x) (SXSTACKtoken(x).string_table_entry)
#define pcomment(x) (SXSTACKtoken(x).comment)

/* A C T I O N */
SXVOID dec_act(SXINT code, SXINT numact)
{
switch (code)
{
case SXOPEN:
break;

case SXCLOSE:
break;

case SXERROR:is_error=SXTRUE;break;
case SXINIT:is_error=SXFALSE;break;
case SXFINAL:break;
case SXACTION:
if(is_error)return;
if(SXSTACKtop()>=sx_stack_size){
sx_stack_size*=2;
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
		is_err = SXTRUE;
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
