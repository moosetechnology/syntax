/* Cet include implemente les @predicats utilises par distance pour le traitement des erreurs */
/* Est inclus par distance_rcgp.c */
/* marche en 2 passes (-DSINGLE_PASS=0) */
/* La premiere passe calcule et stocke l'arbre unique de poids minimal (comme store_elem_tree) */
/* distance_rcgp.c doit etre compile avec l'option -Dstore_elem_tree=_distance_first_pass_for_semact
   dont l'execution assure les links vers les bonnes fonctions de la 1ere passe */
/* La deuxieme passe (sur un arbre unique) sort les messages d'erreur */

/* Pour la seconde passe */
#define _distance_semact_init		sxvoid
#define _distance_semact_final		sxvoid
#define _distance_semact_last_pass	sxbvoid

#if 0
/* Action par defaut appelee si aucune @function n'est specifiee sur une clause */
#define sem_void			NULL
#endif

/* Actions semantiques externes appelees depuis distance_rcgp.c */

static struct Aij_struct 	*pAij;
static SXINT			source_index, object_index;
static SXINT			*lb, *ub;

/* ATTENTION, on sait que le nombre de fils d'un noeud de la RCG distance est au plus 1 */
/* On stocke le poids en rho [3] */
/* La structure ainsi fabriquee est utilisable par les "_walk" */
/* On sait que les trois args de &distance(L,X,Y) sont tels que X est un range du source
   et Y un range de la chaine corrigee */

struct dist_node {
    struct dist_node	*next;
    SXINT			rho [4];
};

struct dist_area {
    struct dist_area	*next;
    struct dist_node	spf [1];
};

static struct dist_area	*dist_area_hd;
static struct dist_node	*dist_top;
static SXINT		dist_room, dist_size;

static SXINT*
allocate_dist_area (char_size)
    SXINT	char_size;
{
    struct dist_area	*new_area;

    new_area = (struct dist_area*) sxalloc (1, sizeof(struct dist_area*)+char_size);
    new_area->next = dist_area_hd;
    dist_area_hd = new_area;
    dist_top = &(new_area->spf [0]);
    dist_room = char_size;
}

static void
free_dist_area ()
{
    struct dist_area	*area_ptr, *next_area_ptr;

    for (area_ptr = dist_area_hd; area_ptr != NULL; area_ptr = next_area_ptr) {
	next_area_ptr = area_ptr->next;
	area_ptr->next = NULL;
	sxfree (area_ptr);
    }

    dist_area_hd = NULL;
}

static void
_distance_first_pass_init (init_size)
    SXINT init_size;
{
    if (dist_area_hd == NULL) {
	allocate_dist_area (dist_size = init_size*sizeof(struct dist_node)); /* Pourquoi pas */
    }
}

