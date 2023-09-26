
/* Action par defaut appelee si aucune @function n'est specifiee sur une clause */
#define sem_void			NULL

#define _rnatag_semact_init		sxvoid
#define _rnatag_semact_final		sxvoid
#define _rnatag_semact_last_pass	sxbvoid

static int Watson_Crick [5][5]= {
    /*	0	A	C	G	U	*/	         
/* 0 */{0,	0,	0,	0,	0},
/* A */{0,	0,	0,	0,	2},
/* C */{0,	0,	0,	4,	0},
/* G */{0,	0,	5,	0,	1},
/* U */{0,	3,	0,	0,	0}
};

static float Bulge[19] = { 0,
3.3, 5.2, 6.0, 6.7, 7.4, 8.2, 9.1, 10.0, 10.5, 11.0, 0, 11.8, 0, 12.5, 0, 13.0, 0, 13.6
};

static float Hairpin[19] = { 0,
0, 0, 7.4, 5.9, 4.4, 4.3, 4.1, 4.1, 4,2, 4.3, 0, 4.9, 0, 5.6, 0, 6.1, 0, 6.7
};

static float Internal[19] = { 0,
0, 0.8, 1.3, 1.7, 2.1, 2.5, 2.6, 2.8, 3.1, 3.6, 0, 4.4, 0, 5.1, 0, 5.6, 0, 6.2
};

static float PairxPair [6][6]= {
    /*	0	GU	AU	UA	CG	GC	*/	         
/* 0  */{0.,	0.,	0.,	0.,	0.,	0.},
/* GU */{0.,	-0.5,	-0.5,	-0.7,	-1.5,	-1.3},
/* AU */{0.,	-0.5,	-0.9,	-1.1,	-1.8,	-2.3},
/* UA */{0.,	-0.7,	-0.9,	-0.9,	-1.7,	-2.1},
/* CG */{0.,	-1.9,	-2.1,	-2.3,	-2.9,	-3.4}
/* GC */{0.,	-1.5,	-1.7,	-1.8,	-2.0,	-2.9}
};

static SEM_TYPE	*SEM_PTR;

static int	area_size, area_top;
static char	*type_area_hd;
static float	*free_energy_hd;

static void
realloc_area ()
{
    type_area_hd = (char *) sxrealloc (type_area_hd, (area_size *= 2)+1, sizeof (char));
    free_energy_hd = (char *) sxalloc (free_energy_hd, area_size+1, sizeof (float));
}

static void
_rnatag_semact_init (init_size)
    int init_size;
{
    if (type_area_hd == NULL) {
	type_area_hd = (char *) sxalloc ((area_size = init_size)+1, sizeof (char));
	free_energy_hd = (char *) sxalloc (area_size+1, sizeof (float));
    }
}

static void
_rnatag_semact_final (init_size)
    int init_size;
{
    if (type_area_hd) {
	sxfree (type_area_hd);
	sxfree (free_energy_hd)
    }
}

#define UNKNOWN			0
#define SINGLE_STRAND		1
#define WATSON_CRICK_PAIR	2
#define LEFT_BULGE		3
#define RIGHT_BULGE		4
#define LEFT_RIGHT_BULGE	5
#define CROSS_WATSON_CRICK_PAIR	6
#define CROSS_LEFT_BULGE	7
#define CROSS_RIGHT_BULGE	8
#define CROSS_LEFT_RIGHT_BULGE	9


static BOOLEAN
_wc (rho0, ilb, iub)
    int *rho0, *ilb, *iub;
{
    return Watson_Crick [SOURCE(iub[0])][SOURCE(iub[1])];
}

/* rho est un tableau de taille son_nb+2
   rho[0] contient le numero p de la cause courante.  Si p est <0 la clause est -p et
   l'appel est cyclique
       rho[1] contient l'identifiant du vecteur de range de la LHS
       rho[2], ..., rho[son_nb+1] contient les identifiants des vecteurs de range de la RHS */
/* sem_disp est un tableau global de taille PID dont chaque element est un pointeur
   vers un tableau local a chaque module ds lequel est stocke la semantique (element
   de type SEM_TYPE, c'est une union) */

