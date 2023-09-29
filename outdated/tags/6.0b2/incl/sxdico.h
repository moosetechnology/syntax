/*------------------------------*/
/*          sxdico.h          	*/
/*------------------------------*/

#ifndef sxdico_h
#define sxdico_h

extern int  sxdico_get         (/* struct dico *dico, unsigned char **kw, int *kwl */);
extern int  sxdico_get_bounded (/* struct dico *dico, unsigned char **kw, int *kwl, int kind_bound */);
extern void sxdico_process     (/* struct mot2 input, struct dico *output */);
extern void sxdico2c           (/* struct dico	*dico;
			       FILE		*file;
			       char             *dico_name;
			       BOOLEAN          is_static; */);
struct dico {
    unsigned int 	max, init_base, class_mask, stop_mask, is_suffix_mask, base_shift, partiel;
    unsigned char	*char2class;
    unsigned int	*comb_vector;
};

#define TOTAL        0
#define PARTIEL      1

#define TIME         0
#define SPACE        1
#define PREFIX_BOUND 2
#define SUFFIX_BOUND 3


struct mot2 {
    int 	nb, size, optim_kind, process_kind, total_lgth, print_on_sxtty;
    char 	**string;
    int 	*lgth, *code, *min, *max;
};

#endif /* #ifndef sxdico_h */




