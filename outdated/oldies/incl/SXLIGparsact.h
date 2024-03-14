/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1996 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */

/*	TRAITEMENT DES LIG		*/
/* Les actions et les predicats associes aux symboles de la bnf correspondent
   aux piles de la LIG initiale.
   L'analyse CF est effectuee par le parser Earley/LC
   Cette version genere une RDG (Rightmost Derivation Grammar). C'est la CFG dont
   le langage est l'ensemble des derivations droites valides de la LIG initiale qui
   derivent dans la chaine source.
*/


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Jeu 17 Oct 1996 11:53:35:	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "XxY.h"

#define Xpack	1
/* Les Aij sont definis par un X_header */
#include "semact.h"

static int		*_CTRLSTACK_T, _CTRLSTACK_SIZE;

#define CTRLSTACK_REALLOC(s,l)	s = ((int*) sxrealloc ((s)-1, l+2, sizeof (int)) + 1)
#define CTRLSTACK_OFLW(s,d)	(_CTRLSTACK_SIZE=(s)[-1]+=(s)[-1]+(d), CTRLSTACK_REALLOC (s,_CTRLSTACK_SIZE))
#define TOP(s)			(*(s))
#define CTRLSTACK_SIZE(s)	((s)[-1])
#define CTRLSTACK_IS_OFLW(s,d)	(TOP(s)+(d)>=CTRLSTACK_SIZE(s))
#define INCR(s)			(++*(s))
#define DECR(s)			((*(s))--)
#define PUSH(s,x)		(s)[INCR(s)]=(x)
#define CTRLSTACK_PUSH(s,x)	(((TOP(s)==CTRLSTACK_SIZE(s)) ? CTRLSTACK_OFLW(s,0) : 0), PUSH(s,x))
#define POP(s)			(s)[DECR(s)]
#define GET(s,x)		(s)[x]
#define IS_EMPTY(s)		(TOP(s)==0)
#define CTRLSTACK_CHECK(s,d)	(CTRLSTACK_IS_OFLW(s,d) ? CTRLSTACK_OFLW(s,d) : 0)
#define CTRLSTACK_ALLOC(l)	(_CTRLSTACK_T = (int*) sxalloc (l+2, sizeof (int))+1, _CTRLSTACK_T[0]=0, _CTRLSTACK_T[-1]=l, _CTRLSTACK_T)
#if statistics
static void
_CTRLSTACK_FREE(s,n)
    int	*s;
    char *n;
{
    fprintf (sxtty, "name = %s, size = %i\n", n, s[-1]);
    sxfree (s-1);
}

#define CTRLSTACK_FREE(s,n) 	_CTRLSTACK_FREE(s,n), s=NULL
#else
#define CTRLSTACK_FREE(s,n) 	sxfree(s-1), s=NULL
#endif



#define SECONDARY	0
#define PRIMARY		1

#define IS_PUSHPOP	01
#define IS_PUSHPOP_EQ1	02
#define IS_PUSHPOP_EQn	04
#define IS_EQ1_EQ1	010
#define IS_EQ1_EQn	020

#if is_initial_LIG==0
#include <values.h>
    /* Definit la constante HIBITS == 1<<15 pour les mots de 32! */
#endif

static int		*LFprod, *LFprolon, *LFlispro, *LFprod2next;
#if is_initial_LIG==0
static int		*LFlhs;
#endif
static int		xLFprod_top, xLFprod, xLFlispro_top, xLFlispro;
static bool		LFhas_cycles;

static XxY_header	EQUAL1_hd, PUSH1_hd, POP1_hd, Axs_hd;
static int		EQUAL1_top, POP1_top, PUSH1_top;

struct rel_attr {
    int	prod;
};

static struct rel_attr	*EQUAL12attr, *PUSH12attr, *POP12attr;
static struct rel_attr	*SECONDARY2attr;

#if SXLIGis_normal_form==0
static struct dum_attr {
    int			Aij;
    struct rel_attr	attr;
} *dumAij2attr;
static int		dumAij_top, dumAij;
#endif

#if is_initial_LIG==0
static SXBA		LIGcyclic_prod_set;
#endif

static XxY_header	EQUALn_hd, POPn_hd;
static int		*UNIQUE_stack, *EQUAL1_stack, *EQUALn_stack, *PUSHPOPn_stack, *POP1_stack, *POPn_stack;
static char		*EQUALn2kind;
#if SXLIGis_normal_form==0
static SXBA		POP1_is_local, EQUALn_is_local, POPn_is_local;
#endif

static int		EQUALn_top, POPn_top, PUSHPOPn_top;
static int		UNIQUE_nttop, EQUALn_ntbot, EQUALn_nttop, POPn_ntbot, POPn_nttop, PUSHPOPn_ntbot, PUSHPOPn_nttop;
static int		RDGtmin;

static SXBA		UNIQUE_set, EQUAL1_set, EQUALn_set, PUSHPOPn_set, POP1_set, POPn_set;


static int		RDGleft_delta, RDGxnt_size;
#if RDGis_left_part_empty==0
#define RDGnt2left_nt(A)	((A)==0?0:(A)+RDGleft_delta)
#define RDGleft_nt2nt(A)	(A-RDGleft_delta)
#define RDGis_left_nt(A)	(A>PUSHPOPn_nttop)
#endif

#define RDG_left		1
#define RDG_right		2
#define RDG_left_right		3


static int		*RDGlhs, *RDGprolon, *RDGlispro;
static int		xRDGprod_top, xRDGprod, xRDGlispro_top, xRDGlispro, RDGprod_nb;

static int		RDGtree_nb;
static int		*RDGlhs_hd, *RDGprod2next;
static int		RDGmax_rhs_lgth;

#if 0
#if is_initial_LIG==0
static bool		RDGhas_cycles;
static SXBA		RDGcyclic_nt_set, RDGprod_set, RDGnt_set2;
#endif
#endif

static int		*RDGnt_stack, *RDGrhsnt2prod, *RDGprod2ntnb, *RDGitem2prod;
static SXBA		RDGnt_set;

/* A FAIRE */
/* Si la grammaire n'est pas en forme normale et une forme tordue de grammaire cyclique,
   (ou ds le cas d'une LIG --pas une LIGed forest--)
   les elements (A, B) des relations ne permettent pas de determiner si l'evolution de
   la pile entre A et B est due uniquement au fait que la grammaire n'est pas en forme
   normale et donc ce NT va produire la chaine vide ds la RDG, et peut donc etre ignore
   ou si il faut en tenir compte.
   A(..) -> A(..)
   A(..) -> A(..a a)
   A(..a a) -> A(..)
   On a (A =n A) et donc check_locality(A,A)==true ce qui est faux.
   A priori 3 solutions :

   1)Noter a la construction de chaque relation un couple d'attributs (is_local, is_non_local)
   A la composition intersection des is_local et union des is_non_local
   check_locality(A rel B) == is_local & !is_non_local.
   PB: si le couple d'attributs (is_local, is_non_local) d'un element deja existant change, il faut
   propager recursivement ce changement sur tous les elements composes a partir de cet element!!

   2)Dans le cas douteux, check_locality retourne false et, apres generation, on rend la RDG
   epsilon free.

   3)Dans le cas douteux, check_locality retourne false et, on laisse ds la RDG les productions
   vides.

   On essaie la 2!
*/

/* check_locality(Aij,Bkl) ne peut etre vrai que si l'on a Aij <>n Bkl ou Aij <>n Cmn >1 Bkl */
#if SXLIGis_normal_form==1
#define check_locality(Aij,Bkl)	false
#else
static int		_A, _B;
#define check_equality(A,B)	((_A = ((A) < 0 ? dumAij2attr[-A].Aij : (A)), _B = ((B) < 0 ? dumAij2attr[-B].Aij : (B)), (_A == _B ? A != B : false)))
#define check_locality(Aij,Bkl)	(check_equality(Aij,Bkl))
#endif


#ifdef is_sxndparser
/* Le parser est sxndparser, il interprete donc un automate LR ou LC */
#include "sxnd.h"
#endif


static int
new_pa (new_act)
    int	new_act;
{
    int	*pnew_act, *pact, act, nb, i;

    pnew_act = &(SXLIGap_list [SXLIGap_disp [new_act]]);

    for (act = 2; act < new_act; act++) {
	pact = &(SXLIGap_list [SXLIGap_disp [act]]);

	if (*pact == *pnew_act && (nb = pact [1]) == pnew_act [1]) {
	    for (i = 1; i <= nb; i++) {
		if (pact [i+1] != pnew_act [i+1])
		    break;
	    }

	    if (i > nb)
		break;
	}
    }

    return act;
}

static void
LFprod_oflw ()
{
    xLFprod_top *= 2;
    LFprod = (int*) sxrealloc (LFprod, xLFprod_top+1, sizeof (int));
#if is_initial_LIG==0
    LFlhs = (int*) sxrealloc (LFlhs, xLFprod_top+1, sizeof (int));
#endif
    LFprolon = (int*) sxrealloc (LFprolon, xLFprod_top+1, sizeof (int));
    LFprod2next = (int*) sxrealloc (LFprod2next, xLFprod_top+1, sizeof (int));
}

static void
LFlispro_oflw ()
{
    xLFlispro_top *= 2;
    LFlispro = (int*) sxrealloc (LFlispro, xLFlispro_top+1, sizeof (int));
}

#if SXLIGis_normal_form==0
static void
dumAij_oflw ()
{
    dumAij_top *= 2;
    dumAij2attr = (struct dum_attr*) sxrealloc (dumAij2attr, dumAij_top+1, sizeof (struct dum_attr));
}
#endif

static void
RDGprod_oflw ()
{
    xRDGprod_top *= 2;
    RDGlhs = (int*) sxrealloc (RDGlhs, xRDGprod_top+1, sizeof (int));
    RDGprolon = (int*) sxrealloc (RDGprolon, xRDGprod_top+1, sizeof (int));
    RDGprod2ntnb = (int*) sxrealloc (RDGprod2ntnb, xRDGprod_top+1, sizeof (int));
    RDGprod2next = (int*) sxrealloc (RDGprod2next, xRDGprod_top+1, sizeof (int));
}

static void
RDGlispro_oflw ()
{
    xRDGlispro_top *= 2;
    RDGlispro = (int*) sxrealloc (RDGlispro, xRDGlispro_top+1, sizeof (int));
    RDGitem2prod = (int*) sxrealloc (RDGitem2prod, xRDGlispro_top+1, sizeof (int));
}

static void
EQUAL1_oflw (old_size, new_size)
    int		old_size, new_size;
{
    EQUAL12attr = (struct rel_attr*) sxrealloc (EQUAL12attr, new_size +1, sizeof (struct rel_attr));
}

static void
EQUALn_oflw (old_size, new_size)
    int		old_size, new_size;
{
    EQUALn2kind = (char*) sxrealloc (EQUALn2kind, new_size+1, sizeof (char));
    CTRLSTACK_REALLOC (EQUALn_stack, EQUAL1_top+new_size);
    CTRLSTACK_REALLOC (PUSHPOPn_stack, new_size);
#if SXLIGis_normal_form==0
    EQUALn_is_local = sxba_resize (EQUALn_is_local, new_size+1);
#endif    
}

static void
PUSH1_oflw (old_size, new_size)
    int		old_size, new_size;
{
    PUSH12attr = (struct rel_attr*) sxrealloc (PUSH12attr, new_size +1, sizeof (struct rel_attr));
}


static void
POP1_oflw (old_size, new_size)
    int		old_size, new_size;
{
    POP12attr = (struct rel_attr*) sxrealloc (POP12attr, new_size +1, sizeof (struct rel_attr));
#if SXLIGis_normal_form==0
    POP1_is_local = sxba_resize (POP1_is_local, new_size+1);
#endif    
}


static void
POPn_oflw (old_size, new_size)
    int		old_size, new_size;
{
    CTRLSTACK_REALLOC (POPn_stack, new_size);
#if SXLIGis_normal_form==0
    POPn_is_local = sxba_resize (POPn_is_local, new_size+1);
#endif    
}

static void
SXLIGalloc (size)
    int size;
{
        
    RDGmax_rhs_lgth = inputG.maxrhsnt+3; /* []d r G2 G1 []g  ou |G2 G1| == inputG.maxrhsnt */

    XxY_alloc (&Axs_hd, "Axs_hd", size+1 /* !! */, 1, 1, 0, NULL,
#ifdef statistics
	       sxtty
#else
	       NULL
#endif
	       );

    XxY_alloc (&EQUAL1_hd, "EQUAL1_hd", size+1 /* !! */, 1, 1, 1, EQUAL1_oflw,
#ifdef statistics
	       sxtty
#else
	       NULL
#endif
	       );

    EQUAL12attr = (struct rel_attr*) sxalloc (XxY_size (EQUAL1_hd) + 1, sizeof (struct rel_attr));

    XxY_alloc (&PUSH1_hd, "PUSH1_hd", size+1 /* !! */, 1, 1, 1, PUSH1_oflw,
#ifdef statistics
	       sxtty
#else
	       NULL
#endif
	       );

    PUSH12attr = (struct rel_attr*) sxalloc (XxY_size (PUSH1_hd) + 1, sizeof (struct rel_attr));

    XxY_alloc (&POP1_hd, "POP1_hd", size+1, 1, 1, 1, POP1_oflw,
#ifdef statistics
		 sxtty
#else
		 NULL
#endif
		 );

    POP12attr = (struct rel_attr*) sxalloc (XxY_size (POP1_hd) + 1, sizeof (struct rel_attr));
#if SXLIGis_normal_form==0
    POP1_is_local = sxba_calloc (XxY_size (POP1_hd) + 1);
#endif

    SECONDARY2attr = (struct rel_attr*) sxcalloc (size+1, sizeof (struct rel_attr));

    xLFprod_top = size;
    LFprod = (int*) sxalloc (xLFprod_top+1, sizeof (int));
#if is_initial_LIG==0
    LFlhs = (int*) sxalloc (xLFprod_top+1, sizeof (int));
#endif
    LFprolon = (int*) sxalloc (xLFprod_top+1, sizeof (int));
    LFprod2next = (int*) sxalloc (xLFprod_top+1, sizeof (int));
    
    xLFlispro_top = size;
    LFlispro = (int*) sxalloc (xLFlispro_top+1, sizeof (int));

    LFprod [xLFprod = 1] = 1;
    LFprolon [1] = xLFlispro = 1;

#if SXLIGis_normal_form==0
    dumAij_top = size;
    dumAij2attr = (struct dum_attr*) sxcalloc (dumAij_top+1, sizeof (struct dum_attr));
#endif

#if is_initial_LIG==0
    if (inputG.has_cycles)
	LIGcyclic_prod_set = sxba_calloc (inputG.maxprod+1);
#endif
}

