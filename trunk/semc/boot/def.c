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

char WHAT_SEMC_DEF[] = "@(#)SYNTAX - $Id: def.c 4143 2024-08-02 08:50:12Z garavel $" WHAT_DEBUG;

#include "B_tables.h"
#include "semc_vars.h"
#include "sxcommon.h"
#include <strings.h>

extern struct sxtables bnf_tables;

/* dans semc_put.c */
extern void put_case (void);
extern void put_identite (SXINT nat1, SXINT nat2, SXINT pos);
extern void put_postlude (void);

/* dans semc.c */
extern void tilt (SXINT attrno, SXINT ntno, SXINT posp);
extern void creer (SXINT attrg, SXINT attrd, SXINT posd);
extern void creermilieu (SXINT attrg, SXINT attrd, SXINT posd);
extern void creervide (SXINT attrg, SXINT attrd, SXINT posd);
extern SXINT chercher_attr (SXINT ste);
extern SXINT chercher_nt (SXINT ste);
extern SXINT chercher_t (SXINT ste);
extern SXINT posprod (SXINT prodnum, SXINT tntno, SXINT quote);
	
static SXINT nbq, i, xtnt, xattr, pos1;
static char* ptr;

/*   --------------------------------------------------------- */

static void decoder(struct sxtoken ttok)
{
    char *p1, *p2, *nom;

/*
travaille sur terminal_token qui contient un attribut sous la forme:
	$attr?<non-term>?'...' | $attr?term?'...'
ou
	$attr?<non-term>?      | $attr?term? 
ou
	$attr?
  
construit xattr, xtnt et nbq
*/
    terminal_token = ttok;

    if (terminal_token.string_table_entry == SXERROR_STE) {
	/* entree vide si erreur de syntaxe */
	is_err = true;
	xattr = 0;
	xtnt = 0;
	nbq = 0;
	pos1 = 0;
	return;
    }

    nom = sxstrget (terminal_token.string_table_entry);

    p1 = sxindex (nom, '?');		/* pointeur vers le premier ? */
    *p1 = SXNUL;
    xattr = chercher_attr (sxstrretrieve (nom));
    *p1 = '?';

    if (xattr > 0) {
	/* attribut de non terminal */
	p2 = sxrindex (nom, '?');		/* pointeur vers le deuxieme ou
					   l'unique ? */
	if (p2 == p1) {			/* pas de nonterminal donc de quote */
	    /* nom = $attr? */
	    xtnt = nt_pg;
	    nbq = 0;
	    pos1 = 0;
	}
	else {				/* nonterminal apres l'attribut,
					   quotes possibles */
	    /* nom = $attr?<non-term>?'...' ou $attr?<non-term>? */
	    *p2 = SXNUL;

	    terminal_token.source_index.column += p2-p1-2;

	    xtnt = chercher_nt (sxstrretrieve (p1 + 1));
	    *p2 = '?';

	    nbq = 0;
	    while (*(++p2) != SXNUL)
		nbq++;

	    if (!is_err) {
		pos1 = posprod (xprod, xtnt, nbq);
	    }
	    else
		pos1 = 0;
	}
    }
    else if (xattr < 0) {
	/* attribut de terminal */
	p2 = sxrindex (nom, '?');		/* pointeur vers le deuxieme ? */
	if (p2 == p1) {			/* pas de terminal */
	    /* nom = $attr? : interdit */
	    sxerror (terminal_token.source_index
	    ,bnf_tables.err_titles[2][0]
	    ,"%sYou must provide a terminal symbol with a terminal attribute."
			 ,bnf_tables.err_titles[2]+1
		 );
	    is_err = true;
	    xtnt = 0;			/* bidon */
	    nbq = 0;
	    pos1 = 0;
	}				/* pas de terminal */
	else {				/* terminal */
	    /* nom = $attr?term?'...' ou $attr?term? */
	    *p2 = SXNUL;
	    xtnt = chercher_t (sxstrretrieve (p1 + 1));
	    *p2 = '?';

	    nbq = 0;
	    while (*(++p2) != SXNUL)
		nbq++;

	    if (!is_err)
		pos1 = posprod (xprod, xtnt, nbq);
	    else
		pos1 = 0;
	}
    }
    else /* attribut non trouve */
	{
	is_err = true;
	xattr = 0;
	xtnt = 0;
	nbq = 0;
	pos1 = 0;
	return;
	}
}					/* end decoder */

