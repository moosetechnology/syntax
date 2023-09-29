#include "bnf_vars.h"

int	max_attr, nb_definitions, nb_sem_rules, nb_occurrences;
BOOLEAN		is_err;

/* options de yax */

BOOLEAN		is_sem_stats;
int	tbl_lgth;
struct sxtoken	*terminal_token;
int	*nt_to_ste /* [1:ntmax] */ ;
int	*t_to_ste /* [1:-tmax] */ ;
int	*attr_to_ste /* [-NBTERM:tbl_lgth] */ ;
char	**attr_nt /* [1:tbl_lgth] char [ntmax] */ ;
int	*ste_to_tnt /* [1:lst] */ ;
int	*ste_to_attr /* [1:lst] */ ;


