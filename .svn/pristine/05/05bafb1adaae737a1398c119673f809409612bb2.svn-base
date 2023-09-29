/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1989 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 02-12-92 16:21 (pb):		Bug ds la taille de "back_to_attr"	*/
/************************************************************************/
/* 01-08-91 14:45 (pb):		Utilisation des XxYxZ			*/
/************************************************************************/
/* 10-05-90 17:07 (pb):		Changement de la structure des bfsa et	*/
/*				des pss lorsque le type est FSA_ :	*/
/*				init {liste de couples} final type	*/
/************************************************************************/
/* 07-05-90 11:21 (pb):		Le sommet de chaque pss est la h_value	*/
/*				courante				*/
/************************************************************************/
/* 25-04-90 11:32 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#define WHAT	"@(#)XARC.c	- SYNTAX [unix] - 2 Décembre 1992"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "XARC";

#include "P_tables.h"

#include "rlr_optim.h"

#include "SS.h"
#include "RLR.h"


extern SXVOID	print_bfsa ();
extern SXVOID	print_pss ();
extern SXBOOLEAN	ARP_simple_ambiguity ();
extern SXBOOLEAN	PARTIAL_FSA_ARP_construction ();
extern int	TOTAL_FSA_ARP_construction ();
extern int	UNBOUNDED_ARP_construction ();
extern SXVOID	put_new_clone ();
extern SXBOOLEAN	is_old_clone ();
extern SXBOOLEAN	open_listing ();
extern SXBOOLEAN	make_inter_pss ();
extern char	*conflict_kind_to_string ();
extern VARSTR	conflict_paths_to_re ();
extern SXBOOLEAN	bh_mark ();
extern SXBOOLEAN	bh_unmark ();


static SXBA		*X_plus;
static int		X_plus_size;

static SXBA		fork_work_set, final_fork_set;
static int		fork_work_set_m, fork_work_set_M, final_fork_set_m,
                        final_fork_set_M;
static XxY_header	XARC_TxXARC_hd;
static int		*XARC_TxXARC_to_orig;

static SXBOOLEAN		MESSAGE_NEEDED;

static SXBA		CUR_PSS_SET;

static X_header		pss_to_seq_hd;
struct ipss_to_attr_global {
    int repr, lnk, in, occur_nb;
};
struct ipss_to_attr_local {
    int bfsa, local_bfsa;
};
struct ipss_to_attr {
    struct ipss_to_attr_global global;
    struct ipss_to_attr_local local;
};
static struct ipss_to_attr	*ipss_to_attr;
static struct ipss_to_attr_global null_global;
static struct ipss_to_attr_local null_local;
static int		*ipss_stack;
static XxY_header	back_hd;
struct back_to_attr {
    int	ibfsa, orig;
};
static struct back_to_attr	*back_to_attr;

static SXVOID XARC_UNBOUNDED_Q_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ARCs [XARC_UNBOUNDED].attr = (struct ARC_ATTR*) sxrealloc (ARCs [XARC_UNBOUNDED].attr, new_size + 1, sizeof (struct ARC_ATTR));
}
static SXVOID XARC_FSA_Q_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ARCs [XARC_FSA].attr = (struct ARC_ATTR*) sxrealloc (ARCs [XARC_FSA].attr, new_size + 1, sizeof (struct ARC_ATTR));
}

static SXVOID	oflw_XQ0xXQ0 (old_size, new_size)
    int		old_size, new_size;
{
    couples_set = sxba_resize (couples_set, Q0xQ0_top + new_size + 1);
}



static SXVOID	oflw_forks (old_size, new_size)
    int		old_size, new_size;
{
    fork_work_set = sxba_resize (fork_work_set, new_size + 1);
    final_fork_set = sxba_resize (final_fork_set, new_size + 1);
}


static SXVOID	oflw_FORKxT (old_size, new_size)
    int		old_size, new_size;
{
    FORKxT_to_fks = (int*) sxrealloc (FORKxT_to_fks, new_size + 1, sizeof (int))
	 ;
}



static SXVOID	oflw_bfsa (old_size, new_size)
    int		old_size, new_size;
{
    register int i;

    bfsa_to_ate = (int*) sxrealloc (bfsa_to_ate, i = new_size + 1, sizeof (int));

    while (--i > old_size)
	bfsa_to_ate [i] = 0;
}



static SXVOID	oflw_pss_to_seq (old_size, new_size)
    int		old_size, new_size;
{
    ipss_to_attr = (struct ipss_to_attr *) sxrealloc (ipss_to_attr, new_size + 1, sizeof (struct ipss_to_attr));
}


static SXVOID	oflw_back (old_size, new_size)
    int		old_size, new_size;
{
    back_to_attr = (struct back_to_attr *) sxrealloc (back_to_attr, new_size + 1, sizeof (struct back_to_attr));
}


static SXVOID	XARC_allocate ()
{

#define to	4
    
    int		size;
    
    if (fork_work_set == NULL) {
	/* Premier appel de XARC_init */
	null_global.occur_nb = -1;
	XH_alloc (&bfsa_hd, "bfsa_hd", to * xac2, 4, to + 2, is_automaton && is_listing_opened ? oflw_bfsa : NULL, statistics_file);
	XH_alloc (&fks_hd, "fks_hd", to * 2 * xac2, 4, 2, NULL, statistics_file);
	XxY_alloc (&forks, "forks", to * xac2, 4, 0, 0, oflw_forks, statistics_file);
	fork_work_set = sxba_calloc (XxY_size (forks) + 1);
	final_fork_set = sxba_calloc (XxY_size (forks) + 1);
	X_alloc (&pss_to_seq_hd, "pss_to_seq", xac2, 3, oflw_pss_to_seq, statistics_file);  
	ipss_to_attr = (struct ipss_to_attr *) sxalloc (X_size (pss_to_seq_hd) + 1, sizeof (struct ipss_to_attr));
	ipss_stack = SS_alloc (xac2);
	XxY_alloc (&back_hd, "back", xac2, 4, 1, 1, oflw_back, statistics_file);
	back_to_attr = (struct back_to_attr *) sxalloc (XxY_size (back_hd) + 1, sizeof (struct back_to_attr));
	Q0_to_occur_nb = (int*) sxalloc (xac2, sizeof (int));
	XxY_alloc (&Q0xN_hd, "Q0xN", xac2, 4, 0, 0, NULL, statistics_file);
	XxY_alloc (&XQ0xXQ0_hd, "XQ0xXQ0", xac2, 4, 0, 0, oflw_XQ0xXQ0, statistics_file);
	couples_set = sxba_resize (couples_set, Q0xQ0_top + XxY_size (XQ0xXQ0_hd) + 1);
	XxY_alloc (&FORKxT_hd, "FORKxT", to * xac2, 4, 0, 0, oflw_FORKxT, statistics_file);
	FORKxT_to_fks = (int*) sxalloc (XxY_size (FORKxT_hd) + 1, sizeof (int));
	lgt [11] = 20;
	
	lpss = SS_alloc (xac2);
	
	if (is_automaton && is_listing_opened) {
	    bfsa_to_ate = (int*) sxcalloc (XH_size (bfsa_hd) + 1, sizeof (int));
	}
	
	ARP_alloc ();
    }
    else {
    	/* Le premier appel a deja ete effectue, on se contente de
	   reinitialiser les structures */
	register int	i, lim;
	
	XH_clear (&bfsa_hd);
	XH_clear (&fks_hd);
	XxY_clear (&forks);
	XxY_clear (&FORKxT_hd);
	XxY_clear (&Q0xN_hd);
	XxY_clear (&XQ0xXQ0_hd);
	
	if (bfsa_to_ate != NULL) {
	    i = XH_size (bfsa_hd);
	    
	    while (i > 0)
		bfsa_to_ate [i--] = 0;
	}
	
	ARP_realloc ();
    }
}



static SXVOID	fks_to_fork_set (xfks, set, m, M)
    int		xfks, *m, *M;
    SXBA	set;
{
    /* Emplit set avec xfks */
    register int	x, lim, V;
    
    lim = XH_X (fks_hd, xfks + 1);
    V = XH_list_elem (fks_hd, x = XH_X (fks_hd, xfks));
    *m = min (*m, V);
    SXBA_1_bit (set, V);
    
    while (++x < lim) {
	V = XH_list_elem (fks_hd, x);
	SXBA_1_bit (set, V);
    }

    *M = max (*M, V);
}


