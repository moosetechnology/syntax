#include <values.h>

typedef struct {
    struct bag_disp_hd {
	SXINT		size;
	SXBA_ELT	*set;
    }		*hd;
    SXBA_ELT	*pool_top;
    SXINT		hd_size, hd_top, hd_high, pool_size, room;
    char	*name;
    SXINT		used_size, prev_used_size, total_size; /* #if EBUG */
} bag_header;


static void
bag_alloc (pbag, name, size)
    bag_header	*pbag;
    char	*name;
    SXINT 	size;
{
    SXBA_ELT	*ptr;

    pbag->name = name;
    pbag->hd_top = 0;
    pbag->hd_high = 0;
    pbag->hd_size = 1;
    pbag->pool_size = size;
    pbag->hd = (struct bag_disp_hd*) sxalloc (pbag->hd_size, sizeof (struct bag_disp_hd));
    pbag->pool_top = pbag->hd [0].set = (SXBA_ELT*) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));
    pbag->hd [0].size = pbag->pool_size;
    pbag->room = pbag->pool_size;

#if EBUG
    pbag->total_size = pbag->pool_size + 2;
    pbag->used_size = pbag->prev_used_size = 0;
#endif
}



static SXBA
bag_get (pbag, size)
    bag_header	*pbag;
    SXINT		size;
{
    SXINT 	slice_nb = SXNBLONGS (size) + 1;
    SXBA	set;

    if (slice_nb > pbag->room)
    {
	if (++pbag->hd_top >= pbag->hd_size)
	    pbag->hd = (struct bag_disp_hd*) sxrealloc (pbag->hd,
							pbag->hd_size *= 2,
							sizeof (struct bag_disp_hd));

	if (pbag->hd_top <= pbag->hd_high) {
	    pbag->pool_size = pbag->hd [pbag->hd_top].size;

	    while (slice_nb > (pbag->room = pbag->pool_size)) {
		if (++pbag->hd_top >  pbag->hd_high)
		    break;

		pbag->pool_size = pbag->hd [pbag->hd_top].size;
	    }
	}

	if (pbag->hd_top > pbag->hd_high) {	    
	    while (slice_nb > (pbag->room = pbag->pool_size))
		pbag->pool_size *= 2;

#if EBUG
	    printf ("Bag %s: New bag of size %i is created.\n", pbag->name, pbag->pool_size);
	    pbag->total_size += pbag->pool_size + 2;
#endif

	    pbag->hd_high = pbag->hd_top;
	    pbag->hd [pbag->hd_top].set = (SXBA) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));
	    pbag->hd [pbag->hd_top].size = pbag->pool_size;
	}

	pbag->pool_top = pbag->hd [pbag->hd_top].set;
    }

    *(set = pbag->pool_top) = size;
    pbag->pool_top += slice_nb;
    pbag->room -= slice_nb;

#if EBUG
    pbag->used_size += slice_nb;
#endif

    return set;
}



static void
bag_free (pbag)
    bag_header	*pbag;
{
#if EBUG
    printf ("Bag %s: used_size = %i bytes, total_size = %i bytes\n",
	    pbag->name,
	    (pbag->used_size > pbag->prev_used_size ? pbag->used_size : pbag->prev_used_size) *
	    sizeof (SXBA_ELT) + (pbag->hd_high + 1) * sizeof (struct bag_disp_hd),
    	    pbag->total_size * sizeof (SXBA_ELT) + pbag->hd_size * sizeof (struct bag_disp_hd));
#endif

    do {
	sxfree (pbag->hd [pbag->hd_high].set);
    } while (--pbag->hd_high >= 0);

    sxfree (pbag->hd);
}
    

static void
bag_clear (pbag)
    bag_header	*pbag;
{
    SXBA_ELT	*p, *p_top;

    /* RAZ des SXBA. */
    p = pbag->hd [pbag->hd_top].set;

    while (p < pbag->pool_top)
	*p++ = 0L;

    while (--pbag->hd_top >= 0) {
	p_top = p = pbag->hd [pbag->hd_top].set;
	p_top += pbag->hd [pbag->hd_top].size+2;

	while (p < p_top)
	    *p++ = 0L;
    }

    pbag->hd_top = 0;
    pbag->pool_top = pbag->hd [0].set;
    pbag->pool_size = pbag->hd [0].size;
    pbag->room = pbag->pool_size;

#if EBUG
    if (pbag->prev_used_size < pbag->used_size)
	pbag->prev_used_size = pbag->used_size;

    pbag->used_size = 0;
#endif
}




/* Action par defaut appelee si aucune @function n'est specifiee sur une clause */
#define sem_void		NULL

#define	GU	1
#define	AU	2
#define	UA	3
#define	CG	4
#define	GC	5


static SXINT Watson_Crick [7][7]= {
    /*	0	A	C	G	U	.	eof */	         
/* 0 */{0,	0,	0,	0,	0,	0,	0},
/* A */{0,	0,	0,	0,	AU,	0,	0},
/* C */{0,	0,	0,	CG,	0,	0,	0},
/* G */{0,	0,	GC,	0,	GU,	0,	0},
/* U */{0,	UA,	0,	0,	0,	0,	0},
/* . */{0,	0,	0,	0,	0,	0,	0},
/*eof*/{0,	0,	0,	0,	0,	0,	0},
};


SXINT wc_pid = 1;
SXINT wc_arity = 2;


static SXINT	area_size, area_top;
static float	*free_energy_hd;
static SXINT	*left_hd, *right_hd, *next_hd;
static SXBA	*bm_hd;
static bag_header	source_bag;
static SXINT	np1, deuxn;
#if EBUG
static SXBA	cur_bag;
#endif

static char	*main_pair_source;
static SXBA	*st_memo;
static X_header	st_rhoA_hd;
static X_header	st_range_1;
static SEM_TYPE	*st_sem_disp0;

struct main_parse {
    struct parse {
	float free_energy;
	char	*source;
    } main;
    SXINT			*pk_source;
    SXINT			pk_parse_nb, local_parse_nb;
    struct parse	*pk_parses;
};

static struct main_parse	*main_parses;
static SXINT			main_parse_nb;
static SXBOOLEAN			is_pk_pass;

static SXBOOLEAN		is_initial_stem_energy, is_self_complementary_energy, is_internal_loop_penalty, is_unpaired_base_increment;


SXBOOLEAN
_wc (rho0, ilb, iub)
    SXINT *rho0, *ilb, *iub;
{
    SXINT		nb;
    char	c, *p, *ptop;
    char ret_val = Watson_Crick [SOURCE(iub[0])][SOURCE(iub[1])];
#if EBUG
static char bases[] = " ACGU.";
#endif

    if (ret_val && is_pk_pass) {
	/* On est dans la passe qui recherche les pseudo-noeuds */
	/* Ce couple de bases doit encadrer des bases non correctement parenthesees */
	nb = 0;
	ptop = main_pair_source + iub[1];
	p = main_pair_source + iub[0];

	while (nb >= 0 && ++p < ptop) {
	    c = *p;

	    if (c == '(')
		nb++;
	    else
		if (c == ')')
		    nb--;
	}

	if (nb == 0)
	    ret_val = 0;
    }

#if EBUG
printf ("(Watson-Crick): wc (%c[%i], %c[%i]) %s\n", bases[SOURCE(iub[0])], iub[0], bases[SOURCE(iub[1])], iub[1],
	ret_val ? "TRUE" : "FALSE");
#endif
    *rho0 = 0; /* Prudence */

    return ret_val;
}

