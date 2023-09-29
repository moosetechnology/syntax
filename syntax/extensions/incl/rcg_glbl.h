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
#ifndef rcg_glbl_h
#define rcg_glbl_h

/* rcg_glbl.h */


#include "rcg_sglbl.h"
#include "X.h"
#include "XxY.h"
#include "XxYxZ.h"
#include "XH.h"
#include "sxword.h"

/* Variables globales des analyseurs RCG. */


/*

   Signification des options de compilation -D
   EBUG_ALLOC		Permet des verifications sur l'allocateur de memoires
   EBUG4		Verifie la coherence de l'analyse (sxtrap).
   EBUG3		Imprime les clauses instanciees
   EBUG2		Permet de connaitre qq quantites (nb d'appels de predicats, de clauses, ...)
                        Attention, ds le cas rav2, necessite le depliage de toutes les parties droites
			pour calculer le nombre exact de clauses instanciees (c,a peut prendre du temps)
   EBUG			Imprime le deroulement d'une analyse
   PARSER		Execution en analyseur (et non reconnaisseur)
   RECOGNIZER		Execution en reconnaisseur (et non en analyseur)
   GENERATOR		Utilisation en generation ANY=t doit etre defini.  
			Implique SEMANTICS (a cause des appels negatifs, il faut connaitre tout l'ensemble de chaines
			qui marche pour pouvoir prendre son complementaire).
			Implique SINGLE_PASS mais interdit LEX.
   SEMANTICS		Active les appels a la semantique (*semact.first_pass)(...) etc
                        En general, il est raisonnable d'avoir aussi MEMO
   FIRST_AND_LAST	Active les tests de First et last
   MEMON		Active le processus de memoisation des predicats instancies. Deux instantiations identiques ne sont
                        executees qu'une fois.
   MEMOP		Suppression lev 7/6/2002
   CHECK_CYCLE		Sort des warnings sur la decouverte dynamique de cycles => CYCLIC
   CYCLIC		Permet de produire une foret partagee complete, meme cyclique.
                        Normalement, avec cette option, toutes les clauses instanciees sont sorties une fois et
			une seule mais l'ordre peut ne pas etre correct.  Il se peut qu'un predicat en partie droite soit
			utilise sans etre encore defini.  
			Cela etant, certaines semantiques n'ont pas besoin d'ordre correct (exemple
			les guiding parsers).  Si on veut un ordre correct, il faut forcer MULTIPLE_PASS.
			ATTENTION.  Si une grammaire est cyclique et si l'option CYCLIC n'est pas positionee, il est
			possible que des clauses instanciees ne soit pas produites.
   SINGLE_PASS		L'analyse syntaxique est effectuee en une seule passe.  Des sous-arbres complets peuvent
                        donc ne pas etre connectes a l'axiome  
			N'a aucun effet si SEMANTICS n'est pas positionne
   MULTIPLE_PASS	La foret est stockee par la 1ere passe et la semantique executee
			au cours de la seconde.  Dans ce cas on est sur que chaque sous-arbre traite fait
			partie d'un arbre correct.  Implique SEMANTICS
   PARSER && !MEMO	Les sous arbres communs (et la semantique) sont reexecutes
   LEX			Traitement de la lexicalisation.
                        Remplit is_Lex
                        Permet, pour chaque clause, de tester en temps unitaire si les terminaux presents
                        dans les args de cette clause sont reellement presents dans le source
                        Active l'execution des A-clauses sous forme de switch
   LEX3			Autre version du traitement de la lexicalisation, implique LEX
                        Reduit, par un algo de point fixe dynamique, l'ensemble des clauses filtrees par la lexicalisation
   DYNAM_LEX		Complementaire de LEX, permet, avant de lancer l'execution d'une clause lexicalisee
                        de tester que les ancres lexicales sont dans les ranges (et dans le bon ordre)
			Si GUIDED, utilise les numeros des clauses du guide pour decider de l'execution d'une clause
   RCVR			Autorise le lancement du traitement d'erreur		
   MAX_DIST		Distance maximale entre la chaine source et la chaine corrigee
   ANY=t		Permet de faire de la generation (partielle).  Par exemple le texte source "a_bb_c", si le code
                        de \_ est t, permet de generer toutes les phrases telles que "aabbcc".
			Si -DGENERATOR, un token du source dont le code est t est distingue' et sa comparaison avec un
			token qcq de la grammaire est toujours valide ; FIRST_AND_LAST est valide.
			Sinon, aucun texte source ne doit comporter de token de code t.
   AG			Le source est un DAG, en chantier!!
   SDAG			Sequencial DAG. Le source est un DAG particulier tq chaque etat i n'a qu'un seul successeur (au plus)
                        qui est i+1 mais le nb d'etiquettes (terminaux) de la transition entre i et i+1 peut
			etre multiple. A FAIRE, traiter le cas GENERATOR
   PARSE_FOREST		Sort sur stdout la foret partagee.  Implique SEMANTICS
   GUIDED		Analyse RCG "guidee" ou "couplee" par la "parse_forest" d'une analyse 1-rcg.  Utilise l'ensemble
                        des clauses du guide. => LEX (A VOIR => LEX3 ?)
   GUIDED2		Les predicats instancies du guide sont utilises a l'appel. => GUIDED
   GUIDED3		Les predicats instancies du guide sont utilises a la definition. => GUIDED
   GUIDED4		Les clauses instanciees du guide sont utilisees. => GUIDED
   FGUIDED		Fully guided/coupled Utilise tous les renseignements du guide (en particulier les appels de
                        predicats) pour diriger l'analyse.  Implique GUIDED
   COUPLED		Utilisee ds RCG_parser pour fabriquer un load module en 2 phases. La premiere construit le guide
                        qui est memorise en interne et utilise directement par la 2eme phase.
   BCG			Build Coupled Guide.  Indique au rcgp qu'il est en train de construire un guide couple'.  Implique
                        que PARSE_FOREST est non defini.  Implique SEMANTICS
   LARGE		Doit etre positionne si n<<logn<<lognt ne tient pas sur un mot.
   ROBUST               Permet une analyse robuste. Implique SINGLE_PASS
*/