/* Sur une grammaire de c il existe un [sous-]arbre de la relation back qui contient
   166 pss (la plupart de hauteur 2) et 2077 transitions. Chaque noeud est donc, en
   moyenne relie a 13 autres!.
   Ce graphe contient 2 knots, l'un de 8 noeuds l'autre de 23.
   Apres transformation en DAG (detection et fabrication des knots),
   l'appel de fork_collecting produit 2480640 appels de ubstack_do. Ce nombre est le
   nombre de [sous-]chemins du DAG => explosion de bfsa_hd.
   Ca se produit ds les conditions suivantes:
   Constructeur LR(3), conflit Shift/Reduce sur l'etat 118. Transitions :
   1 ->"," 2 ->"%NAME 5 ->")" 34
   L'etat 5 contient 2 pss identiques (22=60-119) et la transition sur ")" produit
   l'etat 34 qui contient 2 ensembles de pss de 166 elements chacuns. Un certain nombre
   de ces pss (31 et tous de hauteur 2) ont en sommet de pile un etat conflictuel
   (en LALR(1)) du LR(0) qui a ete detecte comme etant ambigu par le test d'ambiguite
   (cycle de includes et read non vide).
   Peut-on conclure qqc sur le conflit initial si, apres simulation sur l'automate
   LR(0) de la reconnaissance de "," "%NAME" ")" on tombe sur un etat (une pss?)
   implique ds une ambiguite?
   Il semblerait qu'on ne puisse rien dire. La grammaire
   <S>	= <A> ;
   <A>	= a <A> ;
   <A>	= a <A> b ;
   <A>	= <B> d ;
   <A>	= <C> b e ;
   <B>	= c ;
   <C>	= c d ;
   est ambigue :
      A => a A => a a A b et
      A => a A b => a a A b
   cette ambiguite est detectee dans l'etat 7.
   Il y a egalement un conflit Shift/Reduce en LALR(1) ds l'etat 2 qui ne se resoud
   pas en [LA]LR(2) mais en LALR(3) bien que l'un des pss a pour sommet l'etat 7.

   La solution qui consiste a :
   representer les bfsa par des couples (a, b) ou a est un bfsa (ou 0 pour le bfsa
   initial) et b la representation d'un DAG (contenant des knots) par des ensembles
   de couples (cette representation semble permettre de reconstituer exactement
   les memes piles que precedemment) semble tout de meme impraticable vu le nombre
   de chemins (plus de 2 millions) trouve. */

static SXVOID put_in_fork_work_set (bfsa, pss)
    int bfsa, pss;
{
    int	fork;

    if (SXBA_bit_is_set (CUR_PSS_SET, pss)) {
	XxY_set (&forks, bfsa, pss, &fork);
	
	if (SXBA_bit_is_reset_set (fork_work_set, fork)) {
	    fork_work_set_m = min (fork_work_set_m, fork);
	    fork_work_set_M = max (fork_work_set_M, fork);
	}
    }
}

static SXVOID LRpi_Next (bfsa, pss)
    int bfsa;
    register int	pss;
{
    /* On memorise les resultats de l'application de Next sur un pss. */
    register int	x, top_state, n;
    int			lim, ptn;

    top_state = TopState (pss);

    if (SXBA_bit_is_set (Next_states_set, top_state)) {
	if (X_set (&pss_to_Next_hd, pss, &ptn))
	    /* deja_calcule dans l'ARC */
	    n = pss_to_Next [ptn];
	else
	    sxtrap (ME, "LRpi_Next");

	for (lim = XH_X(Next_hd, n + 1), x = XH_X (Next_hd, n); x < lim; x++) {
	    put_in_fork_work_set (bfsa, XH_list_elem (Next_hd, x));
	}
    }
    else
	put_in_fork_work_set (bfsa, pss);
}


static SXVOID couples_set_to_XH (header)
    XH_header	*header;
{
    register int couple = 0;

    while ((couple = sxba_scan_reset (couples_set, couple)) > 0)
	XH_push (*header, couple);
}

static SXVOID XH_to_couples_set (bot, top)
    register int	*bot, *top;
{
    register int couple;

    while (bot < top) {
	couple = *bot++;
	SXBA_1_bit (couples_set, couple);
    }
}


/* Pour la grammaire de c, un DAG de back a les caracteristiques suivantes:
     - 167 noeuds
     - back a 2038 couples
     - Q0xN a 2441 elements
     - XQ0xXQ0 en a 3863
     - Il y a (entre autre) 253 occurrences differentes du meme etat LR(0) ds
       les chemins du DAG. */

static SXVOID back_to_couples_set (ibfsa, X, Y)
    int ibfsa, X, Y;
{
/* Soit X ->ibfsa Y l'element de back labelle' par ibfsa et
   ibfsa = bot:p1 p2 ... pl:top la pile ibfsa des etats du LR (0). On a :
      - p1 le bot du pss associe a X
      - pl le bot du pss associe a Y
      - les (pi, pi-1) sont des elements de Q0xQ0_hd */
    register int	p, q;
    register int	*bot, *top;
    int			xp, xq, pq, nb;

    bot = IBFSA_BOT (ibfsa);
    top = IBFSA_TOP (ibfsa) - 1;
    p = *top;
    
    if (pss_kind == UNBOUNDED_ && (nb = ipss_to_attr [Y].global.occur_nb) != 0) {
	XxY_set (&Q0xN_hd, p, nb, &xp);
	p = xp + xac2;
    }
    
    while (--top > bot) {
	q = *top;
	
	if (pss_kind == UNBOUNDED_) {
	    if ((nb = ++(Q0_to_occur_nb [q])) > 0) {
		XxY_set (&Q0xN_hd, q, nb, &xq);
		q = xq + xac2;
	    }
	}
	
	if (p < xac2 && q < xac2)
	    pq = XxY_is_set (&Q0xQ0_hd, p, q);
	else {
	    XxY_set (&XQ0xXQ0_hd, p, q, &pq);
	    pq += Q0xQ0_top;
	}
	
	SXBA_1_bit (couples_set, pq);
	p = q;
    }
    
    q = *bot;
    
    if (pss_kind == UNBOUNDED_ && (nb = ipss_to_attr [X].global.occur_nb) != 0) {
	XxY_set (&Q0xN_hd, q, nb, &xq);
	q = xq + xac2;
    }
    
    if (p < xac2 && q < xac2)
	pq = XxY_is_set (&Q0xQ0_hd, p, q);
    else {
	XxY_set (&XQ0xXQ0_hd, p, q, &pq);
	pq += Q0xQ0_top;
    }
    
    SXBA_1_bit (couples_set, pq);
}



static SXVOID	process_knot (ipss_set)
    register SXBA	ipss_set;
{
    /* ipss_set contient un knot de back. */
    /* Tous les elements du knot sont equivalents. */
    /* Calcul du representant de la classe d'equivalence (on choisit le plus
       petit X), stockage du knot. */
    register int	ipss, new_ipss, x, y;
    int			Y, repr;

    ipss_to_attr [repr].global.repr = new_ipss = ipss = repr = sxba_scan (ipss_set, 0);

    while ((new_ipss = sxba_scan (ipss_set, new_ipss)) > 0) {
	ipss_to_attr [new_ipss].global.repr = repr;
	ipss_to_attr [ipss].global.lnk = new_ipss;
	ipss = new_ipss;
    } 

    ipss_to_attr [ipss].global.lnk = 0;
}



static SXBOOLEAN check_loop_on_back (X1)
    int		X1;
{
    /* On sait que back n'est pas un arbre, on regarde si back a un cycle. */
    register int	x, X2;
    SXBOOLEAN		is_cycle = SXFALSE;

    PUSH (pss_check_stack, X1);

    XxY_Xforeach (back_hd, X1, x) {
	if (pss_check_stack [X2 = XxY_Y (back_hd, x)] != 0 || check_loop_on_back (X2)) {
	    is_cycle = SXTRUE;
	    break;
	}
    }

    POP (pss_check_stack, X1);
    SXBA_0_bit (pss_work_set, X1);
    return is_cycle;
}



static SXBOOLEAN call_check_loop_on_back ()
{
    register int ipss = 0;

    while ((ipss = sxba_scan (pss_work_set, ipss)) > 0) {
	if (check_loop_on_back (ipss))
	    return SXTRUE;
    }

    return SXFALSE;
}


static SXVOID down_occur_nb ();
static SXVOID up_occur_nb (Y, nb)
    int Y, nb;
{
    register int X, i;

    ipss_to_attr [Y].global.occur_nb = nb;

    XxY_Yforeach (back_hd, Y, i) {
	if (back_to_attr [i].ibfsa == 0 &&
	    ipss_to_attr [X = XxY_X (back_hd, i)].global.occur_nb == -1) {
	    up_occur_nb (X, nb);
	    down_occur_nb (X, nb);
	}
    }
}

static SXVOID down_occur_nb (X, nb)
    int X, nb;
{
    register int Y, i;

    ipss_to_attr [X].global.occur_nb = nb;

    XxY_Xforeach (back_hd, X, i) {
	if (back_to_attr [i].ibfsa == 0 &&
	    ipss_to_attr [Y = XxY_Y (back_hd, i)].global.occur_nb == -1) {
	    down_occur_nb (Y, nb);
	    up_occur_nb (Y, nb);
	}
    }
}