/* sons est un tableau de taille sons_nb+1, il matche clause.
   Si clause = A0 --> A1 ... Ap, sons[i] est l'identifiant du module ds lequel est defini Ai */

/* Si is_first, c'est la premiere fois qu'on fait une reduc vers (A, rho[0]), clause est une A_clause */

static BOOLEAN
empty_single_strand (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
#include <values.h>
    int	cur_top;

    /* rnatag()		--> . @empty_single_strand */
    /* return_type = SINGLE_STRAND */
    if (is_first) {
	if (SEM_PTR == NULL)
	    /* Un seul module ? */
	    SEM_PTR = sem_disp [sons [local_pid]];

	if (area_top == area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
	type_area_hd [cur_top] = SINGLE_STRAND;
	free_energy_hd [cur_top] = MAXFLOAT;
    }

    return TRUE;
}  

static char single_strand_type []= {UNKNOWN,SINGLE_STRAND,LEFT_BULGE,LEFT_BULGE,LEFT_RIGHT_BULGE,LEFT_RIGHT_BULGE};

static BOOLEAN
single_strand (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    int 	X_top;
    float	X_energy;
	
    /* rnatag(T X) --> &StrLen(1, T) rnatag(X) . @single_strand */
    X_top = SEM_PTR [rho [2]].ntger;
    X_energy = free_energy_hd [X_top];

    if (is_first) {
	if (area_top == area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
	lgth_area_hd [cur_top] = lgth_area_hd [X_top];
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy >= free_energy_hd [cur_top])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    type_area_hd [cur_top] = single_strand_type [type_area_hd [X_top]];
    lgth_area_hd [cur_top]++;
    free_energy_hd [cur_top] = X_energy;

    return TRUE;
}  

static BOOLEAN
pairing (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    int 	X_top, X_lgth, lsi, rsi, WC1, WC2;
    float	X_energy;
    Aij_struct	*Aij_struct_ptr;

    /* rnatag(T1 X T2)	--> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X) &wc(T1, T2) rnatag(X) . @pairing */

    X_top = SEM_PTR [rho [3]].ntger;
    X_type = type_area_hd [X_top];
    X_energy = free_energy_hd [X_top];

    switch (X_type) {
    case UNKNOWN:
	break;

    case SINGLE_STRAND: /* hairpin loop */
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Hairpin [X_lgth];
	break;

    case WATSON_CRICK_PAIR:
	Aij_struct_ptr = Aij2struct [local_pid] (rho [3]);
	lsi = Aij_struct_ptr->lb [0];
	rsi = Aij_struct_ptr->ub [0];
	WC2 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)];
	WC1 = Watson_Crick [LSOURCE (lsi-1)][SOURCE (rsi+1)];

	X_energy += PairxPair [WC1] [WC2];
	break;

    case LEFT_BULGE:
    case RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Bulge [X_lgth];
	break;


    case LEFT_RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Internal [X_lgth];
	break;
    }

    if (is_first) {
	if (area_top == area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy >= free_energy_hd [cur_top])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    type_area_hd [cur_top] = WATSON_CRICK_PAIR;
    lgth_area_hd [cur_top] = 0;
    free_energy_hd [cur_top] = X_energy;

    return TRUE;
}  

static BOOLEAN
catenate (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    int 	X_top, X_lgth, ls1, rsi, WC1, WC2;
    float	X_energy;
    Aij_struct	*Aij_struct_ptr;

    /* rnatag(T1 X1 T2 X2) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X) !&StrLen(0, Y)
                               &wc(T1, T2) sss(X1, X2) . @catenate */

    X_top = SEM_PTR [rho [3]].ntger;
    X_type = type_area_hd [X_top];
    X_energy = free_energy_hd [X_top];

    switch (X_type) {
    case UNKNOWN:
	break;

    case SINGLE_STRAND: /* hairpin loop */
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Hairpin [X_lgth];
	break;

    case WATSON_CRICK_PAIR:
	Aij_struct_ptr = Aij2struct [local_pid] (rho [3]);
	lsi = Aij_struct_ptr->lb [0];
	rsi = Aij_struct_ptr->ub [0];
	WC2 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)];
	WC1 = Watson_Crick [LSOURCE (lsi-1)][SOURCE (rsi+1)];

	X_energy += PairxPair [WC1] [WC2];
	break;

    case LEFT_BULGE:
    case RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	energy = X_energy + Bulge [X_lgth];
	break;


    case LEFT_RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Internal [X_lgth];
	break;
    }

    X_energy += free_energy_hd [X_top+1]; /* X2_energie */

    if (is_first) {
	if (area_top == area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy >= free_energy_hd [cur_top])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    if (type_area_hd [X_top+1] == SINGLE_STRAND) {
	type_area_hd [cur_top] = RIGHT_BULGE;
	lgth_area_hd [cur_top] = lgth_area_hd [X_top+1];
    }
    else {
	type_area_hd [cur_top] = UNKNOWN;
	lgth_area_hd [cur_top] = 0;
    }

    free_energy_hd [cur_top] = X_energy;

    return TRUE;
}  