#ifdef ROBUST
#ifdef MULTIPLE_PASS
#undef MULTIPLE_PASS
#endif
#ifndef SINGLE_PASS
#define SINGLE_PASS
#endif
#endif

#ifdef BCG
#ifndef SEMANTICS
#define SEMANTICS
#endif
#ifdef PARSE_FOREST
#undef PARSE_FOREST
#endif
#endif

#ifdef PARSE_FOREST
#ifndef SEMANTICS
#define SEMANTICS
#endif
#endif

#ifdef FGUIDED
#ifndef GUIDED
#define GUIDED
#endif
#endif

#ifdef GUIDED4
#ifndef GUIDED
#define GUIDED
#endif
#endif

#ifdef GUIDED3
#ifndef GUIDED
#define GUIDED
#endif
#endif

#ifdef GUIDED2
#ifndef GUIDED
#define GUIDED
#endif
#endif

#ifdef GUIDED
#ifndef LEX
#define LEX
#endif
#endif

#ifdef LEX3
#ifndef LEX
#define LEX
#endif
#endif

#ifdef AG
#ifdef SDAG
#undef SDAG
#endif
#ifndef GENERATOR
#define GENERATOR
#endif
#endif

#ifdef SDAG
#ifdef GENERATOR
#undef GENERATOR
#endif
#ifdef AG
#undef AG
#endif
#endif

#ifdef GENERATOR
#ifndef SINGLE_PASS
#define SINGLE_PASS
#endif
#ifndef SEMANTICS
#define SEMANTICS
#endif
#endif

#ifdef CHECK_CYCLE
#ifndef CYCLIC
#define CYCLIC
#endif
#endif

#if 0
#ifdef MEMOP
#ifndef MEMON
#define MEMON
#endif
#endif
#endif /* 0 */

#ifdef PARSER
#ifdef RECOGNIZER
#undef RECOGNIZER
#endif
#else
#ifndef RECOGNIZER
#define RECOGNIZER
#endif
#endif


#ifdef RECOGNIZER
#ifdef PARSER
#undef PARSER
#endif
#else
#ifndef PARSER
#define PARSER
#endif
#endif

#ifdef MULTIPLE_PASS
#ifdef SINGLE_PASS
#undef SINGLE_PASS
#endif
#ifndef SEMANTICS
#define SEMANTICS
#endif
#else
#ifndef SINGLE_PASS
#define SINGLE_PASS
#endif
#endif