static SXVOID compute_occur_nb (R, N)
    int R, N;
{
/* On associe a chaque noeud de la relation back un numero.
   Les noeuds relies par des ibfsa nuls doivent porter le meme numero
   Les noeuds dont les etats caracteristiques sont differents peuvent
   porter le meme numero. On essaie d'utiliser le numero 0 au maximum. */
    register int	i, L, Y, nb;

    /* initialisation :
	ipss_to_attr [*].global.occur_nb = -1;
       deja effectuee. */

    for (i = 1; i < xac2; i++)
	Q0_to_occur_nb [i] = -1;

    /* On s'occupe tout d'abord des noeuds lies par des ibfsa nuls */
    L = X_top (back_hd);

    for (i = 1; i <= L;i++) {
	if (back_to_attr [i].ibfsa == 0 &&
	    ipss_to_attr [Y = XxY_Y (back_hd, i)].global.occur_nb == -1) {
	    nb = ++(Q0_to_occur_nb [BotState (X_X (pss_to_seq_hd, Y))]);
	    up_occur_nb (Y, nb);
	    down_occur_nb (Y, nb);
	}
    }

    for (i = 1; i <= N;i++) {
	/* On regarde les noeuds */
	if (ipss_to_attr [i].global.occur_nb == -1) {
	    /* non traites. */
	    Y = X_X (pss_to_seq_hd, i);

	    if (i <= R) /* Les racines */
		Y = XxY_X (PSSxT_hd, -Y);

	    /* Tous */
	    ipss_to_attr [i].global.occur_nb = ++(Q0_to_occur_nb [BotState (Y)]);
	}
    }
}


static VARSTR make_ibfsa_to_ate (vstr, ibfsa, X, Y)
    VARSTR	vstr;
    int		ibfsa, X, Y;
{
    register int	ate, prev_lgth, nb;
    register int	*bot, *top;
    char		s [8];

    if ((nb = ipss_to_attr [X].global.occur_nb) != 0) {
	sprintf (s, "%d:", nb);
	varstr_catenate (vstr, s);
    }

    if ((ate = ibfsa_to_ate [ibfsa]) == 0) {
	prev_lgth = varstr_length (vstr);
	bot = IBFSA_BOT (ibfsa);
	top = IBFSA_TOP (ibfsa);
	sprintf (s, "%d", *bot);
	varstr_catenate (vstr, s);    

	while (++bot < top) {
	    sprintf (s, " %d", *bot);
	    varstr_catenate (vstr, s);
	}

	ibfsa_to_ate [ibfsa] = sxstrsave (varstr_tostr (vstr) + prev_lgth);
    }
    else
	vstr = varstr_catenate (vstr, sxstrget (ate));

    if ((nb = ipss_to_attr [Y].global.occur_nb) != 0) {
	sprintf (s, ":%d", nb);
	varstr_catenate (vstr, s);
    }

    return vstr;
}

static int make_ubfsa_to_ate ()
{
    /* On est dans le cas pss_kind == UNBOUNDED_ */
    /* On prepare la sortie de l'automate en calculant une pseudo expression
       reguliere pour le bfsa associe au noeud courant. Les elements de back
       mis en jeu sont dans la SS_stack ws */
    register int	top, back_elem, in_nb, ibfsa;
    int			bot, x, X, bfsa;
    SXBOOLEAN		is_knot, is_first;
    char		s [12];

    bot = SS_bot (ws);
    top = SS_top (ws);
    is_knot = SXFALSE;
    in_nb = 0;

    while (--top >= bot) {
	x = ipss_to_attr [X = XxY_X (back_hd, back_elem = SS_get (ws, top))].global.repr;

	if (x != 0 && x == ipss_to_attr [XxY_Y (back_hd, back_elem)].global.repr) {
	    is_knot = SXTRUE;
	    SS_get (ws, top) = -back_elem;
	}
	else
	    in_nb++;
    }

    varstr_raz (vstr);

    /* On saute les boucles */
    if (in_nb > 0) {
	top = SS_top (ws);
	is_first = SXTRUE;

	if (in_nb > 1)
	    varstr_catenate (vstr, "(");
	
	while (--top >= bot) {
	    if ((back_elem = SS_get (ws, top)) > 0) {
		if (!is_first)
		    varstr_catenate (vstr, "|");
		else
		    is_first = SXFALSE;
		
		bfsa = ipss_to_attr [X = XxY_X (back_hd, back_elem)].local.bfsa;
		sprintf (s, "<%d>", bfsa);
		varstr_catenate (vstr, s);
		
		if ((ibfsa = back_to_attr [back_elem].ibfsa) != 0) {
		    varstr_catenate (vstr, ".");
		    make_ibfsa_to_ate (vstr, ibfsa, X, XxY_Y (back_hd, back_elem));
		}
	    }
	}
	
	if (in_nb > 1)
	    varstr_catenate (vstr, ")");
    }
    else {
	sprintf (s, "<%d>", ipss_to_attr [1].local.bfsa);
	varstr_catenate (vstr, s);
    }

    /* On traite les boucles */
    if (is_knot) {
	varstr_catenate (vstr, ".{");
	top = SS_top (ws);
	is_first = SXTRUE;

	while (--top >= bot) {
	    if ((back_elem = SS_get (ws, top)) < 0) {
		/* X et Y appartiennent au meme knot de representant x */
		/* On fabrique "{p1 ... pl | q1 ... qn | ... }"
		   ou les "p1 ... pl" sont les chaines des ibfsa. */
		/* Sur les knot on a  : bfsa == 0 et ibfsa != 0 */
		if (!is_first)
		    varstr_catenate (vstr, "|");
		else
		    is_first = SXFALSE;
		
		/* On remet en place pour la suite... */
		SS_get (ws, top) = back_elem = -back_elem;
		make_ibfsa_to_ate (vstr, back_to_attr [back_elem].ibfsa, XxY_X (back_hd, back_elem), XxY_Y (back_hd, back_elem));
	    }
	}
	    
	varstr_catenate (vstr, "}");
    }
	
    return sxstrsave (varstr_tostr (vstr));
}


static int make_bfsa_to_ate (bfsa_init, local_bfsa, xtnd_local_bfsa, bfsa, ret)
    int bfsa_init, local_bfsa, xtnd_local_bfsa, bfsa, *ret;
{
    /* On est dans le cas pss_kind == FSA_ */
    /* On prepare la sortie de l'automate en calculant une pseudo expression
       reguliere pour le bfsa associe au noeud courant.
       e <i> s  avec :
          - e et s les entree et sortie du bfsa et <i> l'identifiant de local_bfsa
       ou <i> est de la forme :
       {<j>|p q| ...}
	  - <j> le sous-bfsa de bfsa_init
	  - p q les couples de local_bfsa. */
    register int	*bot, *top;
    register int	init, couple;
    char		s [16];

    varstr_raz (vstr);
    varstr_catenate (vstr, "{");

    if (bfsa_init != 0) {
	bot = BFSA_BOT (*BFSA_BOT (bfsa_init));

	if ((init = *(bot + 1)) != 0) {
	    sprintf (s, "<%d>|", init);
	    varstr_catenate (vstr, s);
	}
    }

    bot = BFSA_BOT (local_bfsa);
    top = BFSA_TOP (local_bfsa);

    while (--top >= bot) {
	couple = *top;
	sprintf (s, "%d-%d", XxY_Y (Q0xQ0_hd, couple), XxY_X (Q0xQ0_hd, couple));
	varstr_catenate (vstr, s);
	    
	if (top > bot)
	    varstr_catenate (vstr, "|");
    }

    varstr_catenate (vstr, "}");
    *ret = sxstrsave (varstr_tostr (vstr));
    bot = BFSA_BOT (*BFSA_BOT (bfsa));
    sprintf (s, "%d <%d> %d", *bot, xtnd_local_bfsa, *(bot + 2)); /* e <i> s */
    return sxstrsave (s);
}



/* Dans le cas UNBOUNDED_, l'automate arriere est represente par une XH_list
   de bfsa ou chaque bfsa est une XH_list de la forme (initial, {(M, N)}, final)
   et chaque element M ou N de l'ensemble de couples {(M, N)} est
      - soit un etat de l'automate LR (0)
      - soit un couple (q, i) ou q est un etat de l'automate LR(0) et i un entier
        >0 --- le couple (q, 0) est represente par q et l'identifiant de (q, i) est
	stocke en negatif ---. L'entier i permet de differencier les occurrences de
	q dans les chemins de la relation back afin de pouvoir coder ces chemins
	par des ensembles de couples.

   Principe :
      A chaque noeud K du graphe de la relation back est associer un etat s du LR(0)
      (c'est l'etat initial du pss associe). A chaque arete entrante ds K est
      associe un ibfsa qui, si non nul, a pour sommet s. A chaque arete sortante de K
      est associe un ibfsa qui, si non nul, a pour fond s. A chaque noeud est associe
      un numero, tous les noeuds lies par des ibfsa nuls doivent avoir le meme numero.
      Si aucun etat n'a d'occurrence multiple, il n'est pas necessaire de leur associer
      des numeros. */

