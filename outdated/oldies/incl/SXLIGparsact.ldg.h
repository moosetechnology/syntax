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
*/


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Jeu 17 Oct 1996 11:53:35:	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "XxY.h";
#include "semact.h";

bool			is_print_prod, is_print_time;
#define TIME_INIT	0
#define TIME_FINAL	1

static int		*_CTRL_T, _CTRL_SIZE;

#define CTRL_REALLOC(s,l)	s = ((int*) sxrealloc ((s)-1, l+2, sizeof (int)) + 1)
#define CTRL_OFLW(s,d)	(_CTRL_SIZE=(s)[-1]+=(s)[-1]+(d), CTRL_REALLOC (s,_CTRL_SIZE))
#define TOP(s)		(*(s))
#define CTRL_SIZE(s)	((s)[-1])
#define CTRL_IS_OFLW(s,d)	(TOP(s)+(d)>=CTRL_SIZE(s))
#define INCR(s)		(++*(s))
#define DECR(s)		((*(s))--)
#define PUSH(s,x)	(s)[INCR(s)]=(x)
#define CTRL_PUSH(s,x)	(((TOP(s)==CTRL_SIZE(s)) ? CTRL_OFLW(s,0) : 0), PUSH(s,x))
#define POP(s)		(s)[DECR(s)]
#define IS_EMPTY(s)	(TOP(s)==0)
#define CTRL_CHECK(s,d)	(CTRL_IS_OFLW(s,d) ? CTRL_OFLW(s,d) : 0)
#define CTRL_ALLOC(l)	(_CTRL_T = (int*) sxalloc (l+2, sizeof (int))+1, _CTRL_T[0]=0, _CTRL_T[-1]=l, _CTRL_T)
#if statistics
static void
CTRL_FREE(s,n)
    int	*s;
    char *n;
{
    fprintf (sxtty, "name = %s, size = %i\n", n, s[-1]);
    sxfree (s-1);
}
#else
#define CTRL_FREE(s,n) 	sxfree(s-1);
#endif



#define SECONDARY	0
#define PRIMARY		1

#define IS_PUSHPOP	01
#define IS_PUSHPOP_EQ1	02
#define IS_PUSHPOP_EQn	04
#define IS_EQ1_EQ1	010
#define IS_EQ1_EQn	020



static int		*LFprod, *LFprolon, *LFlispro, *LFprod2next;;
static int		xLFprod_top, xLFprod, xLFlispro_top, xLFlispro;


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

static bool		LIGhas_cycles;
static SXBA		LIGcyclic_prod_set;

static XxY_header	EQUALn_hd, POPn_hd;
static int		*UNIQUE_stack, *EQUAL1_stack, *EQUALn_stack, *PUSHPOPn_stack, *POP1_stack, *POPn_stack;
static char		*EQUALn2kind;

static int		EQUALn_top, POPn_top, PUSHPOPn_top;
static int		UNIQUE_nttop, EQUALn_ntbot, EQUALn_nttop, POPn_ntbot, POPn_nttop, PUSHPOPn_ntbot, PUSHPOPn_nttop;
static int		PUSH1_ntbot;

static SXBA		UNIQUE_set, EQUAL1_set, EQUALn_set, PUSHPOPn_set, POP1_set, POPn_set;


static int		*LDGlhs, *LDGprolon, *LDGlispro;
static int		xLDGprod_top, xLDGprod, xLDGlispro_top, xLDGlispro, LDGprod_nb;

static int		LDGphrases_nb;
static int		*LDGsuffix_stack, *LDGprefix_stack;
static int		*LDGlhs_hd, *LDGprod2next;
static bool		LDGhas_cycles;
static SXBA		LDGcyclic_nt_set, LDGprod_set, LDGnt_set2;
static int		LDGmax_rhs_lgth;

static int		*LDGnt_stack, *LDGrhsnt2prod, *LDGprod2ntnb, *LDGitem2prod;
static SXBA		LDGnt_set;