#if is_initial_LIG==0
static void
SXLIG_semact_init (size)
    int size;
{
    /* On alloue les structures contenant les relations de niveau 1 */
    SXLIGalloc (size);
}
#endif

static int
SXLIG_semact (i, j, prod_core, rhs_stack)
    unsigned short	*prod_core;
    int 	i, j, rhs_stack [];
{
    int		prod = prod_core [0];
    int		A, B, AB, As, sB, AsB, Aij, secAij, Bkl, ssymb, X, keep;
    int		act_no, prdct_no, atype, ptype, pop_nb, push_nb, item, spf_item, nbnt;
    int		nb, Bpri, Bsec;
    int		*pca, *pcp;
#if is_initial_LIG==0

    if (inputG.has_cycles && (prod & HIBITS)) {
	/* On est dans le cas cyclique,
	   on a deja ete appele avec cette production =>
	   on previent le parser que le point fixe est atteint. */
	prod &= ~(HIBITS);
	SXBA_1_bit (LIGcyclic_prod_set, prod);
	LFhas_cycles = true;

	return 0;
    }
#endif

#if SXLIGis_reduced==0
    /* La grammaire initiale n'est pas reduite */
    if (!SXBA_bit_is_set (SXLIGt_set, prod))
	/* Cette production est inutile */
	return 1;
#endif

    /* La foret partagee ne contient que les NT */
    
    Aij = rhs_stack [0];
#if is_initial_LIG==0
    LFlhs [xLFprod] = Aij;
#endif
    act_no = SXLIGaction [prod];

#if SXLIGis_normal_form==0
    if (SXLIGprod2dum_nb [prod] - dumAij >= dumAij_top)
	dumAij_oflw ();
#endif

    if (act_no >= 0) {
	pca = SXLIGap_list + SXLIGap_disp [act_no];
	atype = *pca++;
	pop_nb = *pca;

	keep = (atype == PRIMARY) ? 1 : 0; /* On laise (au plus) keep pop */

#if SXLIGis_normal_form==0
	if (pop_nb > keep) {
	    B = Aij;

	    do {
		A = B;
		B = --dumAij;
		dumAij2attr [-dumAij].Aij = Aij;

		ssymb = *++pca;
		XxY_set (&Axs_hd, B, ssymb, &sB);

		if (!XxY_set (&POP1_hd, A, sB, &AsB)) {
		    /* Nouveau triplet AsB */
		    SXBA_1_bit (POP1_is_local, AsB);
		    POP12attr [AsB].prod = 0;
		}
	    } while (--pop_nb > keep);

	    Aij = B;
	}
#endif
	
    }
    else {
	pop_nb = 0;
	atype = SECONDARY;
    }

    if (atype == SECONDARY) {
#if SXLIGis_normal_form==0
	if (Aij < 0) {
	    Aij = -Aij;
	    dumAij2attr [Aij].attr.prod = xLFprod;
	    LFprod2next [xLFprod] = 0;
	}
	else {
	    LFprod2next [xLFprod] = SECONDARY2attr [Aij].prod;
	    SECONDARY2attr [Aij].prod = xLFprod;
	}
#else
	LFprod2next [xLFprod] = SECONDARY2attr [Aij].prod;
	SECONDARY2attr [Aij].prod = xLFprod;
#endif
    }
    
    LFprod [xLFprod] = prod;

    nbnt = inputG.prod2nbnt [prod];

    if (nbnt > 0) {
	if (xLFlispro + nbnt >= xLFlispro_top) LFlispro_oflw ();

	item = inputG.prolon [prod];
	i = 0;

	while ((X = inputG.lispro [item]) != 0) {
	    if (X > 0) {
		Bkl = rhs_stack [++i];
		prdct_no = SXLIGprdct [item];

		if (prdct_no >= 0) {
		    pcp = SXLIGap_list + SXLIGap_disp [prdct_no];
		    ptype = *pcp++;
		    push_nb = *pcp;

		    if (ptype == PRIMARY) {
#if SXLIGis_normal_form==1
			B = Bkl;
#else
			B = pop_nb+push_nb > 1 ? --dumAij : Bkl;

			if (B < 0)
			    dumAij2attr [-dumAij].Aij = Bkl;
#endif

			if (pop_nb == 1) {
			    ssymb = *++pca;
			    XxY_set (&Axs_hd, B, ssymb, &sB);

			    if (!XxY_set (&POP1_hd, Aij, sB, &AsB)) {
				/* Nouveau triplet AsB*/
				/* Ce POP1 n'est jamais local! */
				POP12attr [AsB].prod = 0;
			    }

			    LFprod2next [xLFprod] = POP12attr [AsB].prod;
			    POP12attr [AsB].prod = xLFprod;
#if SXLIGis_normal_form==0
			    A = B;
#endif
			}
			else {
			    /* pop_nb == 0 */
			    if (push_nb == 0) {
				if (!XxY_set (&EQUAL1_hd, Aij, Bkl, &AB)) {
				    EQUAL12attr [AB].prod = 0;
				}

				LFprod2next [xLFprod] = EQUAL12attr [AB].prod;
				EQUAL12attr [AB].prod = xLFprod;
			    }
			    else {
				ssymb = *++pcp;
				XxY_set (&Axs_hd, Aij, ssymb, &As);

				if (!XxY_set (&PUSH1_hd, As, B, &AsB))
				    PUSH12attr [AsB].prod = 0;

				LFprod2next [xLFprod] = PUSH12attr [AsB].prod;
				PUSH12attr [AsB].prod = xLFprod;
#if SXLIGis_normal_form==0
				if (--push_nb > 0)
				    A = B;
#endif
			    }
			}
			
#if SXLIGis_normal_form==0
			if (push_nb > 0) {
			   do {
				B = push_nb == 1 ? Bkl : --dumAij;

				if (B < 0)
				    dumAij2attr [-dumAij].Aij = Bkl;

				ssymb = *++pcp;
				XxY_set (&Axs_hd, A, ssymb, &As);
				XxY_set (&PUSH1_hd, As, B, &AsB);
				A = B;
			    } while (--push_nb > 0);
			}
#endif			
		    }
		    else {
			/* SECONDARY PREDICATE */
#if SXLIGis_normal_form==0
			if (push_nb > 0) {
			    B = --dumAij;
			    dumAij2attr [-dumAij].Aij = Bkl;

			    Bsec = pAij_top - B;

			    do {
				A = B;
				B = push_nb == 1 ? Bkl : --dumAij;

				if (B < 0) {
				    dumAij2attr [-dumAij].Aij = Bkl;
				}

				ssymb = *++pcp;
				XxY_set (&Axs_hd, A, ssymb, &As);
				XxY_set (&PUSH1_hd, As, B, &AsB);
			    } while (--push_nb > 0);

			    Bkl = Bsec;
			}
#endif

			LFlispro [xLFlispro] = Bkl;
			xLFlispro++;
		    }
		}
		else {
		    /* On ne met pas le primaire */
		    LFlispro [xLFlispro] = Bkl;
		    xLFlispro++;
		}
	    }

	    item++;
	}
    }

    if (++xLFprod >= xLFprod_top) LFprod_oflw ();

    LFprolon [xLFprod] = xLFlispro;

    return 1;
}