/* Le probleme de la forme normale des bfsa semble difficile a regler avec la nouvelle
   implantation, les numeros d'occurrences associes a chaque etat du LR(0) etant locaux
   pour une racine de back donnee. C'est l'ARP qui decidera des intersections. */
   

static SXVOID fork_collecting (bfsa_init, pssxt)
    int	bfsa_init, pssxt;
{
    /* pss_to_seq_hd contient en sequence toutes les images des pss de la relation back
       courante. */
    /* On utilise les knot.
       Idee etant qu'on a alors un DAG. On effectue un tri topologique sur les noeud
       pour propager les chemins. */
    register int	x, x1, y, i;
    int			N, y1, bfsa, X1, ibfsa, bot, top, init, local_bfsa, ate, ate_init, bfsa_init_ate, final, orig, local_bfsa_init, xtnd_local_bfsa;
    SXBOOLEAN		couples_set_is_empty, couples_set_has_been_cleared;
    char		s [12];
    /* Dans le cas pss_kind == FSA_, les bfsa sont des triplets (i, k, f) ou :
       i est l'etat initial
       f est l'etat final
       k est l'identifiant ds bfsa d'une liste de couples. */

    orig = X_is_set (&pss_to_seq_hd, -pssxt);
    init = pss_kind == FSA_ ? germe : BotState (XxY_X (PSSxT_hd, pssxt));

    /* On commence par remplir, pour chaque pss du DAG issu de orig
       le nombre d'aretes entrantes.
       Pour les knots, ce nombre est cumule sur leur representant. */
    sxba_empty (pss_work_set);
    SXBA_1_bit (pss_work_set, orig);
    SS_push (ipss_stack, orig); /* racine */
    /* ipss_to_attr [*].global.in == 0 */

    do {
	x = ipss_to_attr [x1 = SS_pop (ipss_stack)].global.repr;
	ipss_to_attr [x1].local = null_local; /* raz de la partie "locale" */

	XxY_Xforeach (back_hd, x1, i) {
	    back_to_attr [i].orig = orig; /* marquage du DAG courant */
	    y = ipss_to_attr [y1 = XxY_Y (back_hd, i)].global.repr;

	    if (SXBA_bit_is_reset_set (pss_work_set, y1))
		SS_push (ipss_stack, y1);

	    if (x == 0 || y != x) {
		/* y1 n'appartient pas au meme knot que x1 */
		if (y == 0)
		    y = y1;

		(ipss_to_attr [y].global.in)++;
	    }
	}
    } while (!SS_is_empty (ipss_stack));

    SS_push (ipss_stack, orig); /* racine */
    SS_clear (ws);
    ipss_to_attr [orig].local.bfsa = bfsa_init;
    local_bfsa_init =
	(pss_kind == FSA_ && bfsa_init != 0) ? *(BFSA_BOT (*BFSA_BOT (bfsa_init)) + 1) : 0;
    /* ipss_to_attr [orig].local.local_bfsa == 0; */

    /* qqsoit pss_kind, local_bfsa designe ds bfsa_hd une liste de couples (sans init ni
       final). */

    if (pss_kind == UNBOUNDED_ && (x = ipss_to_attr [orig].global.occur_nb) != 0) {
	XxY_set (&Q0xN_hd, init, x, &init);
	init += xac2;
    }

    /* tri topologique. */
    while (!SS_is_empty (ipss_stack)) {
	x = ipss_to_attr [x1 = SS_pop (ipss_stack)].global.repr;

	if (x1 != orig) {
	    couples_set_is_empty = SXTRUE;
	    SS_clear (ws);
	    y1 = x1;
	    
	    do {
		XxY_Yforeach (back_hd, y1, i) {
		    /* On cumule les couples des ibfsa de toutes les aretes entrantes. */
		    if (back_to_attr [i].orig == orig &&
			((ibfsa = back_to_attr [i].ibfsa) != 0 ||
			ipss_to_attr [XxY_X (back_hd, i)].local.local_bfsa != 0)) {
			if (ibfsa != 0 || !SS_is_empty (ws))
			    couples_set_is_empty = SXFALSE;

			SS_push (ws, i);
		    }
		}
	    } while ((y1 = ipss_to_attr [y1].global.lnk) != 0) /* cas des knot */;
	    
	    if (couples_set_is_empty) {
		if (SS_is_empty (ws)) {
		    local_bfsa = 0;
		    bfsa = bfsa_init;
		}
		else {
		    X1 = XxY_X (back_hd, SS_pop (ws));
		    local_bfsa = ipss_to_attr [X1].local.local_bfsa;
		    bfsa = ipss_to_attr [X1].local.bfsa;
		}
	    }
	    else {
		bot = SS_bot (ws);
		top = SS_top (ws);

		while (--top >= bot) {
		    X1 = XxY_X (back_hd, i = SS_get (ws, top));

		    if ((ibfsa = back_to_attr [i].ibfsa) != 0)
			back_to_couples_set (ibfsa, X1, XxY_Y (back_hd, i));

		    if ((local_bfsa = ipss_to_attr [X1].local.local_bfsa) != 0)
			XH_to_couples_set (BFSA_BOT (local_bfsa), BFSA_TOP (local_bfsa));
		}
	    }

	    couples_set_has_been_cleared = SXFALSE;
	    y1 = x1;
	    
	    do {
		X1 = X_X (pss_to_seq_hd, y1);
		
		if (!couples_set_is_empty) {
		    if (!couples_set_has_been_cleared) {
			couples_set_to_XH (&bfsa_hd);
			XH_set (&bfsa_hd, &local_bfsa);

			if (local_bfsa_init != 0) {
			    XH_to_couples_set (BFSA_BOT (local_bfsa), BFSA_TOP (local_bfsa));
			    XH_to_couples_set (BFSA_BOT (local_bfsa_init), BFSA_TOP (local_bfsa_init));
			    couples_set_to_XH (&bfsa_hd);
			    XH_set (&bfsa_hd, &xtnd_local_bfsa);
			}
			else
			    xtnd_local_bfsa = local_bfsa;

			couples_set_has_been_cleared = SXTRUE;
		    }
		    
		    XH_push (bfsa_hd, init);
		    XH_push (bfsa_hd, xtnd_local_bfsa);
		    final = BotState (X1);

		    if (pss_kind == UNBOUNDED_ && (y = ipss_to_attr [y1].global.occur_nb) != 0) {
			XxY_set (&Q0xN_hd, final, y, &final);
			final += xac2;
		    }
		    
		    XH_push (bfsa_hd, final);
		    XH_set (&bfsa_hd, &bfsa);

		    if (pss_kind == UNBOUNDED_) {
			if (bfsa_init != 0) {
			    bot = XH_X (bfsa_hd, bfsa_init);
			    top = XH_X (bfsa_hd, bfsa_init + 1) - 1; /* pas 0 */
			    
			    while (bot < top)
				XH_push (bfsa_hd, XH_list_elem (bfsa_hd, bot++));
			}
		    }
		    
		    XH_push (bfsa_hd, bfsa);
		    XH_push (bfsa_hd, pss_kind == FSA_ ? 0 : -1);
		    XH_set (&bfsa_hd, &bfsa);
		}
		
		ipss_to_attr [y1].local.local_bfsa = local_bfsa;
		ipss_to_attr [y1].local.bfsa = bfsa;

		if (bfsa_to_ate != NULL && bfsa_to_ate [bfsa] == 0) {
		    bfsa_to_ate [bfsa] = pss_kind == UNBOUNDED_ ? make_ubfsa_to_ate () :
			    (TopState (X1) > 0 ? make_bfsa_to_ate (bfsa_init, local_bfsa, xtnd_local_bfsa, bfsa, &(bfsa_to_ate [xtnd_local_bfsa])) : 0);
		}
		
		if (TopState (X1) > 0)
		    /* Transition vers un etat non nul */
		    /* On calcule le nouveau fork */
		    LRpi_Next (bfsa, X1);
	    } while ((y1 = ipss_to_attr [y1].global.lnk) != 0) ;
	}
	
	do {
	    XxY_Xforeach (back_hd, x1, i) {
		y = ipss_to_attr [y1 = XxY_Y (back_hd, i)].global.repr;
		
		if (x == 0 || y != x) {
		    /* y1 n'appartient pas au meme knot que x1 */
		    if (y == 0)
			y = y1;
		    
		    if (--(ipss_to_attr [y].global.in) == 0)
			SS_push (ipss_stack, y);
		}
	    }
	} while ((x1 = ipss_to_attr [x1].global.lnk) != 0) /* cas des knot */;
    }
}



static int	set_to_fks (set, low, high)
    int		low, high;
    register SXBA	set;
{
    /* Fabrique un "ts" a partir de set */
    register int	fork;
    int			result;

    if (high == 0)
	sxtrap (ME, "set_to_fks");

    for (fork = low; fork != -1; fork = sxba_scan (set, fork)) {
	if (Q0 [TopState (XxY_Y (forks, fork))].t_trans_nb != 0)
	    XH_push (fks_hd, fork);

	if (fork == high)
	    break;
    }

    XH_set (&fks_hd, &result);
    return result;
}