/* A FAIRE */
/* Si la grammaire n'est pas en forme normale et une forme tordue de grammaire cyclique,
   (ou ds le cas d'une LIG --pas une LIGed forest--)
   les elements (A, B) des relations ne permettent pas de determiner si l'evolution de
   la pile entre A et B est due uniquement au fait que la grammaire n'est pas en forme
   normale et donc ce NT va produire la chaine vide ds la LDG, et peut donc etre ignore
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

   2)Dans le cas douteux, check_locality retourne false et, apres generation, on rend la LDG
   epsilon free.

   3)Dans le cas douteux, check_locality retourne false et, on laisse ds la LDG les productions
   vides.

   On essaie la 2!
*/


#if SXLIGis_normal_form==1
#define check_locality(Aij,Bkl)	false
#else
static int		_A, _B;
#define check_equality(A,B)	((_A = ((A) < 0 ? dumAij2attr[-A].Aij : (A)), _B = ((B) < 0 ? dumAij2attr[-B].Aij : (B)), (_A == _B ? A != B : false)))
#define check_locality(Aij,Bkl)	(check_equality(Aij,Bkl))
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
LDGprod_oflw ()
{
    xLDGprod_top *= 2;
    LDGlhs = (int*) sxrealloc (LDGlhs, xLDGprod_top+1, sizeof (int));
    LDGprolon = (int*) sxrealloc (LDGprolon, xLDGprod_top+1, sizeof (int));
    LDGprod2ntnb = (int*) sxrealloc (LDGprod2ntnb, xLDGprod_top+1, sizeof (int));
    LDGprod2next = (int*) sxrealloc (LDGprod2next, xLDGprod_top+1, sizeof (int));
}

static void
LDGlispro_oflw ()
{
    xLDGlispro_top *= 2;
    LDGlispro = (int*) sxrealloc (LDGlispro, xLDGlispro_top+1, sizeof (int));
    LDGitem2prod = (int*) sxrealloc (LDGitem2prod, xLDGlispro_top+1, sizeof (int));
}

EQUAL1_oflw (old_size, new_size)
    int		old_size, new_size;
{
    EQUAL12attr = (struct rel_attr*) sxrealloc (EQUAL12attr, new_size +1, sizeof (struct rel_attr));
}

EQUALn_oflw (old_size, new_size)
    int		old_size, new_size;
{
    EQUALn2kind = (char*) sxrealloc (EQUALn2kind, new_size+1, sizeof (char));
    CTRL_REALLOC (EQUALn_stack, EQUAL1_top+new_size);
    CTRL_REALLOC (PUSHPOPn_stack, new_size);
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
}


static void
POPn_oflw (old_size, new_size)
    int		old_size, new_size;
{
    CTRL_REALLOC (POPn_stack, new_size);
}

static void
SXLIGalloc (size)
    int size;
{
        
    LDGmax_rhs_lgth = inputG.maxrhsnt+1;

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

    SECONDARY2attr = (struct rel_attr*) sxcalloc (size+1, sizeof (struct rel_attr));

    xLFprod_top = size;
    LFprod = (int*) sxalloc (xLFprod_top+1, sizeof (int));
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

#if SXLIGhas_cycles==1
    LIGcyclic_prod_set = sxba_calloc (inputG.maxprod+1);
#endif
}

static void
SXLIGfree ()
{
    XxY_free (&Axs_hd);
    XxY_free (&EQUAL1_hd);
    XxY_free (&PUSH1_hd);
    XxY_free (&POP1_hd);
    XxY_free (&EQUALn_hd);
    XxY_free (&POPn_hd);

    sxfree (EQUAL12attr);
    sxfree (PUSH12attr);
    sxfree (POP12attr);
    sxfree (SECONDARY2attr);

    sxfree (LFprod);
    sxfree (LFlispro);
    sxfree (LFprod2next);
    sxfree (LFprolon);

#if SXLIGis_normal_form==0
    sxfree (dumAij2attr);
#endif

    sxfree (LDGnt_set);

#if SXLIGhas_cycles==1
    sxfree (LIGcyclic_prod_set);
#endif
}

#if is_initial_LIG==0
static void
SXLIG_semact_init (size)
    int size;
{
   
    fill_inputG ();

    /* On alloue les structures contenant les relations de niveau 1 */
    SXLIGalloc (size);
}



static void
SXLIG_semact_final (size)
    int size;
{
    SXLIGfree ();
}
#endif

