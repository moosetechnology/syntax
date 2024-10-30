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

char WHAT_SXLIGPARSACT[] = "@(#)SXLIGparsact.c	- SYNTAX [unix] - Jeu 17 Oct 1996 11:53:35";

static char	ME [] = "SXLIGparsact";

#include "sxunix.h";
#include "XxY.h";
#include "semact.h";

bool			is_print_prod, is_print_time;
#define TIME_INIT	0
#define TIME_FINAL	1



struct sxndlig_common {
    struct sxligparsact	code;
    SXTABLES	*sxtables;
} sxndlig_common;



static int		*_CTRL_T, _CTRL_SIZE;

#define CTRL_OFLW(s,d)	(_CTRL_SIZE=(s)[-1]+=(s)[-1]+(d), s = (int*) sxrealloc ((s)-1, _CTRL_SIZE+2, sizeof (int)) + 1)
#define TOP(s)		(*(s))
#define CTRL_SIZE(s)	((s)[-1])
#define CTRL_IS_OFLW(s,d)	(TOP(s)+(d)>=CTRL_SIZE(s))
#define PUSH(s,x)	(s)[++*(s)]=(x)
#define CTRL_PUSH(s,x)	(((TOP(s)==CTRL_SIZE(s)) ? CTRL_OFLW(s,0) : 0), PUSH(s,x))
#define POP(s)		(s)[(*(s))--]
#define IS_EMPTY(s)	(TOP(s)==0)
#define CTRL_CHECK(s,d)	(CTRL_IS_OFLW(s,d) ? CTRL_OFLW(s,d) : 0)
#define CTRL_ALLOC(l)	(_CTRL_T = (int*) sxalloc (l+2, sizeof (int))+1, _CTRL_T[0]=0, _CTRL_T[-1]=l, _CTRL_T)
#if EBUG
static void
CTRL_FREE(s,n)
    int	*s;
    char *n;
{
    fprintf (sxtty, "name = %s, size = %i, top = %i (%i%%)\n", n, s[-1], s [0], 100*s[0]/s[-1]);
    sxfree (s-1);
}
#else
#define CTRL_FREE(s,n) 	sxfree(s-1);
#endif



#define SECONDARY	0
#define PRIMARY		1

static bool		is_normal_form;

static int		*LFprod, *LFprolon, *LFlispro, *LFprod2next;;
static int		xLFprod_top, xLFprod, xLFlispro_top, xLFlispro;


static XxY_header	EQUAL1_hd, PUSH1_hd, POP1_hd;
static int		*EQUAL12prod, *PUSH12prod, *POP12prod;
static X_header		SECONDARY_hd;
static int		*SECONDARY2prod;


static int		*SXLIGap_disp, *SXLIGap_list, xap_list;
static int		*SXLIGprod2dum_nb;
static int		*SXLIGpost_action, *SXLIGpost_prdct;



static XxY_header	EQUALn_hd, PUSHPOPn_hd, POPn_hd;
static XxY_header	Axs_hd;
static int		EQUAL1_top, POP1_top;
static int		*EQUALn_stack, *PUSHPOPn_stack, *POPn_stack, *UNIQUE_stack;

static int		EQUALn_top, POPn_top, PUSHPOPn_top;
static int		UNIQUE_nttop, EQUALn_ntbot, EQUALn_nttop, POPn_ntbot, POPn_nttop, PUSHPOPn_ntbot, PUSHPOPn_nttop;

static SXBA		UNIQUE_set, EQUALn_set, PUSHPOPn_set, POPn_set;

static int		*dumAij2Aij, dumAij_top, dumAij;

static int		*LDGlhs, *LDGprolon, *LDGlispro;
static int		xLDGprod_top, xLDGprod, xLDGlispro_top, xLDGlispro;

static int		ldg_prod;

static int		_Aij, _Bkl;
#define check_locality(Aij,Bkl)	(_Aij = ((Aij) < 0 ? dumAij2Aij[-Aij] : (Aij)), _Bkl = ((Bkl) < 0 ? dumAij2Aij[-Bkl] : (Bkl)), _Aij == _Bkl)



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

static void
dumAij_oflw ()
{
    dumAij_top *= 2;
    dumAij2Aij = (int*) sxrealloc (dumAij2Aij, dumAij_top+1, sizeof (int));
}


static void
LDGprod_oflw ()
{
    xLDGprod_top *= 2;
    LDGlhs = (int*) sxrealloc (LDGlhs, xLDGprod_top+1, sizeof (int));
    LDGprolon = (int*) sxrealloc (LDGprolon, xLDGprod_top+1, sizeof (int));

}

static void
LDGlispro_oflw ()
{
    xLDGlispro_top *= 2;
    LDGlispro = (int*) sxrealloc (LDGlispro, xLDGlispro_top+1, sizeof (int));
}

EQUAL1_oflw (old_size, new_size)
    int		old_size, new_size;
{
    EQUAL12prod = (int*) sxrealloc (EQUAL12prod, new_size +1, sizeof (int));
}


static void
PUSH1_oflw (old_size, new_size)
    int		old_size, new_size;
{
    PUSH12prod = (int*) sxrealloc (PUSH12prod, new_size +1, sizeof (int));
}


static void
POP1_oflw (old_size, new_size)
    int		old_size, new_size;
{
    POP12prod = (int*) sxrealloc (POP12prod, new_size +1, sizeof (int));
}


