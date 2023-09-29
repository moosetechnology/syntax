#define pt_num_version 6

struct P_constants {
	int tmax;
	int xtmax;
	int nbpro;
	int xnbpro;
	int ntmax;
	int Mtstate;
    	int Mntstate;
    	int Mvec;
    	int mgerme;
    	int Mgerme;
    	int deltavec;
    	int init_state;
    	int final_state;
    	int mMprdct;
    	int mMred;
    	int mMrednt;
    	int Mprdct;
    	int Mfe;
    	int M0ref;
    	int Mref;
    	int nd_degree;
    	int Mprdct_list;
	int sem_kind;
   };


struct P_reduction {
    int		action,
                reduce,
                lgth,
                lhs;
    };

struct P_item {
    int		check,
                action;
    };

struct P_prdct {
    int		prdct,
                action;
    };

struct P_base {
    int		defaut,
                commun,
                propre;
    };

struct P_prdct_list {
    int		red_no,
                pos,
                prdct;
    };

struct parser_ag_item {
	long			bnf_modif_time;
	struct P_constants 	constants; 
	struct P_reduction 	*reductions 	/* 1:Mred */ ;
	struct P_base		*t_bases	/* 1:Mtstate */ ,
	                        *nt_bases	/* 1:Mntstate */ ;
	struct P_item	        *vector	   	/* deltavec+1:deltavec+Mvec */ ;
	struct P_prdct          *prdcts         /* Mred+1:Mprdct */ ;
	int			*germe_to_gr_act/* mgerme+1:Mgerme */;
	struct P_prdct_list	*prdct_list	/* 1:Mprdct_list */;
   };