#ifdef SINGLE_PASS
#ifdef MULTIPLE_PASS
#undef MULTIPLE_PASS
#endif
#else
#ifndef MULTIPLE_PASS
#define MULTIPLE_PASS
#endif
#endif


#ifdef PARSER
#define is_parser		1
#define is_recognizer		0
#else
#define is_recognizer		1	
#define is_parser		0
#endif


#ifdef SEMANTICS
#define is_semantics		1
#else
#define is_semantics		0
#endif


#ifdef FIRST_AND_LAST
#define is_first_last		1
#else
#define is_first_last		0
#endif

#ifdef SINGLE_PASS
#define is_single_pass		1
#else
#define is_single_pass		0
#endif

#ifdef MULTIPLE_PASS
#define is_multiple_pass	1
#else
#define is_multiple_pass	0
#endif

#ifdef RCVR
#define is_rcvr			1
#else
#define is_rcvr			0
#endif

#ifdef SDAG
#define is_sdag			1
#else
#define is_sdag			0
#endif

#ifdef GENERATOR
#define is_generator		1
#else
#define is_generator		0
#endif

#ifdef CHECK_CYCLE
#define is_check_cycle		1
#else
#define is_check_cycle		0
#endif

#ifdef CYCLIC
#define is_cyclic		1
#else
#define is_cyclic		0
#endif

#if 0
#ifdef MEMOP
#define is_memop		1
#else
#define is_memop		0
#endif
#endif /* 0 */

#ifdef MEMON
#define is_memon		1
#else
#define is_memon		0
#endif

#ifdef FGUIDED
#define is_fully_guided		1
#else
#define is_fully_guided		0
#endif

#ifdef GUIDED4
#define is_guided4		1
#else
#define is_guided4		0
#endif

#ifdef GUIDED3
#define is_guided3		1
#else
#define is_guided3		0
#endif

#ifdef GUIDED2
#define is_guided2		1
#else
#define is_guided2		0
#endif

#ifdef GUIDED
#define is_guided		1
#else
#define is_guided		0
#endif

#ifdef LEX3
#define is_lex3			1
#else
#define is_lex3			0
#endif

#if 0
#ifdef LEX
#define is_lex			1
#else
#define is_lex			0
#endif
#endif /* 0 */

#if is_lexicalization
#define is_lex			1
#else
#define is_lex			0
#endif

#ifdef DYNAM_LEX
#define is_dynam_lex		1
#else
#define is_dynam_lex		0
#endif

#ifdef PARSE_FOREST
#define is_parse_forest		1
#else
#define is_parse_forest		0
#endif

#ifdef LARGE
#define is_large	1
#else
#define is_large	0
#endif

#ifdef BCG
#define is_build_coupled_guide	1
#else
#define is_build_coupled_guide	0
#endif

#ifndef is_coupled
#ifdef COUPLED
#define is_coupled		1
#else
#define is_coupled		0
#endif
#endif /* is_coupled */

#ifdef ROBUST
#define is_robust		1
#else
#define is_robust		0
#endif


#ifdef SX_DFN_EXT_VAR_RCG
/* Compilation de RCG_parser */
SX_GLOBAL_VAR_RCG XxY_header	        XxY_lhs_guide [PID];
SX_GLOBAL_VAR_RCG struct spf_node	**XxY_lhs_guide2rhs [PID];
#else /* SX_DFN_EXT_VAR_RCG */
/* Compilation des modules */
SX_GLOBAL_VAR_RCG XxY_header	        XxY_lhs_guide [];
SX_GLOBAL_VAR_RCG struct spf_node	**XxY_lhs_guide2rhs [];
#endif /* SX_DFN_EXT_VAR_RCG */

#define GET_FULL_GUIDE(q,p,r,c)	((range=XxY_is_set (&(XxY_lhs_guide [p]), r, c)) ? (q=XxY_lhs_guide2rhs [p][range], SXTRUE) : SXFALSE)

/* typedef unsigned short	ushort; */
typedef void 		(*vfun)();
typedef SXBOOLEAN	(*bfun)();
typedef SXINT		(*ifun)();

struct fun_calls {
    vfun	first_pass_init,
                second_pass_init,
		first_pass_final,
		second_pass_final,
                first_pass_loop,
                second_pass_loop,
		oflw,
                nl_Ai_processing;

    ifun	first_pass,
                second_pass,
		last_pass;

    bfun        lexicalizer;