static SXVOID	reached_states (reached_set, couples_set, normal)
    SXBA	reached_set, couples_set;
    SXBOOLEAN	normal;
{
    /* couples_set contient un ensemble d'element de Q0xQ0.
       reached_set contient init, un etat de Q0.
       Cette procedure remplit reached_set avec tous les etats qu'il
       est possible d'atteindre depuis init en utilisant les elements
       de couples_set, soit dans le sens normal (transitions de
       l'automate LR(0), soit dans le sens retrograde. */
    register int	couple, X, Y;
    register SXBOOLEAN	is_stable = SXFALSE;

    while (!is_stable) {
	is_stable = SXTRUE;
	couple = 0;

	while ((couple = sxba_scan (couples_set, couple)) > 0) {
	    X = normal ? XxY_X (Q0xQ0_hd, couple) : XxY_Y (Q0xQ0_hd, couple);

	    if (SXBA_bit_is_set (reached_set, X)) {
		Y = normal ? XxY_Y (Q0xQ0_hd, couple) : XxY_X (Q0xQ0_hd, couple);

		if (SXBA_bit_is_reset_set (reached_set, Y)) {
		    is_stable = SXFALSE;
		}

		SXBA_0_bit (couples_set, couple);

		if (!normal)
		    /* Cas des bfsa */
		    SS_push (ws, couple);
	    }
	}
    }

    sxba_empty (couples_set);
}



SXVOID make_inter (bot1, top1, bot2, top2, normal, states_set)
    register int	*bot1, *bot2, *top1, *top2;
    SXBOOLEAN		normal;
    SXBA		states_set;
{
    /* bot(1|2) et top(1|2) designent des ensembles de couples de Q0xQ0.
       states_set contient init un etat de Q0.
       Calcule dans states_set l'ensemble des etats qu'il est possible d'atteindre
       depuis init par des chemins communs. */
    register SXBOOLEAN	pushed;
    register int	c1, c2;

    pushed = SXFALSE;
    
    /* On calcule les couples communs. */
    while (bot1 < top1 && bot2 < top2) {
	/* bot1 et bot2 sont corrects */
	if ((c1 = *bot1) == (c2 = *bot2)) {
	    pushed = SXTRUE;
	    SXBA_1_bit (couples_set, c1);
	    ++bot1, ++bot2;
	}
	else if (c1 < c2)
	    ++bot1;
	else
	    ++bot2;
    }
    
    if (pushed)
	reached_states (states_set, couples_set, normal);
}




static SXVOID back_to_forest ()
{
    register int		x, y, i;
    register struct back	*aback;
    register SXBA		set, loop_set;
    SXBOOLEAN			is_a_forest;
    int				x1, x2, x1bx2, X1, N, l;
    int				R;

    /* les ipss des racines vont de 1 a R = X_top (pss_to_seq_hd). */
    if ((R = X_top (pss_to_seq_hd)) > 0) {
	is_a_forest = SXTRUE;
	XxY_clear (&back_hd);
	sxba_empty (pss_work_set);
	SS_clear (ws);

	for (i = 1; i <= R;i++) {
	    x = PSSxT_to_xpts [-X_X (pss_to_seq_hd, x1 = i)].back_head;
	    ipss_to_attr [x1].global = null_global;

	    for (;;) {
		while (x != 0) {
		    if (!X_set (&pss_to_seq_hd, (aback = back + x)->X2, &x2)) {
			ipss_to_attr [x2].global = null_global;
			SS_push (ws, x2);
		    }
		    else {
			SXBA_1_bit (pss_work_set, x2);
			is_a_forest = SXFALSE;
		    }
		    
		    XxY_set (&back_hd, x1, x2, &x1bx2);
		    back_to_attr [x1bx2].ibfsa = aback->ibfsa;
		    back_to_attr [x1bx2].orig = 0; /* initialisation */
		    x = aback->lnk;
		}

		if (SS_is_empty (ws))
		    break;

		x = back_head [X_X (pss_to_seq_hd, x1 = SS_pop (ws))];
	    }
	}
	
	N = X_top (pss_to_seq_hd);

	if (!is_a_forest) {
	    /* pss_work_set contient les ipss qui ont des aretes entrantes multiples. */
	    CLEAR (pss_check_stack);
	    
	    if (call_check_loop_on_back ()) {
		/* On calcule le[s] knot[s]. */
		/* Il y a X_top (&pss_to_seq_hd) pss dans la relation back */
		
		if (X_plus == NULL) {
		    X_plus_size = N + 1;
		    X_plus = sxbm_calloc (X_plus_size + 1, X_plus_size);
		}
		else {
		    if (N + 1 > (l = X_plus_size)) {
			X_plus_size = N + 1;
			X_plus = sxbm_resize (X_plus, l, X_plus_size + 1, X_plus_size);
		    }
		    
		    while (--l >= 0)
			sxba_empty (X_plus [l]);
		}
		
		for (x1 = XxY_top (back_hd); x1 > 0; x1--) {
		    x2 = XxY_Y (back_hd, x1);
		    SXBA_1_bit (X_plus [XxY_X (back_hd, x1)], x2);
		}
		
		fermer (X_plus, X_plus_size);
		set = X_plus [0];
		loop_set = X_plus [X_plus_size];
		
		for (x = 1; x <= N; x++) {
		    if (SXBA_bit_is_set (X_plus [x], x))
			SXBA_1_bit (set, x);
		}
		
		x = 0;
		
		while ((x = sxba_scan_reset (set, x)) > 0) {
		    sxba_empty (loop_set);
		    SXBA_1_bit (loop_set, x);
		    y = x;
		    
		    while ((y = sxba_scan (set, y)) > 0) {
			if (SXBA_bit_is_set (X_plus [x], y) && SXBA_bit_is_set (X_plus [y], x)) {
			    SXBA_0_bit (set, y);
			    SXBA_1_bit (loop_set, y);
			}
		    }
		    
		    /* loop_set contient un knot */
		    process_knot (loop_set);
		}
	    }
	}

	if (pss_kind == UNBOUNDED_)
	    compute_occur_nb (R, N);
    }
}


SXBOOLEAN	XARC_state_scan (XARC, qtq, F)
    struct ARC_struct	*XARC;
    int			qtq;
    SXBOOLEAN		(*F)();
{
    /* Cette procedure parcourt les forks de XARC_state et pour chaque couple
       fork1, fork2 dont les pss sont compatibles avec pss_sets, appelle
       F (fork1, fork2, type1, type2).
       Si F retourne SXTRUE, la visite est interrompue. */
    register int	y2, y1, x2, x1;
    register SXBA	pss_set1, pss_set2;
    int			lim, top1, top2, fork1, fork2, pss1, pss2, type1, type2;
    int			xk1, xk2, XARC_state;

    XARC_state = XxYxZ_Z (XARC->QxTxQ_hd, qtq);

    for (xk1 = 0, lim = XH_X (XARC->Q_hd, XARC_state + 1) - 2, x1 = XH_X (XARC->Q_hd, XARC_state); x1
	 < lim; x1++, xk1++) {
	y1 = XH_list_elem (XARC->Q_hd, x1++);
	type1 = XH_list_elem (XARC->Q_hd, x1);
	pss_set1 = pss_sets [xk1];

	for (top1 = XH_X (fks_hd, y1 + 1), y1 = XH_X (fks_hd, y1); y1 < top1; y1++) {
	    pss1 = XxY_Y (forks, fork1 = XH_list_elem (fks_hd, y1));

	    if (SXBA_bit_is_set (pss_set1, pss1)) {
		for (xk2 = xk1 + 1, x2 = x1 + 1; x2 <= lim; x2++, xk2++) {
		    y2 = XH_list_elem (XARC->Q_hd, x2++);
		    type2 = XH_list_elem (XARC->Q_hd, x2);
		    pss_set2 = pss_sets [xk2];

		    for (top2 = XH_X (fks_hd, y2 + 1), y2 = XH_X (fks_hd, y2);
			 y2 < top2; y2++) {
			pss2 = XxY_Y (forks, fork2 = XH_list_elem (fks_hd, y2));

			if (SXBA_bit_is_set (pss_set2, pss2)) {
			    if (F (XARC, qtq, fork1, fork2, type1, type2))
				return SXTRUE;
			}
		    }
		}
	    }
	}
    }

    return SXFALSE;
}


static SXBOOLEAN	FSA_case (XARC, qtq, fork1, fork2, type1, type2)
    struct ARC_struct	*XARC;
    int			qtq, fork1, fork2, type1, type2;
{
    register int pss1, pss2;

    return ((pss1 = XxY_Y (forks, fork1)) == (pss2 = XxY_Y (forks, fork2)) ||
	make_inter_pss (pss1, pss2)) &&
	    PARTIAL_FSA_ARP_construction (XARC, XxY_X (forks, fork1), XxY_X (forks, fork2));
}