static bool
SXLIG_semact (i, j, prod_core, rhs_stack)
    unsigned short	*prod_core;
    int 	i, j, rhs_stack [];
{
    int		prod = prod_core [0];
    int		A, B, AB, As, sB, AsB, Aij, secAij, Bkl, ssymb, X, keep;
    int		act_no, prdct_no, atype, ptype, pop_nb, push_nb, item, spf_item, nbnt;
    int		nb, Bpri, Bsec;
    int		*pca, *pcp;
#include <values.h>
    /* Definit la constante HIBITS == 1<<15 pour les mots de 32! */

    if (inputG.has_cycles && (prod & HIBITS)) {
	/* On est dans le cas cyclique,
	   on a deja ete appele avec cette production =>
	   on previent le parser que le point fixe est atteint. */
#if SXLIGhas_cycles==1
	prod &= ~(HIBITS);
	SXBA_1_bit (LIGcyclic_prod_set, prod);
#endif
	return false;
    }

#if SXLIGis_reduced==0
    /* La grammaire initiale n'est pas reduite */
    if (!SXBA_bit_is_set (SXLIGt_set, prod))
	/* Cette production est inutile */
	return true;
#endif

    /* La foret partagee ne contient que les NT */
    
    Aij = rhs_stack [0];
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

    return true;
}




