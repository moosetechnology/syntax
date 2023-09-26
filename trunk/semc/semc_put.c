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
#include "B_tables.h"
#include "semc_vars.h"

char WHAT_SEMC_PUT[] = "@(#)SYNTAX - $Id: semc_put.c 3599 2023-09-18 12:57:19Z garavel $" WHAT_DEBUG;

/* --------------------------------------------------------- */

static SXVOID put_term_attr (void)
{
fputs("\n#define skx(x) (x)",sxstdout);
fputs("\n#define pcode(x) (SXSTACKtoken(x).lahead)",sxstdout);
fputs("\n#define ptoken(x) (SXSTACKtoken(x))",sxstdout);
fputs("\n#define ptext(x) (sxstrget(SXSTACKtoken(x).string_table_entry))",sxstdout);
fputs("\n#define pste(x) (SXSTACKtoken(x).string_table_entry)",sxstdout);
fputs("\n#define pcomment(x) (SXSTACKtoken(x).comment)",sxstdout);

} /* end put_term_attr */

/* --------------------------------------------------------- */

static SXVOID put_actact (void)
{
fputs("\n\n/* A C T I O N */", sxstdout);
fputs("\nSXVOID ", sxstdout);
fputs(prgentname, sxstdout);
fputs("_act(SXINT code, SXINT numact)", sxstdout);
fputs("\n{", sxstdout);
fputs("\nswitch (code)", sxstdout);
fputs("\n{", sxstdout);
fputs("\ncase SXOPEN:", sxstdout);

} /* end put_actact */

/* --------------------------------------------------------- */

static SXVOID put_open (void)
{
    SXINT  nat;

    fputc ('\n', sxstdout);

    for (nat = 1; nat <= (SXINT)max_attr; nat++) {
	if (type_attr[nat] != 0) {
	    char *type = sxstrget (type_attr[nat]);

	    fprintf (sxstdout,
		     "%s=(%s*)sxalloc((sx_stack_size=sxP_get_parse_stack_size())+2,sizeof(%s));\n",
		     sxstrget (attr_to_ste[nat]) + 1,
		     type,
		     type);
	}
    }
}					/* end put_open */

/* --------------------------------------------------------- */

static SXVOID put_free (void)
{
    SXINT  nat;

    fputc ('\n', sxstdout);

    for (nat = max_attr; nat >= 1; nat--) {
	if (type_attr[nat] != 0) {
	    fprintf (sxstdout, "sxfree(%s);\n",
		     sxstrget (attr_to_ste[nat]) + 1);
	}
    }
}					/* end put_free */

/* --------------------------------------------------------- */

SXVOID put_copyright (void)
{
   fputs(SXCOPYRIGHT, sxstdout);
} /* end put_copyright */

/* --------------------------------------------------------- */

SXVOID put_avant_decl (void)
{
   fputs("\n#include \"sxversion.h\"", sxstdout);
   fputs("\n#include \"sxunix.h\"", sxstdout);
   fputs("\n\nstatic SXINT sx_stack_size;\n", sxstdout);
} /* end put_avant_decl */

/* --------------------------------------------------------- */

SXVOID put_entre_decl_open (void)
{
    SXINT  nat;

    fputs ("\nstatic SXBOOLEAN is_error;\n", sxstdout);

    for (nat = 1; nat <= (SXINT)max_attr; nat++) {
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

SXVOID put_entre_open_close (void)
{
    fputs ("\ncase SXCLOSE:", sxstdout);
    put_free ();
}					/* put_entre_open_close */

/* --------------------------------------------------------- */

SXVOID put_apres_close (void)
{
    SXINT  nat;

    fputs ("\n\
case SXERROR:is_error=SXTRUE;break;\n\
case SXINIT:is_error=SXFALSE;break;\n\
case SXFINAL:break;\n\
case SXACTION:\n\
if(is_error)return;\n\
if(sxP_get_parse_stack_size()>sx_stack_size){\n\
", sxstdout);				/* } */

    for (nat = 1; nat <= (SXINT)max_attr; nat++) {
	if (type_attr[nat] != 0) {
	    char *nom = sxstrget (attr_to_ste[nat]) + 1;
	    char *type = sxstrget (type_attr[nat]);

	    fprintf (sxstdout,
		     "%s=(%s*)sxrealloc(%s,(sx_stack_size=sxP_get_parse_stack_size())+2,sizeof(%s));\n",
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

SXVOID put_postlude (void)
{
    SXINT  nat;

/*{*/
    fputs ("default:;\n\
}\n\n\
", sxstdout);

    for (nat = 1; nat <= (SXINT)max_attr; nat++)
	if (type_attr[nat] != 0) {
	    char *nom = sxstrget (attr_to_ste[nat]) + 1;

	    fprintf (sxstdout, "%s[SXSTACKnewtop()]=%s[0];\n",
		     nom, nom);
	}
/*{{*/
    /* We add the default case to quiet gcc -Wswitch-default */
    fprintf (sxstdout, "default:break;\n} /* switch (code) */\n} /* end %s_act */\n", prgentname);
}					/* end put_postlude */

/* --------------------------------------------------------- */

SXVOID put_case (void)
{
/* production du case action (xprod)
   et des identites memorisees jusqu'alors */

    if (is_empty) {
	is_empty = SXFALSE;
	fprintf (sxstdout, "case %ld:\n", (SXINT) xprod);
    }

}					/* end put_case */

/* --------------------------------------------------------- */

SXVOID put_identite (SXINT nat1, SXINT nat2, SXINT pos)
{char *p=modele;

nb_identities ++ ;
strcpy(modele, "");

strcat(modele,sxstrget(attr_to_ste[nat1])+1);
strcat(modele, "[0]=");

strcat(modele,sxstrget(attr_to_ste[nat2])+1);
strcat(modele, "[SXSTACKtop()");
if (pos > 1) {
    strcat(modele, "-" );
    while(*p)p++;
    sprintf(p,"%ld",(SXINT) pos-1);
    }
strcat(modele, "];" );

strcat(mod_ident, modele);
strcat(mod_ident, "\n");

} /* end put_identite */

/* --------------------------------------------------------- */