static void
n_level_relations ()
{
    int 	A, B, C, D, AB, AC, BC, AsB, BsC, BsD, CsD, As, sC, sD, ssymb;
    bool	is_new;
    char	*pkind;
#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
    int		A00, B00, C00, D00, sB00, A00s, sD00, Amain, Bmain, Cmain, Dmain;
#endif
#endif

    for (BC = XxY_top (EQUAL1_hd); BC > 0; BC--) {
	B = XxY_X (EQUAL1_hd, BC);
	C = XxY_Y (EQUAL1_hd, BC);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
#if EBUG
	if (B < 0 || C < 0)
	    sxtrap (ME, "n_level_relations");
#endif
	B00 = Aij2A (B);
	C00 = Aij2A (C);
#endif
#endif
	/* A EQUALn C = A EQUAL1 B et B EQUAL1 C  */
	XxY_Yforeach (EQUAL1_hd, B, AB) {
	    A = XxY_X (EQUAL1_hd, AB);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
	    if (SXLIGEQUALn_set != NULL) {
#if EBUG
		if (A < 0)
		    sxtrap (ME, "n_level_relations");
#endif
		A00 = Aij2A (A);

		if ((A00 != C00 || A == C) && !SXBA_bit_is_set (SXLIGEQUALn_set [A00], C00)) {
		    continue;
		}
	    }
#endif
#endif

	    if (!XxY_set (&EQUALn_hd, A, C, &AC)) {
#if SXLIGis_normal_form==0
#if EBUG
		if (check_locality (A, C))
		    sxtrap (ME, "n_level_relations");
#endif
#endif

		EQUALn2kind [AC] = IS_EQ1_EQ1;
		PUSH (EQUALn_stack, AC);
	    }
	}

	/* B POPn sD = B EQUAL1 C et C POP1 sD */
	XxY_Xforeach (POP1_hd, C, CsD) {
	    sD = XxY_Y (POP1_hd, CsD);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
	    if (SXLIGPOPn_set != NULL) {
		D = XxY_X (Axs_hd, sD);
		ssymb = XxY_Y (Axs_hd, sD);
#if SXLIGis_normal_form==1
		Dmain = D;
#else
		Dmain = dumAij2attr [-D].Aij;
#endif
		D00 = Aij2A (Dmain);
		sD00 = SXLIGAxs [D00] [ssymb];

#if EBUG
		if (sD00 == 0)
		    sxtrap (ME, "n_level_relations");
#endif

		if ((B00 != D00 || B == D) && !SXBA_bit_is_set (SXLIGPOPn_set [B00], sD00)) {
		    continue;
		}
	    }
#endif
#endif

	    if (!XxY_set (&POPn_hd, B, sD, &BsD)) {
#if SXLIGis_normal_form==0
#if EBUG
		D = XxY_X (Axs_hd, sD);

		if (check_locality (B, D))
		    sxtrap (ME, "n_level_relations");
#endif
#endif
		PUSH (POPn_stack, BsD);
	    }
	}
    }

    for (BsC = XxY_top (POP1_hd); BsC > 0; BsC--) {
	B = XxY_X (POP1_hd, BsC);
	sC = XxY_Y (POP1_hd, BsC);
	C = XxY_X (Axs_hd, sC);
	ssymb = XxY_Y (Axs_hd, sC);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
#if SXLIGis_normal_form==1
	Cmain = C;
#else
	Cmain = C > 0 ? C : dumAij2attr [-C].Aij;
#endif

	C00 = Aij2A (Cmain);
#endif
#endif

	/* A PUSHPOPn C = As PUSH1 B et B POP1 sC */
	XxY_Yforeach (PUSH1_hd, B, AsB) {
	    As = XxY_X (PUSH1_hd, AsB);
		
	    if (XxY_Y (Axs_hd, As) == ssymb) {
		A = XxY_X (Axs_hd, As);


#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
		if (SXLIGPUSHPOPn_set != NULL) {
#if SXLIGis_normal_form==1
		    Amain = A;
#else
		    Amain = A > 0 ? A : dumAij2attr [-A].Aij;
#endif
		    A00 = Aij2A (Amain);

		    if ((A00 != C00 || A == C) && !SXBA_bit_is_set (SXLIGPUSHPOPn_set [A00], C00)) {
			continue;
		    }
		}
#endif
#endif

		is_new = !XxY_set (&EQUALn_hd, A, C, &AC);
		/* check_locality (A, C) peut etre vrai si on n'est pas en forme normale */
#if SXLIGis_normal_form==0
		if (check_locality (A, C))
		    SXBA_1_bit (EQUALn_is_local, AC);
#endif
		pkind = &(EQUALn2kind [AC]);

		if (is_new)
		    *pkind = (char)0;

		if ((*pkind & IS_PUSHPOP) == 0) {
		    *pkind |= IS_PUSHPOP;
		    PUSH (PUSHPOPn_stack, AC);

		    if (is_new)
			PUSH (EQUALn_stack, AC);
		}
	    }
	}
    }

    while (*EQUALn_stack + *POPn_stack + *PUSHPOPn_stack) {
	while (!IS_EMPTY (EQUALn_stack)) {
	    BC = POP (EQUALn_stack);
	    B = XxY_X (EQUALn_hd, BC);
	    C = XxY_Y (EQUALn_hd, BC);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
#if SXLIGis_normal_form==1
	    Cmain = C;
#else
	    Cmain = C > 0 ? C : dumAij2attr [-C].Aij;
#endif
	    C00 = Aij2A (Cmain);
#endif
#endif

	    /* A EQUALn C = A EQUAL1 B et B EQUALn C  */
#if SXLIGis_normal_form==0
	    /* Les elements d'un EQUAL1 sont toujours > 0 */
	    if (B > 0)
#endif
	    XxY_Yforeach (EQUAL1_hd, B, AB) {
		A = XxY_X (EQUAL1_hd, AB);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
		if (SXLIGEQUALn_set != NULL) {
#if SXLIGis_normal_form==1
		    Amain = A;
#else
		    Amain = A > 0 ? A : dumAij2attr [-A].Aij;
#endif

		    A00 = Aij2A (Amain);

		    if ((A00 != C00 || A == C) && !SXBA_bit_is_set (SXLIGEQUALn_set [A00], C00)) {
			continue;
		    }
		}
#endif
#endif

		is_new = !XxY_set (&EQUALn_hd, A, C, &AC);
#if SXLIGis_normal_form==0
#if EBUG
		if (check_locality (A, C))
		    sxtrap (ME, "n_level_relations");
#endif
#endif
		pkind = &(EQUALn2kind [AC]);

		if (is_new)
		    *pkind = (char)0;

		if ((*pkind & IS_EQ1_EQn) == 0) {
		    *pkind |= IS_EQ1_EQn;
		    PUSH (EQUALn_stack, AC);
		}
	    }

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
#if SXLIGis_normal_form==1
	    Bmain = B;
#else
	    Bmain = B > 0 ? B : dumAij2attr [-B].Aij;
#endif
	    B00 = Aij2A (Bmain);
#endif
#endif

	    /* B POPn sD = B EQUALn C et C POP1 sD */
	    XxY_Xforeach (POP1_hd, C, CsD) {
		sD = XxY_Y (POP1_hd, CsD);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
		if (SXLIGPOPn_set != NULL) {
		    D = XxY_X (Axs_hd, sD);
		    ssymb = XxY_Y (Axs_hd, sD);
#if SXLIGis_normal_form==1
		    Dmain = D;
#else
		    Dmain = dumAij2attr [-D].Aij;
#endif
		    D00 = Aij2A (Dmain);
		    sD00 = SXLIGAxs [D00] [ssymb];

#if EBUG
		    if (sD00 == 0)
			sxtrap (ME, "n_level_relations");
#endif

		    if ((B00 != D00 || B == D) && !SXBA_bit_is_set (SXLIGPOPn_set [B00], sD00)) {
			continue;
		    }
		}
#endif
#endif
		if (!XxY_set (&POPn_hd, B, sD, &BsD)) {
#if SXLIGis_normal_form==0
		    if (EQUALn2kind [BC] == IS_PUSHPOP) {
			D = XxY_X (Axs_hd, sD);

			if (check_locality (B, D))
			    SXBA_1_bit (POPn_is_local, BsD);
		    }

#if EBUG
		    D = XxY_X (Axs_hd, sD);

		    if ((EQUALn2kind [BC] & (~(char)IS_PUSHPOP)) && check_locality (B, D))
			sxtrap (ME, "n_level_relations");
#endif
#endif
		    PUSH (POPn_stack, BsD);
		}
	    }

	    /* A EQUALn C = A PUSHPOPn B et B EQUALn C et  */
	    XxY_Yforeach (EQUALn_hd, B, AB) {
		if (EQUALn2kind [AB] & IS_PUSHPOP) {
		    A = XxY_X (EQUALn_hd, AB);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
		    if (SXLIGEQUALn_set != NULL) {
#if SXLIGis_normal_form==1
			Amain = A;
#else
			Amain = A > 0 ? A : dumAij2attr [-A].Aij;
#endif
			A00 = Aij2A (Amain);

			if ((A00 != C00 || A == C) && !SXBA_bit_is_set (SXLIGEQUALn_set [A00], C00)) {
			    continue;
			}
		    }
#endif
#endif
		    is_new = !XxY_set (&EQUALn_hd, A, C, &AC);
#if SXLIGis_normal_form==0
#if EBUG
		    if (check_locality (A, C))
			sxtrap (ME, "n_level_relations");
#endif
#endif

		    pkind = &(EQUALn2kind [AC]);

		    if (is_new)
			*pkind = (char)0;

		    if ((*pkind & IS_PUSHPOP_EQn) == 0) {
			*pkind |= IS_PUSHPOP_EQn;
			PUSH (EQUALn_stack, AC);
		    }
		}
	    }
	}

	while (!IS_EMPTY (POPn_stack)) {
	    BsC = POP (POPn_stack);
	    B = XxY_X (POPn_hd, BsC);
	    sC = XxY_Y (POPn_hd, BsC);
	    C = XxY_X (Axs_hd, sC);
	    ssymb = XxY_Y (Axs_hd, sC);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
#if SXLIGis_normal_form==1
	    Cmain = C;
#else
	    Cmain = dumAij2attr [-C].Aij;
#endif
	    C00 = Aij2A (Cmain);
#endif
#endif

	    /* A PUSHPOPn C = As PUSH1 B et B POPn sC */
	    XxY_Yforeach (PUSH1_hd, B, AsB) {
		As = XxY_X (PUSH1_hd, AsB);
		
		if (XxY_Y (Axs_hd, As) == ssymb) {
		    A = XxY_X (Axs_hd, As);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
		    if (SXLIGPUSHPOPn_set != NULL) {
#if SXLIGis_normal_form==1
			Amain = A;
#else
			Amain = A > 0 ? A : dumAij2attr [-A].Aij;
#endif
			A00 = Aij2A (Amain);

			if ((A00 != C00 || A == C) && !SXBA_bit_is_set (SXLIGPUSHPOPn_set [A00], C00)) {
			    continue;
			}
		    }
#endif
#endif
		    is_new = !XxY_set (&EQUALn_hd, A, C, &AC);
#if SXLIGis_normal_form==0
		    if (check_locality (A, C))
			SXBA_1_bit (EQUALn_is_local, AC);
#endif
		    pkind = &(EQUALn2kind [AC]);

		    if (is_new)
			*pkind = (char)0;

		    if ((*pkind & IS_PUSHPOP) == 0) {
			*pkind |= IS_PUSHPOP;
			PUSH (PUSHPOPn_stack, AC);

			if (is_new)
			    PUSH (EQUALn_stack, AC);
		    }
		}
	    }
	}

	while (!IS_EMPTY (PUSHPOPn_stack)) {
	    AB = POP (PUSHPOPn_stack);
	    A = XxY_X (EQUALn_hd, AB);
	    B = XxY_Y (EQUALn_hd, AB);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
#if SXLIGis_normal_form==1
	    Amain = A;
#else
	    Amain = dumAij2attr [-A].Aij;
#endif
	    A00 = Aij2A (Amain);
#endif
#endif

	    /* A EQUALn C = A PUSHPOPn B et B EQUAL1 C */
#if SXLIGis_normal_form==0
	    /* Les elements d'un EQUAL1 sont toujours > 0 */
	    if (B > 0)
#endif
	    XxY_Xforeach (EQUAL1_hd, B, BC) {
		C = XxY_Y (EQUAL1_hd, BC);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
		if (SXLIGEQUALn_set != NULL) {
#if SXLIGis_normal_form==1
		    Cmain = C;
#else
		    Cmain = C > 0 ? C : dumAij2attr [-C].Aij;
#endif
		    C00 = Aij2A (Cmain);

		    if ((A00 != C00 || A == C) && !SXBA_bit_is_set (SXLIGEQUALn_set [A00], C00)) {
			continue;
		    }
		}
#endif
#endif

		is_new = !XxY_set (&EQUALn_hd, A, C, &AC);
#if SXLIGis_normal_form==0
#if EBUG
		if (check_locality (A, C))
		    sxtrap (ME, "n_level_relations");
#endif
#endif
		pkind = &(EQUALn2kind [AC]);

		if (is_new)
		    *pkind = (char)0;

		if ((*pkind & IS_PUSHPOP_EQ1) == 0) {
		    *pkind |= IS_PUSHPOP_EQ1;
		    PUSH (EQUALn_stack, AC);
		}
	    }

	    /* A EQUALn C = A PUSHPOPn B et B EQUALn C */
	    XxY_Xforeach (EQUALn_hd, B, BC) {
		C = XxY_Y (EQUALn_hd, BC);

#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
		if (SXLIGEQUALn_set != NULL) {
#if SXLIGis_normal_form==1
		    Cmain = C;
#else
		    Cmain = C > 0 ? C : dumAij2attr [-C].Aij;
#endif
		    C00 = Aij2A (Cmain);

		    if ((A00 != C00 || A == C) && !SXBA_bit_is_set (SXLIGEQUALn_set [A00], C00)) {
			continue;
		    }
		}
#endif
#endif

		is_new = !XxY_set (&EQUALn_hd, A, C, &AC);
#if SXLIGis_normal_form==0
#if EBUG
		if (check_locality (A, C))
		    sxtrap (ME, "n_level_relations");
#endif
#endif
		pkind = &(EQUALn2kind [AC]);

		if (is_new)
		    *pkind = (char)0;

		if ((*pkind & IS_PUSHPOP_EQn) == 0) {
		    *pkind |= IS_PUSHPOP_EQn;
		    PUSH (EQUALn_stack, AC);
		}
	    }
	}
    }

#if is_initial_LIG==0
    if (is_print_time)
	sxtime (TIME_FINAL, "\tSemantic Pass: Relations");
#endif
}

static void
RDGprint_nt (AB)
    int AB;
{
    int 	A, B, sB, ssymb, Aij, Bkl, i, j, k, l, dum, nt_part;
    char	*op, *kind_str;
    char	*Aijstr, *Bklstr;

#if RDGis_left_part_empty==0
    if (RDGis_left_nt (AB)) {
	AB = RDGleft_nt2nt (AB);
	kind_str = " l";
    }
    else
	kind_str = " r";
#else
    kind_str = "";
#endif

    if (AB <= EQUALn_nttop) {
	if (AB <= UNIQUE_nttop) {
	    /* A */
	    kind_str = "";

#if SXLIGis_normal_form==1
	    A = AB;
#else
	    if ((dum = pAij_top - AB) < 0)
		A = dum;
	    else
		A = AB;
#endif
	    op = NULL;
	    ssymb = B = 0;
	}
	else {
	    /* A =n B ou A =1 B */
	    AB -= EQUALn_ntbot;

	    if (AB <= EQUAL1_top) {
		A = XxY_X (EQUAL1_hd, AB);
		B = XxY_Y (EQUAL1_hd, AB);
		op = "=1";
	    }
	    else {
		AB -= EQUAL1_top;
		A = XxY_X (EQUALn_hd, AB);
		B = XxY_Y (EQUALn_hd, AB);
		op = "=n";
	    }

	    ssymb = 0;
	}
    }
    else {
	if (AB <= POPn_nttop) {
	    /* A >1 B ou A >n B */
	    AB -= POPn_ntbot;

	    if (AB <= POP1_top) {
		A = XxY_X (POP1_hd, AB);
		sB = XxY_Y (POP1_hd, AB);
		op = "}1";
	    }
	    else {
		AB -= POP1_top;
		A = XxY_X (POPn_hd, AB);
		sB = XxY_Y (POPn_hd, AB);
		op = "}n";
	    }

	    ssymb = XxY_Y (Axs_hd, sB);
	    B = XxY_X (Axs_hd, sB);
	}
	else {
	    /* A <>n B */
	    AB -= PUSHPOPn_ntbot;
	    A = XxY_X (EQUALn_hd, AB);
	    B = XxY_Y (EQUALn_hd, AB);
	    op = "{}n";
	    ssymb = 0;
	}
    }

#if SXLIGis_normal_form==1
    Aij = A;
    Bkl = B;
    A = B = 0;
#else
    if (A < 0) {
	A = -A;
	Aij = dumAij2attr [A].Aij;
    }
    else {
	Aij = A;
	A = 0;
    }
	
    if (B < 0) {
	B = -B;
	Bkl = dumAij2attr [B].Aij;
    }
    else {
	Bkl = B;
	B = 0;
    }
#endif


#if is_initial_LIG==1
    Aijstr = inputG.ntstring [Aij];

    if (A == 0)
	printf ("<%s", Aijstr);
    else
	printf ("<%s(%i)", Aijstr, A);
#else
    nt_part = Aij2A (Aij);
    Aijstr = inputG.ntstring [nt_part];

    i = Aij2i (Aij);
    j = Aij2j (Aij);

    if (A == 0)
	printf ("<%s[%i..%i]", Aijstr, i, j);
    else
	printf ("<%s(%i)[%i..%i]", Aijstr, A, i, j);
#endif

    if (ssymb != 0)
	printf (" |%s|%s", SXLIGssymbstring [ssymb], op);
    else
	if (op != NULL)
	    printf (" %s", op);

    if (Bkl != 0) {
#if is_initial_LIG==1
	Bklstr = inputG.ntstring [Bkl];

	if (B == 0)
	    printf (" %s", Bklstr);
	else
	    printf (" %s(%i)", Bklstr, B);
#else
	nt_part = Aij2A (Bkl);
	Bklstr = inputG.ntstring [nt_part];

	k = Aij2i (Bkl);
	l = Aij2j (Bkl);

	if (B == 0)
	    printf (" %s[%i..%i]", Bklstr, k, l);
	else
	    printf (" %s(%i)[%i..%i]", Bklstr, B, k, l);
#endif
    }

    printf ("%s> ", kind_str);
}

#if 0
#if is_initial_LIG==0
static void
RDGgen_reduced ()
{
    int prod, lim, item, A, X, t;

    for (prod = 1; prod < xRDGprod; prod++) {
	A = RDGlhs [prod];

	if (SXBA_bit_is_set (RDGnt_set, A)) {
	    t = 0;

	    for (lim = RDGprolon [prod+1], item = RDGprolon [prod]; item < lim; item++) {
		X = RDGlispro [item];

		if (X < 0)
		    t = -X;
		else
		    if (!SXBA_bit_is_set (RDGnt_set, X))
			/* X is useless */
			break;
	    }

	    if (item == lim) {
		/* prod est utile */
		SXBA_1_bit (RDGprod_set, prod);

		if (t > 0 && SXBA_bit_is_set (LIGcyclic_prod_set, t))
		    SXBA_1_bit (RDGcyclic_nt_set, A);
	    }
	}
    }
}
#endif
#endif