#define MAX_LOOP_SIZE	30

static float Bulge[31] = { 0.,
3.3, 5.2, 6.0, 6.7, 7.4, 8.2, 9.1, 10.0, 10.5, 11.0, 11.4, 11.8, 12.2, 12.5, 12.7, 13.0, 13.3, 13.6,
13.8, 14.0, 14.2, 14.4, 14.6, 14.8, 15.0, 15.2, 15.3, 15.5, 15.6, 15.8			       
};

#define Bulge_energy(Xlgth)	((Xlgth > MAX_LOOP_SIZE) ? Bulge [MAX_LOOP_SIZE]+(Xlgth-MAX_LOOP_SIZE)*0.2 : Bulge [Xlgth])

static float Hairpin[31] = { 0.,
0., 0., 7.4, 5.9, 4.4, 4.3, 4.1, 4.1, 4.2, 4.3, 4.6, 4.9, 5.2, 5.6, 5.8, 6.1, 6.4, 6.7,
6.9, 7.1, 7.3, 7.5, 7.7, 7.9, 8.1, 8.3, 8.4, 8.6, 8.7, 8.9				 
};

#define Haipin_energy(Xlgth)	((Xlgth > MAX_LOOP_SIZE) ? Hairpin [MAX_LOOP_SIZE]+(Xlgth-MAX_LOOP_SIZE)*0.2 : Hairpin [Xlgth])

static float Internal[31] = { 0.,
0., 0.8, 1.3, 1.7, 2.1, 2.5, 2.6, 2.8, 3.1, 3.6, 4.0, 4.4, 4.7, 5.1, 5.3, 5.6, 5.9, 6.2,
6.4, 6.6, 6.8, 7.0, 7.2, 7.4, 7.6, 7.8, 7.9, 8.1, 8.2, 8.4
};

#define Internal_energy(Xlgth)	((Xlgth > MAX_LOOP_SIZE) ? Internal [MAX_LOOP_SIZE]+(Xlgth-MAX_LOOP_SIZE)*0.2 : Internal [Xlgth])

static float PairxPair [6][6] = {
    /*	0	GU	AU	UA	CG	GC	*/	         
/* 0  */{0.,	0.,	0.,	0.,	0.,	0.},
/* GU */{0.,	-0.5,	-0.5,	-0.7,	-1.5,	-1.3},
/* AU */{0.,	-0.5,	-0.9,	-1.1,	-1.8,	-2.3},
/* UA */{0.,	-0.7,	-0.9,	-0.9,	-1.7,	-2.1},
/* CG */{0.,	-1.9,	-2.1,	-2.3,	-2.9,	-3.4},
/* GC */{0.,	-1.5,	-1.7,	-1.8,	-2.0,	-2.9}
};


/* Free_energy increments for some unpaired terminal nucleotides */
/* out_stacking [X] [YZ] = [YX...Z] (X debute le stem-body cote 5')
                        ou [Z...Y]X (X est exterieur au stem, cote 3') */
/* in_stacking [X] [YZ]  = X[Y...Z] (X est exterieur au stem, cote 5')
                        ou [Z...XY] (X finit le stem-body cote 3') */