    SXBOOLEAN   (*dynamic_lexicalizer)(SXINT, SXBA);
};

SX_GLOBAL_VAR_RCG SXBOOLEAN		is_second_pass, is_robust_run;
SX_GLOBAL_VAR_RCG SXINT			rcvr_n; /* repere l'index ds global_source du eof du (vrai) source */

SX_GLOBAL_VAR_RCG SXUINT                nt_calls, clause_calls, memo_prdct_hit, memo_clause_hit;
SX_GLOBAL_VAR_RCG SXUINT		iarg_searched_in_guide, iarg_found_in_guide, iprdct_searched_in_guide, iprdct_found_in_guide,
                        iclause_searched_in_guide, iclause_found_in_guide;

SX_GLOBAL_VAR_RCG SXINT			global_pid, global_arity, global_max_arg_nb_per_clause;
SX_GLOBAL_VAR_RCG SXINT			logn, ANDj;
SX_GLOBAL_VAR_RCG SXINT			min_max, local_val;
SX_GLOBAL_VAR_RCG SXINT			call_level; /* #if EBUG niveau d'appel des predicats */
SX_GLOBAL_VAR_RCG SXINT			counter_lb; /* Borne inf des "compteurs" */
SX_GLOBAL_VAR_RCG SXBOOLEAN             input_is_a_dag;


struct Aij_struct {
    char	*Astr;
    SXINT	A, arity;
    SXINT	*lb, *ub;
};

typedef struct Aij_struct*	(*pfun)();
/* extern pfun			Aij2struct []; */


#if EBUG
#define statistics	stdout
#else
#define statistics	NULL
#endif


#define	REPLACE		1
#define	INSERTION	2
#define	DELETION	3
#define	EXCHANGE	4

/* poids de chaque correction */
#define	REPLACE_WGHT	1
#define	INSERTION_WGHT	2
#define	DELETION_WGHT	2
#define	EXCHANGE_WGHT	3

#if 0
#if is_sdag
SXBA	*glbl_source, rcur_tok, lcur_tok;
SXBA	*glbl_out_source; 
#if MEASURES
SXBA            unknown_word_set;
#endif /* MEASURES */
#else /* is_sdag */
SXINT	*glbl_source, rcur_tok, lcur_tok;
SXINT	*glbl_out_source; /* A FAIRE */
#endif /* is_sdag */
#endif /* 0 */




#if AG || is_generator
#define ARG_SEP		(0x80000000)
#define ARG_SEP_EQ	(0x80000001)
#endif

#if 0
#if AG
#include "XxYxZ.h"
#include "bag.h"

struct idag {
    XxYxZ_header	idag_hd;
    SXBA		*delta, *delta_1;
} glbl_idag;
#else /* AG */
#if is_sdag
#define SOURCE(i)	source [i]
#define LSOURCE(i)	lsource [i]
#if is_generator
#define SCAN(i,t)	(SXBA_bit_is_set(SOURCE(i),t)||SXBA_bit_is_set(SOURCE(i),ANY))
#define LSCAN(i,t)	(SXBA_bit_is_set(LSOURCE(i),t)||SXBA_bit_is_set(LSOURCE(i),ANY))
#else /* is_generator */
#define SCAN(i,t)	(SXBA_bit_is_set(SOURCE(i),t))
#define LSCAN(i,t)	(SXBA_bit_is_set(LSOURCE(i),t))
#endif /* is_generator */
#else /* is_sdag */
#define SOURCE(i)	source [i]
#define LSOURCE(i)	lsource [i]
#if is_generator
#define SCAN(i,t)	(SOURCE(i)==t||SOURCE(i)==ANY)
#define LSCAN(i,t)	(LSOURCE(i)==t||LSOURCE(i)==ANY)
#else /* is_generator */
#define SCAN(i,t)	(SOURCE(i)==t)
#define LSCAN(i,t)	(LSOURCE(i)==t)
#endif /* is_generator */
#endif /* is_sdag */
#endif /* AG */

/* Le 21/03/08 */
#define SOURCE(i)	source [i]
#define LSOURCE(i)	lsource [i]
#define SCAN(i,t)	(SXBA_bit_is_set(SOURCE(i),t))
#define LSCAN(i,t)	(SXBA_bit_is_set(LSOURCE(i),t))
#endif /* 0 */