/*   --------------------------------------------------------- */

static void coder (SXINT coder_xattr, SXINT pos)
{
    SXINT  pos2;

    fputs (sxstrget (attr_to_ste[coder_xattr]) + 1, sxstdout);	/* nom de l'attribut */
    pos2 = LGPROD (xprod);		/* longueur de la production */

    if (coder_xattr > 0) {
	/* attribut non-terminal */

	if (pos == 0)
	    fputs ("[0]", sxstdout);
	else if (pos == pos2)
	    fputs ("[SXSTACKtop()]", sxstdout);
	else
	    fprintf (sxstdout, "[SXSTACKtop()-%ld]", (SXINT) (pos2 - pos));
    }
    else if (coder_xattr < 0) {
	/* attribut terminal */

	if (pos == pos2)
	    fputs ("(SXSTACKtop())", sxstdout);
	else
	    fprintf (sxstdout, "(SXSTACKtop()-%ld)", (SXINT) (pos2 - pos));
    }
    else /* cas d'erreur */
        fputs ("[0]", sxstdout);
}					/* end coder */

/*   --------------------------------------------------------- */

static void defauter(void)
{
    SXINT ntpd
	, nbsynv
	, nat
	, pos
	, nbntpd
	, nbsynd
	, k
	, j
        , lgth;

   char c;

/* pour engendrer les definitions d'attributs par defaut */


/*  **************************************** */

/*  1)  initialisations */

/*  nt_pg donne le non-terminal en partie gauche */
       lgth = LGPROD (xprod);
       prolnb = WN[xprod].prolon;
       	/* index lispro de la partie droite */
       maxlis = prolnb + lgth;

       sxinitialise (pos); /* pour faire taire gcc -Wuninitialized */
       nbntpd = 0;/* nombre de non-terminaux en partie droite */

       if (lgth > 0)
	for (i = maxlis - 1; i >= prolnb; i -- ) {
	  if (WI[i].lispro > 0) {
	    if (nbntpd == 0)
	      pos = i;
	  }

	  nbntpd++;
	}

/* pos pointe sur le non-terminal le + a droite pour les regles non vides */

/*  2)  on compte le nombre d'attributs voulus et definis de la regle */
       nbsynv = 0;
       for (j = 2; j <= max_attr; j ++) {
	c = attr_nt [j] [nt_pg];
	if (c == 's' || c == 'v') nbsynv ++ ;
	}

       nbsynd = 0;
       for (j = 1; j <= nb_definitions; j ++) {
	c = attr_nt [defini [j]] [nt_pg];
	if (c == 's' || c == 'v') nbsynd ++ ;
	}

/*  3)  ON ESSAIE D'APPLIQUER LES IDENTITES PAR DEFAUT  */

if (nbntpd == 0) {

/* ------------------------------------ */
/* PAS DE NON TERMINAL EN PARTIE DROITE */
/* ------------------------------------ */

    if ((nbsynv != 0) && (nbsynv != nbsynd)) {
    /* on attendait des definitions et toutes n'ont pas ete faites */

	for (nat = 2; nat <= max_attr; nat ++) {
	    c = attr_nt [nat] [nt_pg];
	    if (c == 's' || c == 'v') {  /*  il faut cette definition */
		for (j = 1; j <= nb_definitions; j ++) {
		     if (nat == defini [j]) goto OKSS;
		     }	  /* for j */
		tilt (nat, nt_pg, 0);    /*  definition absente : erreur  */
		}/* if c = */
OKSS: ;
	    } /*  for nat  */
    } /* il manque des definitions */

}
/* ..................................... */
/*  PAS DE NON-TERMINAL EN PARTIE DROITE */
/* ..................................... */

else

if (nbntpd > 1) {

/* --------------------------------------- */
/* PLUS D'UN NON TERMINAL EN PARTIE DROITE */
/* --------------------------------------- */

    if ((nbsynv != 0) && (nbsynv != nbsynd)) {
    /* on attendait des definitions et toutes n'ont pas ete faites */
        put_case ();
        ntpd = WI[pos].lispro;      /* non terminal le + a droite */

	for (nat = 2; nat <= max_attr; nat ++) {
	    c = attr_nt [nat] [nt_pg];
	    if (c == 's' || c == 'v') {     /*  test definition */
		for (j = 1; j <= nb_definitions; j ++) {
		    if (nat == defini [j]) goto SYNOK;
	        } /* for j */

/*  il manque la definition de nat  */
/*  on essaie de synthetiser a partir du symbole le + a droite	*/
/* recherche si l'attribut s'applique a un seul */
/* symbole en partie droite sinon tilt */

		nbsynd = 0;
		for (k = prolnb; k <= maxlis -1; k ++) {
		    ntpd = WI[k].lispro;
		    if (ntpd > 0) {	 /* symbole non terminal */
			if (attr_nt [nat] [ntpd] != ' ') {
			    nbsynd ++ ; /* nb de symboles qui s'appliquent */
			    pos = k; /* position */
			} /* if ^=' ' */
		    } /* if ntpd */
		} /* for k */

		if (nbsynd != 1) tilt (nat, nt_pg, 0);
		else creermilieu (nat, nat, maxlis -pos);

		}/* if c='s' */
SYNOK: ;
	    } /*  for nat  */
         } /* if ... */
}
/*  ....................................... */
/*  plus d'un non-terminal en partie droite */
/*  ....................................... */

else {

/*  ------------------------------------- */
/*  UN SEUL NON-TERMINAL EN PARTIE DROITE */
/*  ------------------------------------- */

    if ((nbsynv != 0) && (nbsynv != nbsynd)) {
    /* on attendait des definitions et toutes n'ont pas ete faites */
	if (pos == prolnb /* le seul symbole est le + a gauche */
	    && is_empty /* et on a rencontre que des identites */
	    ){}
        else put_case ();

    ntpd = WI[pos].lispro; /* numero de ce symbole le + a gauche */
    for (nat = 2; nat <= max_attr; nat ++) {
	c = attr_nt [nat] [nt_pg];
	if (c == 's' || c == 'v') { /*  il faut cette definition */
	    for (j = 1; j <= nb_definitions; j ++) {
		if (nat == defini [j]) goto SYNYEST;
	    } /* for j */

/*  il manque la definition de nat  */
	    if (attr_nt [nat] [ntpd] == 's') 
		creer (nat, nat, maxlis - pos);
	    else
	    if (attr_nt [nat] [ntpd] == 'v') 
		creervide (nat, nat, maxlis - pos);
	    else tilt (nat, nt_pg, 0);
	    }/* if c== ... */
SYNYEST: ;
	} /*  for nat  */
    }
}
/* ..................................... */
/* un seul non-terminal en partie droite */
/* ..................................... */

WN[xprod].bprosimpl = bident;

if (!is_empty) {
	WN[xprod].action = xprod;
	}
else WN[xprod].action = 0; /* action vide */

return;

} /* end defauter */

