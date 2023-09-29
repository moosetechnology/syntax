#include "sxunix.h"
#include "B_tables.h"
#include "yax_vars.h"
char WHAT_YAXPUT[] = "@(#)SYNTAX - $Id: yax_put.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

VOID	put_release ()
{
    extern char		by_mess [], release_mess [];

    fprintf (sxstdout, "/* Produced by %s, %s */", by_mess, release_mess);
}



VOID	put_prelude ()
{
    fputs ("\n\
#line 27 \"yax_pattern\"\n\
#include \"sxunix.h\"\n\
#ifndef SXSTACK_SIZE\n\
#define SXSTACK_SIZE 128\n\
#endif\n\
static int sx_stack_size=SXSTACK_SIZE;\n\
static SXBOOLEAN sxerrp;\
", sxstdout);
}



VOID	put_deb_act ()
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
SXVOID\n\
", sxstdout);
    fputs (prgentname, sxstdout);
    fputs ("_act(sxcode,sxnact)\n\
int sxcode,sxnact;\n\
{\n\
register YYSTYPE*yypv;\n\
switch(sxcode){\n\
case SXOPEN:yyv=(YYSTYPE*)sxalloc(sx_stack_size+2,sizeof(YYSTYPE));YAXOPEN;break;\n\
case SXCLOSE:sxfree(yyv);YAXCLOSE;break;\n\
case SXERROR:sxerrp=SXTRUE;YAXERROR;break;\n\
case SXINIT:sxerrp=SXFALSE;YAXINIT;break;\n\
case SXFINAL:YAXFINAL;break;\n\
case SXACTION:if(sxerrp)return;\n\
while(SXSTACKtop()>=sx_stack_size)\n\
yyv=(YYSTYPE*)sxrealloc(yyv,(sx_stack_size*=2)+2,sizeof(YYSTYPE));\n\
yypv= &yyv[SXSTACKnewtop()];\n\
yyval= *yypv--;\n\
switch(sxnact){\
", sxstdout);
}



VOID	put_postlude ()
{
    fputs ("\n\
#line 78 \"yax_pattern\"\n\
default:\n\
fputs(\"\\nThe function \\\"\
", sxstdout);
    fputs (prgentname, sxstdout);
    fputs ("_act\\\" is out of date and must be recompiled.\\n\", sxstderr);\n\
abort();\n\
case 0:break;\n\
} /* switch (sxnact) */\n\
yypv[1]=yyval;\n\
} /* switch (sxcode) */\n\
} /* end \
", sxstdout);
    fputs (prgentname, sxstdout);
    fputs ("_act */\n", sxstdout);
}