#if 0
/* Le 17/04/08 */
#define LRSCAN(i,t)	(XxY_is_set (&QxT_hd, i, t))
#define RLSCAN(i,t)	(XxY_is_set (&TxQ_hd, t, i))


#if is_dag
/* Il n'y a pas d'erase dans Q_TxQ_hd => simplification de l'expansion du XxY_Xforeach */
/* Boucle qui pour chaque p et pt connus affecte successivement a` q l'ensemble des etats t.q.  p ->t q */
/* Attention si imbrication de telles boucles, seul pt peut representer la meme variable */
/* Si !input_is_a_dag, le source est en fait une chaine, et les Q_TxQ_hd n'ont pas ete alloues */
#define PTforeach(pt,p,q,qmax,x) for (x=Q_TxQ_hd.lnk_hd[XYZ_X][X_is_set(Q_TxQ_hd.X_hd[XYZ_X],pt)];\
                                      x>0 && (q=XxY_Y(Q_TxQ_hd, x));\
                                      x=Q_TxQ_hd.lnk[XYZ_X][x]\
                                     )\
                                    if (SXBA_bit_is_set (idag.path [q], qmax))

/* Boucle qui pour chaque q et tq connus affecte successivement a` p l'ensemble des etats t.q.  p ->t q */
#define TQforeach(tq,q,p,pmin,x) for (x=QxT_Q_hd.lnk_hd[XYZ_Y][X_is_set(QxT_Q_hd.X_hd[XYZ_Y],tq)];\
                                      x>0 && (p=XxY_Y(QxT_Q_hd, x));\
                                      x=QxT_Q_hd.lnk[XYZ_Y][x]\
                                     )\
                                    if (SXBA_bit_is_set (idag.path [pmin], p))
#endif /* is_dag */
#endif /* 0 */


#if is_dag
#define GET_QSTACK(p,t)       (*(idag.pt2q_disp [p]+t))
#define GET_PSTACK(t,q)       (*(idag.tq2p_disp [q]+t))
#endif /* is_dag */

#define	STREQ_ic	-1
#define	STREQLEN_ic	-2
#define	STRLEN_ic	-3
#define	FALSE_ic	-4
#define	FIRST_ic	-5
#define	LAST_ic		-6
#define	TRUE_ic		-7
#define	LEX_ic		-8
/* ATTENTION, CUT_ic doit avoir le +petit code des predefinis !! */
#define	CUT_ic		-9


