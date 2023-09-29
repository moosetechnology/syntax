#define sem_void			NULL

#define _count_semact_init		sxvoid
#define _count_semact_final		sxvoid
#define _count_semact_last_pass		sxbvoid

static struct Aij_struct 	*pAij;
static int			*lb, *ub;
static int			i, p, q, D[4];

static BOOLEAN
PrintQ2InBaseP (rho, son_nb, sons, is_first)
    int 	*rho, son_nb, sons[];
    BOOLEAN	is_first;
{
    /* Result(R3,R2,R1,R0,P)	-->  . @PrintQ2InBaseP */

    /* On recupere les infos de la LHS */
    pAij = (*Aij2struct [sons [0]])(rho [1]);
    lb = pAij->lb;
    ub = pAij->ub;

    for (i = 0; i <= 3; i++) {
	D[i] = ub[i]-lb[i];
    }

    p = ub[4]-lb[4];
    q = n-p;

    fprintf (stdout, "\nWe get %i*%i = %i = (%i %i %i %i) in base %i\n", q, q, q*q, D[0], D[1], D[2], D[3], p);

    return TRUE;
}  
