#include "sxunix.h"
#include "B_tables.h"
#ifdef	MSDOS
#include "tbc_vars.h"
#else
#include "tabc_vars.h"
#endif
char WHAT_TABCPUT[] = "@(#)SYNTAX - $Id: tabc_put.c 655 2007-06-26 14:27:58Z rlacroix $" WHAT_DEBUG;

/* --------------------------------------------------------- */

static VOID put_term_attr ()
{
fputs("\n#define skx(x) (x)",sxstdout);
fputs("\n#define pcode(x) (STACKtoken(x).lahead)",sxstdout);
fputs("\n#define ptoken(x) (STACKtoken(x))",sxstdout);
fputs("\n#define ptext(x) (sxstrget(STACKtoken(x).string_table_entry))",sxstdout);
fputs("\n#define pste(x) (STACKtoken(x).string_table_entry)",sxstdout);
fputs("\n#define pcomment(x) (STACKtoken(x).comment)",sxstdout);

} /* end put_term_attr */

/* --------------------------------------------------------- */

static VOID put_actact ()
{
fputs("\n\n/* A C T I O N */", sxstdout);
fputs("\nVOID ", sxstdout);
fputs(prgentname, sxstdout);
fputs("_act(code, numact)", sxstdout);
fputs("\nint code;", sxstdout);
fputs("\nint numact;", sxstdout);
fputs("\n{", sxstdout);
fputs("\nswitch (code)", sxstdout);
fputs("\n{", sxstdout);
fputs("\ncase OPEN:", sxstdout);

} /* end put_actact */

/* --------------------------------------------------------- */

static VOID put_open ()
{
    register int  nat;

    fputc ('\n', sxstdout);

    for (nat = 1; nat <= max_attr; nat++) {
	if (type_attr[nat] != 0) {
	    register char *type = sxstrget (type_attr[nat]);

	    fprintf (sxstdout,
		     "%s=(%s*)sxalloc(sx_stack_size+2,sizeof(%s));\n",
		     sxstrget (attr_to_ste[nat]) + 1,
		     type,
		     type);
	}
    }
}					/* end put_open */

/* --------------------------------------------------------- */

static VOID put_free ()
{
    register int  nat;

    fputc ('\n', sxstdout);

    for (nat = max_attr; nat >= 1; nat--) {
	if (type_attr[nat] != 0) {
	    fprintf (sxstdout, "sxfree(%s);\n",
		     sxstrget (attr_to_ste[nat]) + 1);
	}
    }
}					/* end put_free */

/* --------------------------------------------------------- */

VOID put_release (release)
char *release;
{
fputs("/* ", sxstdout);
fputs(release, sxstdout);
fputs(" */\n", sxstdout);

} /* end put_release */

/* --------------------------------------------------------- */

VOID put_avant_decl ()
{
fputs("\n#include \"sxunix.h\"", sxstdout);
fputs("\nstatic int sx_stack_size=10;\n", sxstdout);

} /* end put_avant_decl */

/* --------------------------------------------------------- */

VOID put_entre_decl_open ()
{
    register int  nat;

    fputs ("\nstatic BOOLEAN is_error;\n", sxstdout);

    for (nat = 1; nat <= max_attr; nat++) {
	if (type_attr[nat] != 0) {
	    fprintf (sxstdout, "static %s *%s;\n",
		     sxstrget (type_attr[nat]),
		     sxstrget (attr_to_ste[nat]) + 1);
	}
    }

    put_term_attr ();
    put_actact ();
    put_open ();

}					/* end put_entre_decl_open */

/* --------------------------------------------------------- */

VOID put_entre_open_close ()
{
    fputs ("\ncase CLOSE:", sxstdout);
    put_free ();
}					/* put_entre_open_close */

/* --------------------------------------------------------- */

VOID put_apres_close ()
{
    register int  nat;

    fputs ("\n\
case ERROR:is_error=TRUE;break;\n\
case INIT:is_error=FALSE;break;\n\
case FINAL:break;\n\
case ACTION:\n\
if(is_error)return;\n\
if(STACKtop()>=sx_stack_size){\n\
sx_stack_size*=2;\n\
", sxstdout);				/* } */

    for (nat = 1; nat <= max_attr; nat++) {
	if (type_attr[nat] != 0) {
	    register char *nom = sxstrget (attr_to_ste[nat]) + 1;
	    register char *type = sxstrget (type_attr[nat]);

	    fprintf (sxstdout,
		     "%s=(%s*)sxrealloc(%s,sx_stack_size+2,sizeof(%s));\n",
		     nom, type, nom, type);
	}
    }
/*{*/
    fputs ("};\n\
switch (numact){\n\
case 0:break;\n\
", sxstdout);				/* } */
}					/* put_apres_close */

/* --------------------------------------------------------- */

VOID put_postlude ()
{
    register int  nat;

/*{*/
    fputs ("default:;\n\
}\n\n\
", sxstdout);

    for (nat = 1; nat <= max_attr; nat++)
	if (type_attr[nat] != 0) {
	    register char *nom = sxstrget (attr_to_ste[nat]) + 1;

	    fprintf (sxstdout, "%s[STACKnewtop()]=%s[0];\n",
		     nom, nom);
	}
/*{{*/
    /* We add the default case to quiet gcc -Wswitch-default */
    fprintf (sxstdout, "default:break;\n} /* switch (code) */\n} /* end %s_act */\n", prgentname);
}					/* end put_postlude */

/* --------------------------------------------------------- */

VOID put_case ()
{
/* production du case action (xprod)
   et des identites memorisees jusqu'alors */

    if (is_empty) {
	is_empty = FALSE;
	fprintf (sxstdout, "case %u:\n", xprod);
    }

}					/* end put_case */

/* --------------------------------------------------------- */

VOID put_identite (nat1, nat2, pos)
int nat1, nat2, pos;
{char *p=modele;

nb_identities ++ ;
strcpy(modele, "");

strcat(modele,sxstrget(attr_to_ste[nat1])+1);
strcat(modele, "[0]=");

strcat(modele,sxstrget(attr_to_ste[nat2])+1);
strcat(modele, "[STACKtop()");
if (pos > 1) {
    strcat(modele, "-" );
    while(*p)p++;
    sprintf(p,"%d",pos-1);
    }
strcat(modele, "];" );

strcat(mod_ident, modele);
strcat(mod_ident, "\n");

} /* end put_identite */

/* --------------------------------------------------------- */