static void
RDGprint ()
{
    int prod, lim, item, X;

    RDGprod_nb = 0;

    for (prod = 1; prod < xRDGprod; prod++) {
	for (lim = RDGprolon [prod+1], item = RDGprolon [prod]; item < lim; item++) {
	    X = RDGlispro [item];

	    if (X > 0 && !SXBA_bit_is_set (RDGnt_set, X))
		/* X is useless */
		break;
	}

	if (item == lim) {
	    RDGprod_nb++;
	    RDGprint_nt (RDGlhs [prod]);
	    fputs ("\t=\t", stdout);

	    for (item = RDGprolon [prod]; item < lim; item++) {
		X = RDGlispro [item];

		if (X < 0) {
		    X = LFprod [-X];
		    printf ("\"%i\" ", X);
		}
		else
		    RDGprint_nt (X);
	    }

	    fputs (" ;\n", stdout);
	}
    }

    printf ("\n* Nombre de productions de la grammaire reduite des derivations droites : %i\n", RDGprod_nb);
}

static void
RDGgen_rhs (lhs, pre_nt, lfprod, post_nt, kind)
    int lhs, pre_nt, lfprod, post_nt, kind;
{
    int nbnt = 0, lim, item, secCmn, prod, nb;

    RDGlhs [xRDGprod] = lhs;

    if (pre_nt != 0) {
	RDGitem2prod [xRDGlispro] = RDGrhsnt2prod [pre_nt];
	RDGrhsnt2prod [pre_nt] = xRDGprod;
	RDGlispro [xRDGlispro++] = pre_nt;
	nbnt++;
    }

    if (lfprod > 0) {
	prod = LFprod [lfprod];
	item = LFprolon [lfprod];

#if RDGis_left_part_empty==0
	/* Nb de nt secondaires a la gauche du nt primaire */
	nb = SXLIGprod2left_secnb [prod];

	if ((kind & RDG_left) == 0)
	    item += nb;

	if (kind & RDG_right) {
	    lim = LFprolon [lfprod+1];
	}
	else {
	    lim = item + nb;
	    prod = 0;
	}
#else
	lim = LFprolon [lfprod+1];
#endif

	if (prod > 0)
	    RDGlispro [xRDGlispro++] = -lfprod;

	while (item <= --lim) {
	    /* On genere de droite a gauche */
	    secCmn = LFlispro [lim];

	    if (SXBA_bit_is_reset_set (UNIQUE_set, secCmn)) {
		PUSH (UNIQUE_stack, secCmn);
	    }

	    /* On ne met pas 2 fois le meme */
	    if (RDGrhsnt2prod [secCmn] != xRDGprod) {
		RDGitem2prod [xRDGlispro] = RDGrhsnt2prod [secCmn];
		RDGrhsnt2prod [secCmn] = xRDGprod;
		nbnt++;
	    }

	    RDGlispro [xRDGlispro++] = secCmn;
	}

	
    }
    else
	prod = 0;

    if (post_nt != 0) {
	RDGitem2prod [xRDGlispro] = RDGrhsnt2prod [post_nt];
	RDGrhsnt2prod [post_nt] = xRDGprod;
	RDGlispro [xRDGlispro++] = post_nt;
	nbnt++;
    }

    if (nbnt == 0) {
#if EBUG
	if (lhs > RDGxnt_size)
	    sxtrap (ME, "RDGgen_rhs");
#endif

	RDGprod2next [xRDGprod] = RDGlhs_hd [lhs];
	RDGlhs_hd [lhs] = xRDGprod;

	if (SXBA_bit_is_reset_set (RDGnt_set, lhs)) {
	    /* La RHS ne contient que [T]  */
	    PUSH (RDGnt_stack, lhs);
	}

#if EBUG
	if (lfprod == 0)
	    sxtrap (ME, "RDGgen_rhs");
#endif

#if is_initial_LIG==1
	if (prod > 0)
	    SXBA_1_bit (RDGt_set, prod);
#endif
    }

    RDGprod2ntnb [xRDGprod] = nbnt;
    RDGprolon [++xRDGprod] = xRDGlispro;
}


static void
RDGgen_prod (lhs, pre_nt, rel_attr_ptr, post_nt, kind)
    int 		lhs, pre_nt, post_nt, kind;
    struct rel_attr	*rel_attr_ptr;
{
    int lfprod;

    lfprod = (rel_attr_ptr == NULL) ? 0 : rel_attr_ptr->prod;

    if (xRDGprod + 1 > xRDGprod_top) RDGprod_oflw ();
    if (xRDGlispro + RDGmax_rhs_lgth > xRDGlispro_top) RDGlispro_oflw ();

    RDGgen_rhs (lhs, pre_nt, lfprod, post_nt, kind);

    if (lfprod > 0) {
	while ((lfprod = LFprod2next [lfprod]) != 0) {
	    if (xRDGprod + 1 > xRDGprod_top) RDGprod_oflw ();
	    if (xRDGlispro + RDGmax_rhs_lgth > xRDGlispro_top) RDGlispro_oflw ();

	    RDGgen_rhs (lhs, pre_nt, lfprod, post_nt, kind);
	}
    }
}



static void
RDGuseful_symbols ()
{
    /* RDGnt_stack et RDGnt_set = {A | A -> w et w \in T*} */
    int	A, B, prod, item, X;

    while (!IS_EMPTY (RDGnt_stack)) {
	A = POP (RDGnt_stack);
	prod = RDGrhsnt2prod [A];
	
	while (prod != 0) {
	    /* Plusieurs occurrences du meme NT en RHS comptent pour un. */
	    if (--RDGprod2ntnb [prod] == 0) {
		B = RDGlhs [prod];

#if EBUG
		if (B > RDGxnt_size)
		    sxtrap (ME, "RDGuseful_symbols");
#endif

		RDGprod2next [prod] = RDGlhs_hd [B];
		RDGlhs_hd [B] = prod;

		if (SXBA_bit_is_reset_set (RDGnt_set, B))
		    PUSH (RDGnt_stack, B);

#if is_initial_LIG==1
		item = RDGprolon [prod];

		for (X = RDGlispro [item]; X != 0; X = RDGlispro [++item]) {
		    if (X < 0) {
			X = -X;
			SXBA_1_bit (RDGt_set, X);
		    }
		}
#endif
	    }

	    item = RDGprolon [prod];

	    for (X = RDGlispro [item]; X != A; X = RDGlispro [++item]);

	    prod = RDGitem2prod [item];
	}
    }
}

