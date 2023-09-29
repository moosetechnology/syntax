#ifndef lrfsa_h
#define lrfsa_h

/* lrfsa.h */

extern BOOLEAN          is_AND();

typedef void 		(*vfun)();
typedef BOOLEAN		(*bfun)();
typedef int		(*ifun)();

struct fsaG {
  int     prod_nb, nt_nb, init_state, pid, instantiated_prod_nb;
  SXBA    *t2prod_set;
  SXBA    Lex, fsa_valid_prod_set, *prod2lub;
  bfun    process_fsa;
  vfun    final_fsa;
  BOOLEAN shallow;
};

int                    lfsa_global_pid, rfsa_global_pid;

extern struct fsaG     *lfsaGs [], *rfsaGs [];

#define fsa_prod2lb(c,b) (G.prod2lbs==NULL || SXBA_bit_is_set(G.prod2lbs[c],b))
#define fsa_prod2ub(c,b) (G.prod2ubs==NULL || SXBA_bit_is_set(G.prod2ubs[c],b))
#define fsa_nt2lb(c,b) (G.nt2lbs==NULL || SXBA_bit_is_set(G.nt2lbs[c],b))
#define fsa_nt2ub(c,b) (G.nt2ubs==NULL || SXBA_bit_is_set(G.nt2ubs[c],b))


int                    n;

#if is_sdag
SXBA	               *glbl_source, *glbl_out_source; 
#else
int	               *glbl_source, *glbl_out_source; /* A FAIRE */
#endif

#endif /* #ifndef lrfsa_h */