/* Si on touche a G, penser a changer CURRENT_RCG_PARSER_RELEASE ds rcg_sglbl.h */
struct G {
  SXINT         release;
  char		*name;
  SXINT		clause_nb, arity, rhs_args_nb, tmax, ntmax, lognt, memoN_size, pid, degree, max_clause_id;
  SXINT		guide_clause_nb, guide_lognt, guide_max_clause_id, pass_no;
#if 0
  char		**t2str;
  sxword_header *t_names;
#endif /* 0 */
  char		**nt2str;
  SXINT		*clause2son_nb, **clause2son_pid;
  SXINT		*nt2arity;
  SXINT		*clause2lhst_disp, *lhst_list;
  SXINT		*clause2lhs, *clause2rhs;
  SXINT		*rhs2prdcts_disp, *rhs2prdcts;
  SXINT		*prdct2args_disp, *prdct2args;
  SXINT		*args2symbs_disp, *args2symbs;
  SXINT		*Xoccur_disp, *Xoccur;
  SXINT		*guide_clause2clause/*, *guide_clause2arg_pos, *guide_clause2clause_id*/;
  SXINT		*clause2identical_disp, *clause2identical;
  SXBA		invalid_lex_loop_clause /* loop_lex */, *nt2clause_id_set, *prdct2t_set;
  /* SXINT		*lex_code, *nt2lex_code; */
  SXBA		*clause2rhs_nt_set;
  SXINT		*clause2lhs_nt/*, *clause2Aclause*/, *clause2clause_id;
  SXBA_ELT      *A2max_clause_id;
  SXBA          main_clause_set;
  SXBA          *clause2lbs, *clause2ubs, *nt2lbs, *nt2ubs;
  SXBA		*t2clause_set;
  SXINT         *global_t2local_t, *t2clause, *t2clause_stack;
  SXBA		*Lex;
  SXBA		*pmemoN, *pmemoP;
  SXINT         *local_Aij2global_Aij_pid;
  X_header	*prange_1, *prhoA_hd;
  XxY_header	*prange_2;
  XxYxZ_header	*prange_3;
  XH_header	*prange_n;
  SXBA		*gpmemoN, *gpmemoP;
  X_header	*gprange_1, *gprhoA_hd;
  SXINT		*from2var2clause, *from2var2clause_disp, *from2var2clause_list, *clause2rhs_stack_map, *rhs_stack_map;
  SXBA          loop_clause_set;
  SXINT		*looped_nt, *looped_clause;
  struct fun_calls	parse, semact;
  pfun		Aij2struct;
  SXINT         *clause2nl_stmt_stack; /* index ds nl_stmt_stack */
  SXINT         *nl_stmt_stack; /* Contient des piles d'instructions utilisees pour la construction de sous-dags si le source est un DAG */
  SXBA          *Ai2inside_t_set;
  SXBA          non_right_linear_clause_set;
  SXBA          nl_Ai_set;
  SXINT         *A2A0;
#if 0
  /* Actually unused */
  SXINT         *nl_clause2rhs_pos_disp; /* A chaque clause fait correspondre un index ds nl_rhs_pos2pos_stack_disp ... */
  SXINT         *nl_rhs_pos2pos_stack_disp; /* ... qui a chaque position des args de la RHS fait correspondre ds nl_pos_stack un index ... */
  SXINT         *nl_pos_stack; /* ... qui repere une stack contenant toutes les positions des args (LHS et RHS) qui le contienne (en temps que chaine de var) */
#endif /* 0 */
  struct G	**Gs; /* Pour acceder aux autres modules */
  SXBOOLEAN	is_combinatorial; /* pour la verif dynamique de la coherence des modules / combinatorial */
  SXBOOLEAN	is_linear; /* pour traiter les textes source sous forme de DAG */
  SXBOOLEAN	is_overlapping; /* pour traiter les textes source sous forme de DAG */
  SXBOOLEAN	robust; /* pour savoir si l'analyseur RCG a ete compile avec l'option -DROBUST */
  SXBOOLEAN	is_factorize; /* pour savoir si le constructeur rcg a ete appele avec les options -2var_form et -factorize */
  SXBOOLEAN	is_GET_QSTACK; /* Le parseur a-t-il besoin de GSTACKs ? */
  SXBOOLEAN	is_GET_PSTACK; /* Le parseur a-t-il besoin de PSTACKs ? */
  SXBOOLEAN     is_a_complete_terminal_grammar; /* Les clauses sont toutes de la forme A(t) --> . */
};

SX_GLOBAL_VAR_RCG struct G *Gs [];



#define MAKE_A_RANGE(i,j)		(((i)<<idag.logn)+(j))

#define RANGE2i(ij)	((ij)>>logn)
#define RANGE2j(ij)	((ij)&ANDj)

SX_GLOBAL_VAR_RCG SXBOOLEAN    arity_eq_1_is_used, arity_eq_2_is_used, arity_eq_3_is_used, arity_geq_4_is_used;

SX_GLOBAL_VAR_RCG XH_header    arity_gte_4;
SX_GLOBAL_VAR_RCG XxYxZ_header arity_eq_3;
SX_GLOBAL_VAR_RCG XxY_header   arity_eq_2;
SX_GLOBAL_VAR_RCG X_header     arity_eq_1;

SX_GLOBAL_VAR_RCG SXINT        basic_lexicalization_non_instantiated_valid_clause_nb;


/* #define GET_SEM_NODE(p,r)	(struct spf_node*)sem_disp[p][r].ptr
typedef union {
    double	dbl;
    float	smpl;
    SXINT		ntgr;
    char	*ptr;
} SEM_TYPE;

typedef struct spf_node* SEM_TYPE;

#define GET_SEM_NODE_PTR(p,r)	(struct spf_node**)&(sem_disp[p][r])

typedef struct {
    struct spf_node *bot, *top;
} SEM_TYPE;

typedef struct {
    struct spf_node	*node_ptr;
    SXINT			cycle_kind;
} SEM_TYPE;

#define GET_SEM_NODE_PTR(p,r)	(sem_disp[p][r].bot)
#define GET_SEM_CYCLE_KIND(p,r)		(sem_disp[p][r].cycle_kind)
*/

#define CYCLE	1
#define LOOP	2


