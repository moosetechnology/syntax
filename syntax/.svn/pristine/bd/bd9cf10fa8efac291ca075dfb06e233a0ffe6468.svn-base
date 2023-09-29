/* Tabc Release 6.0, January 17, 2007 */

#include "sxunix.h"
static SXINT sx_stack_size=10;
/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 17-01-07 11:45 (pb):	        Adaptation a la 6.0			*/
/************************************************************************/
/* 20-08-87 17:18 (phd):	Optimisations diverses, notamment	*/
/*				sur les manips de chaines		*/
/************************************************************************/
/* 20-08-87 14:13 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 20-08-87 14:13 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


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
extern VOID put_avant_decl (void);
extern VOID put_entre_decl_open (void);
extern VOID put_entre_open_close (void);
extern VOID put_apres_close (void);

/* dans tabc.c */
extern SXINT chercher_nt (SXINT ste);
extern SXINT placer_attr (struct sxtoken ttok);

static SXINT i, xnt, xattr;
static char* ptr;

static BOOLEAN is_error;

#define skx(x) (x)
#define pcode(x) (STACKtoken(x).lahead)
#define ptoken(x) (STACKtoken(x))
#define ptext(x) (sxstrget(STACKtoken(x).string_table_entry))
#define pste(x) (STACKtoken(x).string_table_entry)
#define pcomment(x) (STACKtoken(x).comment)

/* A C T I O N */
VOID dec_act(SXINT code, SXINT numact)
{
switch (code)
{
case OPEN:
break;

case CLOSE:
break;

case ERROR:is_error=TRUE;break;
case INIT:is_error=FALSE;break;
case FINAL:break;
case ACTION:
if(is_error)return;
if(STACKtop()>=sx_stack_size){
sx_stack_size*=2;
};
switch (numact){
case 0:break;
case 1:

break;
case 2:

break;
case 3:
    terminal_token = ptoken(STACKtop());
    sxput_error (terminal_token.source_index
	 ,"%s You must provide at least one attribute declaration."
	 ,bnf_tables.err_titles[2]
	 );
break;
case 4:
    terminal_token = STACKtoken(STACKtop());
    sxput_error (terminal_token.source_index
	 ,"%s You must provide at least one attribute declaration."
	 ,bnf_tables.err_titles[2]
	 );
break;
case 5:
    put_apres_close();
break;
case 6:
{
    put_avant_decl ();
    ptr = pcomment(STACKtop());
    if (ptr != NULL) {
	fputs (ptr, sxstdout);
	sxfree (ptr);
    }
}
break;
case 7:
     put_avant_decl ();
break;
case 8:
{
    put_entre_decl_open ();
    ptr = pcomment(STACKtop());
    if (ptr != NULL) {
	fputs (ptr, sxstdout);
	sxfree (ptr);
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
    ptr = pcomment(STACKtop());
    if (ptr != NULL) {
	fputs (ptr, sxstdout);
	sxfree (ptr);
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
    type_attr[xattr] = pste(STACKtop());
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
    xattr = placer_attr (ptoken(STACKtop()));
    i = (SXINT)strlen(sxstrget(attr_to_ste [xattr]));
    if (i > M_at ) M_at = i;
break;
case 18:
{
    terminal_token = ptoken(STACKtop());
    xnt = chercher_nt (pste(STACKtop()));
    if (xnt != 0) attr_nt[xattr][xnt] = 's';
}
break;
case 19:
{
    terminal_token = ptoken(STACKtop());
    xnt = chercher_nt (pste(STACKtop()));
    if (xnt != 0) {			/* non terminal connu */
	if (attr_nt[xattr][xnt] != ' ') {
		sxput_error (terminal_token.source_index
		,"%sMultiple occurrence of this non-terminal symbol."
		,bnf_tables.err_titles[1]
		);
		is_err = TRUE;
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