static SXBOOLEAN	fork_equality (XARC, qtq, fork1, fork2, type1, type2)
    struct ARC_struct	*XARC;
    int			qtq, fork1, fork2, type1, type2;
{
  /* On recherche tout d'abord les cas d'ambiguites simples en recherchant les
     egalites sur les forks. */

    if (fork1 == fork2) {
	XARC->is_clonable = SXFALSE;

	if (pss_kind == UNBOUNDED_ && conflict_message && should_print_conflict (AMBIGUOUS_)) {
	    sambig_conflict_messages (XARC, qtq, fork1, type1, type2);
	}

	return SXTRUE;
    }

    return SXFALSE;
}


int	is_LRpi (XARC, qtq)
    struct ARC_struct	*XARC;
    int			qtq;
{
    /* On sait que l'etat de l'ARC correspondant a XARC_state est impur
       et que les pss incrimines sont dans pss_sets.
       On regarde s'il est LR(pi) */
    /* TENTATIVE DE JUSTIFICATION :
       dans le cas UNBOUNDED_ l'egalite de 2 forks => ambiguite.
       Soient [A -> beta.] et [B -> beta' . delta] 2 items du germe
       impliques dans le conflit (delta = epsilon ou delta = t delta').
       Soient "alpha beta" et "alpha' beta'" deux chemins menant de
       l'etat initial au germe. Il existe x, chaine de T*, qui est un suffixe
       de protophrase droite. On a donc :
          S =>* alpha A x => alpha beta x
       et la reconnaissance de x par l'XARC complet conduirait au fork
       (exprime en terme de chemins)
          (alpha beta, S)
       Or les deux forks sont identiques
	  S =>* alpha' B x' => alpha' beta' delta x' => alpha' beta' y x'
       avec alpha' beta' = alpha beta et y x' = x
       Ce qui montre l'existence de deux derivations differentes pour la
       meme proto_phrase.
       La justification precedente est peut-etre suspecte du fait de
       la presence eventuelle de knot/fknot ds les bfsa/pss.
       Voir le cas "pss_kind == FSA_". */

    /* Dans le cas "pss_kind == FSA_", l'egalite de 2 forks
       n'implique pas l'ambiguite exemple : les piles 1-2-2
       et 1-2-2-2 se representent toutes les deux par 1-2-{2}. */
    /* Le probleme avec un fork FSA_ (bfsa, pss) est qu'on ne sait pas
       faire la correspondance entre un chemin de bfsa et LE chemin
       correspondant de pss. On peut repondre (ambiguite) si bfsa et
       pss n'ont pas de boucle. Si non, il est possible ne ne pas pouvoir
       extraire de 2 forks FSA_ egaux, 2 forks UNBOUNDED_ egaux. */
    /* Donc si egalite de 2 forks et bfsa et pss sans boucle => ambiguite.
       FAUX: il faut de plus qu'aucun des forks des etats intermediaires
       aboutissant a celui la n'est eu de boucle (l'elimination d'une boucle
       lors d'une reduction peut etre erronee)
       PB : sortie des messages. Si |bfsa| > 1 et |pss| > 1, on n'est
       pas sur des correspondances.
       Dans un premier temps, on lance le LR (NEW_K_VALUE). */
    register int	ck;

    XARC->is_ARP = SXFALSE;

    if (XARC_state_scan (XARC, qtq, fork_equality))
	return pss_kind == UNBOUNDED_ ? AMBIGUOUS_ : NOT_RoxoLR0_;
    else if (pss_kind == UNBOUNDED_)
	ck = UNBOUNDED_ARP_construction (XARC, qtq, conflict_message);
    else if (XARC->attr [XxYxZ_Z (XARC->QxTxQ_hd, qtq)].conflict_kind == NOT_RoxoLALRk_)
	ck = TOTAL_FSA_ARP_construction (XARC, qtq);
    else if (XARC_state_scan (XARC, qtq, FSA_case))
	ck = NOT_RoxoLR0_;
    else
	ck = NO_CONFLICT_;

    return ck;
}





SXVOID	XARC_print (XARC)
    struct ARC_struct	*XARC;
{
    /* Conflit sur germe, on imprime l'XARC correspondant a arc_no */
    register int	x, bfsa, y, z;
    int			XARC_state, next_XARC_state, ate, fork;
    char		c_name [24], string [12];

    put_edit_nl (4);
    get_constructor_name (c_name, SXFALSE, XARC->pss_kind != UNBOUNDED_, XARC->h_value, XARC->k_value);
    put_edit_nnl (32, c_name);
    put_edit_ap ("   C O N S T R U C T I O N");
    put_edit_nl (1);
    put_edit_nnl (32, "*** Corresponding ");
    put_edit_apnnl (XARC->conflict_kind == NO_CONFLICT_ ? "pure" : "mixed");
    put_edit_ap (" extended subARC ***");
    put_edit_nl (2);
    put_edit (1, "Backward Finite State Automaton list");

    for (bfsa = 1; bfsa <= XH_top (bfsa_hd);bfsa++) {
	if ((ate = bfsa_to_ate [bfsa]) > 0) {
	    x = *(BFSA_TOP (bfsa) - 1);

	    if (XARC->pss_kind == FSA_ && x == 0) {
		y = *(BFSA_BOT (*BFSA_BOT (bfsa)) + 1);

		if ((z = bfsa_to_ate [y]) > 0) {
		    print_bfsa (y, z);
		    bfsa_to_ate [y] = 0;
		}
	    }

	    if (XARC->pss_kind == UNBOUNDED_ && x == -1 ||
		XARC->pss_kind == FSA_ && x == 0) {
		print_bfsa (bfsa, ate);
		bfsa_to_ate [bfsa] = 0;
	    }
	}
    }
    
    
    for (XARC_state = 1; XARC_state < XH_top (XARC->Q_hd); XARC_state++) {
	int		from, lim, lim1, type, t;
	SXBOOLEAN		is_first;
	
	from = XARC->attr [XARC_state].from;
	y = XARC->attr [XARC_state].primes_nb;
	put_edit_nl (3);
	put_edit_nnl (1, "STATE ");
	put_edit_apnb (from);
	
	while (y-- > 0)
	    put_edit_apnnl ("'");
	
	if (XARC->attr [XARC_state].conflict_kind != NO_CONFLICT_) {
	    put_edit_ap (" (conflict) ");
	}
	else if (XARC->ARC->attr [from].conflict_kind != NO_CONFLICT_
		 && (
	     XARC->pss_kind != UNBOUNDED_ || XARC->pss_kind == UNBOUNDED_ && (XARC->attr [XARC_state].
	     lgth == -1 || XARC->attr [XARC_state].lgth >= k_value))
	     )
	    put_edit_ap (" (clonable) ");
	/* else if (xarc [XARC_state + 1] == xarc [XARC_state])
	    put_edit_apnnl (" (unprocessed)"); */

	for (lim1 = XH_X (XARC->Q_hd, XARC_state + 1), z = XH_X (XARC->Q_hd, XARC_state); z < lim1
	     ; z++) {
	    lim = XH_X (fks_hd, (y = XH_list_elem (XARC->Q_hd, z++)) + 1);

	    if ((type = -XH_list_elem (XARC->Q_hd, z) - grand) <= 0)
		put_edit_nnl (1, "Shift");
	    else {
		put_edit_nnl (1, "Red ");
		put_edit_apnb (type);
	    }

	    put_edit_apnnl (" : {");

	    for (y = XH_X (fks_hd, y); y < lim;) {
		fork = XH_list_elem (fks_hd, y);
		put_edit_apnb (fork);
		put_edit_apnnl ("=(");
		sprintf (string, "<%d>", XxY_X (forks, fork));
		put_edit_apnnl (string);
		put_edit_apnnl (", ");
		sprintf (string, "%d", XxY_Y (forks, fork));
		put_edit_apnnl (string);
		put_edit_apnnl (")");

		if (++y < lim)
		    put_edit_apnnl (", ");
	    }

	    put_edit_ap ("}");
	}

	XxYxZ_Xforeach (XARC->QxTxQ_hd, XARC_state, z) {
	    put_edit_nnl (11, xt_to_str (-XxYxZ_Y (XARC->QxTxQ_hd, z)));
	    put_edit_apnnl (" ---> ");
	    next_XARC_state = XxYxZ_Z (XARC->QxTxQ_hd, z);
	    x = XARC->attr [next_XARC_state].from;
	    y = XARC->attr [next_XARC_state].primes_nb;
	    put_edit_apnb (x);
	    
	    while (y-- > 0)
		put_edit_apnnl ("'");
	    
	    put_edit_nl (1);
	}
    }
}