static BOOLEAN
copyXY (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    /* sss(X, Y) --> ss(X, Y) . @copyXY
       sss(X, Y) --> rnatag(X) rnatag(Y) .  @copyXY */

    int 	X_top, Y_top;
    float	X_energy, Y_energie;

    X_top = SEM_PTR [rho [2]].ntger;
    X_energie = free_energy_hd [X_top];
    Y_top = Xtop+1;
    Y_energie = free_energy_hd [Y_top];

    if (is_first) {
	if (area_top+1 >= area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
	area_top++;
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy+Y_energy >= free_energy_hd [cur_top]+free_energy_hd [cur_top+1])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }

    type_area_hd [cur_top] = type_area_hd [X_top];
    free_energy_hd [cur_top] = X_energy;
    lgth_area_hd [cur_top] = lgth_area_hd [X_top];

    cur_top++;

    type_area_hd [cur_top] = type_area_hd [Y_top];
    free_energy_hd [cur_top] = Y_energy;
    lgth_area_hd [cur_top] = lgth_area_hd [Y_top];


    return TRUE;
}

static BOOLEAN
left_single_strand (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    /* ss(T X, Y) --> &StrLen(1, T) !&StrLen(0, X) ss(X,Y) . @left_single_strand
       ssok(T X, Y)--> &StrLen(1, T) !&StrLen(0, X) ssok(X,Y) . @left_single_strand */

    int 	X_top, Y_top;
    float	X_energy, Y_energy;
	
    X_top = SEM_PTR [rho [2]].ntger;
    X_energy = free_energy_hd [X_top];
    Y_top = X_top+1;
    Y_energy = free_energy_hd [Y_top];

    if (is_first) {
	if (area_top+1 >= area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
	area_top++;
	lgth_area_hd [cur_top] = lgth_area_hd [X_top];
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy+Y_energie >= free_energy_hd [cur_top]+free_energy_hd [cur_top+1])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    type_area_hd [cur_top] = single_strand_type [type_area_hd [X_top]];
    lgth_area_hd [cur_top]++;
    free_energy_hd [cur_top] = X_energy;

    cur_top++;

    type_area_hd [cur_top] = type_area_hd [Y_top];
    free_energy_hd [cur_top] = Y_energy;
    lgth_area_hd [cur_top] = lgth_area_hd [Y_top];

    return TRUE;
}

static BOOLEAN
right_single_strand (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    /* ss(X, Y T) --> &StrLen(1, T) !&StrLen(0, Y) ss(X,Y) . @right_single_strand
       ssok(X, Y T) --> &StrLen(1, T) !&StrLen(0, Y) ssok(X,Y) . @right_single_strand */

    int 	X_top, Y_top;
    float	X_energy, Y_energy;
	
    X_top = SEM_PTR [rho [2]].ntger;
    X_energy = free_energy_hd [X_top];
    Y_top = X_top+1;
    Y_energy = free_energy_hd [Y_top];

    if (is_first) {
	if (area_top+1 >= area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
	area_top++;
	lgth_area_hd [cur_top+1] = lgth_area_hd [Y_top];
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy+Y_energie >= free_energy_hd [cur_top]+free_energy_hd [cur_top+1])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    type_area_hd [cur_top] = type_area_hd [X_top];
    lgth_area_hd [cur_top] = lgth_area_hd [X_top];
    free_energy_hd [cur_top] = X_energy;

    cur_top++;

    type_area_hd [cur_top] = single_strand_type [type_area_hd [Y_top]];
    lgth_area_hd [cur_top]++;
    free_energy_hd [cur_top] = Y_energy;
    
    return TRUE;
}

static BOOLEAN
left_pairing (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    int 	X_top, Y_top, X_lgth, lsi, rsi, WC1, WC2;
    float	X_energy, Y_energy;
    Aij_struct	*Aij_struct_ptr;

    /* ss(T1 X T2, Y) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X) &wc(T1, T2) ss(X,Y) . @left_pairing
       ssok(T1 X T2, Y) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X) &wc(T1, T2) ssok(X,Y) . @left_pairing */

    X_top = SEM_PTR [rho [3]].ntger;
    X_type = type_area_hd [X_top];
    X_energy = free_energy_hd [X_top];
    Y_top = X_top+1;
    Y_energy = free_energy_hd [Y_top];

    switch (X_type) {
    case UNKNOWN:
	break;

    case SINGLE_STRAND: /* hairpin loop */
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Hairpin [X_lgth];
	break;

    case WATSON_CRICK_PAIR:
	Aij_struct_ptr = Aij2struct [local_pid] (rho [3]);
	lsi = Aij_struct_ptr->lb [0];
	rsi = Aij_struct_ptr->ub [0];
	WC2 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)];
	WC1 = Watson_Crick [LSOURCE (lsi-1)][SOURCE (rsi+1)];

	X_energy += PairxPair [WC1] [WC2];
	break;

    case LEFT_BULGE:
    case RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Bulge [X_lgth];
	break;


    case LEFT_RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Internal [X_lgth];
	break;
    }

    if (is_first) {
	if (area_top+1 >= area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
	area_top++;
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy + Y_energie >= free_energy_hd [cur_top]+free_energy_hd [cur_top+1])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    type_area_hd [cur_top] = WATSON_CRICK_PAIR;
    lgth_area_hd [cur_top] = 0;
    free_energy_hd [cur_top] = X_energy;

    cur_top++;

    type_area_hd [cur_top] = type_area_hd [Y_top];
    lgth_area_hd [cur_top] = lgth_area_hd [Y_top];
    free_energy_hd [cur_top] = Y_energy;

    return TRUE;
}

static BOOLEAN
right_pairing (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    int 	X_top, Y_top, Y_lgth, Y_type, lsi, rsi, WC1, WC2;
    float	X_energy, Y_energy;
    Aij_struct	*Aij_struct_ptr;

    /* ss(X, T1 Y T2) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, Y) &wc(T1, T2) ss(X,Y) . @right_pairing
       ssok(X, T1 Y T2) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, Y) &wc(T1, T2) ssok(X,Y) . @right_pairing*/

    X_top = SEM_PTR [rho [3]].ntger;
    X_energy = free_energy_hd [X_top];
    Y_top = X_top+1;
    Y_energy = free_energy_hd [Y_top];
    Y_type = type_area_hd [Y_top];

    switch (Y_type) {
    case UNKNOWN:
	break;

    case SINGLE_STRAND: /* hairpin loop */
	Y_lgth = lgth_area_hd [Y_top];

	if (Y_lgth > 18) return FALSE;

	Y_energy += Hairpin [Y_lgth];
	break;

    case WATSON_CRICK_PAIR:
	Aij_struct_ptr = Aij2struct [local_pid] (rho [3]);
	lsi = Aij_struct_ptr->lb [1];
	rsi = Aij_struct_ptr->ub [1];
	WC2 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)];
	WC1 = Watson_Crick [LSOURCE (lsi-1)][SOURCE (rsi+1)];

	Y_energy += PairxPair [WC1] [WC2];
	break;

    case LEFT_BULGE:
    case RIGHT_BULGE:
	Y_lgth = lgth_area_hd [Y_top];

	if (Y_lgth > 18) return FALSE;

	Y_energy += Bulge [Y_lgth];
	break;


    case LEFT_RIGHT_BULGE:
	Y_lgth = lgth_area_hd [Y_top];

	if (Y_lgth > 18) return FALSE;

	Y_energy += Internal [Y_lgth];
	break;
    }

    if (is_first) {
	if (area_top+1 >= area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
	area_top++;
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy + Y_energie >= free_energy_hd [cur_top]+free_energy_hd [cur_top+1])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    type_area_hd [cur_top] = type_area_hd [X_top];
    lgth_area_hd [cur_top] = lgth_area_hd [X_top];
    free_energy_hd [cur_top] = X_energy;

    cur_top++;

    type_area_hd [cur_top] = WATSON_CRICK_PAIR;
    lgth_area_hd [cur_top] = 0;
    free_energy_hd [cur_top] = Y_energy;

    return TRUE;
}