static void
_distance_first_pass_final ()
{
    if (dist_area_hd)
	free_dist_area ();
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

static SXINT
get_weight (rho, son_nb, sons)
    SXINT 	*rho, son_nb, sons[];
{
    SXINT		val = 0, rhoi, son;

    if (*rho++ > 0) {
	/* Non cyclique */
	for (son = son_nb; son > 0; son--) {
	    if ((rhoi = rho [son]) > 0)
		/* Si rhoi < 0 => appel de &StrEq ("a", X) et -rhoi est l'indice de "a"
		   ds le source */
		val += ((struct dist_node*)sem_disp [sons [son]] [rhoi].ptr)->rho [3];
	}
    }

    return val;
}


static void
propagate_min (local_weight, rho, son_nb, sons, is_first)
    SXINT 	local_weight, *rho, son_nb, sons[];
    bool	is_first;
{
    SXINT		val, *p, i;
    
    /* calcul du poids courant */
    val = get_weight (rho, son_nb, sons)+local_weight;

    if (is_first) {
	/* On stocke */
	/* Y-a-t-il la place ? */
	if (sizeof (struct dist_node) > dist_room) {
	    allocate_dist_area (dist_size);
	}

	/* dist_bot est un ptr vers un dist_node de la bonne dimension */
	dist_room -= sizeof (struct dist_node);
#if EBUG
	if (dist_room < 0)
	    sxtrap ("distance_semact.h", "sem_void");
#endif

	dist_top->next = NULL;
	sem_disp [sons [0]] [rho [1]].ptr = (char*) dist_top;

	p = &(dist_top->rho [0]);
	p [3] = val+1;		/* truc */
	dist_top++;
    }
    else {
	/* On ecrase l'ancien si + grand */
	p = &(((struct dist_node*)sem_disp [sons [0]] [rho [1]].ptr)->rho [0]);
    }

    if (p [3] > val) {
	p [3] = val;

	for (i = -1; i <= son_nb; i++)
	    *p++ = *rho++;
    }
}


static bool
sem_void (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
{
    /* Rien n'est specifie sur la clause */
    /* On ne conserve que le min */
    SXINT		val, *p;

    if (!is_second_pass) {
	propagate_min (0, rho, son_nb, sons, is_first);
    }

    return true;
}


#if 0
/* Ancienne version de distance */
static bool
rcvr_change (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    bool	is_first;
{
    if (is_second_pass) {
	/* On recupere les infos de la LHS */
	pAij = (*Aij2struct [sons [0]])(rho [1]);
	lb = pAij->lb;
	source_index = lb [1]+1;

	if (source_index > rcvr_n) {
	    /* pointe ds la chaine corrige'e */
	    object_index = source_index;
	    source_index = lb [2]+1;
	}
	else {
	    object_index = lb [2]+1;
	}

	/* Les tokens source_index et object_index sont echanges */
	/* source_index pointe ds la chaine source */
	/* object_index pointe ds la chaine corrigee */
	   
	rcg_rcvr_mess (REPLACE, source_index, source_index, object_index);
    }
    else
	propagate_min (REPLACE_WGHT, rho, son_nb, sons, is_first);

    return true;
}


static bool
rcvr_del (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    bool	is_first;
{
    SXINT		local_weight;

    pAij = (*Aij2struct [sons [0]])(rho [1]);
    lb = pAij->lb;
    source_index = lb [1]+1;

    if (is_second_pass) {
	/* On recupere les infos de la LHS */
	if (source_index > rcvr_n) {
	    /* pointe ds la chaine corrigee */
	    source_index = lb [2]+1;
	    /* suppression de source */
	    rcg_rcvr_mess (DELETION, source_index, source_index, source_index);
	}
	else {
	    /* insertion de objet devant source */
	    object_index = lb [2]+1;
	    rcg_rcvr_mess (INSERTION, source_index, object_index, source_index);
	}
    }
    else {
	local_weight = (source_index > rcvr_n) ? DELETION_WGHT : INSERTION_WGHT;
	propagate_min (local_weight, rho, son_nb, sons, is_first);
    }

    return true;
}


static bool
rcvr_swap (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    bool	is_first;
{
    if (is_second_pass) {
	/* On recupere les infos de la LHS */
	pAij = (*Aij2struct [sons [0]])(rho [1]);
	lb = pAij->lb;
	source_index = lb [1]+1; /* pointe ds la chaine source */

	if (source_index > rcvr_n) {
	    /* pointe ds la chaine corrigee */
	    source_index = lb [2]+1;
	}

	/* source_index pointe ds la chaine source */
	rcg_rcvr_mess (EXCHANGE, source_index, source_index, source_index+1);
    }
    else
	propagate_min (EXCHANGE_WGHT, rho, son_nb, sons, is_first);

    return true;
}
#endif

#if is_multiple_pass
static bool _distance_semact ();
void
_distance_first_pass_for_semact (for_semact)
struct for_semact *for_semact;
{
for_semact->first_pass_init = _distance_first_pass_init;
for_semact->first_pass = _distance_semact;
for_semact->first_pass_final = _distance_first_pass_final;
}
#endif


static bool
rcvr_substitute (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    bool	is_first;
{
    /* Est appele sur
       &distance(L1 L2,X1 X2,Y1 Y2) --> &StrLen(1,L2) &StrLen(1,X2) &StrLen(1,Y2) !&StrEq(X2,Y2)
				    &distance(L1,X1,Y1) . @rcvr_substitute
    */
    if (is_second_pass) {
	/* On recupere les infos de la LHS */
	pAij = (*Aij2struct [sons [0]])(rho [1]);
	ub = pAij->ub;
	source_index = ub [1]; /* source_index pointe ds la chaine source */
	object_index = ub [2]; /* object_index pointe ds la chaine corrigee */
	rcg_rcvr_mess (REPLACE, source_index, source_index, object_index);
    }
    else
	propagate_min (REPLACE_WGHT, rho, son_nb, sons, is_first);

    return true;
}


static bool
rcvr_insert (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    bool	is_first;
{
    /* La correction est une insertion dans la chaine source */
    /* ATTENTION est appele sur :
       &distance(L,,Y)		--> &StrEqLen(L,Y) . @rcvr_insert
       &distance(L1 L2,X,Y1 Y2)	--> &StrLen(1,L2) &StrLen(1,Y2) &distance(L1,X,Y1) . @rcvr_insert
    */
    SXINT		local_weight;

    /* On recupere les infos de la LHS */
    pAij = (*Aij2struct [sons [0]])(rho [1]);
    lb = pAij->lb;
    ub = pAij->ub;
    source_index = ub [1]+1;

    if (is_second_pass) {
	if (lb [1] == ub [1]) {
	    object_index = lb [2]+1; /* insertion de objet devant source */

	    while (object_index <= ub [2]) {
		rcg_rcvr_mess (INSERTION, source_index, object_index, source_index);
		object_index++;
	    }
	}
	else {
	    object_index = ub [2]; /* insertion de objet devant source */
	    rcg_rcvr_mess (INSERTION, source_index, object_index, source_index);
	}

    }
    else {
	local_weight = (lb [1] == ub [1]) ? INSERTION_WGHT*(ub [2] - lb [2]) : INSERTION_WGHT;
	propagate_min (local_weight, rho, son_nb, sons, is_first);
    }

    return true;
}



static bool
rcvr_delete (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    bool	is_first;
{
    /* La correction est une suppression dans la chaine source */
    /* ATTENTION est appele sur :
       &distance(L,X,)		--> &StrEqLen(L,X) . @rcvr_delete
       &distance(L1 L2,X1 X2,Y)	--> &StrLen(1,L2) &StrLen(1,X2) &distance(L1,X1,Y) . @rcvr_delete
    */
    SXINT		local_weight;

    /* On recupere les infos de la LHS */
    pAij = (*Aij2struct [sons [0]])(rho [1]);
    lb = pAij->lb;
    ub = pAij->ub;
    source_index = ub [1];

    if (is_second_pass) {
	/* suppression de source */

	if (lb [2] == ub [2]) {
	    while (lb [1] < source_index) {
		rcg_rcvr_mess (DELETION, source_index, source_index, source_index);
		source_index--;
	    }
	}
	else
	    rcg_rcvr_mess (DELETION, source_index, source_index, source_index);
    }
    else {
	local_weight = (lb [2] == ub [2]) ? DELETION_WGHT*(ub [1] - lb [1]) : DELETION_WGHT;
	propagate_min (local_weight, rho, son_nb, sons, is_first);
    }

    return true;
}


static bool
rcvr_swap (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    bool	is_first;
{
    /* Est appele sur :
       &distance(L1 L2,X1 X2 X3,Y1 Y2 Y3)
				--> &StrLen(1,L2) &StrLen(1,X2) &StrLen(1,X3) &StrEq(X2,Y3)
				    &StrEq(X3,Y2) !&StrEq(X2,Y2) &distance(L1,X1,Y1) . @rcvr_swap
    */
    if (is_second_pass) {
	/* On recupere les infos de la LHS */
	pAij = (*Aij2struct [sons [0]])(rho [1]);
	ub = pAij->ub;
	source_index = ub [1]-1; /* source_index pointe ds la chaine source */
	rcg_rcvr_mess (EXCHANGE, source_index, source_index, source_index+1);
    }
    else
	propagate_min (EXCHANGE_WGHT, rho, son_nb, sons, is_first);

    return true;
}