static void
RDGextraction (S0n)
    int S0n;
{
    int			AB, ABn, AC, ACn, BC, BCn, AsB, AsC, AsCn, BsC, BsCn, As, sC, ssymb;
    int			Aij, secAij, Bkl, Bmn, Ckl;
    char		*pkind;
    struct rel_attr	*rel_attr_ptr;


    if (SXBA_bit_is_reset_set (UNIQUE_set, S0n))
	PUSH (UNIQUE_stack, S0n);

    do {
	/* On choisit l'ordre d'execution :
	   UNIQUE EQUALn POPn PUSHPOPn POP1 EQUAL1 */
	while (!IS_EMPTY (UNIQUE_stack)) {
	    secAij = POP (UNIQUE_stack);

#if SXLIGis_normal_form==1
	    Aij = secAij;
	    rel_attr_ptr = &(SECONDARY2attr [Aij]);
#else
	    if ((Aij = pAij_top-secAij) < 0) {
		rel_attr_ptr = &(dumAij2attr [-Aij].attr);
	    }
	    else {
		Aij = secAij;
		rel_attr_ptr = &(SECONDARY2attr [Aij]);
	    }
#endif
	    
	    if (rel_attr_ptr->prod != 0)
		/* On cherche les Aij-productions secondaires */
		RDGgen_prod (secAij, 0, rel_attr_ptr, 0, RDG_left_right);

	    /* On cherche les Bkl-productions secondaires telles que Aij =1 Bkl */
	    XxY_Xforeach (EQUAL1_hd, Aij, AB) {
		Bkl =  XxY_Y (EQUAL1_hd, AB);
#if SXLIGis_normal_form==0
#if EBUG
		if (Bkl < 0)
		    sxtrap (ME, "RDGextraction");
#endif
#endif

		rel_attr_ptr = &(SECONDARY2attr [Bkl]);

		if (rel_attr_ptr->prod != 0) {
		    if (SXBA_bit_is_reset_set (EQUAL1_set, AB))
			PUSH (EQUAL1_stack, AB);

		    /* On parcourt les Bkl-productions secondaires de la foret partagee */
#if RDGis_left_part_empty==0
		    RDGgen_prod (secAij, (EQUALn_ntbot + AB), rel_attr_ptr,
				 RDGnt2left_nt (EQUALn_ntbot + AB), RDG_left_right);
#else
		    RDGgen_prod (secAij, (EQUALn_ntbot + AB), rel_attr_ptr,
				 0, RDG_left_right);
#endif
		}
	    }

	    /* On cherche les Bkl-productions secondaires telles que Aij =n Bkl */
	    XxY_Xforeach (EQUALn_hd, Aij, AB) {
		Bkl =  XxY_Y (EQUALn_hd, AB);

#if SXLIGis_normal_form==1
		rel_attr_ptr = &(SECONDARY2attr [Bkl]);
#else
		rel_attr_ptr = (Bkl < 0) ? &(dumAij2attr [-Bkl].attr) : &(SECONDARY2attr [Bkl]);
#endif

		if (rel_attr_ptr->prod != 0) {
#if SXLIGis_normal_form==0
		    if (SXBA_bit_is_set (EQUALn_is_local, AB))
			ABn = 0;
		    else
#endif
		    {
			if (EQUALn2kind [AB] == IS_PUSHPOP) {
			    /* Optimisation si ce n'est qu'un PUSHPOP */
			    if (SXBA_bit_is_reset_set (PUSHPOPn_set, AB))
				PUSH (PUSHPOPn_stack, AB);

			    ABn = PUSHPOPn_ntbot + AB;
			}
			else {
			    if (SXBA_bit_is_reset_set (EQUALn_set, AB))
				PUSH (EQUALn_stack, AB);

			    ABn = EQUALn_ntbot + EQUAL1_top + AB;
			}
		    }

		    /* On parcourt les Bkl-productions secondaires de la foret partagee */
#if RDGis_left_part_empty==0
		    RDGgen_prod (secAij, ABn, rel_attr_ptr, RDGnt2left_nt(ABn), RDG_left_right);
#else
		    RDGgen_prod (secAij, ABn, rel_attr_ptr, 0, RDG_left_right);
#endif
		}
	    }
	}

	while (!IS_EMPTY (EQUALn_stack)) {
	    AC = POP (EQUALn_stack);
	    ACn = EQUALn_ntbot + EQUAL1_top + AC;
	    Aij = XxY_X (EQUALn_hd, AC);
	    Ckl = XxY_Y (EQUALn_hd, AC);
	    pkind = &(EQUALn2kind [AC]);

	    if (*pkind & IS_PUSHPOP) {
		/* On a Aij <>n Ckl */
		if (SXBA_bit_is_reset_set (PUSHPOPn_set, AC))
		    PUSH (PUSHPOPn_stack, AC);

		RDGgen_prod (ACn, PUSHPOPn_ntbot + AC, NULL, 0, RDG_right);
#if RDGis_left_part_empty==0
		RDGgen_prod (RDGnt2left_nt(ACn), RDGnt2left_nt(PUSHPOPn_ntbot + AC), NULL, 0, RDG_left);
#endif
	    }

	    /* On cherche Bmn tq Aij =n Ckl se decompose en Aij =1 Bmn =1 Ckl */
	    if (*pkind & IS_EQ1_EQ1) {
		XxY_Xforeach (EQUAL1_hd, Aij, AB) {
		    Bmn = XxY_Y (EQUAL1_hd, AB);

		    if ((BC = XxY_is_set (&EQUAL1_hd, Bmn, Ckl)) > 0) {
			if (SXBA_bit_is_reset_set (EQUAL1_set, BC))
			    PUSH (EQUAL1_stack, BC);

			RDGgen_prod (ACn, 0, &(EQUAL12attr [AB]), EQUALn_ntbot + BC, RDG_right);
#if RDGis_left_part_empty==0
			RDGgen_prod (RDGnt2left_nt(ACn), RDGnt2left_nt(EQUALn_ntbot + BC),
				     &(EQUAL12attr [AB]), 0, RDG_left);
#endif
		    }
		}
	    }

	    /* On cherche Bmn tq Aij =n Ckl se decompose en Aij =1 Bmn =n Ckl */
	    if (*pkind & IS_EQ1_EQn) {
		XxY_Xforeach (EQUAL1_hd, Aij, AB) {
		    Bmn = XxY_Y (EQUAL1_hd, AB);

		    if ((BC = XxY_is_set (&EQUALn_hd, Bmn, Ckl)) > 0) {
#if SXLIGis_normal_form==0
			if (SXBA_bit_is_set (EQUALn_is_local, BC))
			    BCn = 0;
			else
#endif
			{
			    if (EQUALn2kind [BC] == IS_PUSHPOP) {
				/* Optimisation si ce n'est qu'un PUSHPOP */
				if (SXBA_bit_is_reset_set (PUSHPOPn_set, BC))
				    PUSH (PUSHPOPn_stack, BC);

				BCn = PUSHPOPn_ntbot + BC;
			    }
			    else {
				if (SXBA_bit_is_reset_set (EQUALn_set, BC))
				    PUSH (EQUALn_stack, BC);

				BCn = EQUALn_ntbot + EQUAL1_top + BC;
			    }
			}

			RDGgen_prod (ACn, 0, &(EQUAL12attr [AB]), BCn, RDG_right);
#if RDGis_left_part_empty==0
			RDGgen_prod (RDGnt2left_nt(ACn), RDGnt2left_nt(BCn), &(EQUAL12attr [AB]), 0, RDG_left);
#endif
		    }
		}
	    }

	    /* On cherche Bmn tq Aij =n Ckl se decompose en Aij <>n Bmn =1 Ckl */
	    if (*pkind & IS_PUSHPOP_EQ1) {
		XxY_Xforeach (EQUALn_hd, Aij, AB) {
		    if (EQUALn2kind [AB] & IS_PUSHPOP) {
			Bmn = XxY_Y (EQUALn_hd, AB);

			if ((BC = XxY_is_set (&EQUAL1_hd, Bmn, Ckl)) > 0) {
			    /* C'est un bon */
			    if (SXBA_bit_is_reset_set (EQUAL1_set, BC))
				PUSH (EQUAL1_stack, BC);

#if SXLIGis_normal_form==0
			    if (SXBA_bit_is_set (EQUALn_is_local, AB))
				ABn = 0;
			    else
#endif
			    {
				if (SXBA_bit_is_reset_set (PUSHPOPn_set, AB))
				    PUSH (PUSHPOPn_stack, AB);

				ABn = PUSHPOPn_ntbot + AB;
			    }

			    RDGgen_prod (ACn, ABn, NULL, EQUALn_ntbot + BC, RDG_right);
#if RDGis_left_part_empty==0
			    RDGgen_prod (RDGnt2left_nt(ACn), RDGnt2left_nt(EQUALn_ntbot + BC),
					  NULL, RDGnt2left_nt(ABn), RDG_left);
#endif
			}
		    }
		}
	    }

	    /* On cherche Bmn tq Aij =n Ckl se decompose en Aij <>n Bmn =n Ckl */
	    if (*pkind & IS_PUSHPOP_EQn) {
		XxY_Xforeach (EQUALn_hd, Aij, AB) {
		    if (EQUALn2kind [AB] & IS_PUSHPOP) {
			Bmn = XxY_Y (EQUALn_hd, AB);

			if ((BC = XxY_is_set (&EQUALn_hd, Bmn, Ckl)) > 0) {
			    /* C'est un bon */
#if SXLIGis_normal_form==0
			    if (SXBA_bit_is_set (EQUALn_is_local, BC))
				BCn = 0;
			    else
#endif
			    {
				if (EQUALn2kind [BC] == IS_PUSHPOP) {
				    /* Optimisation si ce n'est qu'un PUSHPOP */
				    if (SXBA_bit_is_reset_set (PUSHPOPn_set, BC))
					PUSH (PUSHPOPn_stack, BC);

				    BCn = PUSHPOPn_ntbot + BC;
				}
				else {
				    if (SXBA_bit_is_reset_set (EQUALn_set, BC))
					PUSH (EQUALn_stack, BC);

				    BCn = EQUALn_ntbot + EQUAL1_top + BC;
				}
			    }

#if SXLIGis_normal_form==0
			    if (SXBA_bit_is_set (EQUALn_is_local, AB))
				ABn = 0;
			    else
#endif
			    {
				if (SXBA_bit_is_reset_set (PUSHPOPn_set, AB))
				    PUSH (PUSHPOPn_stack, AB);

				ABn = PUSHPOPn_ntbot + AB;
			    }
			    
			    RDGgen_prod (ACn, ABn, NULL, BCn, RDG_right);
#if RDGis_left_part_empty==0
			    RDGgen_prod (RDGnt2left_nt(ACn), RDGnt2left_nt(BCn), NULL, RDGnt2left_nt(ABn), RDG_left);
#endif
			}
		    }
		}
	    }
	}


	while (!IS_EMPTY (POPn_stack)) {
	    AsC = POP (POPn_stack);
	    AsCn = AsC+POP1_top;
	    Aij = XxY_X (POPn_hd, AsC);
	    sC = XxY_Y (POPn_hd, AsC);

	    /* On cherche Bmn tq Aij >ns Ckl se decompose en Aij =n Bmn >1s Ckl */
	    XxY_Yforeach (POP1_hd, sC, BsC) {
		Bmn = XxY_X (POP1_hd, BsC);

		if ((AB = XxY_is_set (&EQUAL1_hd, Aij, Bmn)) > 0) {
		    if (SXBA_bit_is_reset_set (EQUAL1_set, AB))
			PUSH (EQUAL1_stack, AB);

		    RDGgen_prod (POPn_ntbot + AsCn, EQUALn_ntbot + AB, &(POP12attr [BsC]), 0, RDG_right);
#if RDGis_left_part_empty==0
		    RDGgen_prod (RDGnt2left_nt(POPn_ntbot + AsCn), 0, &(POP12attr [BsC]),
				 RDGnt2left_nt(EQUALn_ntbot + AB), RDG_left);
#endif
		}

		if ((AB = XxY_is_set (&EQUALn_hd, Aij, Bmn)) > 0) {
#if SXLIGis_normal_form==0
		    if (SXBA_bit_is_set (EQUALn_is_local, AB))
			ABn = 0;
		    else
#endif
		    {
			if (EQUALn2kind [AB] == IS_PUSHPOP) {
			    /* Optimisation si ce n'est qu'un PUSHPOP */
			    if (SXBA_bit_is_reset_set (PUSHPOPn_set, AB))
				PUSH (PUSHPOPn_stack, AB);

			    ABn = PUSHPOPn_ntbot + AB;
			}
			else {
			    if (SXBA_bit_is_reset_set (EQUALn_set, AB))
				PUSH (EQUALn_stack, AB);

			    ABn = EQUALn_ntbot + EQUAL1_top + AB;
			}
		    }
		    
		    RDGgen_prod (POPn_ntbot + AsCn, ABn, &(POP12attr [BsC]), 0, RDG_right);
#if RDGis_left_part_empty==0
		    RDGgen_prod (RDGnt2left_nt(POPn_ntbot + AsCn), 0, &(POP12attr [BsC]),
				 RDGnt2left_nt(ABn), RDG_left);
#endif
		}
	    }
	}

	while (!IS_EMPTY (PUSHPOPn_stack)) {
	    AC = POP (PUSHPOPn_stack);
	    Aij = XxY_X (EQUALn_hd, AC);
	    Ckl = XxY_Y (EQUALn_hd, AC);

	    /* On cherche Bmn tq Aij <>n Ckl se decompose en Aij <1s Bmn >ns Ckl */
	    XxY_Xforeach (Axs_hd, Aij, As) {
		ssymb = XxY_Y (Axs_hd, As);

		if ((sC = XxY_is_set (&Axs_hd, Ckl, ssymb)) > 0) {
		    XxY_Xforeach (PUSH1_hd, As, AsB) {
			Bmn = XxY_Y (PUSH1_hd, AsB);

			if ((BsC = XxY_is_set (&POP1_hd, Bmn, sC)) > 0) {
			    /* On en tient un */
#if SXLIGis_normal_form==0
			    if (SXBA_bit_is_set (POP1_is_local, BsC))
				BsCn = 0;
			    else
#endif
			    {
				if (SXBA_bit_is_reset_set (POP1_set, BsC))
				    PUSH (POP1_stack, BsC);

				BsCn = POPn_ntbot + BsC;
			    }

			    RDGgen_prod (PUSHPOPn_ntbot + AC, 0, &(PUSH12attr [AsB]), BsCn, RDG_right);
#if RDGis_left_part_empty==0
			    RDGgen_prod (RDGnt2left_nt(PUSHPOPn_ntbot + AC), RDGnt2left_nt(BsCn),
					 &(PUSH12attr [AsB]), 0, RDG_left);
#endif
			}

			if ((BsC = XxY_is_set (&POPn_hd, Bmn, sC)) > 0) {
#if SXLIGis_normal_form==0
			    if (SXBA_bit_is_set (POPn_is_local, BsC))
				BsCn = 0;
			    else
#endif
			    {
				if (SXBA_bit_is_reset_set (POPn_set, BsC))
				    PUSH (POPn_stack, BsC);

				BsCn = POPn_ntbot + BsC + POP1_top;
			    }
	    
			    RDGgen_prod (PUSHPOPn_ntbot + AC, 0, &(PUSH12attr [AsB]), BsCn, RDG_right);
#if RDGis_left_part_empty==0
			    RDGgen_prod (RDGnt2left_nt(PUSHPOPn_ntbot + AC), RDGnt2left_nt(BsCn),
					 &(PUSH12attr [AsB]), 0, RDG_left);
#endif
			}
		    }
		}
	    }
	}

	while (!IS_EMPTY (POP1_stack)) {
	    AsC = POP (POP1_stack);
	    RDGgen_prod (POPn_ntbot + AsC, 0, &(POP12attr [AsC]), 0, RDG_right);
#if RDGis_left_part_empty==0
	    RDGgen_prod (RDGnt2left_nt(POPn_ntbot + AsC), 0, &(POP12attr [AsC]), 0, RDG_left);
#endif
	}

	while (!IS_EMPTY (EQUAL1_stack)) {
	    AC = POP (EQUAL1_stack);
	    RDGgen_prod (EQUALn_ntbot + AC, 0, &(EQUAL12attr [AC]), 0, RDG_right);
#if RDGis_left_part_empty==0
	    RDGgen_prod (RDGnt2left_nt(EQUALn_ntbot + AC), 0, &(EQUAL12attr [AC]), 0, RDG_left);
#endif
	}
    } while (*UNIQUE_stack + *EQUALn_stack + *POPn_stack);

    if (is_print_time) {
	fprintf (sxtty, "Nombre de productions de la grammaire des derivations droites : %i\n", xRDGprod-1);
    }
}



#if 0
#if is_initial_LIG==0
static bool
RDGseek_cycles (A)
    int A;
{
    int B, prod, item, X;
    bool	is_cycle;

    prod = RDGrhsnt2prod [A];
	
    while (prod != 0) {
	if (SXBA_bit_is_set (RDGprod_set, prod)) {
	    /* prod est utile */
	    B = RDGlhs [prod];

	    if (SXBA_bit_is_reset_set (RDGnt_set2, B)) {
		is_cycle = RDGseek_cycles (B);
		SXBA_0_bit (RDGnt_set2, B);
		SXBA_0_bit (RDGcyclic_nt_set, B); /* B est teste */
	    }
	    else {
		is_cycle = true;
	    }

	    if (is_cycle)
		return true;
	}

	item = RDGprolon [prod];

	for (X = RDGlispro [item]; X != A; X = RDGlispro [++item]);

	prod = RDGitem2prod [item];
    }

    return false;
}

static void
RDGtest_cycles ()
{
    /* On cherche si le cardinal du langage de la RDG est non borne' */
    int nt;

    nt = 0;

    while ((nt = sxba_scan (RDGcyclic_nt_set, nt)) > 0) {
	/* A est un candidat au test de recursivite */
	SXBA_1_bit (RDGnt_set2, nt);

	if (RDGseek_cycles (nt)) {
	    RDGhas_cycles = true;
	    return;
	}

	SXBA_0_bit (RDGnt_set2, nt);
	SXBA_0_bit (RDGcyclic_nt_set, nt); /* Deja teste */
    }
}
#endif
#endif