static BOOLEAN
left_catenate (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    int 	X_top, Y_top, X_lgth, ls1, rsi, WC1, WC2;
    float	X_energy, Y_energy;
    Aij_struct	*Aij_struct_ptr;

    /* ss(T1 X1 T2 X2, Y) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X1) !&StrLen(0, X2)
			      &wc(T1, T2) ss(X1,X2) rnatag(Y) . @left_catenate
       ssok(T1 X1 T2 X2, Y) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X1) !&StrLen(0, X2)
			        &wc(T1, T2) ssok(X1, X2) rnatag(Y) . @left_catenate */

    X_top = SEM_PTR [rho [3]].ntger;
    X_type = type_area_hd [X_top];
    X_energy = free_energy_hd [X_top];
    Y_top = SEM_PTR [rho [4]].ntger;
    Y_energy = free_energy_hd [Y_top];

    switch (X_type) {
    case UNKNOWN:
	break;

    case SINGLE_STRAND: /* hairpin loop */
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Hairpin [X_lgth];
	break;

    case WATSON_CRICK_PAIR:
	Aij_struct_ptr = Aij2struct [local_pid] (rho [3]);
	lsi = Aij_struct_ptr->lb [0];
	rsi = Aij_struct_ptr->ub [0];
	WC2 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)];
	WC1 = Watson_Crick [LSOURCE (lsi-1)][SOURCE (rsi+1)];

	X_energy += PairxPair [WC1] [WC2];
	break;

    case LEFT_BULGE:
    case RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	energy = X_energy + Bulge [X_lgth];
	break;


    case LEFT_RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Internal [X_lgth];
	break;
    }

    X_energy += free_energy_hd [X_top+1]; /* X2_energie */

    if (is_first) {
	if (area_top+1 >= area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top;
	area_top += 2;
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy + Y_energy >= free_energy_hd [cur_top] + free_energy_hd [cur_top+1])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    if (type_area_hd [X_top+1] == SINGLE_STRAND) {
	type_area_hd [cur_top] = RIGHT_BULGE;
	lgth_area_hd [cur_top] = lgth_area_hd [X_top+1];
    }
    else {
	type_area_hd [cur_top] = UNKNOWN;
	lgth_area_hd [cur_top] = 0;
    }

    free_energy_hd [cur_top] = X_energy;

    cur_top++;

    type_area_hd [cur_top] = type_area_hd [Y_top];
    lgth_area_hd [cur_top] = lgth_area_hd [Y_top];
    free_energy_hd [cur_top] = Y_energy;

    return TRUE;
}