static void
n_level_relations ()
{
    int 	A, B, C, AB, AC, BC, AsB, BsC, BsD, CsD, As, sC, sD, ssymb;
    bool	is_new;
    char	*pkind;
#if SXLIGis_leveln_complete==0
#if SXLIGuse_reduced==1
    int		D, A00, B00, C00, D00, sB00, A00s, sD00, Amain, Bmain, Cmain, Dmain;
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
LDGprint_nt (AB)
    int AB;
{
    int 	A, B, sB, ssymb, Aij, Bkl, i, j, k, l, dum;
    char	*op, *Aijstr, *Bklstr;

    if (AB <= EQUALn_nttop) {
	if (AB <= UNIQUE_nttop) {
	    /* A */
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
    Aijstr = inputG.ntstring [Aij2A (Aij)];

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
	    printf (" %s> ", Bklstr);
	else
	    printf (" %s(%i)> ", Bklstr, B);
#else
	Bklstr = inputG.ntstring [Aij2A (Bkl)];

	k = Aij2i (Bkl);
	l = Aij2j (Bkl);

	if (B == 0)
	    printf (" %s[%i..%i]> ", Bklstr, k, l);
	else
	    printf (" %s(%i)[%i..%i]> ", Bklstr, B, k, l);
#endif
    }
    else
	fputs ("> ", stdout);
}

static void
LDGgen_reduced ()
{
    int prod, lim, item, A, X, t;

    for (prod = 1; prod < xLDGprod; prod++) {
	A = LDGlhs [prod];

	if (SXBA_bit_is_set (LDGnt_set, A)) {
	    t = 0;

	    for (lim = LDGprolon [prod+1], item = LDGprolon [prod]; item < lim; item++) {
		X = LDGlispro [item];

		if (X < 0)
		    t = -X;
		else
		    if (!SXBA_bit_is_set (LDGnt_set, X))
			/* X is useless */
			break;
	    }

	    if (item == lim) {
		/* prod est utile */
		SXBA_1_bit (LDGprod_set, prod);

		if (t > 0 && SXBA_bit_is_set (LIGcyclic_prod_set, t))
		    SXBA_1_bit (LDGcyclic_nt_set, A);
	    }
	}
    }
}


static void
LDGprint ()
{
    int prod, lim, item, X;

    LDGprod_nb = 0;

    for (prod = 1; prod < xLDGprod; prod++) {
	for (lim = LDGprolon [prod+1], item = LDGprolon [prod]; item < lim; item++) {
	    X = LDGlispro [item];

	    if (X > 0 && !SXBA_bit_is_set (LDGnt_set, X))
		/* X is useless */
		break;
	}

	if (item == lim) {
	    LDGprod_nb++;
	    LDGprint_nt (LDGlhs [prod]);
	    fputs ("\t=\t", stdout);

	    for (item = LDGprolon [prod]; item < lim; item++) {
		X = LDGlispro [item];

		if (X < 0)
		    printf ("\"%i\" ", -X);
		else
		    LDGprint_nt (X);
	    }

	    fputs (" ;\n", stdout);
	}
    }

    printf ("\n* Nombre de productions de la grammaire reduite des derivations lineaires : %i\n", LDGprod_nb);
}

static void
LDGgen_rhs (lhs, pre_nt, lfprod, post_nt)
    int lhs, pre_nt, lfprod, post_nt;
{
    int nbnt = 0, lim, item, secCmn, prod;

    LDGlhs [xLDGprod] = lhs;

    if (pre_nt > 0) {
	LDGitem2prod [xLDGlispro] = LDGrhsnt2prod [pre_nt];
	LDGrhsnt2prod [pre_nt] = xLDGprod;
	LDGlispro [xLDGlispro++] = pre_nt;
	nbnt++;
    }

    if (lfprod > 0) {
	lim = LFprolon [lfprod+1];
	item = LFprolon [lfprod];

	while (item <= --lim) {
	    /* On genere de droite a gauche */
	    secCmn = LFlispro [lim];

	    if (SXBA_bit_is_reset_set (UNIQUE_set, secCmn)) {
		PUSH (UNIQUE_stack, secCmn);
	    }

	    /* On ne met pas 2 fois le meme */
	    if (LDGrhsnt2prod [secCmn] != xLDGprod) {
		LDGitem2prod [xLDGlispro] = LDGrhsnt2prod [secCmn];
		LDGrhsnt2prod [secCmn] = xLDGprod;
		nbnt++;
	    }

	    LDGlispro [xLDGlispro++] = secCmn;
	}

	prod = LFprod [lfprod];
	LDGlispro [xLDGlispro++] = -prod;
    }

    if (post_nt > 0) {
	LDGitem2prod [xLDGlispro] = LDGrhsnt2prod [post_nt];
	LDGrhsnt2prod [post_nt] = xLDGprod;
	LDGlispro [xLDGlispro++] = post_nt;
	nbnt++;
    }

    if (nbnt == 0) {
#if EBUG
	if (lhs > PUSHPOPn_nttop)
	    sxtrap (ME, "LDGgen_rhs");
#endif

	LDGprod2next [xLDGprod] = LDGlhs_hd [lhs];
	LDGlhs_hd [lhs] = xLDGprod;

	if (SXBA_bit_is_reset_set (LDGnt_set, lhs)) {
	    /* La RHS ne contient que [T]  */
	    PUSH (LDGnt_stack, lhs);
	}

#if EBUG
	if (lfprod == 0)
	    sxtrap (ME, "LDGgen_rhs");
#endif

#if is_initial_LIG==1
	SXBA_1_bit (LDGt_set, prod);
#endif

    }

    LDGprod2ntnb [xLDGprod] = nbnt;
    LDGprolon [++xLDGprod] = xLDGlispro;
}


static void
LDGgen_prod (lhs, pre_nt, rel_attr_ptr, post_nt)
    int 		lhs, pre_nt, post_nt;
    struct rel_attr	*rel_attr_ptr;
{
    int lfprod;

    lfprod = (rel_attr_ptr == NULL) ? 0 : rel_attr_ptr->prod;

    if (xLDGprod + 1 > xLDGprod_top) LDGprod_oflw ();
    if (xLDGlispro + LDGmax_rhs_lgth > xLDGlispro_top) LDGlispro_oflw ();

    LDGgen_rhs (lhs, pre_nt, lfprod, post_nt);

    if (lfprod > 0) {
	while ((lfprod = LFprod2next [lfprod]) != 0) {
	    if (xLDGprod + 1 > xLDGprod_top) LDGprod_oflw ();
	    if (xLDGlispro + LDGmax_rhs_lgth > xLDGlispro_top) LDGlispro_oflw ();

	    LDGgen_rhs (lhs, pre_nt, lfprod, post_nt);
	}
    }
}



static void
LDGuseful_symbols ()
{
    /* LDGnt_stack et LDGnt_set = {A | A -> w et w \in T*} */
    int	A, B, prod, item, X;

    while (!IS_EMPTY (LDGnt_stack)) {
	A = POP (LDGnt_stack);
	prod = LDGrhsnt2prod [A];
	
	while (prod != 0) {
	    /* Plusieurs occurrences du meme NT en RHS comptent pour un. */
	    if (--LDGprod2ntnb [prod] == 0) {
		B = LDGlhs [prod];

#if EBUG
		if (B > PUSHPOPn_nttop)
		    sxtrap (ME, "LDGuseful_symbols");
#endif

		LDGprod2next [prod] = LDGlhs_hd [B];
		LDGlhs_hd [B] = prod;

		if (SXBA_bit_is_reset_set (LDGnt_set, B))
		    PUSH (LDGnt_stack, B);

#if is_initial_LIG==1
		item = LDGprolon [prod];

		for (X = LDGlispro [item]; X != 0; X = LDGlispro [++item]) {
		    if (X < 0) {
			X = -X;
			SXBA_1_bit (LDGt_set, X);
		    }
		}
#endif
	    }

	    item = LDGprolon [prod];

	    for (X = LDGlispro [item]; X != A; X = LDGlispro [++item]);

	    prod = LDGitem2prod [item];
	}
    }
}

static void
LDGextraction (S0n)
    int S0n;
{
    int			AB, ABn, AC, ACn, BC, BCn, AsB, AsC, AsCn, BsC, As, sC, ssymb;
    int			Aij, secAij, Bkl, Bmn, Ckl;
    bool		is_local, is_local2;
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
		LDGgen_prod (secAij, 0, rel_attr_ptr, 0);

	    /* On cherche les Bkl-productions secondaires telles que Aij =1 Bkl */
	    XxY_Xforeach (EQUAL1_hd, Aij, AB) {
		Bkl =  XxY_Y (EQUAL1_hd, AB);
#if SXLIGis_normal_form==0
#if EBUG
		if (Bkl < 0)
		    sxtrap (ME, "LDGextraction");
#endif
#endif

		rel_attr_ptr = &(SECONDARY2attr [Bkl]);

		if (rel_attr_ptr->prod != 0) {
		    if (SXBA_bit_is_reset_set (EQUAL1_set, AB))
			PUSH (EQUAL1_stack, AB);

		    /* On parcourt les Bkl-productions secondaires de la foret partagee */
		    LDGgen_prod (secAij, 0, rel_attr_ptr, EQUALn_ntbot + AB);
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
		    is_local = check_locality (Aij, Bkl); /* decomposition en operations elementaires
							     d'une pile */

		    if (!is_local) {
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
		    else
			ABn = 0;

		    /* On parcourt les Bkl-productions secondaires de la foret partagee */
		    LDGgen_prod (secAij, 0, rel_attr_ptr, ABn);
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

		LDGgen_prod (ACn, PUSHPOPn_ntbot + AC, NULL, 0);
	    }

	    /* On cherche Bmn tq Aij =n Ckl se decompose en Aij =1 Bmn =1 Ckl */
	    if (*pkind & IS_EQ1_EQ1) {
		XxY_Xforeach (EQUAL1_hd, Aij, AB) {
		    Bmn = XxY_Y (EQUAL1_hd, AB);

		    if ((BC = XxY_is_set (&EQUAL1_hd, Bmn, Ckl)) > 0) {
			if (SXBA_bit_is_reset_set (EQUAL1_set, BC))
			    PUSH (EQUAL1_stack, BC);

			LDGgen_prod (ACn, EQUALn_ntbot + BC, &(EQUAL12attr [AB]), 0);
		    }
		}
	    }

	    /* On cherche Bmn tq Aij =n Ckl se decompose en Aij =1 Bmn =n Ckl */
	    if (*pkind & IS_EQ1_EQn) {
		XxY_Xforeach (EQUAL1_hd, Aij, AB) {
		    Bmn = XxY_Y (EQUAL1_hd, AB);

		    if ((BC = XxY_is_set (&EQUALn_hd, Bmn, Ckl)) > 0) {
			is_local = check_locality (Bmn, Ckl);

			if (!is_local) {
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
			else
			    BCn = 0;

			LDGgen_prod (ACn, BCn, &(EQUAL12attr [AB]), 0);
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
			    is_local2 = check_locality (Aij, Bmn);

			    if (SXBA_bit_is_reset_set (EQUAL1_set, BC))
				PUSH (EQUAL1_stack, BC);

			    if (!is_local2) {
				if (SXBA_bit_is_reset_set (PUSHPOPn_set, AB))
				    PUSH (PUSHPOPn_stack, AB);
			    }

			    LDGgen_prod (ACn, EQUALn_ntbot + BC,
					  NULL, is_local2 ? 0 : PUSHPOPn_ntbot + AB);
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
			    is_local = check_locality (Bmn, Ckl);
			    is_local2 = check_locality (Aij, Bmn);

			    
			    if (!is_local) {
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
			    else
				BCn = 0;


			    if (!is_local2) {
				if (SXBA_bit_is_reset_set (PUSHPOPn_set, AB))
				    PUSH (PUSHPOPn_stack, AB);

				ABn = PUSHPOPn_ntbot + AB;
			    }
			    else
				ABn = 0;

			    LDGgen_prod (ACn, BCn, NULL, ABn);
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

		    LDGgen_prod (POPn_ntbot + AsCn, 0, &(POP12attr [BsC]), EQUALn_ntbot + AB);
		}

		if ((AB = XxY_is_set (&EQUALn_hd, Aij, Bmn)) > 0) {
		    is_local = check_locality (Aij, Bmn);

		    if (!is_local) {
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
		    else
			ABn = 0;

		    LDGgen_prod (POPn_ntbot + AsCn, 0, &(POP12attr [BsC]), ABn);
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
			    is_local = check_locality (Bmn, Ckl);

			    if (!is_local) {
				if (SXBA_bit_is_reset_set (POP1_set, BsC))
				    PUSH (POP1_stack, BsC);
			    }

			    LDGgen_prod (PUSHPOPn_ntbot + AC, is_local ? 0 : POPn_ntbot + BsC,
					  &(PUSH12attr [AsB]), 0);
			}

			if ((BsC = XxY_is_set (&POPn_hd, Bmn, sC)) > 0) {
			    is_local = check_locality (Bmn, Ckl);

			    if (!is_local) {
				if (SXBA_bit_is_reset_set (POPn_set, BsC))
				    PUSH (POPn_stack, BsC);
			    }

			    LDGgen_prod (PUSHPOPn_ntbot + AC, is_local ? 0 : POPn_ntbot + BsC + POP1_top,
					  &(PUSH12attr [AsB]), 0);
			}
		    }
		}
	    }
	}

	while (!IS_EMPTY (POP1_stack)) {
	    AsC = POP (POP1_stack);
	    LDGgen_prod (POPn_ntbot + AsC, 0, &(POP12attr [AsC]), 0);
	}

	while (!IS_EMPTY (EQUAL1_stack)) {
	    AC = POP (EQUAL1_stack);
	    LDGgen_prod (EQUALn_ntbot + AC, 0, &(EQUAL12attr [AC]), 0);
	}
    } while (*UNIQUE_stack + *EQUALn_stack + *POPn_stack);

    if (is_print_time) {
	fprintf (sxtty, "Nombre de productions de la grammaire des derivations lineaires : %i\n", xLDGprod-1);
    }
}


static void
LDGprint_phrase (stack)
    int *stack;
{
    int bot, top;

    printf ("Phrase %i:", LDGphrases_nb);

    for (top = TOP (stack), bot = 1; bot <= top; bot++)
	printf (" %i", stack [bot]);

    fputs ("\n", stdout);
}




static void
LDGgen_phrases ()
{
    int	Y, X, prod, bot, top, lgth, ptop, stop;

    if (IS_EMPTY (LDGsuffix_stack)) {
	/* On a une phrase ds prefix_stack */
	LDGphrases_nb++;

	if (is_print_prod) {
	    LDGprint_phrase (LDGprefix_stack);
	}
    }
    else {
	if ((Y = POP (LDGsuffix_stack)) < 0) {
	    CTRL_PUSH (LDGprefix_stack, -Y);
	    LDGgen_phrases ();
	    DECR (LDGprefix_stack);
	}
	else {
	    ptop = TOP (LDGprefix_stack);
	    stop = TOP (LDGsuffix_stack);

	    for (prod = LDGlhs_hd [Y]; prod > 0; prod = LDGprod2next [prod]) {
		top = LDGprolon [prod+1];
		bot = LDGprolon [prod];

		while (bot < top) {
		    if ((X = LDGlispro [bot]) > 0)
			break;

		    CTRL_PUSH (LDGprefix_stack, -X);
		    bot++;
		}

		if ((lgth = top - bot) > 0) {
		    CTRL_OFLW (LDGsuffix_stack, lgth);

		    while (bot <= --top) {
			PUSH (LDGsuffix_stack, LDGlispro [top]);
		    }
		}

		LDGgen_phrases ();

		TOP (LDGsuffix_stack) = stop;
		TOP (LDGprefix_stack) = ptop;
	    }
	}

	CTRL_PUSH (LDGsuffix_stack, Y);
    }
}

static void
LDGseek_cycles ()
{
    /* On cherche si le cardinal du langage de la LDG est non borne' */
    int nt, A, B, prod, item, X;

    nt = 0;

    while ((nt = sxba_scan (LDGcyclic_nt_set, nt)) > 0) {
	/* A est un candidat au test de recursivite */
	PUSH (LDGnt_stack, nt);
	SXBA_1_bit (LDGnt_set2, nt);

	while (!IS_EMPTY (LDGnt_stack)) {
	    A = POP (LDGnt_stack);
	    prod = LDGrhsnt2prod [A];
	
	    while (prod != 0) {
		if (SXBA_bit_is_set (LDGprod_set, prod)) {
		    /* prod est utile */
		    B = LDGlhs [prod];

		    if (SXBA_bit_is_reset_set (LDGnt_set2, B))
			PUSH (LDGnt_stack, B);
		    else {
			LDGhas_cycles = true;
			return;
		    }
		}

		item = LDGprolon [prod];

		for (X = LDGlispro [item]; X != A; X = LDGlispro [++item]);

		prod = LDGitem2prod [item];
	    }
	}

	sxba_empty (LDGnt_set2);
    }
}
	

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

    EQUALn_stack = CTRL_ALLOC (EQUAL1_top+XxY_size (EQUALn_hd));
    PUSHPOPn_stack = CTRL_ALLOC (XxY_size (EQUALn_hd));	
    EQUALn2kind = (char*) sxalloc (XxY_size (EQUALn_hd)+1, sizeof (char));

    XxY_alloc (&POPn_hd, "POPn_hd", EQUAL1_top+POP1_top+1, 1, 0, 0, POPn_oflw,
#ifdef statistics
	       sxtty
#else
	       NULL
#endif
	       );

    POPn_stack = CTRL_ALLOC (XxY_size (POPn_hd));

    n_level_relations ();

    EQUAL1_set = sxba_calloc (EQUAL1_top+1);
    EQUAL1_stack = CTRL_ALLOC (EQUAL1_top);

    EQUALn_top = XxY_top (EQUALn_hd);
    EQUALn_set = sxba_calloc (EQUALn_top+1);

    POP1_set = sxba_calloc (POP1_top+1);
    POP1_stack = CTRL_ALLOC (POP1_top);

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
    PUSH1_ntbot = PUSHPOPn_nttop;
    /* Les NT de la LDG se partitionnent :
       0 		< 	[Aij] 			<= UNIQUE_nttop
       UNIQUE_nttop 	< 	[Aij =n Bkl] 		<= EQUALn_nttop
       EQUALn_nttop 	< 	[Aij >n Bkl Bkl] 	<= POPn_nttop
       POPn_nttop 	< 	[Aij <>n Bkl] 		<= PUSHPOPn_nttop
       */

    /* On produit la LDG correspondant */
    xLDGprod_top = pAij_top;
    LDGlhs = (int*) sxalloc (xLDGprod_top+1, sizeof (int));
    LDGprolon = (int*) sxalloc (xLDGprod_top+1, sizeof (int));
    xLDGlispro_top = pAij_top;
    LDGlispro = (int*) sxalloc (xLDGlispro_top+1, sizeof (int));

    LDGprolon [xLDGprod = 1] = xLDGlispro = 1;

    LDGnt_stack = CTRL_ALLOC (PUSHPOPn_nttop);
    LDGnt_set = sxba_calloc (PUSHPOPn_nttop/* +PUSH1_top */+1);

    LDGrhsnt2prod = (int*) sxcalloc (PUSHPOPn_nttop+1, sizeof (int));
    LDGprod2ntnb = (int*) sxalloc (xLDGprod_top+1, sizeof (int));
    LDGitem2prod = (int*) sxalloc (xLDGlispro_top+1, sizeof (int));

    LDGlhs_hd = (int*) sxcalloc (PUSHPOPn_nttop+1, sizeof (int));
    LDGprod2next = (int*) sxalloc (xLDGprod_top+1, sizeof (int));

    UNIQUE_set = sxba_calloc (UNIQUE_nttop+1);
    UNIQUE_stack = CTRL_ALLOC (UNIQUE_nttop);
    
    LDGextraction (S0n);

    LDGuseful_symbols ();

#if SXLIGhas_cycles==1
    /* La LIG est cyclique, cette cyclicite se manifeste-t-elle sur ce source ? */
    /* On cherche si le cardinal du langage de la LDG est non borne' */
    LDGprod_set = sxba_calloc (xLDGprod+1);
    LDGcyclic_nt_set = sxba_calloc (PUSHPOPn_nttop+1);
    LDGnt_set2 = sxba_calloc (PUSHPOPn_nttop+1);

    LDGgen_reduced ();
    LDGseek_cycles ();

    if (LDGhas_cycles)
	fputs ("\nWarning: there is an unbounded number of valid LIG parse trees.\n", sxtty);

    sxfree (LDGcyclic_nt_set);
    sxfree (LDGprod_set);
    sxfree (LDGnt_set2);
#endif

    CTRL_FREE (LDGnt_stack, "LDGnt_stack");
    sxfree (LDGrhsnt2prod);
    sxfree (LDGprod2ntnb);
    sxfree (LDGitem2prod);

    if (is_print_prod) {
	LDGprint ();
    }

    CTRL_FREE (EQUALn_stack, "EQUALn_stack");
    CTRL_FREE (PUSHPOPn_stack, "PUSHPOPn_stack");
    sxfree (EQUALn2kind);
    CTRL_FREE (POPn_stack, "POPn_stack");
    sxfree (EQUAL1_set);
    CTRL_FREE (EQUAL1_stack, "EQUAL1_stack");
    sxfree (EQUALn_set);
    sxfree (POP1_set);
    CTRL_FREE (POP1_stack, "POP1_stack");
    sxfree (POPn_set);
    sxfree (PUSHPOPn_set);
    sxfree (UNIQUE_set);
    CTRL_FREE (UNIQUE_stack, "UNIQUE_stack");
	
#if is_initial_LIG==0
    if (!LDGhas_cycles) {
	/* A FAIRE : traitement du cas cyclique! */
	LDGprefix_stack = CTRL_ALLOC (xLDGprod);
	LDGsuffix_stack = CTRL_ALLOC (xLDGprod);

	CTRL_PUSH (LDGsuffix_stack, S0n);

	LDGgen_phrases ();

	CTRL_FREE (LDGsuffix_stack, "LDGsuffix_stack");
	CTRL_FREE (LDGprefix_stack, "LDGprefix_stack");
    }
#endif

    sxfree (LDGlhs_hd);
    sxfree (LDGprod2next);
    sxfree (LDGlhs);
    sxfree (LDGprolon);
    sxfree (LDGlispro);

#if is_initial_LIG==0
    if (is_print_time)
	sxtime (TIME_FINAL, "\tSemantic Pass: Linear Derivation Grammar");
#endif

    return true;
}


#if is_initial_LIG==0
int
SXLIG_actions (what, arg)
    int		what;
    struct sxtables	*arg;
{
    switch (what) {
    case SXOPEN:
	for_semact.sem_init = SXLIG_semact_init;
	for_semact.sem_final = SXLIG_semact_final;
	for_semact.semact = SXLIG_semact;
	for_semact.parsact = NULL;
	for_semact.prdct = NULL;
	for_semact.constraint = NULL;
	for_semact.sem_pass = SXLIG_sem_pass;
#if 0
	/* Ds le cas des LIG, les Aij sont stockes ds le X_header Aij_hd */
	for_semact.need_Aij2A_i_j = false;
	for_semact.need_pack_unpack = false;
#endif

	break;

    case SXINIT:
    case SXACTION:
    case SXERROR:
    case SXFINAL:
    case SXCLOSE:
    case SXSEMPASS:
	/* INUTILISES */
	break;

    default:
	fputs ("The function \"SXLIG_actions\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
#endif
