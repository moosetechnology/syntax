/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

#ifndef dag_scanner_h
#define dag_scanner_h

extern SXINT  dag_scanner (SXINT what, SXTABLES *arg);
/* dag_scanner (SXACTION, NULL); */
/* dag_scanner (SXSEMPASS, NULL); */
/* dag_scanner (SXCLOSE, NULL); */

extern int  read_a_dag (void (*f)(SXBA, SXINT, struct sxtoken *, SXBA) /* store_dag */, 
			  void (*pre_fill)(void), 
			  void (*fill)(void), 
			  SXINT (*check)(void), 
			  void (*g)(void) );

extern SXINT  put_repair_tok (SXINT t, char *t_str, char *comment_str, struct sxsource_coord *psource_index);
extern void distribute_comment (SXINT t);
extern void fill_Tij2tok_no (SXINT Tpq, SXINT maxTpq);

#include "XxYxZ.h"

/* Le resultat du scanner est un DAG stocke' ds dag_hd */
XxYxZ_header dag_hd;
/* Il a un etat initial init_pos et un etat final final_pos */
SXINT init_pos, final_pos;
/* Nombre d'occurrences des terminaux du source (terminaux, pas formes-flechies) */
SXINT t_occur_nb;
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
SXINT last_tok_no;
/* C'est l'utilisateur du scanneur qui decide du type de foreach qu'il veut realiser sur le dag */ 
/* dag_foreach doit etre initialise par l'utilisateur (s'il veut des foreach) */
SXINT  dag_foreach [6];
/* tok_no est un index ds toks_buf vers le token associe a la forme flechie X */
struct sxtoken *toks_buf;
/* Le champ lahead du token est mis a la valeur retournee par la recherche ds le dico specifiee a
   la compilation de dag_scanner.c */
/* On a init_pos <= lpos < rpos <= final_pos */
/* Ajoute' le 24/02/06. Alloue' et rempli par distribute_comment (), permet des manips sur les commentaires
   qui ne peuvent pas etre faites ds le champ comment des toks_buf */
char **dag2comment;

/* C,a permet a l'utilisateur de specifier qq defauts si la forme-flechie n'est pas ds le dico */
extern char *ciu_name_ptr, *lciu_name_ptr, *digits_name_ptr;
#define CHECK_LOWER 1
#define CHECK_UPPER 2
extern SXINT default_set;

#define DAG 1
#define UDAG 2
extern SXINT dag_kind;

/* Pour permettre des modifs dynamiques du dag, et pour conserver la propriete si (lpos, tok_no, rpos)
   on a lpos < rpos (par exemple traitement des erreurs), on maintient 2 tableaux dag_state2mlstn et mlstn2dag_state
   tels que la propriete precedente s'applique sur dag_state2mlstn[lpos] < dag_state2mlstn[rpos].
   Ce sont donc les milestones qui sont strictement croissants.
*/

/* Suppression des correspondances le 04/10/06 */
#define dag_state2mlstn(x)   (x)
#define mlstn2dag_state(x)   (x)

SXBA *mlstn_path; /* mlstn_path [i] = {j | i ->x j, x \in T^+} */

#endif /* dag_scanner_h */
