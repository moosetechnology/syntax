#ifndef dag_scanner_h
#define dag_scanner_h


/* Le DAG est lu ds le fichier source_file de nom source_file_name */
FILE *source_file;
char *source_file_name;

extern int dag_scanner (/* what, arg */);
/* dag_scanner (ACTION, NULL); */
/* dag_scanner (SEMPASS, NULL); */
/* dag_scanner (CLOSE, NULL); */

extern int read_a_dag (/* store_dag */);

#include "XxYxZ.h"

/* Le resultat du scanner est un DAG stocke' ds dag_hd */
XxYxZ_header dag_hd;
/* Il a un etat initial init_pos et un etat final final_pos */
int init_pos, final_pos;
/* C'est un ensemble de triplets (lpos, tok_no, rpos)
   Il y a une transition sur tok_no depuis l'etat lpos vers l'etat rpos.  
   tok_no est l'indice du token dans l'expression reguliere source.  Ex pour 
   (a b | c [d]) e
    1 2   3  4   5
    last_tok_no est l'indice du dernier token (ici 5) 
    Pour les pos on a
     (a b | c [d]) e
    1  2     3    4 5
    init_pos = 1, final_pos = 5
    cad : (1, "a", 2), (2, "b", 4), (1, "c", 3), (1, "c", 4), (3, "d", 4), (4, "e", 5)
*/
int last_tok_no;
/* C'est l'utilisateur du scanneur qui decide du type de foreach qu'il veut realiser sur le dag */ 
/* dag_foreach doit etre initialise par l'utilisateur (s'il veut des foreach) */
int  dag_foreach [6];
/* tok_no est un index ds toks_buf vers le token associe a la forme flechie X */
struct sxtoken *toks_buf;
/* Le champ lahead du token est mis a la valeur retournee par la recherche ds le dico specifiee a
   la compilation de dag_scanner.c */

/* On a init_pos <= lpos < rpos <= final_pos */

/* C,a permet a l'utilisateur de specifier qq defauts si la forme-flechie n'est pas ds le dico */
char *ciu_name_ptr, *lciu_name_ptr, *digits_name_ptr;
#define CHECK_LOWER 1
#define CHECK_UPPER 2
int default_set;

/* Pour permettre des modifs dynamiques du dag, et pour conserver la propriete si (lpos, tok_no, rpos)
   on a lpos < rpos (par exemple traitement des erreurs), on maintient 2 tableaux dag_state2mlstn et mlstn2dag_state
   tels que la propriete precedente s'applique sur dag_state2mlstn[lpos] < dag_state2mlstn[rpos].
   Ce sont donc les milestones qui sont strictement croissants.
*/

int init_mlstn, final_mlstn;
int *dag_state2mlstn, *mlstn2dag_state;
struct sxsource_coord *dag_state2source_index;
SXBA *mlstn_path; /* mlstn_path [i] = {j | i ->x j, x \in T^+} */

#endif