static BOOLEAN
right_catenate (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    int 	X_top, Y_top, X_lgth, ls1, rsi, WC1, WC2;
    float	X_energy, Y_energy;
    Aij_struct	*Aij_struct_ptr;

    /* ss(X, Y1 T1 Y2 T2) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, Y1) !&StrLen(0, Y2)
			      &wc(T1, T2) ss(Y1, Y2) rnatag(X) . @right_catenate
       ssok(X, Y1 T1 Y2 T2) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, Y1) !&StrLen(0, Y2)
			        &wc(T1, T2) ssok(Y1, Y2) rnatag(X) . @right_catenate */

    X_top = SEM_PTR [rho [4]].ntger;
    X_energy = free_energy_hd [X_top];

    Y_top = SEM_PTR [rho [3]].ntger+1;
    Y_type = type_area_hd [Y_top+1];
    Y_energy = free_energy_hd [Y_top];

    switch (Y_type) {
    case UNKNOWN:
	break;

    case SINGLE_STRAND: /* hairpin loop */
	Y_lgth = lgth_area_hd [Y_top];

	if (Y_lgth > 18) return FALSE;

	Y_energy += Hairpin [Y_lgth];
	break;

    case WATSON_CRICK_PAIR:
	Aij_struct_ptr = Aij2struct [local_pid] (rho [3]);
	lsi = Aij_struct_ptr->lb [0];
	rsi = Aij_struct_ptr->ub [0];
	WC2 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)];
	WC1 = Watson_Crick [LSOURCE (lsi-1)][SOURCE (rsi+1)];

	Y_energy += PairxPair [WC1] [WC2];
	break;

    case LEFT_BULGE:
    case RIGHT_BULGE:
	Y_lgth = lgth_area_hd [Y_top];

	if (Y_lgth > 18) return FALSE;

	energy = Y_energy + Bulge [Y_lgth];
	break;


    case LEFT_RIGHT_BULGE:
	Y_lgth = lgth_area_hd [Y_top];

	if (Y_lgth > 18) return FALSE;

	Y_energy += Internal [Y_lgth];
	break;
    }

    Y_energy += free_energy_hd [Y_top-1]; /* Y1_energie */

    if (is_first) {
	if (area_top+1 >= area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top;
	area_top += 2;
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy + Y_energy >= free_energy_hd [cur_top] + free_energy_hd [cur_top+1])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    type_area_hd [cur_top] = type_area_hd [X_top];
    lgth_area_hd [cur_top] = lgth_area_hd [X_top];
    free_energy_hd [cur_top] = X_energy;

    cur_top++;

    if (type_area_hd [Y_top-1] == SINGLE_STRAND) {
	type_area_hd [cur_top] = LEFT_BULGE;
	lgth_area_hd [cur_top] = lgth_area_hd [Y_top-1];
    }
    else {
	type_area_hd [cur_top] = UNKNOWN;
	lgth_area_hd [cur_top] = 0;
    }

    free_energy_hd [cur_top] = Y_energy;

    return TRUE;
}