/*   --------------------------------------------------------- */

static bool is_error;

#define skx(x) (x)
#define pcode(x) (SXSTACKtoken(x).lahead)
#define ptoken(x) (SXSTACKtoken(x))
#define ptext(x) (sxstrget(SXSTACKtoken(x).string_table_entry))
#define pste(x) (SXSTACKtoken(x).string_table_entry)
#define pcomment(x) (SXSTACKtoken(x).comment)

/* A C T I O N */
void def_act(SXINT code, SXINT numact, struct sxtables *arg)
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
{
    if (!is_ident) {
	if (sxsvar.sxlv.terminal_token.comment != NULL) {
	    fputs (sxsvar.sxlv.terminal_token.comment, sxstdout);
	    /* sxfree (sxsvar.sxlv.terminal_token.comment); */
	}
    }
    if (!is_empty) {
	fputs (mod_ident, sxstdout);
	fputs ("break;\n", sxstdout);
    }
    put_postlude ();
}
break;
case 4:
    defauter();
break;
case 5:
{
    if (!is_ident) {
	ptr = pcomment(SXSTACKtop());
	if (ptr != NULL) {
	    fputs (ptr, sxstdout);
	    /* sxfree (ptr); */
	}
    }
    if (!is_empty) {
	fputs (mod_ident, sxstdout);
	fputs ("break;\n", sxstdout);
    }
    xprod++;
    nb_definitions = 0;
    nt_pg = WN[xprod].reduc;
    is_empty = true;
    is_ident = true;
    mod_ident[0] = SXNUL;
}
break;
case 7:
    is_ident = true;