#if 0
struct which_guide {
    SXINT	pid, arity, *tcrho, *tcgclause;
    struct {
	SXINT lhs;
	struct spf_node	*next;
    } array [1];
};
#endif

/* FIRST_LAST est aussi utilise par &First et &Last */
#if is_sdag
extern SXBOOLEAN is_AND();
#if is_generator
#define FIRST_LAST(tok,fl)	(!SXBA_bit_is_set(tok,ANY) && !is_AND(tok,fl))
#else /* is_generator */
#define FIRST_LAST(tok,fl)	(!is_AND(tok,fl))
#endif /* is_generator */
#else /* !is_sdag */
#if is_generator
#define FIRST_LAST(tok,fl)	(tok != ANY && !SXBA_bit_is_set(fl,tok))
#else /* is_generator */
#define FIRST_LAST(tok,fl)	(!SXBA_bit_is_set(fl,tok))
#endif /* is_generator */
#endif /* !is_sdag */


/* A FAIRE : supprimer coupled_clause */

#if is_guided
#   if is_large
#   if is_fully_guided
#   define coupled_prdct(k,nt)		((tcrho[k]=X_is_set(ptcrhoA_hd,(X_is_set(ptcrange_1,MAKE_ij(ilb[k],iub[k]))<<G.guide_lognt)+nt))>0)
#   else /* !is_fully_guided */
#      if is_guided4
#      define coupled_clause(k,cl)	(SXBA_bit_is_set(ptcmemoP[tcrho[k]],cl))
#         if is_guided2
#         define coupled_prdct(k,nt)	((tc2rho[k]=X_is_set(ptcrhoA_hd,(X_is_set(ptcrange_1,MAKEj_ij(olb[k],oub[k]))<<G.guide_lognt)+nt))>0 &&\
					 TCMEMON1(k))
#         else /* ! is_guided2 */
#            if is_guided3
#            define coupled_prdct(k,nt)	((tcrho[k]=X_is_set(ptcrhoA_hd,(X_is_set(ptcrange_1,MAKE_ij(ilb[k],iub[k]))<<G.guide_lognt)+nt))>0 &&\
					 TCMEMON1(k))
#            else /* !is_guided3 */
#            define coupled_prdct(k,nt)	((tcrho[k]=X_is_set(ptcrhoA_hd,(X_is_set(ptcrange_1,MAKE_ij(ilb[k],iub[k]))<<G.guide_lognt)+nt))>0)
#            endif /* is_guided3 */
#         endif /* is_guided2 */
#      else /* !is_guided4 */
#      define coupled_clause(k,cl)	(SXTRUE)
#         if is_guided2
#         define coupled_prdct(k,nt)	((tc2rho[k]=X_is_set(ptcrhoA_hd,(X_is_set(ptcrange_1,MAKE_ij(olb[k],oub[k]))<<G.guide_lognt)+nt))>0 &&\
					 TCMEMON1(k))
#         else /* ! is_guided2 */
#            if is_guided3
#            define coupled_prdct(k,nt)	((tcrho[k]=X_is_set(ptcrhoA_hd,(X_is_set(ptcrange_1,MAKE_ij(ilb[k],iub[k]))<<G.guide_lognt)+nt))>0 &&\
					 TCMEMON1(k))
#            else /* !is_guided3 */
#            define coupled_prdct(k,nt)	(SXTRUE)
#            endif /* is_guided3 */
#         endif /* is_guided2 */
#      endif /* is_guided4 */
#   endif /* is_fully_guided */
#   else /* ! is_large */
#   if is_fully_guided
#   define coupled_prdct(k,nt)		((tcrho[k]=X_is_set(ptcrhoA_hd,(MAKE_ij(ilb[k],iub[k])<<G.guide_lognt)+nt))>0)
#   else /* !is_fully_guided */
#      if is_guided4
#      define coupled_clause(k,cl)	(SXBA_bit_is_set(ptcmemoP[tcrho[k]],cl))
#         if is_guided2
#         define coupled_prdct(k,nt)	((tc2rho[k]=X_is_set(ptcrhoA_hd,(MAKE_ij(olb[k],oub[k])<<G.guide_lognt)+nt))>0 &&\
					 TCMEMON1(k))