static void
SECONDARY_oflw (old_size, new_size)
    int		old_size, new_size;
{
    SECONDARY2prod = (int*) sxrealloc (SECONDARY2prod, new_size +1, sizeof (int));
}


static void
SXLIG_semact_init (size)
    int size;
{
    int ssymb, prod, act_no, prdct_no, lim, item, i, pop_nb, push_nb, X;
    int type, post_action, post_prdct, dum_nb;
    int	*pca, *pcalim, *pcp, *pcplim;
    int ap_no, new_ap_no, old_xap_list;

    fill_inputG ();

    /* A FAIRE STATIQUEMENT */
    /* Chaque action ou predicat ap delimite ds SXLIPap_list une sequence comprise entre
       SXLIGap_disp [ap] et SXLIGap_disp [ap+1]-1 dont la structure verifie:
       (PRIMARY|SECONDARY) stack_lgth {stack_elems} post_ap
       ap = 0 refere la pile vide () codee SECONDARY 0 -1
       ap = 1 refere la pile any (..) codee PRIMARY 0 -1 */
    /* Index de l'objet primaire en RHS : 0, 1, ... */
    
    SXLIGprod2dum_nb = (int*) sxcalloc (inputG.maxprod+1, sizeof (int));

    SXLIGpost_action = (int*) sxcalloc (inputG.maxprod+1, sizeof (int));
    SXLIGpost_prdct = (int*) sxcalloc (inputG.maxitem+1, sizeof (int));

    SXLIGap_disp = (int*) sxcalloc (inputG.maxprod+2, sizeof (int));
    SXLIGap_list = (int*) sxcalloc (4*inputG.maxprod+1, sizeof (int)); /* Taille au pif */ 

    SXLIGap_disp [0] = 0;
    SXLIGap_list [0] = SECONDARY;
    SXLIGap_list [1] = 0;
    SXLIGap_disp [1] = 2;
    SXLIGap_list [2] = PRIMARY;
    SXLIGap_list [3] = 0;
    xap_list = 4;
    ap_no = 2;
    
    /* pc [0] = SECONDARY | PRIMARY
       pc [1] = level
       pc [2] = longueur du test du predicat
       pc [3] = longueur du prefixe commun entre action et predicat. */

    /* Les noms PUSH1 et POP1 des operateurs viennent de la vision derivation (et non pas
       analyse) des LIG: on POP sur l'action de la LHS et on PUSH sur le predicat de la RHS. */

    /* PROVISOIRE!! */
    inputG.constraints [1] = 10001;
    inputG.prdct [4] = 10001;

    is_normal_form = true;

    SXLIGpost_action [0] = -1;
    SXLIGpost_prdct [0] = SXLIGpost_prdct [1] = SXLIGpost_prdct [2] = -1;

    for (prod = 1; prod <= inputG.maxprod; prod++) {
	act_no = inputG.constraints [prod];
	pop_nb = 0;
	dum_nb = 0;
	new_ap_no = -1;

	if (act_no >= 10000) {
	    act_no -= 10000;

	    if (act_no > 1) {
		pca = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [act_no];
		type = *pca; /* PRIMARY|SECONDARY */
		pcalim = sxndlig_common.code.prdct_or_act_code +
				sxndlig_common.code.prdct_or_act_disp [act_no + 1] - 1;
		pca += 3;
		post_action = *pcalim--;
		pop_nb = pcalim - pca;
		dum_nb += pop_nb;

		if (pop_nb == 0) {
		     new_ap_no = type == SECONDARY ? 0 : 1;
		}
		else {
		    SXLIGap_disp [ap_no] = xap_list;

		    old_xap_list = xap_list;

#if EBUG
		    if (xap_list+2+pop_nb >= 4*inputG.maxprod)
			sxtrap (ME, "SXLIG_semact_init");
#endif

		    SXLIGap_list [xap_list++] = type;
		    SXLIGap_list [xap_list++] = pop_nb;

		    do {
			SXLIGap_list [xap_list++] = *pcalim;
		    } while (pca < --pcalim);

		    if ((new_ap_no = new_pa (ap_no)) == ap_no) {
			/* Nouvelle action */
			ap_no++;
		    }
		    else {
			xap_list = old_xap_list;
		    }

		    if (type == SECONDARY)
			is_normal_form = false;
		}
	    }
	    else
		new_ap_no = act_no;

	    new_ap_no += 10000;
	}
	else {
	    post_action = -1;
	    new_ap_no = act_no;
	}

	inputG.constraints [prod] = new_ap_no;
	SXLIGpost_action [prod] = post_action;

	item = inputG.prolon [prod];

	while ((X = inputG.lispro [item]) != 0) {
	    prdct_no = inputG.prdct [item];

	    if (X > 0 && prdct_no >= 10000) {
		prdct_no -= 10000;
		pcp = sxndlig_common.code.prdct_or_act_code + sxndlig_common.code.prdct_or_act_disp [prdct_no];
		type = *pcp; /* PRIMARY|SECONDARY */

		if (prdct_no > 1) {
		    pcplim = sxndlig_common.code.prdct_or_act_code +
			sxndlig_common.code.prdct_or_act_disp [prdct_no + 1] - 1;
		    pcp += 3;
		    post_prdct = *pcplim--;
		    push_nb = pcplim - pcp;
		    dum_nb += push_nb;

		    if (push_nb == 0) {
			new_ap_no = type == SECONDARY ? 0 : 1;
		    }
		    else {
			SXLIGap_disp [ap_no] = xap_list;

			old_xap_list = xap_list;

#if EBUG
			if (xap_list+2+push_nb >= 4*inputG.maxprod)
			    sxtrap (ME, "SXLIG_semact_init");
#endif
			SXLIGap_list [xap_list++] = type;
			SXLIGap_list [xap_list++] = push_nb;

			do {
			    SXLIGap_list [xap_list++] = *++pcp;
			} while (pcp < pcplim);

			if ((new_ap_no = new_pa (ap_no)) == ap_no) {
			    /* Nouvelle action */
			    ap_no++;
			}
			else {
			    xap_list = old_xap_list;
			}

			if (type == SECONDARY || type == PRIMARY && pop_nb+push_nb > 1)
			    is_normal_form = false;
		    }
		}
		else
		    new_ap_no = prdct_no;

		new_ap_no += 10000;
	    }
	    else {
		post_prdct = -1;
		new_ap_no = prdct_no;
	    }

	    inputG.prdct [item] = new_ap_no;
	    SXLIGpost_prdct [item] = post_prdct;

	    item++;
	}

	if (dum_nb > 1)
	    SXLIGprod2dum_nb [prod] = dum_nb-1;
    }

    /* On alloue les structures contenant les relations de niveau 1 */
        
    XxY_alloc (&Axs_hd, "Axs_hd", size+1 /* !! */, 1, 1, 0, NULL,
#ifdef EBUG
	       sxtty
#else
	       NULL
#endif
	       );

    XxY_alloc (&EQUAL1_hd, "EQUAL1_hd", size+1 /* !! */, 1, 1, 1, EQUAL1_oflw,
#ifdef EBUG
	       sxtty
#else
	       NULL
#endif
	       );

    EQUAL12prod = (int*) sxalloc (XxY_size (EQUAL1_hd) + 1, sizeof (int));

    XxY_alloc (&EQUALn_hd, "EQUALn_hd", size+1 /* !! */, 1, 1, 0, NULL,
#ifdef EBUG
	       sxtty
#else
	       NULL
#endif
	       );

    XxY_alloc (&PUSH1_hd, "PUSH1_hd", size+1 /* !! */, 1, 1, 1, PUSH1_oflw,
#ifdef EBUG
	       sxtty
#else
	       NULL
#endif
	       );

    PUSH12prod = (int*) sxalloc (XxY_size (PUSH1_hd) + 1, sizeof (int));

    XxY_alloc (&PUSHPOPn_hd, "PUSHPOPn_hd", size+1 /* !! */, 1, 1, 1, NULL,
#ifdef EBUG
	       sxtty
#else
	       NULL
#endif
	       );

    XxY_alloc (&POP1_hd, "POP1_hd", size+1, 1, 1, 1, POP1_oflw,
#ifdef EBUG
		 sxtty
#else
		 NULL
#endif
		 );

    POP12prod = (int*) sxalloc (XxY_size (POP1_hd) + 1, sizeof (int));

    XxY_alloc (&POPn_hd, "POPn_hd", size+1, 1, 0, 0, NULL,
#ifdef EBUG
		 sxtty
#else
		 NULL
#endif
		 );

    if (!is_normal_form) {
	X_alloc (&SECONDARY_hd, "SECONDARY_hd", size+1, 1, SECONDARY_oflw,
#ifdef EBUG
		 sxtty
#else
		 NULL
#endif
		 );

	SECONDARY2prod = (int*) sxalloc (X_size (SECONDARY_hd) + 1, sizeof (int));
    }
    else
	SECONDARY2prod = (int*) sxcalloc (size, sizeof (int));

    EQUALn_stack = CTRL_ALLOC (size);
    PUSHPOPn_stack = CTRL_ALLOC (size);	
    POPn_stack = CTRL_ALLOC (size);

    xLFprod_top = size;
    LFprod = (int*) sxalloc (xLFprod_top+1, sizeof (int));
    LFprolon = (int*) sxalloc (xLFprod_top+1, sizeof (int));
    LFprod2next = (int*) sxalloc (xLFprod_top+1, sizeof (int));
    
    xLFlispro_top = size;
    LFlispro = (int*) sxalloc (xLFlispro_top+1, sizeof (int));

    LFprod [xLFprod = 1] = 1;
    LFprolon [1] = xLFlispro = 1;

    if (!is_normal_form) {
	dumAij_top = size;
	dumAij2Aij = (int*) sxcalloc (dumAij_top+1, sizeof (int));
    }
}