static BOOLEAN
cross_pairing (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    int 	X_top, Y_top, X_lgth, lsi, rsi, WC1, WC2;
    float	X_energy, Y_energy;
    Aij_struct	*Aij_struct_ptr;

    /* ss(T1 X, Y T2) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X) !&StrLen(0, Y)
			  &wc(T1, T2) ssok(X, Y) . @cross_pairing
       ssok(T1 X, Y T2) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X) !&StrLen(0, Y)
			    &wc(T1, T2) ssok(X,Y) . @cross_pairing */

    X_top = SEM_PTR [rho [3]].ntger;
    X_type = type_area_hd [X_top];
    X_energy = free_energy_hd [X_top];

    /* On suppose que X...Y n'est pas un SINGLE_STRAND => pas de CROSS_SINGLE_STRAND */

    switch (X_type) {
    case UNKNOWN:
    case SINGLE_STRAND:
    case WATSON_CRICK_PAIR:
    case LEFT_BULGE:
    case LEFT_RIGHT_BULGE:
    case RIGHT_BULGE:
	/* On commence une CROSS_WATSON_CRICK_PAIR */
	X_energy += free_energy_hd [X_top+1]; 
	break;

    case CROSS_WATSON_CRICK_PAIR:
	Aij_struct_ptr = Aij2struct [local_pid] (rho [3]);
	lsi = Aij_struct_ptr->lb [0];
	rsi = Aij_struct_ptr->ub [1];
	WC2 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)];
	WC1 = Watson_Crick [LSOURCE (lsi-1)][SOURCE (rsi+1)];

	X_energy += PairxPair [WC1] [WC2];
	break;

    case CROSS_LEFT_BULGE:
    case CROSS_RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Bulge [X_lgth];
	break;


    case CROSS_LEFT_RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Internal [X_lgth];
	break;

    case SINGLE_STRAND:
	/* ERROR (to be treated) */
    }

    if (is_first) {
	if (area_top+1 >= area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
	area_top++;
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy >= free_energy_hd [cur_top]+free_energy_hd [cur_top+1])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    type_area_hd [cur_top] = CROSS_WATSON_CRICK_PAIR;
    lgth_area_hd [cur_top] = 0;
    free_energy_hd [cur_top] = X_energy;

    cur_top++;
    type_area_hd [cur_top] = CROSS_WATSON_CRICK_PAIR;
    lgth_area_hd [cur_top] = 0;
    free_energy_hd [cur_top] = 0.;

    return TRUE;
}