#         else /* ! is_guided2 */
#            if is_guided3
#            define coupled_prdct(k,nt)	((tcrho[k]=X_is_set(ptcrhoA_hd,(MAKE_ij(ilb[k],iub[k])<<G.guide_lognt)+nt))>0 &&\
					 TCMEMON1(k))
#            else /* !is_guided3 */
#            define coupled_prdct(k,nt)	((tcrho[k]=X_is_set(ptcrhoA_hd,(MAKE_ij(ilb[k],iub[k])<<G.guide_lognt)+nt))>0)
#            endif /* is_guided3 */
#         endif /* is_guided2 */
#      else /* !is_guided4 */
#      define coupled_clause(k,cl)	(SXTRUE)
#         if is_guided2
#         define coupled_prdct(k,nt)	((tc2rho[k]=X_is_set(ptcrhoA_hd,(MAKE_ij(olb[k],oub[k])<<G.guide_lognt)+nt))>0 &&\
					 TCMEMON1(k))
#         else /* ! is_guided2 */
#            if is_guided3
#            define coupled_prdct(k,nt)	((tcrho[k]=X_is_set(ptcrhoA_hd,(MAKE_ij(ilb[k],iub[k])<<G.guide_lognt)+nt))>0 &&\
					 TCMEMON1(k))
#            else /* !is_guided3 */
#            define coupled_prdct(k,nt)	(SXTRUE)
#            endif /* is_guided3 */
#         endif /* is_guided2 */
#      endif /* is_guided4 */
#   endif /* is_fully_guided */
#   endif /* is_large */
#endif /* is_guided */



SX_GLOBAL_VAR_RCG SXBA	           *out_ff2clause_set [];
SX_GLOBAL_VAR_RCG SXUINT           instantiated_clause_nb, dynam_lex_call_nb, dynam_lex_false_call_nb;
SX_GLOBAL_VAR_RCG SXUINT           total_clause_nb, total_clause1_nb, total_clause2_nb;

#if is_earley_guide
/* Ds le cas d'un guide earley, l'acces a tcmemoN1 est inutile (attention il n'est pas alloue) */
#define TCMEMON1(k) SXTRUE
#else /* !is_earley_guide */
#define TCMEMON1(k) (SXBA_bit_is_set(tcmemoN1,tc2rho[k]))
#endif /* !is_earley_guide */

SX_GLOBAL_VAR_RCG void          process_loop (struct G *G, SXINT *rho, ifun semact_action, SXINT cycle_kind);
#if 0
SX_GLOBAL_VAR_RCG void          sem_mem_mngr (struct G *G, SXINT old_size, SXINT new_size);
#endif /* 0 */
SX_GLOBAL_VAR_RCG void	        rcg_rcvr_mess (SXINT kind, SXINT mark, SXINT index1, SXINT index2);
SX_GLOBAL_VAR_RCG SXBOOLEAN	prdct_guide (void);
SX_GLOBAL_VAR_RCG SXINT		*StrConcat (SXBOOLEAN is_first_arg, SXINT *arg, SXINT *Xlb, SXINT *Xub);
SX_GLOBAL_VAR_RCG void		tree_nb (struct G *G), store_elem_tree (struct G *G), parse_forest (struct G *G), build_coupled_guide (struct G *G);
// void		fill_buf (void);
SX_GLOBAL_VAR_RCG void          arg_nb_put_error (char *prdct_name, SXINT def_nb, SXINT decl_nb);
SX_GLOBAL_VAR_RCG void		call_put_error (/* NE pas mettre void */);
SX_GLOBAL_VAR_RCG void		reduce_sdag (void);
#if 0
SX_GLOBAL_VAR_RCG SXINT         rcg_first_bu_dag_pass_processing (struct G *G, SXINT *rho);
SX_GLOBAL_VAR_RCG void          rcg_more_spf_alloc (void);
#endif /* 0 */
SX_GLOBAL_VAR_RCG struct Aij_struct *call_instantiated_prdct2struct (SXINT Aij_pid);
SX_GLOBAL_VAR_RCG SXINT         t_clause_processing (struct G *G, SXINT prdct_id, SXINT *rho0, SXINT p, SXINT q);
#if is_lexicalization
SX_GLOBAL_VAR_RCG void          terminal_lexicalization (struct G *G, SXBA memo_prdct_set);
#endif /* is_lexicalization */

#include "udag_scanner.h"

#include "sxrcg_spf.h"

#endif /*#ifndef rcg_glbl_h */