break;
case 8:
     nb_sem_rules ++ ;
break;
case 9:
     nb_sem_rules ++ ;
break;
case 10:
{
    is_ident = false;
    terminal_token = ptoken(SXSTACKtop()-1);
    decoder (terminal_token);
    if (!is_err) {
	SXINT pos2 = LGPROD (xprod) + 1 - pos1;
	if (xattr > 0) {
	    if (attr_nt[xattr][xtnt] == ' ') {
		sxerror (terminal_token.source_index
                         ,bnf_tables.err_titles[2][0]
			 ,"%sThis attribute has not been declared with this non-terminal symbol."
			 ,bnf_tables.err_titles[2]+1
		     );
		is_err = true;
	    }
	    if (!is_err) {
		if (xattr != defini[nb_definitions]) {
		    if (type_attr[xattr] != type_attr[defini[nb_definitions]]) {
			sxerror (terminal_token.source_index
				 ,bnf_tables.err_titles[2][0]
			         ,"%sIdentity between attributes of different types."
				 ,bnf_tables.err_titles[2]+1
			     );
			is_err = true;
		    }
		    else {
			bident = false;
			put_case ();
			put_identite (defini[nb_definitions], xattr, pos2);
		    }
		}
		else {
		    if ((xtnt == nt_pg) && (pos1 == 0)) {
			sxerror (terminal_token.source_index
                                 ,bnf_tables.err_titles[2][0]
				 ,"%sI beg your pardon!"
				 ,bnf_tables.err_titles[2]+1
			     );
			is_err = true;
		    }
		    else
			put_identite (defini[nb_definitions], xattr, pos2);
		}
	    }
	}
	else {
	    sxerror (terminal_token.source_index
		    ,bnf_tables.err_titles[2][0]
		    ,"%sYou may not use a terminal attribute in an identity."
		    ,bnf_tables.err_titles[2]+1
		 );
	    is_err = true;
	}
    }
    is_ident = true;
}
break;
case 11:
     is_ident = false;