SXVOID	XARC_free ()
{
    if (fork_work_set != NULL) {
	XH_free (&fks_hd);
	XH_free (&bfsa_hd);
	sxfree (fork_work_set), fork_work_set = NULL;
	sxfree (final_fork_set);
	XxY_free (&forks);
	X_free (&pss_to_seq_hd);
	sxfree (ipss_to_attr);
	SS_free (ipss_stack);
	XxY_free (&back_hd);
	sxfree (back_to_attr);
	sxfree (Q0_to_occur_nb);
	XxY_free (&Q0xN_hd);
	XxY_free (&XQ0xXQ0_hd);
	XxY_free (&FORKxT_hd);
	sxfree (FORKxT_to_fks);

	if (bfsa_to_ate != NULL) {
	    sxfree (bfsa_to_ate), bfsa_to_ate = NULL;
	}

	if (lpss != NULL) {
	    SS_free (lpss), lpss = NULL;

	    if (X_plus != NULL) {
		sxbm_free (X_plus), X_plus = NULL;
	    }
	}

	ARP_free ();
    }
}



int	n_to_forks (XARC, XARC_state, n, type)
    struct ARC_struct	*XARC;
    int		XARC_state, n, type;
{
    /* n est l'indice dans bfsa_hd d'un bfsa. Retourne dans ws les forks
       associe au type "type" de l'etat conflictuel de l'XARC correspondant
       a XARC_state, pss_sets tel que fork = (bfsa, pss) et n designe bfsa. */
    register int	x, y, bfsa, xk;
    register SXBA	pss_set;
    int		pss, fork, top, lim;

    for (xk = 0, lim = XH_X (XARC->Q_hd, XARC_state + 1), x = XH_X (XARC->Q_hd, XARC_state); x <
	 lim; x++, xk++) {
	if (XH_list_elem (XARC->Q_hd, ++x) == type) {
	    y = XH_list_elem (XARC->Q_hd, x - 1);
	    pss_set = pss_sets [xk];

	    for (top = XH_X (fks_hd, y + 1), y = XH_X (fks_hd, y); y < top; y++)
		 {
		pss = XxY_Y (forks, fork = XH_list_elem (fks_hd, y));

		if (SXBA_bit_is_set (pss_set, pss)) {
		    if (n >= XH_X (bfsa_hd, bfsa = XxY_X (forks, fork)) && n <
			 XH_X (bfsa_hd, bfsa + 1))
			SS_push (ws, fork);
		}
	    }

	    return;
	}
    }
}



static SXVOID XARC_init_state_building (XARC)
    struct ARC_struct	*XARC;
{
    
    /* Calcul de l'etat initial dans les cas d'un XARC (FSA_ ou UNBOUNDED_). */
    register int			cur_red;
    register struct init_state_attr	*pinit;
    int					pss_init, fork, bfsa_init, init_fork_list, xred_no,
                                        repr_red_no, xkas, indice, XARC_state, ipss;
    
    XARC->conflict_t_set = NULL;

    /* Seuls les forks dont un pss existe sont stockes. */
    fill_kas (XARC->ARC, 1, kas, pss_sets, &xkas, NO_CONFLICT_);
    
    XH_push (bfsa_hd, germe);
    XH_push (bfsa_hd, 0);
    XH_push (bfsa_hd, germe);
    XH_set (&bfsa_hd, &bfsa_init);
    XH_push (bfsa_hd, bfsa_init);
    XH_push (bfsa_hd, pss_kind == FSA_ ? 0 : -1);
    XH_set (&bfsa_hd, &bfsa_init);
    
    if (bfsa_to_ate != NULL) {
	char	s [8];
	
	sprintf (s, "%d", germe);
	bfsa_to_ate [bfsa_init] = sxstrsave (s);
    }
    
    XH_push (pss_hd, germe);
    
    if (pss_kind == FSA_)
	XH_push (pss_hd, germe);
    
    XH_push (pss_hd, h_value);
    XH_set (&pss_hd, &pss_init);
    
    X_clear (&pss_to_seq_hd);
    
    for (cur_red = 0; cur_red < RED_NO; cur_red++) {
	XxY_set (&PSSxT_hd, pss_init, -cur_red, &indice); /* bidon */
	PSSxT_to_xpts [indice].back_head = init_state_attr [cur_red].back_head;
	X_set (&pss_to_seq_hd, -indice, &ipss);
    }
    
    back_to_forest ();
    final_fork_set_M = 0;
    
    for (cur_red = 0; cur_red < RED_NO; cur_red++) {
	pinit = init_state_attr + cur_red;
	CUR_PSS_SET = pss_sets [cur_red];
	sxba_empty (fork_work_set);
	fork_work_set_m = maxint;
	fork_work_set_M = 0;
	fork_collecting (bfsa_init, XxY_is_set (&PSSxT_hd, pss_init, -cur_red));
	pinit->fork_list = init_fork_list = set_to_fks (fork_work_set, fork_work_set_m,
							fork_work_set_M);
	xred_no = -grand - pinit->type;
	repr_red_no = PROD_TO_RED (xred_no);
	
	if (!has_several_equiv_reductions) {
	    XH_push (XARC->Q_hd, init_fork_list);
	    XH_push (XARC->Q_hd, -grand - repr_red_no);
	}
	else {
	    if (cur_red + 1 < RED_NO && (xred_no = -grand - init_state_attr [cur_red + 1].type,
					 PROD_TO_RED (xred_no) == repr_red_no)) {
		if (final_fork_set_M == 0) {
		    sxba_copy (final_fork_set, fork_work_set);
		    final_fork_set_m = fork_work_set_m;
		    final_fork_set_M = fork_work_set_M;
		}
		else {
		    sxba_or (final_fork_set, fork_work_set);
		    final_fork_set_m = min (final_fork_set_m, fork_work_set_m);
		    final_fork_set_M = max (final_fork_set_M, fork_work_set_M);
		}
	    }
	    else {
		if (final_fork_set_M != 0) {
		    init_fork_list = set_to_fks (final_fork_set, final_fork_set_m, final_fork_set_M);
		    final_fork_set_M = 0;
		}
		
		XH_push (XARC->Q_hd, init_fork_list);
		XH_push (XARC->Q_hd, -grand - repr_red_no);
	    }
	}
    }
    
    if (init_state_attr [RED_NO].type != 0 /* shift_reduce conflict */ ) {
	int		fks;
	
	XxY_set (&forks, bfsa_init, pss_init, &fork);
	XH_push (fks_hd, fork);
	XH_set (&fks_hd, &fks);
	init_state_attr [RED_NO].fork_list = fks;
	XH_push (XARC->Q_hd, fks);
	XH_push (XARC->Q_hd, init_state_attr [RED_NO].type);
    }
    
    XH_set (&XARC->Q_hd, &XARC_state);

    if (XARC_state != 1)
	sxtrap (ME, "XARC_construction");
    
    XARC->attr [0].lgth = XARC->attr [1].lgth = 0;
    XARC->attr [0].is_marked = XARC->attr [1].is_marked = SXFALSE;
    XARC->conflict_kind = XARC->attr [1].conflict_kind = NO_CONFLICT_;
    XARC->attr [1].is_tree_orig = SXFALSE;
    XARC->attr [1].from = 1;
    XARC->attr [0].primes_nb = 0;
    XARC->attr [1].primes_nb = 1;
    XARC->is_clonable = SXTRUE;
    XARC->is_initiated = SXTRUE;
} 