static bool
SXLIG_sem_pass (S0n)
    int S0n;
{
    EQUAL1_top = XxY_top (EQUAL1_hd);
    POP1_top = XxY_top (POP1_hd);
    PUSH1_top = XxY_top (PUSH1_hd);

    XxY_alloc (&EQUALn_hd, "EQUALn_hd", EQUAL1_top+POP1_top+1 /* !! */, 1, 1, 1, EQUALn_oflw,
#ifdef statistics
	       sxtty
#else
	       NULL
#endif
	       );

    EQUALn_stack = CTRLSTACK_ALLOC (EQUAL1_top+XxY_size (EQUALn_hd));
    PUSHPOPn_stack = CTRLSTACK_ALLOC (XxY_size (EQUALn_hd));	
    EQUALn2kind = (char*) sxalloc (XxY_size (EQUALn_hd)+1, sizeof (char));
#if SXLIGis_normal_form==0
    EQUALn_is_local = sxba_calloc (XxY_size (EQUALn_hd)+1);
#endif

    XxY_alloc (&POPn_hd, "POPn_hd", EQUAL1_top+POP1_top+1, 1, 0, 0, POPn_oflw,
#ifdef statistics
	       sxtty
#else
	       NULL
#endif
	       );

    POPn_stack = CTRLSTACK_ALLOC (XxY_size (POPn_hd));
#if SXLIGis_normal_form==0
    POPn_is_local = sxba_calloc (XxY_size (EQUALn_hd)+1);
#endif

    n_level_relations ();

    EQUAL1_set = sxba_calloc (EQUAL1_top+1);
    EQUAL1_stack = CTRLSTACK_ALLOC (EQUAL1_top);

    EQUALn_top = XxY_top (EQUALn_hd);
    EQUALn_set = sxba_calloc (EQUALn_top+1);

    POP1_set = sxba_calloc (POP1_top+1);
    POP1_stack = CTRLSTACK_ALLOC (POP1_top);

    POPn_top = XxY_top (POPn_hd);
    POPn_set = sxba_calloc (POPn_top+1);
    PUSHPOPn_top = XxY_top (EQUALn_hd);
    PUSHPOPn_set = sxba_calloc (PUSHPOPn_top+1);

    UNIQUE_nttop = pAij_top;
#if SXLIGis_normal_form==0
    UNIQUE_nttop += -dumAij;
#endif

    EQUALn_ntbot = UNIQUE_nttop;
    POPn_ntbot = EQUALn_nttop = UNIQUE_nttop + EQUAL1_top + EQUALn_top;
    PUSHPOPn_ntbot = POPn_nttop = EQUALn_nttop + POP1_top + POPn_top;
    PUSHPOPn_nttop = POPn_nttop + PUSHPOPn_top;
    RDGtmin = PUSHPOPn_nttop;
    /* Les NT de la RDG se partitionnent :
       0 		< 	[Aij] 			<= UNIQUE_nttop
       UNIQUE_nttop 	< 	[Aij =n Bkl] 		<= EQUALn_nttop
       EQUALn_nttop 	< 	[Aij >n Bkl Bkl] 	<= POPn_nttop
       POPn_nttop 	< 	[Aij <>n Bkl] 		<= PUSHPOPn_nttop

       Les nt "gauche" qui correspondent aux demi-arbres a la gauche des spines se codent
       par [X]g = [X]d + RDGleft_delta
       Les nt "UNIQUE" n'ayant pas de contre partie gauche.
       */


    /* On produit la RDG correspondant */
#if RDGis_left_part_empty==0
    RDGleft_delta = PUSHPOPn_nttop-UNIQUE_nttop;
#endif
    /* sinon RDGleft_delta==0 */
    RDGxnt_size = PUSHPOPn_nttop+RDGleft_delta;
    xRDGprod_top = pAij_top;
    RDGlhs = (int*) sxalloc (xRDGprod_top+1, sizeof (int));
    RDGprolon = (int*) sxalloc (xRDGprod_top+1, sizeof (int));
    xRDGlispro_top = pAij_top;
    RDGlispro = (int*) sxalloc (xRDGlispro_top+1, sizeof (int));

    RDGprolon [xRDGprod = 1] = xRDGlispro = 1;

    RDGnt_stack = CTRLSTACK_ALLOC (RDGxnt_size);
    RDGnt_set = sxba_calloc (RDGxnt_size+1);

    RDGrhsnt2prod = (int*) sxcalloc (RDGxnt_size+1, sizeof (int));
    RDGprod2ntnb = (int*) sxalloc (xRDGprod_top+1, sizeof (int));
    RDGitem2prod = (int*) sxalloc (xRDGlispro_top+1, sizeof (int));

    RDGlhs_hd = (int*) sxcalloc (RDGxnt_size+1, sizeof (int));
    RDGprod2next = (int*) sxalloc (xRDGprod_top+1, sizeof (int));

    UNIQUE_set = sxba_calloc (UNIQUE_nttop+1);
    UNIQUE_stack = CTRLSTACK_ALLOC (UNIQUE_nttop);
    
    RDGextraction (S0n);

    RDGuseful_symbols ();

#if 0
#if is_initial_LIG==0
    /* On regarde si le langage de la RDG est infini.
       En general c'est le cas sur la LIG initiale, on ne le teste donc pas */
    if (LFhas_cycles) {
	/* La foret partagee est cyclique */
	/* On cherche si le cardinal du langage de la RDG est non borne' */
	/* La LIG est cyclique, cette cyclicite se manifeste-t-elle sur ce source ? */
	/* On cherche si le cardinal du langage de la RDG est non borne' */
	RDGprod_set = sxba_calloc (xRDGprod+1);
	RDGcyclic_nt_set = sxba_calloc (RDGxnt_size+1);
	RDGnt_set2 = sxba_calloc (RDGxnt_size+1);

	RDGgen_reduced ();
	RDGtest_cycles ();

	if (RDGhas_cycles)
	    fputs ("\nWarning: there is an unbounded number of valid LIG parse trees.\n", sxtty);

	sxfree (RDGcyclic_nt_set), RDGcyclic_nt_set = NULL;
	sxfree (RDGprod_set), RDGprod_set = NULL;
	sxfree (RDGnt_set2), RDGnt_set2 = NULL;
    }
#endif
#endif

    CTRLSTACK_FREE (RDGnt_stack, "RDGnt_stack");
    sxfree (RDGrhsnt2prod), RDGrhsnt2prod = NULL;
    sxfree (RDGprod2ntnb), RDGprod2ntnb = NULL;
    sxfree (RDGitem2prod), RDGitem2prod = NULL;

    if (is_print_prod) {
	RDGprint ();
    }

    CTRLSTACK_FREE (EQUALn_stack, "EQUALn_stack");
    CTRLSTACK_FREE (PUSHPOPn_stack, "PUSHPOPn_stack");
#if SXLIGis_normal_form==0
    sxfree (EQUALn_is_local), EQUALn_is_local = NULL;
#endif
    CTRLSTACK_FREE (POPn_stack, "POPn_stack");
#if SXLIGis_normal_form==0
    sxfree (POPn_is_local), POPn_is_local = NULL;
#endif
    sxfree (EQUAL1_set), EQUAL1_set = NULL;
    CTRLSTACK_FREE (EQUAL1_stack, "EQUAL1_stack");
    sxfree (EQUALn_set), EQUALn_set = NULL;
    sxfree (POP1_set), POP1_set = NULL;
    CTRLSTACK_FREE (POP1_stack, "POP1_stack");
    sxfree (POPn_set), POPn_set = NULL;
    sxfree (PUSHPOPn_set), PUSHPOPn_set = NULL;
    sxfree (UNIQUE_set), UNIQUE_set = NULL;
    CTRLSTACK_FREE (UNIQUE_stack, "UNIQUE_stack");

#if is_initial_LIG==0
    if (is_print_time)
	sxtime (TIME_FINAL, "\tSemantic Pass: Rightmost Derivation Grammar");
#endif

    return true;
}

static void
SXLIG_free2 ()
{
    /* inutilise ds les features */
    XxY_free (&Axs_hd);
    XxY_free (&EQUAL1_hd);
    sxfree (EQUAL12attr), EQUAL12attr = NULL;
    XxY_free (&PUSH1_hd);
    sxfree (PUSH12attr), PUSH12attr = NULL;
    XxY_free (&POP1_hd);
    sxfree (POP12attr), POP12attr = NULL;
#if SXLIGis_normal_form==0
    sxfree (POP1_is_local), POP1_is_local = NULL;
#endif
    sxfree (SECONDARY2attr), SECONDARY2attr = NULL;

    sxfree (LFprolon), LFprolon = NULL;
    sxfree (LFprod2next), LFprod2next = NULL;
    sxfree (LFlispro), LFlispro = NULL;

#if SXLIGis_normal_form==0
    sxfree (dumAij2attr), dumAij2attr = NULL;
#endif

#if is_initial_LIG==0
    if (inputG.has_cycles)
	sxfree (LIGcyclic_prod_set), LIGcyclic_prod_set = NULL;
#endif

    XxY_free (&EQUALn_hd);
    sxfree (EQUALn2kind), EQUALn2kind = NULL;
    XxY_free (&POPn_hd);
}


static void
SXLIG_free3 ()
{

    sxfree (RDGlhs_hd), RDGlhs_hd = NULL;
    sxfree (RDGprod2next), RDGprod2next = NULL;
    sxfree (RDGlhs), RDGlhs = NULL;
    sxfree (RDGprolon), RDGprolon = NULL;
    sxfree (RDGlispro), RDGlispro = NULL;
    sxfree (RDGnt_set), RDGnt_set = NULL;

    sxfree (LFprod), LFprod = NULL;
#if is_initial_LIG==0
    sxfree (LFlhs), LFlhs = NULL;
#endif
}


#if is_initial_LIG==0
#if 0
static int		*RDGsuffix_stack, *RDGprefix_stack;

static void
RDGprint_phrase (stack)
    int *stack;
{
    int bot, top;

    printf ("Right tree #%i:", RDGtree_nb);

    for (top = TOP (stack), bot = 1; bot <= top; bot++)
	printf (" %i", stack [bot]);

    fputs ("\n", stdout);
}




static void
RDGgen_phrases ()
{
    int	Y, X, prod, bot, top, lgth, ptop, stop;

    if (IS_EMPTY (RDGsuffix_stack)) {
	/* On a une phrase ds prefix_stack */
	RDGtree_nb++;

	if (is_print_prod) {
	    RDGprint_phrase (RDGprefix_stack);
	}
    }
    else {
	if ((Y = POP (RDGsuffix_stack)) < 0) {
	    Y = LFprod [-Y];
	    CTRLSTACK_PUSH (RDGprefix_stack, Y);
	    RDGgen_phrases ();
	    DECR (RDGprefix_stack);
	}
	else {
	    ptop = TOP (RDGprefix_stack);
	    stop = TOP (RDGsuffix_stack);

	    for (prod = RDGlhs_hd [Y]; prod > 0; prod = RDGprod2next [prod]) {
		top = RDGprolon [prod+1];
		bot = RDGprolon [prod];

		while (bot < top) {
		    if ((X = RDGlispro [bot]) > 0)
			break;

		    CTRLSTACK_PUSH (RDGprefix_stack, -X);
		    bot++;
		}

		if ((lgth = top - bot) > 0) {
		    CTRLSTACK_OFLW (RDGsuffix_stack, lgth);

		    while (bot <= --top) {
			PUSH (RDGsuffix_stack, RDGlispro [top]);
		    }
		}

		RDGgen_phrases ();

		TOP (RDGsuffix_stack) = stop;
		TOP (RDGprefix_stack) = ptop;
	    }
	}

	CTRLSTACK_PUSH (RDGsuffix_stack, Y);
    }
}


static void
SXLIG_features (S0n)
    int S0n;
{
	
    if (!RDGhas_cycles) {
	/* A FAIRE : traitement du cas cyclique! */
	static void features_processing ();

	if (is_print_prod) {
	    RDGprefix_stack = CTRLSTACK_ALLOC (xRDGprod);
	    RDGsuffix_stack = CTRLSTACK_ALLOC (xRDGprod);

	    CTRLSTACK_PUSH (RDGsuffix_stack, S0n);

	    RDGgen_phrases ();

	    CTRLSTACK_FREE (RDGsuffix_stack, "RDGsuffix_stack");
	    CTRLSTACK_FREE (RDGprefix_stack, "RDGprefix_stack");
	}

	features_processing (S0n);
    }

}
#endif


static int
SXLIG_semantics (S0n)
    int	S0n;
{
    static void features_processing ();

#ifdef is_sxndparser
    /* Le param est exprime est un "parse_stack.symbols".
       Normalement, l'axiome de la LF est en LHS de la 1ere regle. */
    S0n = LFlhs [1];
#endif

    SXLIG_sem_pass (S0n);

    SXLIG_free2 ();

    features_processing (S0n);

    SXLIG_free3 ();

#ifdef is_sxndparser
    X_free (&Aij_hd);
#endif

    return 1;
}



/* Doit etre compile avec l'option -Dfeatures_action=L_features_action */
/* Les "features_action" peuvent etre appele depuis ici ou directement depuis
   le parser lorsque ces features sont associes a une CFG (et non a une LIG). */
/* sxsyntax appelle
   (*(tables->SXP_tables.semact)) (SXOPEN, tables);
   qui est connecte soit ici (avec SXLIG_actions) ds le cas des LIG
   soit avec L_features_action ds le cas CFG. Ce point d'entree ne fait rien.
   Il en est de meme de
   (*(tables->SXP_tables.semact)) (SXINIT, tables);
   (*(tables->SXP_tables.semact)) (SXFINAL, tables);
   (*(tables->SXP_tables.semact)) (SXSEMPASS, tables);
   (*(tables->SXP_tables.semact)) (SXCLOSE, tables);

   En revanche, le point d'entree
   (*(tables->analyzers.parser)) (SXACTION, tables);
   du parser appelle
   (*(sxplocals.SXP_tables.semact)) (SXACTION, &for_semact);
   qui va remplir tous les champs de for_semact :

struct for_semact {
    void	(*sem_init) (),
                (*sem_final) ();

    bool	(*prdct) (),
                (*parsact) (),
                (*semact) (),
                (*constraint) (),
                (*sem_pass) ();

    bool	need_Aij2A_i_j,
                need_pack_unpack;
}

   et la "semantique" sera appelee par l'intermediaire de la structure for_semact
   que ce soit depuis le parser ou depuis SXLIGparsact.
*/

#include "XH.h"
static struct for_semact	for_features;
static int			*RDGparse_stack;
static int			*RDGprefix_stack;
/* Un [sous-]arbre t est un element de RDGright_trees:
   t = (p, t1, ..., tm)
   ou p est une production de la foret partagee (cette prod a m nt en rhs) et
   (t1, ..., tm) est une liste dont les ti sont les sous-arbres
   associes aux nt de la rhs. */


static XH_header		RDGright_trees;
static SXBA			RDGright_tree_is_valid;

/* Utilises uniquement si LFhas_cycles */
static int			*RDGprod2used_nb;
static SXBA			LFprod_set, LFfixed_point_reached;

static void
RDGright_trees_oflw (old_size, new_size)
    int		old_size, new_size;
{
    RDGright_tree_is_valid = sxba_resize (RDGright_tree_is_valid, new_size+1);

    if (for_features.oflw != NULL)
	(*for_features.oflw) (new_size);
}


static void
RDGprint_tree (root)
    int	root;
{
    int		*params;
    int		i, j, k, Aij, lfprod, prod, nbnt, nt_part;
    char	*Aijstr;
	
    params = &(XH_list_elem (RDGright_trees, XH_X (RDGright_trees, root)));

    lfprod = params [0];

    prod = LFprod [lfprod];
    Aij = LFlhs [lfprod];
    nt_part = Aij2A (Aij);
    Aijstr = inputG.ntstring [nt_part];

    i = Aij2i (Aij);
    j = Aij2j (Aij);

    printf ("<%s[%i..%i]-%i> (", Aijstr, i, j, prod);

    nbnt = inputG.prod2nbnt [prod];

    for (k = 1; k <= nbnt; k++) {
	RDGprint_tree (params [k]);

	if (k < nbnt)
	    fputs (", ", stdout);
    }

    
    fputs (")", stdout);
}


