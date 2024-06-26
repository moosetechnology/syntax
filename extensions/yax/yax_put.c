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
#include "sxunix.h"
#include "B_tables.h"
#include "yax_vars.h"
char WHAT_YAXPUT[] = "@(#)SYNTAX - $Id: yax_put.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

void	put_release (void)
{
    extern char		by_mess [], release_mess [];

    fprintf (sxstdout, "/* Produced by %s, %s */", by_mess, release_mess);
}



void	put_prelude (void)
{
    fputs ("\n\
#line 27 \"yax_pattern\"\n\
#include \"sxunix.h\"\n\
#ifndef SXSTACK_SIZE\n\
#define SXSTACK_SIZE 128\n\
#endif\n\
static SXINT sx_stack_size=SXSTACK_SIZE;\n\
static bool sxerrp;\
", sxstdout);
}



void	put_deb_act (void)
{
    fputs ("\n\
#line 37 \"yax_pattern\"\n\
static YYSTYPE yyval, *yyv;\n\
#ifndef YAXOPEN\n\
#define YAXOPEN\n\
#endif\n\
#ifndef YAXCLOSE\n\
#define YAXCLOSE\n\
#endif\n\
#ifndef YAXERROR\n\
#define YAXERROR\n\
#endif\n\
#ifndef YAXINIT\n\
#define YAXINIT\n\
#endif\n\
#ifndef YAXFINAL\n\
#define YAXFINAL\n\
#endif\n\
",sxstdout);
    fputs ("\n\
/* A C T I O N */\n\
void\n\
", sxstdout);
    fputs (prgentname, sxstdout);
    fputs ("_act(sxcode,sxnact)\n\
SXINT sxcode,sxnact;\n\
{\n\
YYSTYPE*yypv;\n\
switch(sxcode){\n\
case SXOPEN:yyv=(YYSTYPE*)sxalloc(sx_stack_size+2,sizeof(YYSTYPE));YAXOPEN;break;\n\
case SXCLOSE:sxfree(yyv);YAXCLOSE;break;\n\
case SXERROR:sxerrp=true;YAXERROR;break;\n\
case SXINIT:sxerrp=false;YAXINIT;break;\n\
case SXFINAL:YAXFINAL;break;\n\
case SXACTION:if(sxerrp)return;\n\
while(SXSTACKtop()>=sx_stack_size)\n\
yyv=(YYSTYPE*)sxrealloc(yyv,(sx_stack_size*=2)+2,sizeof(YYSTYPE));\n\
yypv= &yyv[SXSTACKnewtop()];\n\
yyval= *yypv--;\n\
switch(sxnact){\
", sxstdout);
}



void	put_postlude (void)
{
    fputs ("\n\
#line 78 \"yax_pattern\"\n\
default:\n\
fputs(\"\\nThe function \\\"\
", sxstdout);
    fputs (prgentname, sxstdout);
    fputs ("_act\\\" is out of date and must be recompiled.\\n\", sxstderr);\n\
sxexit(1);\n\
case 0:break;\n\
} /* switch (sxnact) */\n\
yypv[1]=yyval;\n\
} /* switch (sxcode) */\n\
} /* end \
", sxstdout);
    fputs (prgentname, sxstdout);
    fputs ("_act */\n", sxstdout);
}