static BOOLEAN
cross_pairing_right_catenate (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    int 	X_top, Y_top, X_lgth, lsi, rsi, WC1, WC2;
    float	X_energy, Y_energy;
    Aij_struct	*Aij_struct_ptr;

    /* ss(T1 X, Y1 T2 Y2) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X) !&StrLen(0, Y1) !&StrLen(0, Y2)
			      &wc(T1, T2) ssok(X, Y1) rnatag(Y2) . @cross_pairing_right_catenate
       ssok(T1 X, Y1 T2 Y2) --> &StrLen(1, T1) &StrLen(1, T2) !&StrLen(0, X) !&StrLen(0, Y1) !&StrLen(0, Y2)
			        &wc(T1, T2) ssok(X, Y1) rnatag(Y2) . @cross_pairing_right_catenate */

    X_top = SEM_PTR [rho [3]].ntger;
    X_type = type_area_hd [X_top];
    X_energy = free_energy_hd [X_top];

    Y_top = SEM_PTR [rho [3]].ntger;
    Y_type = type_area_hd [Y_top];
    Y_energy = free_energy_hd [Y_top];

    /* On suppose que X...Y n'est pas un SINGLE_STRAND => pas de CROSS_SINGLE_STRAND */

    switch (X_type) {
    case UNKNOWN:
    case SINGLE_STRAND:
    case WATSON_CRICK_PAIR:
    case LEFT_BULGE:
    case LEFT_RIGHT_BULGE:
    case RIGHT_BULGE:
	/* On commence une CROSS_WATSON_CRICK_PAIR, mais le resultat depend du type de Y2 */
	X_energy += free_energy_hd [X_top+1]; 
	break;

    case CROSS_WATSON_CRICK_PAIR:
	Aij_struct_ptr = Aij2struct [local_pid] (rho [3]);
	lsi = Aij_struct_ptr->lb [0];
	rsi = Aij_struct_ptr->ub [1];
	WC2 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)];
	WC1 = Watson_Crick [LSOURCE (lsi-1)][SOURCE (rsi+1)];

	X_energy += PairxPair [WC1] [WC2];
	break;

    case CROSS_LEFT_BULGE:
    case CROSS_RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Bulge [X_lgth];
	break;


    case CROSS_LEFT_RIGHT_BULGE:
	X_lgth = lgth_area_hd [X_top];

	if (X_lgth > 18) return FALSE;

	X_energy += Internal [X_lgth];
	break;

    case SINGLE_STRAND:
	/* ERROR (to be treated) */
    }

    if (is_first) {
	if (area_top+1 >= area_size)
	    realloc_area ();

	SEM_PTR [rho [1]].ntger = cur_top = area_top++;
	area_top++;
    }
    else {
	cur_top = SEM_PTR [rho [1]].ntger;
	
	if (X_energy + Y_energie >= free_energy_hd [cur_top] + free_energy_hd [cur_top+1])
	    /* On suppose que l'energie est additive */
	    return FALSE;
    }
   
    if (Y_type == SINGLE_STRAND) {
	type_area_hd [cur_top] = CROSS_RIGHT_BULGE;
	lgth_area_hd [cur_top] = lgth_area_hd [Y_top];
	free_energy_hd [cur_top] = X_energy;

	cur_top++;

	type_area_hd [cur_top] = CROSS_RIGHT_BULGE;
	lgth_area_hd [cur_top] = 0;
	free_energy_hd [cur_top] = 0.;
    }
    else {
	type_area_hd [cur_top] = UNKNOWN;
	lgth_area_hd [cur_top] = 0;
	free_energy_hd [cur_top] = X_energy;

	cur_top++;

	type_area_hd [cur_top] = UNKNOWN;
	lgth_area_hd [cur_top] = 0;
	free_energy_hd [cur_top] = Y_energy;
    }

    return TRUE;
}