static void
RDGgen_right_trees ()
{
    int		Y, X, prod, bot, top, lgth, ptop, stop, tree, new_tree, nbnt, lfprod;
    int		min, nb, next_min, ret_val;
    int		*params;
    bool	go_on, is_first_time;

    if (IS_EMPTY (RDGprefix_stack)) {
	/* On a reconnu un arbre */
	RDGtree_nb++;

	if (is_print_prod) {
	    tree = GET (RDGparse_stack, TOP (RDGparse_stack));
	    printf ("Tree #%i: ", RDGtree_nb);
	    RDGprint_tree (tree);
	    fputs ("\n", stdout);
	}
    }
    else {
	Y = POP (RDGprefix_stack);

	if (Y < 0 ) {
	    /* On fabrique un nouvel arbre */
	    lfprod = -Y;
	    XH_push (RDGright_trees, lfprod);
	    nbnt = inputG.prod2nbnt [LFprod [lfprod]];

	    for (top = TOP (RDGparse_stack), bot = top - nbnt + 1; bot <= top; bot++) {
		tree = GET (RDGparse_stack, bot);
		XH_push (RDGright_trees, tree);
	    }

	    TOP (RDGparse_stack) = top - nbnt;

	    ret_val = 0;
	    is_first_time = false;

	    if (XH_set (&RDGright_trees, &new_tree)) {
		/* Existe deja */
		if (SXBA_bit_is_set (RDGright_tree_is_valid, new_tree)) {
		    /* On continue */
		    go_on = true;
		}
		else {
		    /* Deja vu et contraintes incompatibles, abandon */
		    go_on = false;
		}
	    }
	    else {
		/* for_features.semact peut retourner 3 valeurs :
		   non : echec de l'evaluation sur l'arbre courant => abandon
		   oui : Les contraintes sur les "features" sont verifiees, on peut continuer
		   Dans le cas cyclique
		   oui+pt fixe atteint (HIBITS | 1) => abandonner si on retombe sur lfprod */

		params = &(XH_list_elem (RDGright_trees, XH_X (RDGright_trees, new_tree)));

		if (!LFhas_cycles || (is_first_time = SXBA_bit_is_reset_set (LFprod_set, lfprod))) {
		    /* for_features.semact != NULL */
		    if (for_features.semact == NULL || (*for_features.semact) (new_tree, params)) {
			/* Les contraintes sur les "features" sont verifiees, on peut
			   continuer */
			SXBA_1_bit (RDGright_tree_is_valid, new_tree);
			go_on = true;
		    }
		    else {
			/* Abandon de l'arbre */
			go_on = true;
		    }
		}
		else {
		    /* Detection d'une circularite */
		    if (SXBA_bit_is_set (LFfixed_point_reached, lfprod))
			/* Le point fixe a deja ete calcule : Abandon */
			go_on = false;
		    else {
			/* On previent for_features.semact que lfprod a deja ete trouve */
			ret_val = for_features.semact == NULL || (*for_features.semact) (new_tree | HIBITS, &params);

			if (ret_val != 0) {
			    /* Ca continue de marcher... */
			    SXBA_1_bit (RDGright_tree_is_valid, new_tree);
			    go_on = true;

			    if (ret_val &= HIBITS)
				/* Pt fixe atteint */
				SXBA_1_bit (LFfixed_point_reached, lfprod);
			}
			else {
			    go_on = false;
			}
		    }
		}
	    }

	    if (go_on) {
		CTRLSTACK_PUSH (RDGparse_stack, new_tree);
		RDGgen_right_trees ();
		DECR (RDGparse_stack);
	    }

	    /* remise en l'etat */
	    if (is_first_time)
		SXBA_0_bit (LFprod_set, lfprod);

	    if (ret_val != 0)
		SXBA_0_bit (LFfixed_point_reached, lfprod);

	    params = &(XH_list_elem (RDGright_trees, XH_X (RDGright_trees, new_tree)));

	    for (nb = 1; nb <= nbnt; nb++)
		PUSH (RDGparse_stack, params [nb]);
	}
	else {
	    stop = TOP (RDGprefix_stack);

	    if (LFhas_cycles) {
		/* La foret partagee est cyclique */
		/* Si la RDG est recursive, on execute les Y_prod de maniere equitable
		   (jusqu'a RDGmax_cycles). */
		min = -1;

		for (prod = RDGlhs_hd [Y]; prod > 0; prod = RDGprod2next [prod]) {
		    nb = RDGprod2used_nb [prod];

		    /* RDGmax_cycles est une option de compilation qui indique la profondeur
		       max de l'utilisation d'une regle recursive (-1 indique pas de limite) */
		    if ((RDGmax_cycles == -1 || nb <= RDGmax_cycles) &&
			(min == -1 || nb < min))
			min = nb;
		}
	    }
	    else
		min = 0;

	    while (min != -1) {
		next_min = -1;

		for (prod = RDGlhs_hd [Y]; prod > 0; prod = RDGprod2next [prod]) {
		    nb = LFhas_cycles ? RDGprod2used_nb [prod] : 0;

		    if (nb == min) {
			/* On execute prod */
			top = RDGprolon [prod+1];
			bot = RDGprolon [prod];

			while (bot < top) {
			    X = RDGlispro [bot];

			    if (X < 0)
				CTRLSTACK_PUSH (RDGprefix_stack, X);
			    else {
				if (SXBA_bit_is_set (RDGnt_set, X))
				    /* Il est utile */
				    CTRLSTACK_PUSH (RDGprefix_stack, X);
				else
				    /* On abandonne cette prod */
				    break;
			    }

			    bot++;
			}

			if (bot == top) {
			    /* prod est accessible */
			    if (LFhas_cycles)
				RDGprod2used_nb [prod]++;

			    RDGgen_right_trees ();

			    if (LFhas_cycles)
				RDGprod2used_nb [prod]--;
			}

			/* Remise en etat */
			TOP (RDGprefix_stack) = stop;

		    }
		    else {
			if (nb > min && (next_min == -1 || nb < next_min)) {
			    next_min = nb;
			}
		    }
		}

		min = next_min;
	    }
	}

	PUSH (RDGprefix_stack, Y);
    }
}


static void
features_processing (S0n)
    int S0n;
{
    /* initialisation de for_features */
    features_action (SXACTION, &for_features);

    XH_alloc (&RDGright_trees, "RDGright_trees", xLFprod, 1, 4, RDGright_trees_oflw,
#ifdef statistics
	       sxtty
#else
	       NULL
#endif
	       );
    
    RDGright_tree_is_valid = sxba_calloc (XH_size (RDGright_trees)+1);

    RDGparse_stack = CTRLSTACK_ALLOC (xRDGprod);
    RDGprefix_stack = CTRLSTACK_ALLOC (xRDGprod);

    if (LFhas_cycles) {
	RDGprod2used_nb = (int*) sxcalloc (xRDGprod+1, sizeof (int));
	LFprod_set = sxba_calloc (xLFprod+1);
	LFfixed_point_reached = sxba_calloc (xLFprod+1);
    }

    if (for_features.sem_init != NULL)
	(*for_features.sem_init) (XH_size (RDGright_trees));
    
    CTRLSTACK_PUSH (RDGprefix_stack, S0n);
    RDGtree_nb = 0;

    RDGgen_right_trees ();

    CTRLSTACK_FREE (RDGprefix_stack, "RDGprefix_stack");
    CTRLSTACK_FREE (RDGparse_stack, "RDGparse_stack");

    sxfree (RDGright_tree_is_valid), RDGright_tree_is_valid = NULL;

    if (RDGprod2used_nb != NULL) {
	sxfree (RDGprod2used_nb), RDGprod2used_nb = NULL;
	sxfree (LFprod_set), LFprod_set = NULL;
	sxfree (LFfixed_point_reached), LFfixed_point_reached = NULL;
    }

    if (for_features.sem_pass != NULL)
	(*for_features.sem_pass) (S0n);

    if (for_features.sem_final != NULL)
	(*for_features.sem_final) ();

    XH_free (&RDGright_trees);
}





#ifdef is_sxndparser
/* Le parser est sxndparser, il interprete donc un automate LR ou LC */

static int
SXLIG_ndsemact (rhs_stack)
    int		*rhs_stack;
{
    /* rhs_stack est une SS_stack */
    /* Version ou l'on transforme les nt (p, i, j) (et les prod de la foret partagee) de sxndparser en
       (lhs(p), i, j) */
    static int	start_seq_prod, lhs_seq;
    int		prod, act_no, prdct_no;
    int		A, B, AB, As, sB, AsB, Aij, secAij, Bkl, ssymb, keep;
    int		atype, ptype, pop_nb, push_nb;
    int		nb, Bpri, Bsec;
    int		gitem, rule_lgth, bot, top;
    int		*pca, *pcp;
    bool	is_in_seq;

    int		x, git, lfitem, bo, Xmn, i, j, k, l, range;

    bot = SS_bot (rhs_stack);
    Aij = SS_get (rhs_stack, bot);
    prod = XxY_X (parse_stack.symbols, Aij);

    if (prod == 0)
	/* On saute les instanciations du super axiome! */
	return 1;

#if SXLIGis_reduced==0
    /* La grammaire initiale n'est pas reduite */
    if (!SXBA_bit_is_set (SXLIGt_set, prod))
	/* Cette production est inutile */
	return 0;
#endif

    /* sxndparser est tel que les productions de la foret partagee parviennent en sequences qui regroupent
       toutes les pij-productions. */

    /* On cherche si la prod courante est la premiere d'une sequence */
    if (xLFprod > 1) {
	/* On compare avec la prod precedente */
	is_in_seq = (Aij == lhs_seq);
    }
    else {
	is_in_seq = false;
    }

    top = SS_top (rhs_stack);
    gitem = inputG.prolon [prod];

    if (is_in_seq) {
	/* On recherche si cette prod est equivalente a une de la sequence */
	for (x = start_seq_prod; x < xLFprod; x++) {
	    git = gitem;
	    lfitem = LFprolon [x];
	    bo = bot;

	    while (++bo < top) {
		if ((Bkl = SS_get (rhs_stack, bo)) > 0) {
		    prdct_no = SXLIGprdct [git];

		    if (prdct_no < 0 || SXLIGap_list [SXLIGap_disp [prdct_no]] == SECONDARY) {
			Xmn = LFlispro [lfitem++];

			/* Il faut comparer Bkl et Xmn */
			/* On est sur que les nt de inputG sont identiques */
			range = XxY_Y (parse_stack.symbols, Bkl);

			if (range < 0) {
			    k = -range;
			    l = k+1;
			}
			else {
			    k = XxY_X (parse_stack.ranges, range);
			    l = XxY_Y (parse_stack.ranges, range);
			}

			if (k != Aij2i (Xmn) || l != Aij2j (Xmn))
			    break;
		    }
		}

		git++;
	    }

	    if (bo == top)
		/* equivalence */
		return 1;
	}
    }
    else {
	start_seq_prod = xLFprod;
	lhs_seq = Aij;
    }

    A = inputG.lhs [XxY_X (parse_stack.symbols, Aij)];
    range = XxY_Y (parse_stack.symbols, Aij);

    if (range < 0) {
	i = -range;
	j = i+1;
    }
    else {
	i = XxY_X (parse_stack.ranges, range);
	j = XxY_Y (parse_stack.ranges, range);
    }

    MAKE_Aij (Aij, MAKE_A0j (A, j), i);

    LFlhs [xLFprod] = Aij;

    act_no = SXLIGaction [prod];

#if SXLIGis_normal_form==0
    if (SXLIGprod2dum_nb [prod] - dumAij >= dumAij_top)
	dumAij_oflw ();
#endif

    if (act_no >= 0) {
	pca = SXLIGap_list + SXLIGap_disp [act_no];
	atype = *pca++;
	pop_nb = *pca;

	keep = (atype == PRIMARY) ? 1 : 0; /* On laise (au plus) keep pop */

#if SXLIGis_normal_form==0
	if (pop_nb > keep) {
	    B = Aij;

	    do {
		A = B;
		B = --dumAij;
		dumAij2attr [-dumAij].Aij = Aij;

		ssymb = *++pca;
		XxY_set (&Axs_hd, B, ssymb, &sB);

		if (!XxY_set (&POP1_hd, A, sB, &AsB)) {
		    /* Nouveau triplet AsB */
		    SXBA_1_bit (POP1_is_local, AsB);
		    POP12attr [AsB].prod = 0;
		}
	    } while (--pop_nb > keep);

	    Aij = B;
	}
#endif
	
    }
    else {
	pop_nb = 0;
	atype = SECONDARY;
    }

    if (atype == SECONDARY) {
#if SXLIGis_normal_form==0
	if (Aij < 0) {
	    Aij = -Aij;
	    dumAij2attr [Aij].attr.prod = xLFprod;
	    LFprod2next [xLFprod] = 0;
	}
	else {
	    LFprod2next [xLFprod] = SECONDARY2attr [Aij].prod;
	    SECONDARY2attr [Aij].prod = xLFprod;
	}
#else
	LFprod2next [xLFprod] = SECONDARY2attr [Aij].prod;
	SECONDARY2attr [Aij].prod = xLFprod;
#endif
    }
    
    LFprod [xLFprod] = prod;

    if ((rule_lgth = top-bot) > 1) {
	if (xLFlispro + rule_lgth >= xLFlispro_top) LFlispro_oflw ();

	gitem = inputG.prolon [prod];

	while (++bot < top) {
	    if ((Bkl = SS_get (rhs_stack, bot)) > 0) {
		B = SXLIGlhs [XxY_X (parse_stack.symbols, Bkl)];
		range = XxY_Y (parse_stack.symbols, Bkl);

		if (range < 0) {
		    k = -range;
		    l = k+1;
		}
		else {
		    k = XxY_X (parse_stack.ranges, range);
		    l = XxY_Y (parse_stack.ranges, range);
		}

		MAKE_Aij (Bkl, MAKE_A0j (B, l), k);

		prdct_no = SXLIGprdct [gitem];

		if (prdct_no >= 0) {
		    pcp = SXLIGap_list + SXLIGap_disp [prdct_no];
		    ptype = *pcp++;
		    push_nb = *pcp;

		    if (ptype == PRIMARY) {
#if SXLIGis_normal_form==1
			B = Bkl;
#else
			B = pop_nb+push_nb > 1 ? --dumAij : Bkl;

			if (B < 0)
			    dumAij2attr [-dumAij].Aij = Bkl;
#endif

			if (pop_nb == 1) {
			    ssymb = *++pca;
			    XxY_set (&Axs_hd, B, ssymb, &sB);

			    if (!XxY_set (&POP1_hd, Aij, sB, &AsB)) {
				/* Nouveau triplet AsB*/
				/* Ce POP1 n'est jamais local! */
				POP12attr [AsB].prod = 0;
			    }

			    LFprod2next [xLFprod] = POP12attr [AsB].prod;
			    POP12attr [AsB].prod = xLFprod;
#if SXLIGis_normal_form==0
			    A = B;
#endif
			}
			else {
			    /* pop_nb == 0 */
			    if (push_nb == 0) {
				if (!XxY_set (&EQUAL1_hd, Aij, Bkl, &AB)) {
				    EQUAL12attr [AB].prod = 0;
				}

				LFprod2next [xLFprod] = EQUAL12attr [AB].prod;
				EQUAL12attr [AB].prod = xLFprod;
			    }
			    else {
				ssymb = *++pcp;
				XxY_set (&Axs_hd, Aij, ssymb, &As);

				if (!XxY_set (&PUSH1_hd, As, B, &AsB))
				    PUSH12attr [AsB].prod = 0;

				LFprod2next [xLFprod] = PUSH12attr [AsB].prod;
				PUSH12attr [AsB].prod = xLFprod;
#if SXLIGis_normal_form==0
				if (--push_nb > 0)
				    A = B;
#endif
			    }
			}
			
#if SXLIGis_normal_form==0
			if (push_nb > 0) {
			    do {
				B = push_nb == 1 ? Bkl : --dumAij;

				if (B < 0)
				    dumAij2attr [-dumAij].Aij = Bkl;

				ssymb = *++pcp;
				XxY_set (&Axs_hd, A, ssymb, &As);
				XxY_set (&PUSH1_hd, As, B, &AsB);
				A = B;
			    } while (--push_nb > 0);
			}
#endif			
		    }
		    else {
			/* SECONDARY PREDICATE */
#if SXLIGis_normal_form==0
			if (push_nb > 0) {
			    B = --dumAij;
			    dumAij2attr [-dumAij].Aij = Bkl;

			    Bsec = pAij_top - B;

			    do {
				A = B;
				B = push_nb == 1 ? Bkl : --dumAij;

				if (B < 0) {
				    dumAij2attr [-dumAij].Aij = Bkl;
				}

				ssymb = *++pcp;
				XxY_set (&Axs_hd, A, ssymb, &As);
				XxY_set (&PUSH1_hd, As, B, &AsB);
			    } while (--push_nb > 0);

			    Bkl = Bsec;
			}
#endif

			LFlispro [xLFlispro] = Bkl;
			xLFlispro++;
		    }
		}
		else {
		    /* On ne met pas le primaire */
		    LFlispro [xLFlispro] = Bkl;
		    xLFlispro++;
		}
	    }

	    gitem++;
	}
    }

    if (++xLFprod >= xLFprod_top) LFprod_oflw ();

    LFprolon [xLFprod] = xLFlispro;

    return 1;
}