static float Out_Stacking [7][6] = {
    /*	0	GU	AU	UA	CG	GC	*/	         
/* 0  */{0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
/* A */{0.0,	0.0,	0.0,	0.0,	-1.7,	-1.1},
/* C */{0.0,	0.0,	0.0,	-0.1,	0.0,	0.0},
/* G */{0.0,	0.0,	0.0,	0.0,	-1.7,	-1.3},
/* U */{0.0,	0.0,	0.0,	-0.1,	-1.2,	0.0},
/* . */{0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
/*eof*/{0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
	    
};

static float In_Stacking [7][6] = {
    /*	0	GU	AU	UA	CG	GC	*/	         
/* 0  */{0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
/* A */{0.0,	0.0,	0.0,	0.0,	0.0,	-0.2},
/* C */{0.0,	0.0,	0.0,	-0.2,	-0.2,	0.0},
/* G */{0.0,	0.0,	0.0,	0.0,	-0.2,	0.0},
/* U */{0.0,	0.0,	-0.2,	-0.2,	-0.1,	0.0},
/* . */{0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
/*eof*/{0.0,	0.0,	0.0,	0.0,	0.0,	0.0},
};



#define INITIAL_STEM_ENERGY		3.4
#define SELF_COMPLEMENTARY_ENERGY	0.4

static SXBOOLEAN
self_complementary (lsi, lgth)
    SXINT lsi, lgth;
{
    /* Si les 2 brins d'un stem sont "self complementary" (je ne sais pas ce que c'est! je prends meme
       base sur un brin), on ajoute une penalite de 0.4 kcal/mol */
    SXINT base = SOURCE (lsi);

    while (--lgth > 0) {
	lsi++;

	if (SOURCE (lsi) != base)
	    return SXFALSE;
    }

    return SXTRUE;
}


static void
realloc_area ()
{
    area_size *= 2;
    left_hd = (SXINT *) sxrealloc (left_hd, area_size+1, sizeof (SXINT));
    right_hd = (SXINT *) sxrealloc (right_hd, area_size+1, sizeof (SXINT));
    free_energy_hd = (float *) sxrealloc (free_energy_hd, area_size+1, sizeof (float));
    bm_hd = (SXBA *) sxrealloc (bm_hd, area_size+1, sizeof (SXBA));

    if (best_tree_number > 1)
	next_hd = (SXINT *) sxrealloc (next_hd, area_size+1, sizeof (SXINT));
}

static void
_RNA_semact_init (init_size)
    SXINT init_size;
{
    area_top = 1;

    if (free_energy_hd == NULL) {
	fputs ("dl&1 => is_internal_loop_penalty\n\
dl&2 => is_self_complementary_energy\n\
dl&4 => is_initial_stem_energy\n\
dl&8 => is_unpaired_base_increment\n", stdout);

	if (debug_level & 8)
	    is_unpaired_base_increment = SXTRUE;

	if (debug_level & 4)
	    is_initial_stem_energy = SXTRUE;

	if (debug_level & 2)
	    is_self_complementary_energy = SXTRUE;

	if (debug_level & 1)
	    is_internal_loop_penalty = SXTRUE;

	np1 = n+1;
	deuxn = 2*n;
	area_size = init_size;
	left_hd = (SXINT *) sxalloc (area_size+1, sizeof (SXINT));
	right_hd = (SXINT *) sxalloc (area_size+1, sizeof (SXINT));
	free_energy_hd = (float *) sxalloc (area_size+1, sizeof (float));
	bm_hd = (SXBA *) sxalloc (area_size+1, sizeof (SXBA));
	bag_alloc (&source_bag, "source_bag", (area_size+1)*(SXNBLONGS(np1)+1));

	if (best_tree_number > 1)
	    next_hd = (SXINT *) sxalloc (area_size+1, sizeof (SXINT));
    }
    else {
	bag_clear (&source_bag);
    }
}

static void
_RNA_semact_final ()
{
    if (!is_pk_pass && free_energy_hd) {
	sxfree (left_hd);
	sxfree (right_hd);
	sxfree (free_energy_hd), free_energy_hd = NULL;
	sxfree (bm_hd);
	bag_free (&source_bag);

	if (best_tree_number > 1)
	    sxfree (next_hd);
    }
}

static SXINT
tree (Xtop, i)
    SXINT Xtop, i;
{
    SXINT			left, right, lgth;

    /*
       sons		left	right
       ss		-l	0
       hp		-l	0
       lb		-l	X
       rna		-l	X
       stem		-n-l	X
       balancedil	-2n-2l	X
       rb		X	-l
       clover		X	Y
       */


    left = left_hd [Xtop];
    right = right_hd [Xtop];
#if EBUG
    if (right < -n)
	sxtrap ("RNA_semact.h", "tree");
#endif

    if (left < -deuxn || (left < 0 && -n <= left)) {
	if (left < -deuxn)
	    /* balanced internal loop */
	    lgth = (-left - deuxn)/2;
	else
	    /* .... */
	    lgth = -left;

	while (lgth--) {
	    i++;
#if EBUG
	    if (SXBA_bit_is_set (cur_bag, i))
		sxtrap ("RNA_semact.h", "tree");
#endif
	    main_pair_source [i] = '.';
	}
    }
    else {
	if (left < -n) {
	    /* stem */
	    i++;
#if EBUG
	    if (!SXBA_bit_is_set (cur_bag, i))
		sxtrap ("RNA_semact.h", "tree");
#endif
	    main_pair_source [i] = '(';
	}
	else {
	    if (left > 0)
		i = tree (left, i);
	}
    }

    if (right < 0) {	
	/* .... */
	lgth = -right;

	while (lgth--) {
	    i++;
#if EBUG
	    if (SXBA_bit_is_set (cur_bag, i))
		sxtrap ("RNA_semact.h", "tree");
#endif
	    main_pair_source [i] = '.';
	}
    }
    else {
	if (right > 0)
	    i = tree (right, i);
    }

    if (left < -deuxn) {
	/* balanced internal loop */
	lgth = (-left - deuxn)/2;

	while (lgth--) {
	    i++;
#if EBUG
	    if (SXBA_bit_is_set (cur_bag, i))
		sxtrap ("RNA_semact.h", "tree");
#endif
	    main_pair_source [i] = '.';
	}
    }
    else {
	if (left < -n) {
	    /* stem */
	    i++;
#if EBUG
	    if (!SXBA_bit_is_set (cur_bag, i))
		sxtrap ("RNA_semact.h", "tree");
#endif
	    main_pair_source [i] = ')';
	}
    }

    return i;
}



static SXBOOLEAN
_RNA_semact_last_pass (S0n, start_symbol_pid, ret_val)
    SXINT 	S0n, start_symbol_pid;
    SXBOOLEAN	ret_val;
{
    /* ret_val est vrai ssi l'analyse a marche' */
    /* start_symbol_pid est l'identifiant du module qui contient l'axiome */
    /* S0n est l'identifiant du couple axiome, (vecteur de) range [0..n] */
    SXINT 	main_top, Xtop, i, j, k;
    static SXINT	rho0, zero, top;
    SXBOOLEAN	ret_val_2;
    float	free_energy_1, free_energy_2;
    char	c, *s1, *s2;
    struct main_parse	*main_parse, *main_parse_top;
    struct parse 	*pk_parse;

    if (ret_val) {
	/* On memorise les structures principales */
	st_memo = memo;
	st_rhoA_hd = rhoA_hd;
	st_range_1 = range_1;
	st_sem_disp0 = sem_disp [0];

	is_pk_pass = SXTRUE; /* modifie _wc () */

	main_top = sem_disp [0] [S0n].ntgr;
	main_parse_nb = 0;
	main_parse = main_parses = (struct main_parse *) sxalloc (best_tree_number+1, sizeof (struct main_parse));
	    
	do {
	    main_parse_nb++;
	    main_parse++;
	    main_parse->main.free_energy = free_energy_hd [main_top];
	    main_parse->main.source = main_pair_source = s1 = (char *) sxalloc (np1+1, sizeof (char));
#if EBUG
	    cur_bag = bm_hd [main_top];
#endif

	    tree (main_top, 0);

	    s1 [np1] = (char)0;

	    /* On fabrique un nouveau source */
	    main_parse->pk_source = (SXINT *) sxalloc (np1+1, sizeof (SXINT));

	    main_parse->pk_source [0] = glbl_source [0];
	
	    for (i = 1;i <= n;i++) {
		if (main_pair_source [i] == '.')
		    main_parse->pk_source [i] = glbl_source [i];
		else
		    main_parse->pk_source [i] = 5; /* code de '.' */
	    }

	    main_parse->pk_source [np1] = glbl_source [np1];
	} while ((main_top = ((next_hd == NULL) ? 0 : next_hd [main_top])) > 0);

	main_parse_top = main_parse;
	main_parse = main_parses;

	while (++main_parse <= main_parse_top) {
	    /* On pointe sur le nouveau source */
	    main_pair_source = main_parse->main.source; /* utisise par _wc () */
	    source = main_parse->pk_source;
	    lsource = source+1;

	    /* On alloue les structures pour les pseudo-knots */
	    RNA_first_pass_init ();

	    /* C'est parti */
#if EBUG
	    fputs ("\n**************** pseudo-knot pass ****************\n", stdout);

	    /* On sort le nouveau source */
	    for (i = 1;i <= n;i++)
		putchar(t2str [SOURCE (i)][0]);

	    putchar('\n');
#endif

	    top = n;
	    ret_val_2 = (*first_pass) (&rho0, &zero, &top);

	    /* Fin de la passe sur les pseudo-noeuds */
	    Xtop = sem_disp [0] [rho0].ntgr;

	    pk_parse = main_parse->pk_parses = (struct parse *) sxalloc (best_tree_number+1, sizeof (struct parse));
	    main_parse->pk_parse_nb = 0;

	    do {
		main_parse->pk_parse_nb++;
		pk_parse++;
		pk_parse->free_energy = free_energy_hd [Xtop];
		
		pk_parse->source = main_pair_source = s2 = (char *) sxalloc (np1+1, sizeof (char));
#if EBUG
		cur_bag = bm_hd [Xtop];
#endif
		tree (Xtop, 0);

		s2 [np1] = (char)0;
	    } while ((Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])) > 0);

	    /* On remet en etat */
	    /* Attention, _RNA_semact_final () va etre appele mais is_pk_pass == SXTRUE */
	    RNA_first_pass_final ();

	}
    
	/* On reaccede au vrai source */
	source = glbl_source;
	lsource = glbl_source+1;

	/* On sort le vrai source */
	for (i = 1;i <= n;i++)
	    putchar(t2str [SOURCE (i)][0]);

	putchar('\n');

	main_parse = main_parses;

	for (i = 1; i <= main_parse_nb; i++) {
	    main_parse++;
	    /* On sort les paires principales et l'energie correspondante */
	    fprintf (stdout, "%s Main free-energy = %f kcal/mol\n", (s1 = main_parse->main.source)+1,
		     free_energy_1 = main_parse->main.free_energy);

	    pk_parse = main_parse->pk_parses;
	    

	    for (j = 1; j <= main_parse->pk_parse_nb; j++) {
		pk_parse++;
		/* On sort les pseudo-noeuds et l'energie correspondante */
		fprintf (stdout, "%s Pseudo-knot free-energy = %f kcal/mol\n", (s2 = pk_parse->source)+1,
			 free_energy_2 = pk_parse->free_energy);

		/* On ne tient compte des pseudo-noeuds que si l'energie a diminue */
		if (free_energy_2 < 0.0) {
		    pk_parse->free_energy += free_energy_1;
		    main_parse->local_parse_nb = j;

		    for (k = 1;k <= n;k++) {
			c = s2 [k];
			s2 [k] = (c == '.') ? s1 [k] : ((c == '(') ? '[' : ']');
		    }

		    fprintf (stdout, "%s Total free-energy = %f kcal/mol\n", s2+1, free_energy_1+free_energy_2);
		}
	    }
	}

	memo = st_memo;
	rhoA_hd = st_rhoA_hd;
	range_1 = st_range_1;
	sem_disp [0] = st_sem_disp0;
	is_pk_pass = SXFALSE;

	main_parse = main_parses;

	for (i = 1; i <= main_parse_nb; i++) {
	    main_parse++;
	    sxfree (main_parse->main.source);
	    sxfree (main_parse->pk_source);
	    pk_parse = main_parse->pk_parses;

	    for (j = 1; j <= main_parse->pk_parse_nb;j++) {
		pk_parse++;
		sxfree (pk_parse->source);
	    }

	    sxfree (main_parse->pk_parses);
	}

	sxfree (main_parses);
    }
    else
	fputs ("This source text is not a sentence.\n", stdout);

    return ret_val;
}


#if EBUG
static void
debug_print (action_name, rho, Xenergy, Xlgth, is_first, cur_top, son1_top, son2_top)
    char *action_name;
    SXINT *rho, Xlgth, cur_top, son1_top, son2_top;
    float Xenergy;
    SXBOOLEAN is_first;
{
    struct Aij_struct	*Aij_struct_ptr;
    
    Aij_struct_ptr = Aij2struct [local_pid] (rho [1]);
    printf ("%s ([%i..%i]) @%s return %s: clause_nb=%i, cur_top=%i, son1_top=%i, son2_top=%i, is_first=%s, energy=%f, lgth=%i;\n",
	    Aij_struct_ptr->A, Aij_struct_ptr->lb [0], Aij_struct_ptr->ub [0], action_name,
	    (cur_top > 0) ? "TRUE" : "FALSE", rho[0], cur_top, son1_top, son2_top,
	    is_first ? "TRUE" : "FALSE", Xenergy, Xlgth);
}
#endif

static SXINT
where (head_ptr, Xenergy, is_first, nb)
    SXINT		*head_ptr, *nb;
    SXBOOLEAN	is_first;
    float	Xenergy;
{
    SXINT cur_top, prev_top, next_top, prev_last_top, last_top, bnb;

    if (is_first) {
	if (area_top == area_size)
	    realloc_area ();

	*head_ptr = cur_top = area_top++;

	if (next_hd) next_hd [cur_top] = 0;

	return cur_top;
    }

    next_top = *head_ptr; 

    if (next_hd) {
	prev_top = last_top = 0;

	do {
	    (*nb)++;

	    if (Xenergy < free_energy_hd [next_top]) {
		if (*nb == best_tree_number) {
		    /* On remplace next_top */
		    return next_top;
		}
#if EBUG
		if (*nb > best_tree_number) sxtrap ("RNA_semact.h", "where");
#endif
		/* Soit on cree un nouveau bucket, soit on remplace la plus forte energie */
		last_top = next_top;
		bnb = *nb;

		while ((last_top = next_hd [prev_last_top = last_top]) > 0) {
		    if (++bnb == best_tree_number) {
			/* On reutilise le dernier */
			next_hd [prev_last_top] = 0;
			cur_top = last_top;
			break;
		    }
		}

		break;
	    }
	} while ((next_top = next_hd [prev_top = next_top]) > 0);

	if (*nb == best_tree_number) return 0; /* niveau complet et meilleur */
	
	if (last_top == 0) {
	    /* On cree un nouveau */
	    if (area_top == area_size)
		realloc_area ();

	    cur_top = area_top++;
	}

	next_hd [cur_top] = next_top;

	if (prev_top == 0)
	    *head_ptr = cur_top;
	else
	    next_hd [prev_top] = cur_top;

	/* Si <0 => on reutilise un bucket */
	return (last_top == 0) ? cur_top : -cur_top;
    }

    if (Xenergy >= free_energy_hd [next_top])
	return 0;

    return -next_top; /* On reutilise un bucket */
}



static SXBOOLEAN
share (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT	Xtop = sem_disp [0] [rho [2]].ntgr;

    /*
     axiome(X) --> RNA(X) . @share
    */

#if EBUG
    if (!is_first)
	sxtrap ("RNA_semact.h","share");

    debug_print ("share", rho, free_energy_hd [Xtop], 0, is_first, Xtop, Xtop, 0);
#endif

    sem_disp [0] [rho [1]].ntgr = Xtop;
 
    return is_first;
}


static SXBOOLEAN
copy (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, cur_top;
    float		Xenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;
    SXBA		ss;

    /*
       RNA(X1 X2)		--> &StrLen(7, X1) CLOVER(X1 X2) .			@copy
       STEM_BODY(X1 X2)		--> &StrLen(9, X1) LOOP(X1 X2) .			@copy
       STEM_BODY(X1 X2)		--> &StrLen(14, X2) STEM_CLOVER(X1 X2) .		@copy
    */

    Xtop = sem_disp [0] [rho [2]].ntgr;

    do {
	Xenergy = free_energy_hd [Xtop];

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    if (cur_top < 0) {
		cur_top = -cur_top;
	    }

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = 0;
	    right_hd [cur_top] = Xtop;
	    bm_hd [cur_top] = bm_hd [Xtop];
	}

#if EBUG
	debug_print ("copy", rho, Xenergy, 0, is_first, cur_top, Xtop, 0);
#endif

    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}



static SXBOOLEAN
single_strand (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT		cur_top, lgth;
    SXINT		*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT		nb = 0;

    /*
       RNA(SS) --> . @single_strand
    */

    if ((cur_top = where (head_ptr, MAXFLOAT, is_first, &nb)) != 0) {
	if (cur_top < 0) {
	    cur_top = -cur_top;
	}

	free_energy_hd [cur_top] = MAXFLOAT;
	lgth = Aij2struct [local_pid] (rho [1])->ub [0] - Aij2struct [local_pid] (rho [1])->lb [0];
	left_hd [cur_top] =  -lgth;
	right_hd [cur_top] = 0;
	bm_hd [cur_top] = bag_get (&source_bag, np1);
    }

#if EBUG
    debug_print ("single_strand", rho, MAXFLOAT, lgth, is_first, cur_top, 0, 0);
#endif

    return cur_top > 0;
}




static SXBOOLEAN
left_ss (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, Xlgth, cur_top;
    float		Xenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;

    /*
       CLOVER(SS X1 X2)		--> !&StrLen(0, SS) &StrLen(7, X2) SS_CLOVER(X1 X2) .	@left_ss
       STEM_CLOVER(SS X1 X2)	--> !&StrLen(0, SS) &StrLen(14, X2)
				    SS_STEM_CLOVER(X1 X2) .				@left_ss
    */

    Xtop = sem_disp [0] [rho [2]].ntgr; /* Always the first predicate call */
    Xlgth = Aij2struct [local_pid] (rho [2])->lb [0] - Aij2struct [local_pid] (rho [1])->lb [0];

    do {
	Xenergy = free_energy_hd [Xtop];

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    /* cur_top est le nbieme de la liste */
	    if (cur_top < 0) {
		cur_top = -cur_top;
	    }

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = -Xlgth;
	    right_hd [cur_top] = Xtop;
	    bm_hd [cur_top] = bm_hd [Xtop];
	}

#if EBUG
	debug_print ("left_ss", rho, Xenergy, Xlgth, is_first, cur_top, Xtop, 0);
#endif
    
    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}


static SXBOOLEAN
stem_rna (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT			cur_top, Xtop, Ytop, nb, lsi, rsi, T1, T2, T4, iT4, WC21, Ylgth;
    float		Xenergy, Yenergy, OSenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr;
    struct Aij_struct	*Aij_struct_ptr;
    SXBA		ss;
    /*
       CLOVER(X1 X2 Y)			--> &StrLen(7, X1) STEM(X1 X2) RNA(Y) .			@stem_rna
    */
    Xtop = sem_disp [0] [rho [2]].ntgr;

    if (is_unpaired_base_increment) {
	/* Ici, on est sur que le terminal devant X1 est non libre */
	Aij_struct_ptr = Aij2struct [local_pid] (rho [2]);
	rsi = Aij_struct_ptr->ub [0];

	if (Ylgth = (Aij2struct [local_pid] (rho [3])->ub [0] - rsi)) {
	    /* |Y| > 0 */
	    /* On calcule l'energie, mais elle ne sera utilisee que conditionnellement */
	    lsi = Aij_struct_ptr->lb [0];
	    T1 = LSOURCE (lsi);
	    T2 = SOURCE (rsi);
	    iT4 = rsi+1;
	    T4 = SOURCE (iT4);
	    WC21 = Watson_Crick [T2] [T1];
	    OSenergy = Out_Stacking [T4][WC21];
	}
    }
    else
	Ylgth = 0;

    do {
	head_ptr = &(sem_disp [0] [rho [1]].ntgr); /* On repart d'ici a chaque X */
	Xenergy = free_energy_hd [Xtop];
	nb = 0;
	Ytop = sem_disp [0] [rho [3]].ntgr;

	do {
	    if ((Yenergy = free_energy_hd [Ytop]) == MAXFLOAT)
		Yenergy = Xenergy;
	    else
		Yenergy += Xenergy;

	    if (Ylgth) {
		ss = bm_hd [Ytop];

		if (!SXBA_bit_is_set (ss, iT4)) {
		    /* T4 est libre */
		    Yenergy += OSenergy;
		}
	    }

	    if ((cur_top = where (head_ptr, Yenergy, is_first && !ret_val, &nb)) != 0) {
		if (cur_top < 0) {
		    cur_top = -cur_top;
		    /* On peut reutiliser le sxba precedant */
		    ss = bm_hd [cur_top];
		}
		else
		    ss = bm_hd [cur_top] = bag_get (&source_bag, np1);

		sxba_copy (ss, bm_hd [Xtop]);
		sxba_or (ss, bm_hd [Ytop]);

		head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
		ret_val = SXTRUE;
		free_energy_hd [cur_top] = Yenergy;
		left_hd [cur_top] = Xtop;
		right_hd [cur_top] = Ytop;
	    }

#if EBUG
	    debug_print ("stem_rna", rho, Yenergy, 0, is_first, cur_top, Xtop, Ytop);
#endif
    
	} while (nb < best_tree_number && (Ytop = ((next_hd == NULL) ? 0 : next_hd [Ytop])));
    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}


static SXBOOLEAN
ss_stem_rna (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT			cur_top, Xtop, Ytop, nb, lsi, rsi, T1, T2, T3, iT4, T4, WC12, WC21, Ylgth;
    float		Xenergy, Yenergy, ISenergy, OSenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr;
    struct Aij_struct	*Aij_struct_ptr;
    SXBA		ss;
    /*
       SS_CLOVER(X1 X2 Y)	--> &StrLen(7, X1) STEM(X1 X2) RNA(Y) .			@ss_stem_rna
    */
    Xtop = sem_disp [0] [rho [2]].ntgr;

    if (is_unpaired_base_increment) {
	/* Ici, on est sur que le terminal devant X1 est libre */
	Aij_struct_ptr = Aij2struct [local_pid] (rho [2]);
	rsi = Aij_struct_ptr->ub [0];
	lsi = Aij_struct_ptr->lb [0];
	T1 = LSOURCE (lsi);
	T2 = SOURCE (rsi);
	WC12 = Watson_Crick [T1] [T2];
	T3 = SOURCE (lsi);
	ISenergy = In_Stacking [T3][WC12];

	if (Ylgth = (Aij2struct [local_pid] (rho [3])->ub [0] - rsi)) {
	    /* |Y| > 0 */
	    /* On calcule l'energie, mais elle ne sera utilisee que conditionnellement */
	    iT4 = rsi+1;
	    T4 = SOURCE (iT4);
	    WC21 = Watson_Crick [T2] [T1];
	    OSenergy = Out_Stacking [T4][WC21];
	}
    }
    else
	Ylgth = 0;

    do {
	head_ptr = &(sem_disp [0] [rho [1]].ntgr); /* On repart d'ici a chaque X */
	Xenergy = free_energy_hd [Xtop];
	nb = 0;
	Ytop = sem_disp [0] [rho [3]].ntgr;

	do {
	    if ((Yenergy = free_energy_hd [Ytop]) == MAXFLOAT)
		Yenergy = Xenergy;
	    else
		Yenergy += Xenergy;

	    if (is_unpaired_base_increment)
		Yenergy += ISenergy;

	    if (Ylgth) {
		ss = bm_hd [Ytop];

		if (!SXBA_bit_is_set (ss, iT4)) {
		    /* T4 est libre */
		    Yenergy += OSenergy;
		}
	    }

	    if ((cur_top = where (head_ptr, Yenergy, is_first && !ret_val, &nb)) != 0) {
		if (cur_top < 0) {
		    cur_top = -cur_top;
		    /* On peut reutiliser le sxba precedant */
		    ss = bm_hd [cur_top];
		}
		else
		    ss = bm_hd [cur_top] = bag_get (&source_bag, np1);

		sxba_copy (ss, bm_hd [Xtop]);
		sxba_or (ss, bm_hd [Ytop]);

		head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
		ret_val = SXTRUE;
		free_energy_hd [cur_top] = Yenergy;
		left_hd [cur_top] = Xtop;
		right_hd [cur_top] = Ytop;
	    }

#if EBUG
	    debug_print ("ss_stem_rna", rho, Yenergy, 0, is_first, cur_top, Xtop, Ytop);
#endif
    
	} while (nb < best_tree_number && (Ytop = ((next_hd == NULL) ? 0 : next_hd [Ytop])));
    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}



static SXBOOLEAN
stem (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, cur_top, lsi, rsi, WC12, WC21, lgth, T1, T2, T3, T4;
    float		Xenergy, WCenergy;
    SXBOOLEAN		stem_end, ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;
    struct Aij_struct	*Aij_struct_ptr;
    SXBA		ss;

    /*
       STEM(T1 X1 X2 T2)	--> &StrLen(1, T1) &StrLen(1, T2) &StrLen(7, X1) 
				    &wc(T1, T2) STEM(X1 X2) .				@stem
       STEM2(T1 X T2)		--> &StrLen(1, T1) &StrLen(1, T2)
				    &wc(T1, T2) STEM1(X) .				@stem
    */
    Xtop = sem_disp [0] [rho [3]].ntgr;
    Aij_struct_ptr = Aij2struct [local_pid] (rho [3]);
    lsi = Aij_struct_ptr->lb [0];
    rsi = Aij_struct_ptr->ub [0];
    WC21 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)]; /* Les internes */
    rsi++;
    T1 = SOURCE (lsi);
    T2 = SOURCE (rsi);
    WC12 = Watson_Crick [T1][T2]; /* Les externes */
    /* lsi et rsi repere les T1 et T2 externes */
#if EBUG
    if (WC12 == 0 || WC21 == 0) sxtrap ("RNA_semact.h", "stem");
#endif

    WCenergy = PairxPair [WC21] [WC12];

    if (is_self_complementary_energy) {
	T3 = SOURCE (lsi-1);
	T4 = SOURCE (rsi+1);
	stem_end = Watson_Crick [T3][T4] == 0;
    }
    else
	stem_end = SXFALSE;

    do {
	Xenergy = free_energy_hd [Xtop] + WCenergy;

	if ((lgth = left_hd [Xtop]) == 0)
	    lgth = -n-3;
	else
	    lgth--;

	if (stem_end && self_complementary (lsi, -lgth-n))
	    Xenergy += SELF_COMPLEMENTARY_ENERGY;

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    if (cur_top < 0) {
		cur_top = -cur_top;
	    }

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = lgth;
	    right_hd [cur_top] = Xtop;
	    ss = bm_hd [cur_top] = bag_get (&source_bag, np1);
	    sxba_copy (ss, bm_hd [Xtop]);
	    SXBA_1_bit(ss, lsi);
	    SXBA_1_bit(ss, rsi);
	}

#if EBUG
	debug_print ("stem", rho, Xenergy, -n-lgth, is_first, cur_top, Xtop, 0);
#endif

    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}




static SXBOOLEAN
copy_stem (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, cur_top;
    float		Xenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;
    SXBA		ss;

    /*
       STEM(X1 X2)		--> &StrLen(7, X1) STEM2(X1 X2) .			@copy_stem
    */

    Xtop = sem_disp [0] [rho [2]].ntgr;

    do {
	Xenergy = free_energy_hd [Xtop];

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    if (cur_top < 0) {
		cur_top = -cur_top;
	    }

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = 0;
	    right_hd [cur_top] = Xtop;
	    bm_hd [cur_top] = bm_hd [Xtop];
	}

#if EBUG
	debug_print ("copy_stem", rho, Xenergy, 2, is_first, cur_top, Xtop, 0);
#endif

    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}




static SXBOOLEAN
init_stem (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, cur_top, lsi, rsi, T1, T2, T3, T4, WC12, WC21, iT3, iT4;
    float		Xenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;
    struct Aij_struct	*Aij_struct_ptr;
    SXBA		ss;
    /*
       STEM1(T1 X T2)		--> &StrLen(1, T1) &StrLen(1, T2)
				    &wc(T1, T2) STEM_BODY(X) .				@init_stem
    */
    Xtop = sem_disp [0] [rho [3]].ntgr;
    Aij_struct_ptr = Aij2struct [local_pid] (rho [1]);
    lsi = Aij_struct_ptr->lb [0]+1;
    rsi = Aij_struct_ptr->ub [0];
    /* lsi et rsi reperent T1 et T2 */

    if (is_unpaired_base_increment) {
	T1 = SOURCE (lsi);
	T2 = SOURCE (rsi);
	WC12 = Watson_Crick [T1] [T2];
	WC21 = Watson_Crick [T2] [T1];
	iT3 = lsi+1;
	iT4 = rsi-1;
	T3 = SOURCE (iT3);
	T4 = SOURCE (iT4);
    }

    do {
	Xenergy = free_energy_hd [Xtop];

	if (is_initial_stem_energy)
	    Xenergy += INITIAL_STEM_ENERGY;
	    
	if (is_unpaired_base_increment) {
	    ss =  bm_hd [Xtop];

	    if (!SXBA_bit_is_set (ss, iT3))
		Xenergy += Out_Stacking [T3][WC12];

	    if (!SXBA_bit_is_set (ss, iT4))
		Xenergy += In_Stacking [T4][WC21];
	}

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    if (cur_top < 0) {
		cur_top = -cur_top;
		ss = bm_hd [cur_top];
	    }
	    else
		ss = bm_hd [cur_top] = bag_get (&source_bag, np1);

	    sxba_copy (ss, bm_hd [Xtop]);
	    SXBA_1_bit(ss, lsi);
	    SXBA_1_bit(ss, rsi);

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = -n-1;
	    right_hd [cur_top] = Xtop;
	}

#if EBUG
	debug_print ("init_stem", rho, Xenergy, 1, is_first, cur_top, Xtop, 0);
#endif

    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}



static SXBOOLEAN
left_bulge (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, SSlgth, cur_top, lsi, rsi, T1, T2, T3, WC12;
    float		Xenergy, Benergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;
    struct Aij_struct	*Aij_struct_ptr;

    /*
       STEM_BODY(SS X1 X2)	--> !&StrLen(0, SS) &StrLen(7, X2) STEM(X1 X2) .	@left_bulge
    */
    Xtop = sem_disp [0] [rho [2]].ntgr; 
    SSlgth = Aij2struct [local_pid] (rho [2])->lb [0] - Aij2struct [local_pid] (rho [1])->lb [0];
    Benergy = Bulge_energy (SSlgth);

    if (is_unpaired_base_increment) {
	/* Ici, on est sur que le terminal devant X1 est libre */
	/* Ici, on est sur que le terminal apres X2 est non libre */
	Aij_struct_ptr = Aij2struct [local_pid] (rho [2]);
	rsi = Aij_struct_ptr->ub [0];
	lsi = Aij_struct_ptr->lb [0];
	T1 = LSOURCE (lsi);
	T2 = SOURCE (rsi);
	WC12 = Watson_Crick [T1] [T2];
	T3 = SOURCE (lsi);
	Benergy += In_Stacking [T3][WC12];
    }

    do {
	Xenergy = free_energy_hd [Xtop] + Benergy;

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    if (cur_top < 0) {
		cur_top = -cur_top;
	    }

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = -SSlgth;
	    right_hd [cur_top] = Xtop;
	    bm_hd [cur_top] = bm_hd [Xtop];
	}

#if EBUG
	debug_print ("left_bulge", rho, Xenergy, SSlgth, is_first, cur_top, Xtop, 0);
#endif

    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}


static SXBOOLEAN
right_bulge (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, SSlgth, cur_top, lsi, rsi, T1, T2, T4, WC21;
    float		Xenergy, Benergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;
    struct Aij_struct	*Aij_struct_ptr;

    /*
       STEM_BODY(X1 X2 SS)	--> !&StrLen(0, SS) &StrLen(7, X2) STEM(X1 X2) .	@right_bulge
    */
    Xtop = sem_disp [0] [rho [2]].ntgr; 
    SSlgth = Aij2struct [local_pid] (rho [1])->ub [0] - Aij2struct [local_pid] (rho [2])->ub [0];
    Benergy = Bulge_energy (SSlgth);

    if (is_unpaired_base_increment) {
	/* Ici, on est sur que le terminal devant X1 est non libre */
	/* Ici, on est sur que le terminal apres X2 est libre */
	Aij_struct_ptr = Aij2struct [local_pid] (rho [2]);
	rsi = Aij_struct_ptr->ub [0];
	lsi = Aij_struct_ptr->lb [0];
	T1 = LSOURCE (lsi);
	T2 = SOURCE (rsi);
	WC21 = Watson_Crick [T2] [T1];
	T4 = SOURCE (rsi+1);
	Benergy += Out_Stacking [T4][WC21];
    }

    do {
	Xenergy = free_energy_hd [Xtop] + Benergy;

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    if (cur_top < 0) {
		cur_top = -cur_top;
	    }

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = Xtop;
	    right_hd [cur_top] = -SSlgth;
	    bm_hd [cur_top] = bm_hd [Xtop];
	}

#if EBUG
	debug_print ("right_bulge", rho, Xenergy, SSlgth, is_first, cur_top, Xtop, 0);
#endif

    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}



static SXBOOLEAN
hairpin (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xlgth, cur_top;
    SXINT			nb = 0;
    float		Xenergy;
    struct Aij_struct	*Aij_struct_ptr;

    /*
       STEM_BODY(SS) --> . @hairpin
    */				     
    /* Unique pour un Aij donne' */
    /* On a T1 X T2 avec une paire de base entourant X */
    Aij_struct_ptr = Aij2struct [local_pid] (rho [1]);
    Xlgth = Aij_struct_ptr->ub [0]-Aij_struct_ptr->lb [0];
    Xenergy = Haipin_energy (Xlgth);

    if ((cur_top = where (&(sem_disp [0] [rho [1]].ntgr), Xenergy, is_first, &nb)) != 0) {
	if (cur_top < 0) {
	    cur_top = -cur_top;
	}

	free_energy_hd [cur_top] = Xenergy;
	left_hd [cur_top] = -Xlgth;
	right_hd [cur_top] = 0;
	bm_hd [cur_top] = bag_get (&source_bag, np1);
    }

#if EBUG
    debug_print ("hairpin", rho, Xenergy, Xlgth, is_first, cur_top, 0, 0);
#endif

    return cur_top > 0;
}




static float
internal_loop_penalty (BLlgth, SSlgth, rho)
    SXINT	BLlgth, SSlgth, *rho;
{
    static float f[6] = {0.0, 0.7, 0.6, 0.4, 0.2, 0.1};

    SXINT			half, M, ILlgth, lsi, rsi, WC1, WC2;
    float		penalty, increment;
    struct Aij_struct	*Aij_struct_ptr;
    SXBOOLEAN		GC_WC1, GC_WC2;


    half = BLlgth/2;
    ILlgth = BLlgth+SSlgth;

    if (SSlgth) {
	/* non_balanced internal loop */
	if ((M = half) > 5)
	    M = 5;

	if ((increment = SSlgth * f [M]) > 6.0)
	    increment = 6.0;

	penalty = increment+Internal_energy (ILlgth)-Internal_energy (BLlgth);
    }
    else
	penalty = 0.0;

    /* On va examiner les paires de bases englobant la boucle interne */
    Aij_struct_ptr = Aij2struct [local_pid] (rho [1]);
    lsi = Aij_struct_ptr->lb [0];
    rsi = Aij_struct_ptr->ub [0]+1;
    WC1 = Watson_Crick [SOURCE (lsi)][SOURCE (rsi)]; /* Les externes */
    Aij_struct_ptr = Aij2struct [local_pid] (rho [2]);
    lsi = Aij_struct_ptr->lb [0]+half+1;
    rsi = Aij_struct_ptr->ub [0]-half;
    WC2 = Watson_Crick [LSOURCE (lsi)][SOURCE (rsi)]; /* Les internes */

    GC_WC1 = WC1 == GC || WC1 == CG;
    GC_WC2 = WC2 == GC || WC2 == CG;

    if (!GC_WC1 && !GC_WC2)
	penalty += 1.8;
    else {
	if (!GC_WC1 && GC_WC2 || GC_WC1 && !GC_WC2)
	    penalty += 0.8;
    }

    return penalty;
}

static SXBOOLEAN
right_loop (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, BLlgth, SSlgth, cur_top;
    float		Xenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;
    /*
       LOOP(X1 X2 SS)		--> &StrLen(9, X1) BALANCED_LOOP(X1 X2) .		@right_loop
    */
    Xtop = sem_disp [0] [rho [2]].ntgr; 
    SSlgth = (Aij2struct [local_pid] (rho [1])->ub [0] - Aij2struct [local_pid] (rho [2])->ub [0]);

    do {
	BLlgth = -left_hd [Xtop]-deuxn;
	Xenergy = free_energy_hd [Xtop];

	if (is_internal_loop_penalty)
	    Xenergy += internal_loop_penalty (BLlgth, SSlgth, rho);

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    if (cur_top < 0) {
		cur_top = -cur_top;
	    }

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = Xtop;
	    right_hd [cur_top] = -SSlgth;
	    bm_hd [cur_top] = bm_hd [Xtop];
	}

#if EBUG
	debug_print ("right_loop", rho, Xenergy, BLlgth+SSlgth, is_first, cur_top, Xtop, 0);
#endif

    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}



static SXBOOLEAN
left_loop (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, BLlgth, SSlgth, cur_top;
    float		Xenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;

    /*
       LOOP(SS X1 X2)		--> !&StrLen(0, SS) &StrLen(9, X2)
				    BALANCED_LOOP(X1 X2) .				@left_loop
    */
    Xtop = sem_disp [0] [rho [2]].ntgr; 
    SSlgth = (Aij2struct [local_pid] (rho [2])->lb [0] - Aij2struct [local_pid] (rho [1])->lb [0]);

    do {
	BLlgth = -left_hd [Xtop]-deuxn;
	Xenergy = free_energy_hd [Xtop];

	if (is_internal_loop_penalty)
	    Xenergy += internal_loop_penalty (BLlgth, SSlgth, rho);

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    if (cur_top < 0) {
		cur_top = -cur_top;
	    }

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = -SSlgth;
	    right_hd [cur_top] = Xtop;
	    bm_hd [cur_top] = bm_hd [Xtop];
	}

#if EBUG
	debug_print ("left_loop", rho, Xenergy, BLlgth+SSlgth, is_first, cur_top, Xtop, 0);
#endif

    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}



static SXBOOLEAN
balanced_loop (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT 		Xtop, SSlgth, cur_top, lsi, rsi, T1, T2, T3, T4, WC12, WC21, iT4;
    float		Xenergy, Ienergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr = &(sem_disp [0] [rho [1]].ntgr);
    SXINT			nb = 0;
    struct Aij_struct	*Aij_struct_ptr;

    /*
       BALANCED_LOOP(SS1 X1 X2 SS2)	--> !&StrLen(0, SS1) &StrEqLen (SS1, SS2) &StrLen(7, X1)
				            STEM(X1 X2) .					@balanced_loop
    */
    Xtop = sem_disp [0] [rho [2]].ntgr; 
    Aij_struct_ptr = Aij2struct [local_pid] (rho [2]);
    lsi = Aij_struct_ptr->lb [0];
    SSlgth = 2*(lsi - Aij2struct [local_pid] (rho [1])->lb [0]);
    Ienergy = Internal_energy (SSlgth);

    if (is_unpaired_base_increment) {
	/* Ici, on est sur que le terminal devant X1 n'est pas dans une pair */
	/* Ici, on est sur que le terminal apres X1 n'est pas dans une pair */
	rsi = Aij_struct_ptr->ub [0];
	T1 = LSOURCE (lsi);
	T2 = SOURCE (rsi);
	WC12 = Watson_Crick [T1] [T2];
	T3 = SOURCE (lsi);
	T4 = SOURCE (rsi+1);
	WC21 = Watson_Crick [T2] [T1];
	Ienergy += In_Stacking [T3][WC12] + Out_Stacking [T4][WC21];
    }

    do {
	Xenergy = free_energy_hd [Xtop] + Ienergy;

	if ((cur_top = where (head_ptr, Xenergy, is_first && !ret_val, &nb)) != 0) {
	    if (cur_top < 0) {
		cur_top = -cur_top;
	    }

	    head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
	    ret_val = SXTRUE;
	    free_energy_hd [cur_top] = Xenergy;
	    left_hd [cur_top] = -deuxn-SSlgth;
	    right_hd [cur_top] = Xtop;
	    bm_hd [cur_top] = bm_hd [Xtop];
	}

#if EBUG
	debug_print ("balanced_loop", rho, Xenergy, SSlgth, is_first, cur_top, Xtop, 0);
#endif

    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}




static SXBOOLEAN
stem_clover (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT			cur_top, Xtop, Ytop, nb, lsi, rsi, T1, T2, T4, iT4, WC21;
    float		Xenergy, Yenergy, OSenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr;
    struct Aij_struct	*Aij_struct_ptr;
    SXBA		ss;
    /*
       STEM_CLOVER(X1 X2 Y1 Y2)	--> &StrLen(7, X1) &StrLen(7, Y2)
				    STEM(X1 X2) CLOVER(Y1 Y2) .				@stem_clover
    */
    Xtop = sem_disp [0] [rho [2]].ntgr;

    if (is_unpaired_base_increment) {
	/* Ici, on est sur que le terminal devant X1 est non libre */
	Aij_struct_ptr = Aij2struct [local_pid] (rho [2]);
	rsi = Aij_struct_ptr->ub [0];
	/* On calcule l'energie, mais elle ne sera utilisee que conditionnellement */
	lsi = Aij_struct_ptr->lb [0];
	T1 = LSOURCE (lsi);
	T2 = SOURCE (rsi);
	iT4 = rsi+1;
	T4 = SOURCE (iT4);
	WC21 = Watson_Crick [T2] [T1];
	OSenergy = Out_Stacking [T4][WC21];
    }

    do {
	head_ptr = &(sem_disp [0] [rho [1]].ntgr); /* On repart d'ici a chaque X */
	Xenergy = free_energy_hd [Xtop];
	nb = 0;
	Ytop = sem_disp [0] [rho [3]].ntgr;

	do {
	    if ((Yenergy = free_energy_hd [Ytop]) == MAXFLOAT)
		Yenergy = Xenergy;
	    else
		Yenergy += Xenergy;

	    if (is_unpaired_base_increment) {
		ss = bm_hd [Ytop];

		if (!SXBA_bit_is_set (ss, iT4)) {
		    /* T4 est libre */
		    Yenergy += OSenergy;
		}
	    }

	    if ((cur_top = where (head_ptr, Yenergy, is_first && !ret_val, &nb)) != 0) {
		if (cur_top < 0) {
		    cur_top = -cur_top;
		    /* On peut reutiliser le sxba precedant */
		    ss = bm_hd [cur_top];
		}
		else
		    ss = bm_hd [cur_top] = bag_get (&source_bag, np1);

		sxba_copy (ss, bm_hd [Xtop]);
		sxba_or (ss, bm_hd [Ytop]);

		head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
		ret_val = SXTRUE;
		free_energy_hd [cur_top] = Yenergy;
		left_hd [cur_top] = Xtop;
		right_hd [cur_top] = Ytop;
	    }

#if EBUG
	    debug_print ("stem_clover", rho, Yenergy, 0, is_first, cur_top, Xtop, Ytop);
#endif
    
	} while (nb < best_tree_number && (Ytop = ((next_hd == NULL) ? 0 : next_hd [Ytop])));
    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}


static SXBOOLEAN
ss_stem_clover (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    SXBOOLEAN	is_first;
{
    SXINT			cur_top, Xtop, Ytop, nb, lsi, rsi, T1, T2, T3, T4, iT4, WC12, WC21;
    float		Xenergy, Yenergy, ISenergy, OSenergy;
    SXBOOLEAN		ret_val = SXFALSE;
    SXINT			*head_ptr;
    struct Aij_struct	*Aij_struct_ptr;
    SXBA		ss;
    /*
       SS_STEM_CLOVER(X1 X2 Y1 Y2)	--> &StrLen(7, X1) &StrLen(7, Y2)
				            STEM(X1 X2) CLOVER(Y1 Y2) .				@ss_stem_clover
    */
    Xtop = sem_disp [0] [rho [2]].ntgr;

    if (is_unpaired_base_increment) {
	/* Ici, on est sur que le terminal devant X1 est libre */
	Aij_struct_ptr = Aij2struct [local_pid] (rho [2]);
	rsi = Aij_struct_ptr->ub [0];
	lsi = Aij_struct_ptr->lb [0];
	T1 = LSOURCE (lsi);
	T2 = SOURCE (rsi);
	WC12 = Watson_Crick [T1] [T2];
	T3 = SOURCE (lsi);
	ISenergy = In_Stacking [T3][WC12];
	/* On calcule l'energie, mais elle ne sera utilisee que conditionnellement */
	iT4 = rsi+1;
	T4 = SOURCE (iT4);
	WC21 = Watson_Crick [T2] [T1];
	OSenergy = Out_Stacking [T4][WC21];
    }

    do {
	head_ptr = &(sem_disp [0] [rho [1]].ntgr); /* On repart d'ici a chaque X */
	Xenergy = free_energy_hd [Xtop];
	nb = 0;
	Ytop = sem_disp [0] [rho [3]].ntgr;

	do {
	    if ((Yenergy = free_energy_hd [Ytop]) == MAXFLOAT)
		Yenergy = Xenergy;
	    else
		Yenergy += Xenergy;

	    if (is_unpaired_base_increment) {
		Yenergy += ISenergy;
		ss = bm_hd [Ytop];

		if (!SXBA_bit_is_set (ss, iT4)) {
		    /* T4 est libre */
		    Yenergy += OSenergy;
		}
	    }

	    if ((cur_top = where (head_ptr, Yenergy, is_first && !ret_val, &nb)) != 0) {
		if (cur_top < 0) {
		    cur_top = -cur_top;
		    /* On peut reutiliser le sxba precedant */
		    ss = bm_hd [cur_top];
		}
		else
		    ss = bm_hd [cur_top] = bag_get (&source_bag, np1);

		sxba_copy (ss, bm_hd [Xtop]);
		sxba_or (ss, bm_hd [Ytop]);

		head_ptr = (next_hd == NULL) ? NULL : &(next_hd [cur_top]);
		ret_val = SXTRUE;
		free_energy_hd [cur_top] = Yenergy;
		left_hd [cur_top] = Xtop;
		right_hd [cur_top] = Ytop;
	    }

#if EBUG
	    debug_print ("ss_stem_clover", rho, Yenergy, 0, is_first, cur_top, Xtop, Ytop);
#endif
    
	} while (nb < best_tree_number && (Ytop = ((next_hd == NULL) ? 0 : next_hd [Ytop])));
    } while (nb < best_tree_number && (Xtop = ((next_hd == NULL) ? 0 : next_hd [Xtop])));

    return ret_val;
}