break;
case 12:
{
    terminal_token = ptoken(SXSTACKtop());
    if (!is_ident) {
	ptr = pcomment(SXSTACKtop());
	if (ptr != NULL) {
	    fputs (ptr, sxstdout);
	    /* sxfree (ptr); */
	}
    }
    is_err = false;
    decoder (terminal_token);
    if (xattr > 0) {
	if (!is_err) {
	    if (pos1 != 0 || nbq != 0) {
		sxerror (terminal_token.source_index
			 ,bnf_tables.err_titles[2][0]
		         ,"%sIllegal attribute definition of a right side symbol."
			 ,bnf_tables.err_titles[2]+1
		     );
		is_err = true;
	    }
	}
	if (!is_err) {
	    for (i = 1; i <= nb_definitions; i++) {
		if (defini[i] == xattr) {
		    sxerror (terminal_token.source_index
			     ,bnf_tables.err_titles[2][0]
			     ,"%sThis attribute has already been defined."
			     ,bnf_tables.err_titles[2]+1
			 );
		    is_err = true;
		}
	    }
	}
	if (!is_err) {
	    if (attr_nt[xattr][nt_pg] == ' ') {
		sxerror (terminal_token.source_index
			 ,bnf_tables.err_titles[2][0]
			 ,"%sThis non-terminal symbol has not been declared with this attribute."
			 ,bnf_tables.err_titles[2]+1
		     );
		is_err = true;
	    }
	}
	if (!is_err) {
	    nb_definitions++;
	    defini[nb_definitions] = xattr;
	}
    }
    else {
	if (!is_err) {
	    sxerror (terminal_token.source_index
   		     ,bnf_tables.err_titles[2][0]
		     ,"%sYou may not use a terminal attribute in an identity."
   		     ,bnf_tables.err_titles[2]+1
		 );
	    is_err = true;
	}
    }
}
break;
case 13:
{
    terminal_token = ptoken(SXSTACKtop());
    decoder (terminal_token);
    if (xattr > 0) {
	if (!is_err) {
	    if (pos1 != 0 || nbq != 0) {
		sxerror (terminal_token.source_index, bnf_tables.err_titles[2][0]
		    ,"%sIllegal attribute definition of a right side symbol."
		    ,bnf_tables.err_titles[2]+1
		     );
		is_err = true;
	    }
	}
	if (!is_err) {
	    for (i = 1; i <= nb_definitions; i++) {
		if (defini[i] == xattr) {
		    sxerror (terminal_token.source_index, bnf_tables.err_titles[2][0]
			 ,"%sThis attribute has already been defined."
			 ,bnf_tables.err_titles[2]+1
			 );
		    is_err = true;
		}
	    }
	}
	if (!is_err) {
	    if (attr_nt[xattr][nt_pg] == ' ') {
		sxerror (terminal_token.source_index, bnf_tables.err_titles[2][0]
			    ,"%sThis non-terminal symbol has not been declared with this attribute."
			    ,bnf_tables.err_titles[2]+1
		     );
		is_err = true;
	    }
	}
	if (!is_err) {
	    nb_definitions++;
	    defini[nb_definitions] = xattr;
	}
    }
    else {
	if (!is_err) {
	    sxerror (terminal_token.source_index, bnf_tables.err_titles[2][0]
		 ,"%sYou may not use a terminal attribute in an identity."
		 ,bnf_tables.err_titles[2]+1
		 );
	    is_err = true;
	}
    }
}
break;
case 14:

break;
case 15:
     put_case ();
break;
case 16:
	rule_token = ptoken(SXSTACKtop()); /* pour le traitement d'erreur de defauter */
break;
case 18:
{
    terminal_token = ptoken(SXSTACKtop());
    decoder (terminal_token);
    if (xattr > 0) {
	if (!is_err) {
	    if (attr_nt[xattr][xtnt] == ' ') {
		sxerror (terminal_token.source_index, bnf_tables.err_titles[2][0]
			 ,"%sThis attribute has not been declared with this non-terminal symbol."
			 ,bnf_tables.err_titles[2]+1
		     );
		is_err = true;
	    }
	}
    }
    if (terminal_token.comment != NULL) {
	fputs (terminal_token.comment, sxstdout);
	/* sxfree (terminal_token.comment); */
    }
    coder (xattr, pos1);
}
break;
case 19:

break;
default:;
}

default:break;
} /* switch (code) */
} /* end def_act */