static SXBOOLEAN	XARC_trans (XARC, XARC_state, t, next_XARC_state)
    struct ARC_struct	*XARC;
    int			XARC_state, t, *next_XARC_state;
{
    /* On construit l'etat next_XARC_state successeur de XARC_state sur t. */
    int			next_ARC_state;
    register int	xfks, xpis, pss, t_trans_nb;
    int			xpfks, bfsa, next_state, type, indice, lim1, lim2, indice1,
	                fks, xk, xkas, new_pss, ipss, fork;

    next_ARC_state = XARC->whd [t].lnk;
    X_clear (&pss_to_seq_hd);

    for (lim1 = XH_X (XARC->Q_hd, XARC_state + 1), xpis = XH_X (XARC->Q_hd, XARC_state); xpis <
	 lim1; xpis++) {
	xpfks = XH_list_elem (XARC->Q_hd, xpis++);

	for (lim2 = XH_X (fks_hd, xpfks + 1), xfks = XH_X (fks_hd, xpfks); xfks
	     < lim2; xfks++) {
	    /* Il y a des transitions terminales */
	    fork = XH_list_elem (fks_hd, xfks);
	    pss = XxY_Y (forks, fork);

	    if (XxY_is_set (&Q0xV_hd, TopState (pss), -t)) {
		/* sur t */
		if ((XxY_is_set (&FORKxT_hd, fork, t) == 0) && t != -bnf_ag.WS_TBL_SIZE.tmax) {
		    if ((indice = XxY_is_set (&PSSxT_hd, pss, t)) == 0)
			sxtrap (ME, "XARC_trans");
		    
		    if (PSSxT_to_xpts [indice].back_head > 0)
			X_set (&pss_to_seq_hd, -indice, &ipss);
		}
	    }
	}
    }

    back_to_forest ();
    xk = 0;
    fill_kas (XARC->ARC, next_ARC_state, kas, pss_sets, &xkas, NO_CONFLICT_);

    for (xpis = XH_X (XARC->Q_hd, XARC_state); xpis < lim1; xpis++) {
	xpfks = XH_list_elem (XARC->Q_hd, xpis++);
	t_trans_nb = 0;

	for (lim2 = XH_X (fks_hd, xpfks + 1), xfks = XH_X (fks_hd, xpfks); xfks
	     < lim2; xfks++) {
	    /* Il y a des transitions terminales */
	    fork = XH_list_elem (fks_hd, xfks);
	    pss = XxY_Y (forks, fork);

	    if (indice = XxY_is_set (&Q0xV_hd, next_state = TopState (pss), -t))
		 {
		/* Il y a une transition terminale sur t */
		if (t_trans_nb == 0) {
		    CUR_PSS_SET = pss_sets [xk++];
		}

		if (++t_trans_nb > 1) {
		    if (t_trans_nb == 2) {
			sxba_empty (final_fork_set);
			final_fork_set_m = maxint;
			final_fork_set_M = 0;
		    }

		    fks_to_fork_set (fks, final_fork_set, &final_fork_set_m, &
			 final_fork_set_M);
		}

		if (XxY_set (&FORKxT_hd, fork, t, &indice1)) {
		    fks = FORKxT_to_fks [indice1];
		}
		else {
		    sxba_empty (fork_work_set);
		    fork_work_set_m = maxint;
		    fork_work_set_M = 0;

		    if (t != -bnf_ag.WS_TBL_SIZE.tmax) {
			next_state = Q0xV_to_Q0 [indice];
			bfsa = XxY_X (forks, fork);

			if (PSSxT_to_xpts [indice = XxY_is_set (&PSSxT_hd, pss, t)].back_head)
			    fork_collecting (bfsa, indice);
		    }

		    if (t == -bnf_ag.WS_TBL_SIZE.tmax || next_state > 0 && Q0 [
			 next_state].t_trans_nb > 0) {
			/* Il y a [au moins] une transition terminale depuis
			   next_state */
			if (t != -bnf_ag.WS_TBL_SIZE.tmax) {
			    new_pss = make_new_pss (pss, next_state);

			    if (SXBA_bit_is_set (CUR_PSS_SET, new_pss))
				XxY_set (&forks, bfsa, new_pss, &fork);
			    else
				fork = 0;
			}


/* else on garde le fork precedent. */
			
			if (fork != 0) {
			    SXBA_1_bit (fork_work_set, fork);
			    fork_work_set_m = min (fork_work_set_m, fork);
			    fork_work_set_M = max (fork_work_set_M, fork);
			}
		    }

		    fks = FORKxT_to_fks [indice1] = set_to_fks (fork_work_set,
			 fork_work_set_m, fork_work_set_M);
		}
	    }
	}

	if (t_trans_nb > 0) {
	    if (t_trans_nb > 1) {
		fks_to_fork_set (fks, final_fork_set, &final_fork_set_m, &
		     final_fork_set_M);
		fks = set_to_fks (final_fork_set, final_fork_set_m,
		     final_fork_set_M);
	    }

	    XH_push (XARC->Q_hd, fks);

	    if ((type = XH_list_elem (XARC->Q_hd, xpis)) > 0) {
		/* Conflit Shift depuis l'etat initial (type == germe) */
		type = -XxY_is_set (&Q0xV_hd, type, -t);
	    }

	    XH_push (XARC->Q_hd, type);
	}
    }

    if (!XH_set (&(XARC->Q_hd), next_XARC_state)) {
	XARC->attr [*next_XARC_state].from = next_ARC_state;
	XARC->attr [*next_XARC_state].primes_nb = ++(XARC->ARC->attr [next_ARC_state].primes_nb);
	return SXTRUE;
    }

    return SXFALSE;
}


static int XARC_get_conflict_kind (XARC, XARC_state, is_new_arc_state, t)
    struct ARC_struct	*XARC;
    int			XARC_state, t;
    SXBOOLEAN		is_new_arc_state;
{
    register int		ck;
    int				xkas;
    register struct ARC_ATTR	*attr = XARC->attr + XARC_state;

    ck = XARC->ARC->attr [XARC->attr [XARC_state].from].conflict_kind;

    if (t != bnf_ag.WS_TBL_SIZE.tmax && (ck == NO_CONFLICT_ ||
	pss_kind == UNBOUNDED_ &&
	attr->lgth > 0 &&
	attr->lgth < k_value)) {
	ck = NO_CONFLICT_;
    }

    return attr->conflict_kind = ck;
}

int call_XARC_building (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    /* On vient de detecter un etat impur dans l'ARC courant.  Cette fonction
       regarde si cet etat resterait impur en construisant un ARC etendu
       (XARC).  Un XARC est un ARC dans lequel chaque item est un couple
       (fork, type) ou fork est un couple (bfsa, pss) et pss est comme dans
       l'ARC courant et bfsa (BackwardFSA) est un FSA, sous automate de
       l'automate LR(0) et qui contient tous les chemins entre le germe et la
       charniere (etat initial -- fond de pile -- de pss).  On ne construit
       pas XARC en entier, mais uniquement le sous automate qui correspond
       a TOUS les chemins de l'ARC courant menant de l'etat initial a l'etat
       impur. XARC est pur ssi tous les etats de XARC correspondant a
       "impur" sont purs. Dans ce cas la fonction retourne SXTRUE. */
    
    /* Classes de grammaires detectables (is_lr_constructor est positionne)
       
       Soient fork1 = (bfsa1, pss1) et fork2 = (bfsa2, pss2) de type type1
       et type2 deux items de l'etat conflictuel
       
       Si pss_kind == UNBOUNDED_
	  S'il existe bfsa < bfsa1 et bfsa < bfsa2 et pss1 == pss2 alors
	       la grammaire est ambigue
	  Sinon, s'il existe un cycle dans l'XARC alors la grammaire n'est
	       pas LR
	  Sinon elle n'est pas LR(k).

       Si pss_kind != UNBOUNDED_
          l'egalite de fork1 et fork2 n'implique pas forcement l'ambiguite
	  (cas un peu tordus)
	  Elle n'est pas LR(pi). */

    /* Optimisation (et messages de non conformite non dupliques) :

       Pour un type donne, un bfsa est inutile s'il est inclus dans les autres.
       Or, pour un type donne, on n'a pas deux forks identiques et donc on
       n'a pas deux bfsa egaux.
       Pour chaque type on construit un ARP et on garde tous les bfsa dont
       les items apparaissent seuls dans les etats de l'ARP */
    register int	ARC_state, XARC_kind;
    struct ARC_struct	*XARC;
    
    XARC_kind = pss_kind == UNBOUNDED_ ? XARC_UNBOUNDED : XARC_FSA;
    XARC = &(ARCs [XARC_kind]);
    
    if (!XARC->is_allocated) {
	ARC_allocate (XARC, XARC_kind, pss_kind == UNBOUNDED_ ? XARC_UNBOUNDED_Q_oflw :
		      XARC_FSA_Q_oflw);
    }
    
    if (!XARC->is_initiated) {
	XARC->ARC = ARC;

	if (ARC->ARC == NULL) {
	    XARC_allocate ();
	    alloc_UARP ();
	}
	/* Sinon on est ds le cas (tordu) ou les a ARCs sont utilises et alloc_UARP
	   a deja ete appele entre ARC_FSA et XARC_FSA (on est ici entre ARC_UNBOUNDED
	   ---LALR--- et XARC_UNBOUNDED ---LR---). */

	XARC_init_state_building (XARC);
	XARC->pss_kind = pss_kind;
	XARC->h_value = h_value;
	XARC->k_value = k_value;
    }

    /* On remonte dans l'ARC en marquant les etats accessibles. */
    ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, qtq);
    ARC->attr [ARC_state].is_marked = SXTRUE;
    ARC_walk_backward (ARC, qtq, bh_mark, NULL);

    ARC_building (XARC, XARC_trans, XARC_get_conflict_kind);
    
    /* On enleve les marques. */
    ARC_walk_backward (ARC, qtq, bh_unmark, NULL);
    ARC->attr [ARC_state].is_marked = SXFALSE;
    ARC->is_clonable = XARC->is_clonable;

    return XARC->conflict_kind;
}


SXVOID	XARC_resize (old_xac2, new_non_lalr1_state_nb)
    int		old_xac2, new_non_lalr1_state_nb;
{
    /* On est forcement dans le cas is_lr_constructor et des etats ont ete
       clones, xac2 nombre d'etats du LR (0) a donc augmente, on realloue
       donc les structures qui en dependent. */

    register int	i;

    if (xac2 > old_xac2) {
	couples_set = sxba_resize (couples_set, Q0xQ0_top + XxY_size (XQ0xXQ0_hd) + 1);
	LR0_states_set = LR0_sets [5];
	Q0_to_occur_nb = (int*) sxrealloc (Q0_to_occur_nb, xac2, sizeof (int));
    }
}