static void
SXLIG_semact_final (size)
    int size;
{
    sxfree (SXLIGpost_action);
    sxfree (SXLIGpost_prdct);
    sxfree (SXLIGap_disp);
    sxfree (SXLIGap_list);
    sxfree (SXLIGprod2dum_nb);

    XxY_free (&Axs_hd);
    XxY_free (EQUAL1_hd);
    XxY_free (EQUALn_hd);
    XxY_free (PUSH1_hd);
    XxY_free (PUSHPOPn_hd);
    XxY_free (POP1_hd);
    XxY_free (POPn_hd);

    if (!is_normal_form)
	X_free (SECONDARY_hd);

    sxfree(EQUAL12prod);
    sxfree(PUSH12prod);
    sxfree(POP12prod);
    sxfree(SECONDARY2prod);

    CTRL_FREE (EQUALn_stack, "EQUALn_stack");
    CTRL_FREE (PUSHPOPn_stack, "PUSHPOPn_stack");
    CTRL_FREE (POPn_stack, "POPn_stack");
    CTRL_FREE (UNIQUE_stack, "UNIQUE_stack");

    sxfree (LFprod);
    sxfree (LFlispro);
    sxfree (LFprod2next);
    sxfree (LFprolon);

    if (!is_normal_form) {
	sxfree (dumAij2Aij);
    }

    if (UNIQUE_set != NULL) {
	sxfree (UNIQUE_set), UNIQUE_set = NULL;
	sxfree (EQUALn_set);
	sxfree (PUSHPOPn_set);
	sxfree (POPn_set);
    }

    if (LDGlhs != NULL) {
	sxfree (LDGlhs);
	sxfree (LDGprolon);
	sxfree (LDGlispro);
    }
}

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

    /* La foret partagee ne contient que les NT */
    
    Aij = rhs_stack [0];
    act_no = inputG.constraints [prod];

    if (!is_normal_form) {
	X_set (SECONDARY_hd, Aij, &Aij);

	if (SXLIGprod2dum_nb [prod] - dumAij >= dumAij_top)
	    dumAij_oflw ();
    }

    if (act_no >= 10000) {
	act_no -= 10000;
	pca = SXLIGap_list + SXLIGap_disp [act_no];
	atype = *pca++;
	pop_nb = *pca;

	if (pop_nb > 0)
	    CTRL_CHECK (POPn_stack, pop_nb);

	keep = (atype == PRIMARY) ? 1 : 0; /* On laise (au plus) keep pop */

	if (!is_normal_form) {
	    if (pop_nb > keep) {
		B = Aij;

		do {
		    A = B;
		    B = --dumAij;
		    dumAij2Aij [-dumAij] = Aij;

		    X_set (SECONDARY_hd, B, &B);

		    ssymb = *++pca;
		    XxY_set (&Axs_hd, B, ssymb, &sB);

		    if (!XxY_set (&POPn_hd, A, sB, &AsB)) {
			/* Nouveau triplet AsB */
			PUSH (POPn_stack, AsB);
		    }
		} while (--pop_nb > keep);

		Aij = B;
	    }
	}
    }
    else {
	pop_nb = 0;
	atype = SECONDARY;
    }

    if (atype == SECONDARY) {
	if (!is_normal_form) {
	    if (!X_set (&SECONDARY_hd, Aij, &Aij)) {
		SECONDARY2prod [Aij] = 0;
	    }
	}

	LFprod2next [xLFprod] = SECONDARY2prod [Aij];
	SECONDARY2prod [Aij] = xLFprod;
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

		if (!is_normal_form)
		    X_set (SECONDARY_hd, Bkl, &Bkl);

		prdct_no = inputG.prdct [item];

		if (prdct_no >= 10000) {
		    prdct_no -= 10000;
		    pcp = SXLIGap_list + SXLIGap_disp [prdct_no];
		    ptype = *pcp++;
		    push_nb = *pcp;

		    if (ptype == PRIMARY) {
			if (!is_normal_form) {
			    if ((nb = pop_nb+push_nb) > 1) {
				Bpri = B = --dumAij;
				dumAij2Aij [-dumAij] = Bkl;

				do {
				    A = B;
				    B = nb == 2 ? Bkl : --dumAij;

				    if (B < 0) {
					dumAij2Aij [-dumAij] = Bkl;
				    }

				    ssymb = *++pcp;
				    XxY_set (&Axs_hd, A, ssymb, &As);
				    XxY_set (&PUSH1_hd, As, B, &AsB);
				    push_nb--;
				} while (--nb > 1);

				Bkl = Bpri;
			    }
			}

			if (push_nb == 1) {
			    ssymb = *++pcp;
			    XxY_set (&Axs_hd, Aij, ssymb, &As);

			    if (!XxY_set (&PUSH1_hd, As, Bkl, &AsB))
				PUSH12prod [AsB] = 0;

			    LFprod2next [xLFprod] = PUSH12prod [AsB];
			    PUSH12prod [AsB] = xLFprod;
			}
			else {
			    if (pop_nb == 1) {
				ssymb = *++pca;
				XxY_set (&Axs_hd, Bkl, ssymb, &sB);

				if (!XxY_set (&POPn_hd, Aij, sB, &AsB)) {
				    /* Nouveau triplet AsB*/
				    PUSH (POPn_stack, AsB);
				    XxY_set (&POP1_hd, Aij, sB, &AsB);
				    POP12prod [AsB] = 0;
				}

				LFprod2next [xLFprod] = POP12prod [AsB];
				POP12prod [AsB] = xLFprod;
			    }
			    else {
				if (!XxY_set (&EQUALn_hd, Aij, Bkl, &AB)) {
				    CTRL_PUSH (EQUALn_stack, AB);
				    XxY_set (&EQUAL1_hd, Aij, Bkl, &AB);
				    EQUAL12prod [AB] = 0;
				}

				LFprod2next [xLFprod] = EQUAL12prod [AB];
				EQUAL12prod [AB] = xLFprod;
			    }
			}
		    }
		    else {
			/* SECONDARY PREDICATE */
			if (!is_normal_form) {
			    if (push_nb > 0) {
				B = --dumAij;
				dumAij2Aij [-dumAij] = Bkl;

				X_set (&SECONDARY_hd, B, &Bsec);

				do {
				    A = B;
				    B = push_nb == 1 ? Bkl : --dumAij;

				    if (B < 0) {
					dumAij2Aij [-dumAij] = Bkl;
				    }

				    ssymb = *++pcp;
				    XxY_set (&Axs_hd, A, ssymb, &As);
				    XxY_set (&PUSH1_hd, As, B, &AsB);
				} while (--push_nb > 0);

				Bkl = Bsec;
			    }
			}

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
print_LDGnt (AB)
    int AB;
{
    int 	A, B, sB, ssymb, Aij, Bkl, i, j, k, l;
    char	*op, *Aijstr, *Bklstr;

    if (AB <= EQUALn_nttop) {
	if (AB <= UNIQUE_nttop) {
	    /* A */
	    A = AB;
	    op = NULL;
	    ssymb = B = 0;
	}
	else {
	    /* A =n B */
	    AB -= EQUALn_ntbot;
	    A = XxY_X (EQUALn_hd, AB);
	    B = XxY_Y (EQUALn_hd, AB);
	    op = "=n";
	    ssymb = 0;
	}
    }
    else {
	if (AB <= POPn_nttop) {
	    /* A >n B */
	    AB -= POPn_ntbot;
	    A = XxY_X (POPn_hd, AB);
	    sB = XxY_Y (POPn_hd, AB);
	    ssymb = XxY_Y (Axs_hd, sB);
	    B = XxY_X (Axs_hd, sB);
	    op = "}n";
	}
	else {
	    /* A <>n B */
	    AB -= PUSHPOPn_ntbot;
	    A = XxY_X (PUSHPOPn_hd, AB);
	    B = XxY_Y (PUSHPOPn_hd, AB);
	    op = "{}n";
	    ssymb = 0;
	}
    }

    if (A < 0) {
	A = -A;
	Aij = dumAij2Aij [A];
    }
    else {
	Aij = A;
	A = 0;
    }
	
    if (B < 0) {
	B = -B;
	Bkl = dumAij2Aij [B];
    }
    else {
	Bkl = B;
	B = 0;
    }

    Aijstr = inputG.ntstring [Aij2A (Aij)]+1;
    i = Aij2i (Aij);
    j = Aij2j (Aij);

    if (A == 0)
	printf ("<%s[%i..%i]", Aijstr, i, j);
    else
	printf ("<%s(%i)[%i..%i]", Aijstr, A, i, j);

    if (ssymb != 0)
	printf (" |%i|%s", ssymb, op);
    else
	if (op != NULL)
	    printf (" %s", op);

    if (Bkl != 0) {
	Bklstr = inputG.ntstring [Aij2A (Bkl)]+1;
	k = Aij2i (Bkl);
	l = Aij2j (Bkl);

	if (B == 0)
	    printf (" %s[%i..%i]> ", Bklstr, k, l);
	else
	    printf (" %s(%i)[%i..%i]> ", Bklstr, B, k, l);
    }
    else
	fputs ("> ", stdout);
}

static void
print_LDG ()
{
    int prod, lim, item, X;

    for (prod = 1; prod < xLDGprod; prod++) {
	print_LDGnt (LDGlhs [prod]);
	fputs ("\t=\t", stdout);

	for (lim = LDGprolon [prod+1], item = LDGprolon [prod]; item < lim; item++) {
	    X = LDGlispro [item];

	    if (X < 0)
		printf ("\"%i\" ", -X);
	    else
		print_LDGnt (X);
	}

	fputs (" ;\n", stdout);
    }
}

static void
gen_LDGrhs (lfprod)
    int lfprod;
{
    int	lim = LFprolon [lfprod+1], item = LFprolon [lfprod], nb = lim - item, secCmn;

    if (xLDGlispro + nb >= xLDGlispro_top) LDGlispro_oflw ();

    if (nb > 1) {
	do {
	    secCmn = LFlispro [item];

	    if (SXBA_bit_is_reset_set (UNIQUE_set, secCmn))
		PUSH (UNIQUE_stack, secCmn);

	    LDGlispro [xLDGlispro++] = secCmn;
	} while (++item < lim);
    }

    LDGlispro [xLDGlispro++] = -LFprod [lfprod];
}


static bool
SXLIG_sem_pass (S0n)
    int S0n;
{
    int		A, B, C, AB, AC, BC, As, sC, sD, AsB, AsC, BsC, BsD, CsD, ssymb;

    int		Aij, secAij, Bkl, secBkl, Bmn, Ckl, Cmn, secCmn, AppC, lim, act_no, lfprod, prod, item;
    bool	is_local;
    

    EQUAL1_top = XxY_top (EQUALn_hd);
    POP1_top = XxY_top (POPn_hd);

    while (*EQUALn_stack + *POPn_stack) {
	while (!IS_EMPTY (EQUALn_stack)) {
	    BC = POP (EQUALn_stack);
	    B = XxY_X (EQUALn_hd, BC);
	    C = XxY_Y (EQUALn_hd, BC);

	    /* A EQUALn C = A EQUAL1 B et B EQUALn C  */
	    XxY_Yforeach (EQUAL1_hd, B, AB) {
		A = XxY_X (EQUAL1_hd, AB);

		if (!XxY_set (&EQUALn_hd, A, C, &AC)) {
		    CTRL_PUSH (EQUALn_stack, AC);
		}
	    }


	    /* B POPn sD = B EQUALn C et C POP1 sD */
	    XxY_Xforeach (POP1_hd, C, CsD) {
		sD = XxY_Y (POP1_hd, CsD);

		if (!XxY_set (&POPn_hd, B, sD, &BsD)) {
		    CTRL_PUSH (POPn_stack, BsD);
		}
	    }

	    /* A EQUALn C = A PUSHPOPn B et B EQUALn C et  */
	    XxY_Yforeach (PUSHPOPn_hd, B, AB) {
		A = XxY_X (PUSHPOPn_hd, AB);

		if (!XxY_set (&EQUALn_hd, A, C, &AC)) {
		    CTRL_PUSH (EQUALn_stack, AC);
		}
	    }
	}

	while (!IS_EMPTY (POPn_stack)) {
	    BsC = POP (POPn_stack);
	    B = XxY_X (POPn_hd, BsC);
	    sC = XxY_Y (POPn_hd, BsC);
	    C = XxY_X (Axs_hd, sC);
	    ssymb = XxY_Y (Axs_hd, sC);

	    /* A PUSHPOPn C = As PUSH1 B et B POPn sC */
	    XxY_Yforeach (PUSH1_hd, B, AsB) {
		As = XxY_X (PUSH1_hd, AsB);
		
		if (XxY_Y (Axs_hd, As) == ssymb) {
		    A = XxY_X (Axs_hd, As);

		    if (!XxY_set (&PUSHPOPn_hd, A, C, &AC)) {
			CTRL_PUSH (PUSHPOPn_stack, AC);
		    }
		}
	    }
	}

	while (!IS_EMPTY (PUSHPOPn_stack)) {
	    AB = POP (PUSHPOPn_stack);
	    A = XxY_X (PUSHPOPn_hd, AB);
	    B = XxY_Y (PUSHPOPn_hd, AB);

	    /* A EQUALn B = A PUSHPOPn B */
	    if (!XxY_set (&EQUALn_hd, A, B, &AB))
		CTRL_PUSH (EQUALn_stack, AB);

	    /* A EQUALn C = A PUSHPOPn B et B EQUALn C */
	    XxY_Xforeach (EQUALn_hd, B, BC) {
		C = XxY_Y (EQUALn_hd, BC);

		if (!XxY_set (EQUALn_hd, A, C, &AC)) {
		    CTRL_PUSH (EQUALn_stack, AC);
		}
	    }
	}
    }

    if (is_print_time)
	sxtime (TIME_FINAL, "\tSemantic Pass: Relations");

    EQUALn_top = XxY_top (EQUALn_hd);
    EQUALn_set = sxba_calloc (EQUALn_top+1);
    POPn_top = XxY_top (POPn_hd);
    POPn_set = sxba_calloc (POPn_top+1);
    PUSHPOPn_top = XxY_top (PUSHPOPn_hd);
    PUSHPOPn_set = sxba_calloc (PUSHPOPn_top+1);

    EQUALn_ntbot = UNIQUE_nttop = pAij_top;
    POPn_ntbot = EQUALn_nttop = UNIQUE_nttop + EQUALn_top;
    PUSHPOPn_ntbot = POPn_nttop = EQUALn_nttop + POPn_top;
    PUSHPOPn_nttop = POPn_nttop + PUSHPOPn_top;
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

    if (!is_normal_form) {
	UNIQUE_set = sxba_calloc (X_top (SECONDARY_hd)+1);
	UNIQUE_stack = CTRL_ALLOC (X_top (SECONDARY_hd)+1);
    }
    else {
	/* On pourrait compter les secondary!! */
	UNIQUE_set = sxba_calloc (pAij_top+1);
	UNIQUE_stack = CTRL_ALLOC (pAij_top+1);
    }

    if (!is_normal_form)
	X_set (SECONDARY_hd, S0n, &S0n);

    if (SXBA_bit_is_reset_set (UNIQUE_set, S0n))
	PUSH (UNIQUE_stack, S0n);

    do {
	while (!IS_EMPTY (UNIQUE_stack)) {
	    secAij = POP (UNIQUE_stack);

	    if (!is_normal_form)
		Aij = XxY_X (SECONDARY_hd, secAij);
	    else
		Aij = secAij;

	    for (lfprod = SECONDARY2prod [secAij]; lfprod != 0; lfprod = LFprod2next [lfprod]) {
		/* on genere [A] -> [X1] ... [Xl] "prod" */
		LDGlhs [xLDGprod] = Aij;

		gen_LDGrhs (lfprod);

		if (++xLDGprod > xLDGprod_top) LDGprod_oflw ();
		LDGprolon [xLDGprod] = xLDGlispro;
	    }

	    /* Si r1 : A (..) -> \Gamma1 B (..) \Gamma'1 et
	       si r2 : B() -> \Gamma2 \Gamma'2
	       on genere [A] -> [\Gamma2 \Gamma'2] r2 [\Gamma1 \Gamma'1] r1 */


	    /* lfprod' : Cmn () -> \Gamma
	       on genere [Aij] -> [\Gamma] "prod'" [Aij =n Cmn]
	       ssi (Aij, Cmn) \in EQUALn_hd */
	    XxY_Xforeach (EQUALn_hd, Aij, AB) {
		Bkl =  XxY_Y (EQUALn_hd, AB);

		if (!is_normal_form)
		    secBkl = X_is_set (SECONDARY_hd, Bkl, &secBkl);
		else
		    secBkl = Bkl;

		/* On parcourt les Bkl-productions secondaires de la foret partagee */
		for (lfprod = SECONDARY2prod [Bkl]; lfprod != 0; lfprod = LFprod2next [lfprod]) {
		    prod = LFprod [lfprod];
		    /* lfprod : A() -> w1 X1() ... wl Xl() wl+1
		       on genere [A] -> [X1] ... [Xl] "prod" */
		    LDGlhs [xLDGprod] = Aij;

		    gen_LDGrhs (lfprod);

		    is_local = check_locality (Aij, Bkl); /* decomposition en operations elementaires
							     d'une pile */

		    if (!is_local) {
			LDGlispro [xLDGlispro] = EQUALn_ntbot + AB;
			if (++xLDGlispro > xLDGlispro_top) LDGlispro_oflw ();

			if (SXBA_bit_is_reset_set (EQUALn_set, AB))
			    PUSH (EQUALn_stack, AB);
		    }

		    if (++xLDGprod > xLDGprod_top) LDGprod_oflw ();
		    LDGprolon [xLDGprod] = xLDGlispro;
		}
	    }
	}

	while (!IS_EMPTY (EQUALn_stack)) {
	    AC = POP (EQUALn_stack);
	    Aij = XxY_X (EQUALn_hd, AC);
	    Ckl = XxY_Y (EQUALn_hd, AC);

	    if (AC <= EQUAL1_top) {
		/* AC est un element de EQUAL1 */
		/* On parcourt les AijCkl-productions primaires de la foret partagee */
		/* a la recherche de Aij(..) -> \Gamma1 Ckl (..) \Gamma2 */
		for (lfprod = EQUAL12prod [AC]; lfprod != 0; lfprod = LFprod2next [lfprod]) {
		    /* prod est une Aij(..) ->  .. Ckl(..) ... */
		    LDGlhs [xLDGprod] = EQUALn_ntbot + AC;

		    gen_LDGrhs (lfprod);
		    
		    if (++xLDGprod > xLDGprod_top) LDGprod_oflw ();
		    LDGprolon [xLDGprod] = xLDGlispro;
		}
	    }

	    if ((AppC = XxY_is_set (PUSHPOPn_hd, Aij, Ckl)) > 0) {
		/* On a Aij <>n Ckl */
		if (SXBA_bit_is_reset_set (PUSHPOPn_set, AppC))
		    PUSH (PUSHPOPn_stack, AppC);

		LDGlhs [xLDGprod] = EQUALn_ntbot + AC;
		LDGlispro [xLDGlispro] = PUSHPOPn_ntbot + AppC;
		if (++xLDGlispro > xLDGlispro_top) LDGlispro_oflw ();
		if (++xLDGprod > xLDGprod_top) LDGprod_oflw ();
		LDGprolon [xLDGprod] = xLDGlispro;
	    }

	    /* On cherche Bmn tq Aij =n Ckl se decompose en Aij =1 Bmn =n Ckl */
	    XxY_Xforeach (EQUAL1_hd, Aij, AB) {
		Bmn = XxY_Y (EQUAL1_hd, AB);

		if ((BC = XxY_is_set (&EQUALn_hd, Bmn, Ckl)) > 0) {
		    /* On parcourt les AijBmn-productions primaires de la foret partagee */
		    is_local = check_locality (Bmn, Ckl);

		    if (!is_local) {
			if (SXBA_bit_is_reset_set (EQUALn_set, BC))
			    PUSH (EQUALn_stack, BC);
		    }

		    for (lfprod = EQUAL12prod [AB]; lfprod != 0; lfprod = LFprod2next [lfprod]) {
			/* prod est une Aij(..) ->  .. Bmn(..) ... */
			
			LDGlhs [xLDGprod] = EQUALn_ntbot + AC;

			if (!is_local) {
			    LDGlispro [xLDGlispro] = EQUALn_ntbot + BC;
			    if (++xLDGlispro > xLDGlispro_top) LDGlispro_oflw ();
			}

			gen_LDGrhs (lfprod);
			
			if (++xLDGprod > xLDGprod_top) LDGprod_oflw ();
			LDGprolon [xLDGprod] = xLDGlispro;
		    }
		}
	    }

	    /* On cherche Bmn tq Aij =n Ckl se decompose en Aij <>n Bmn =n Ckl */
	    XxY_Xforeach (PUSHPOPn_hd, Aij, AB) {
		Bmn = XxY_Y (PUSHPOPn_hd, AB);

		if ((BC = XxY_is_set (&EQUALn_hd, Bmn, Ckl)) > 0) {
		    /* C'est un bon */
		    LDGlhs [xLDGprod] = EQUALn_ntbot + AC;

		    is_local = check_locality (Bmn, Ckl);

		    if (!is_local) {
			if (SXBA_bit_is_reset_set (EQUALn_set, BC))
			    PUSH (EQUALn_stack, BC);

			LDGlispro [xLDGlispro] = EQUALn_ntbot + BC;
			if (++xLDGlispro > xLDGlispro_top) LDGlispro_oflw ();
		    }

		    is_local = check_locality (Aij, Bmn);

		    if (!is_local) {
			if (SXBA_bit_is_reset_set (PUSHPOPn_set, AB))
			    PUSH (PUSHPOPn_stack, AB);

			LDGlispro [xLDGlispro] = PUSHPOPn_ntbot + AB;
			if (++xLDGlispro > xLDGlispro_top) LDGlispro_oflw ();
		    }

		    if (++xLDGprod > xLDGprod_top) LDGprod_oflw ();
		    LDGprolon [xLDGprod] = xLDGlispro;
		}
	    }
	}

	while (!IS_EMPTY (PUSHPOPn_stack)) {
	    AC = POP (PUSHPOPn_stack);
	    Aij = XxY_X (PUSHPOPn_hd, AC);
	    Ckl = XxY_Y (PUSHPOPn_hd, AC);

	    /* On cherche Bmn tq Aij <>n Ckl se decompose en Aij <1s Bmn >ns Ckl */
	    XxY_Xforeach (Axs_hd, Aij, As) {
		ssymb = XxY_Y (Axs_hd, As);

		if ((sC = XxY_is_set (&Axs_hd, Ckl, ssymb)) > 0) {
		    XxY_Xforeach (PUSH1_hd, As, AsB) {
			Bmn = XxY_Y (PUSH1_hd, AsB);

			if ((BsC = XxY_is_set (&POPn_hd, Bmn, sC)) > 0) {
			    /* On en tient un */
			    is_local = check_locality (Bmn, Ckl);

			    if (!is_local) {
				if (SXBA_bit_is_reset_set (POPn_set, BsC))
				    PUSH (POPn_stack, BsC);
			    }

			    for (lfprod = PUSH12prod [AsB]; lfprod != 0; lfprod = LFprod2next [lfprod]) {
				/* prod est une Aij(..) ->  .. Bmn(..) ... */
			
				LDGlhs [xLDGprod] = PUSHPOPn_ntbot + AC;

				if (!is_local) {
				    LDGlispro [xLDGlispro] = POPn_ntbot + BsC;
				    if (++xLDGlispro > xLDGlispro_top) LDGlispro_oflw ();
				}

				gen_LDGrhs (lfprod);
				
				if (++xLDGprod > xLDGprod_top) LDGprod_oflw ();
				LDGprolon [xLDGprod] = xLDGlispro;
			    }
			}
		    }
		}
	    }
	}

	while (!IS_EMPTY (POPn_stack)) {
	    AsC = POP (POPn_stack);
	    Aij = XxY_X (POPn_hd, AsC);
	    sC = XxY_Y (POPn_hd, AsC);
	    ssymb = XxY_Y (Axs_hd, sC);
	    Ckl = XxY_X (Axs_hd, sC);

	    /* On cherche les POPn qui sont des POP1 */
	    if (AsC <= POP1_top) {
		/* AsC est un element de POP1 */
		/* On parcourt les Aij-productions primaires de la foret partagee */
		/* a la recherche de Aij(.. \gamma) -> \Gamma1 Ckl (..) \Gamma2 */
		for (lfprod = POP12prod [AsC]; lfprod != 0; lfprod = LFprod2next [lfprod]) {
		    /* prod est une Aij(.. ssymb) ->  .. Ckl(..) ... */
		    LDGlhs [xLDGprod] = POPn_ntbot + AsC;

		    gen_LDGrhs (lfprod);

		    if (++xLDGprod > xLDGprod_top) LDGprod_oflw ();
		    LDGprolon [xLDGprod] = xLDGlispro;
		}
	    }

	    /* Attention, un POP1 peut aussi etre un POPn */
	    /* On cherche Bmn tq Aij >ns Ckl se decompose en Aij =n Bmn >1s Ckl */
	    XxY_Yforeach (POP1_hd, sC, BsC) {
		Bmn = XxY_X (POP1_hd, BsC);

		if ((AB = XxY_is_set (&EQUALn_hd, Aij, Bmn)) > 0) {
		    is_local = check_locality (Aij, Bmn);

		    if (!is_local) {
			if (SXBA_bit_is_reset_set (EQUALn_set, AB))
			    PUSH (EQUALn_stack, AB);
		    }

		    for (lfprod = POP12prod [BsC]; lfprod != 0; lfprod = LFprod2next [lfprod]) {
			/* prod est une Aij(.. ssymb) ->  .. Bmn(..) ... */
			
			LDGlhs [xLDGprod] = POPn_ntbot + AsC;

			gen_LDGrhs (lfprod);

			if (!is_local) {
			    LDGlispro [xLDGlispro] = EQUALn_ntbot + AB;
			    if (++xLDGlispro > xLDGlispro_top) LDGlispro_oflw ();
			}

			if (++xLDGprod > xLDGprod_top) LDGprod_oflw ();
			LDGprolon [xLDGprod] = xLDGlispro;
		    }
		}
	    }
	}
    } while (*UNIQUE_stack + *EQUALn_stack + *POPn_stack + *PUSHPOPn_stack);

    if (is_print_prod) {
	print_LDG ();
    }

    if (is_print_time) {
	fprintf (sxtty, "Nombre de productions de la grammaire des derivations lineaires : %i\n", xLDGprod-1);
	sxtime (TIME_FINAL, "\tSemantic Pass: Linear Derivation Grammar");
    }

    return true;
}

void SXLIG_actions (SXINT what, SXINT action_no, SXTABLES *arg)
{
    (void) action_no;
    switch (what) {
    case SXOPEN:
	sxndlig_common.sxtables = arg;
	sxndlig_common.code = *(arg->sxligparsact);

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

#if 0
	/* A FAIRE */
	(*sxndlig_common.code.parsact) (which, arg);
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