#if 0
/* Version ou la foret partagee issue de sxndparser est utilisee telle quelle. */
static int
SXLIG_ndsemact (rhs_stack)
    int		*rhs_stack;
{
    /* rhs_stack est une SS_stack */
    int		prod, act_no, prdct_no;
    int		A, B, AB, As, sB, AsB, Aij, secAij, Bkl, ssymb, keep;
    int		atype, ptype, pop_nb, push_nb;
    int		nb, Bpri, Bsec;
    int		gitem, rule_lgth, bot, top;
    int		*pca, *pcp;

    bot = SS_bot (rhs_stack);
    Aij = SS_get (rhs_stack, bot);
    prod = XxY_X (parse_stack.symbols, Aij);

#if SXLIGis_reduced==0
    /* La grammaire initiale n'est pas reduite */
    if (!SXBA_bit_is_set (SXLIGt_set, prod))
	/* Cette production est inutile */
	return 0;
#endif

    top = SS_top (rhs_stack);

    LFlhs [xLFprod] = Aij;

    act_no = SXLIGaction [prod];

#if SXLIGis_normal_form==0
    if (SXLIGprod2dum_nb [prod] - dumAij >= dumAij_top)
	dumAij_oflw ();
#endif

    if (act_no >= 0) {
	pca = SXLIGap_list + SXLIGap_disp [act_no];
	atype = *pca++;
	pop_nb = *pca;

	keep = (atype == PRIMARY) ? 1 : 0; /* On laise (au plus) keep pop */

#if SXLIGis_normal_form==0
	if (pop_nb > keep) {
	    B = Aij;

	    do {
		A = B;
		B = --dumAij;
		dumAij2attr [-dumAij].Aij = Aij;

		ssymb = *++pca;
		XxY_set (&Axs_hd, B, ssymb, &sB);

		if (!XxY_set (&POP1_hd, A, sB, &AsB)) {
		    /* Nouveau triplet AsB */
		    SXBA_1_bit (POP1_is_local, AsB);
		    POP12attr [AsB].prod = 0;
		}
	    } while (--pop_nb > keep);

	    Aij = B;
	}
#endif
	
    }
    else {
	pop_nb = 0;
	atype = SECONDARY;
    }

    if (atype == SECONDARY) {
#if SXLIGis_normal_form==0
	if (Aij < 0) {
	    Aij = -Aij;
	    dumAij2attr [Aij].attr.prod = xLFprod;
	    LFprod2next [xLFprod] = 0;
	}
	else {
	    LFprod2next [xLFprod] = SECONDARY2attr [Aij].prod;
	    SECONDARY2attr [Aij].prod = xLFprod;
	}
#else
	LFprod2next [xLFprod] = SECONDARY2attr [Aij].prod;
	SECONDARY2attr [Aij].prod = xLFprod;
#endif
    }
    
    LFprod [xLFprod] = prod;

    if ((rule_lgth = top-bot) > 1) {
	if (xLFlispro + rule_lgth >= xLFlispro_top) LFlispro_oflw ();

	gitem = inputG.prolon [prod];

	while (++bot < top) {
	    if ((Bkl = SS_get (rhs_stack, bot)) > 0) {
		prdct_no = SXLIGprdct [gitem];

		if (prdct_no >= 0) {
		    pcp = SXLIGap_list + SXLIGap_disp [prdct_no];
		    ptype = *pcp++;
		    push_nb = *pcp;

		    if (ptype == PRIMARY) {
#if SXLIGis_normal_form==1
			B = Bkl;
#else
			B = pop_nb+push_nb > 1 ? --dumAij : Bkl;

			if (B < 0)
			    dumAij2attr [-dumAij].Aij = Bkl;
#endif

			if (pop_nb == 1) {
			    ssymb = *++pca;
			    XxY_set (&Axs_hd, B, ssymb, &sB);

			    if (!XxY_set (&POP1_hd, Aij, sB, &AsB)) {
				/* Nouveau triplet AsB*/
				/* Ce POP1 n'est jamais local! */
				POP12attr [AsB].prod = 0;
			    }

			    LFprod2next [xLFprod] = POP12attr [AsB].prod;
			    POP12attr [AsB].prod = xLFprod;
#if SXLIGis_normal_form==0
			    A = B;
#endif
			}
			else {
			    /* pop_nb == 0 */
			    if (push_nb == 0) {
				if (!XxY_set (&EQUAL1_hd, Aij, Bkl, &AB)) {
				    EQUAL12attr [AB].prod = 0;
				}

				LFprod2next [xLFprod] = EQUAL12attr [AB].prod;
				EQUAL12attr [AB].prod = xLFprod;
			    }
			    else {
				ssymb = *++pcp;
				XxY_set (&Axs_hd, Aij, ssymb, &As);

				if (!XxY_set (&PUSH1_hd, As, B, &AsB))
				    PUSH12attr [AsB].prod = 0;

				LFprod2next [xLFprod] = PUSH12attr [AsB].prod;
				PUSH12attr [AsB].prod = xLFprod;
#if SXLIGis_normal_form==0
				if (--push_nb > 0)
				    A = B;
#endif
			    }
			}
			
#if SXLIGis_normal_form==0
			if (push_nb > 0) {
			    do {
				B = push_nb == 1 ? Bkl : --dumAij;

				if (B < 0)
				    dumAij2attr [-dumAij].Aij = Bkl;

				ssymb = *++pcp;
				XxY_set (&Axs_hd, A, ssymb, &As);
				XxY_set (&PUSH1_hd, As, B, &AsB);
				A = B;
			    } while (--push_nb > 0);
			}
#endif			
		    }
		    else {
			/* SECONDARY PREDICATE */
#if SXLIGis_normal_form==0
			if (push_nb > 0) {
			    B = --dumAij;
			    dumAij2attr [-dumAij].Aij = Bkl;

			    Bsec = pAij_top - B;

			    do {
				A = B;
				B = push_nb == 1 ? Bkl : --dumAij;

				if (B < 0) {
				    dumAij2attr [-dumAij].Aij = Bkl;
				}

				ssymb = *++pcp;
				XxY_set (&Axs_hd, A, ssymb, &As);
				XxY_set (&PUSH1_hd, As, B, &AsB);
			    } while (--push_nb > 0);

			    Bkl = Bsec;
			}
#endif

			LFlispro [xLFlispro] = Bkl;
			xLFlispro++;
		    }
		}
		else {
		    /* On ne met pas le primaire */
		    LFlispro [xLFlispro] = Bkl;
		    xLFlispro++;
		}
	    }

	    gitem++;
	}
    }

    if (++xLFprod >= xLFprod_top) LFprod_oflw ();

    LFprolon [xLFprod] = xLFlispro;

    return 1;
}
#endif

static int
SXLIG_post_td_init ()
{
    inputG.maxrhsnt = SXLIGmaxrhsnt;
    inputG.maxprod = sxplocals.SXP_tables.P_xnbpro;
    inputG.prolon = SXLIGprolon;
    inputG.prod2nbnt = SXLIGprod2nbnt;
    inputG.lhs = SXLIGlhs;
    inputG.ntstring = SXLIGntstring;

    /* Pour l'instant, prudence. */
    /* Regarder si sxndparser peut etre + precis */
    LFhas_cycles = true;

    pAij_top = XxY_top (parse_stack.symbols);

    X_alloc (&Aij_hd, "Aij_hd", pAij_top+1, 1, NULL,
#ifdef  statistics
	     sxtty
#else
	     NULL
#endif
	     );

    A0j2Aij_size = X_size (Aij_hd)+1;

    SXLIGalloc (pAij_top+1);
}

int
SXLIG_actions (which, arg)
    int		which;
    struct sxtables	*arg;
{
  switch (which) {
  case SXOPEN:
/*  inutile : valeurs par defaut... 
    parse_stack.for_parsact.action_top = NULL;
    parse_stack.for_parsact.action_pop = NULL;
    parse_stack.for_parsact.action_bot = NULL;
    parse_stack.for_parsact.action_new_top = NULL;
    parse_stack.for_parsact.action_final = NULL;
    parse_stack.for_parsact.GC = NULL;
    parse_stack.for_parsact.third_field = NULL;
    parse_stack.for_parsact.post = NULL;
*/
    parse_stack.for_parsact.post_td_init = SXLIG_post_td_init;
    parse_stack.for_parsact.post_td_action = SXLIG_ndsemact;
    parse_stack.for_parsact.post_td_final = SXLIG_semantics;

    return 0;

  case SXCLOSE:
    return 0;

  case SXINIT:
    sxplocals.mode.with_semact = false;
    sxplocals.mode.with_parsact = false;
    sxplocals.mode.with_parsprdct = false;

    return 0;

  case SXFINAL:
    return 0;

  case SXACTION:
    return 0;

  case SXDO:
    return 0;

  case SXUNDO:
    return 0;

  case SXSEMPASS:
    return 0;

  case SXPREDICATE:
    return 1;

  default:
    fputs ("The function \"SXLIG_actions\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
  }

  return 0;
}
#else
int
SXLIG_actions (what, arg)
    int		what;
    char	*arg;
{
    struct for_semact	*for_semact;

    switch (what) {
    case SXOPEN:
    case SXINIT:
    case SXERROR:
    case SXFINAL:
    case SXCLOSE:
    case SXSEMPASS:
	/* INUTILISES */
	break;

    case SXACTION:
	for_semact = (struct for_semact*) arg;

	for_semact->sem_init = SXLIG_semact_init;
	for_semact->sem_final = NULL;
	for_semact->oflw = NULL;
	for_semact->semact = SXLIG_semact;
	for_semact->parsact = NULL;
	for_semact->prdct = NULL;
	for_semact->constraint = NULL;
	for_semact->sem_pass = SXLIG_semantics;
#if 0
	/* Ds le cas des LIG, les Aij sont stockes ds le X_header Aij_hd */
	for_semact->need_Aij2A_i_j = false;
	for_semact->need_pack_unpack = false;
#endif

	break;

	
    

    default:
	fputs ("The function \"SXLIG_actions\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
#endif /* is_sxndparser */

#endif /* is_initial_LIG==0 */
