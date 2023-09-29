#define slice_length	25
#define slice_length2	12

#include "S_tables.h"
#include "P_tables.h"
#include "R_tables.h"
#include "PP_tables.h"
#include "T_tables.h"

struct lecl_tables_s	S;
struct parser_ag_item	P;
struct R_tables_s	R;
struct PP_ag_item	PP;
struct T_ag_item	T;

#define PC P.constants
#define PR P.reductions
#define PT P.t_bases
#define PNT P.nt_bases
#define PV P.vector
#define PGO P.gotos
#define PPR P.prdcts
#define PGA P.germe_to_gr_act

#define SC S.S_tbl_size
#define SM S.S_transition_matrix
#define SP S.SXS_action_or_prdct_code
#define SK S.S_check_kw
#define SA S.SXS_adrp

#define RC R.R_tbl_size
#define SL R.SXS_local_mess
#define SG R.S_global_mess
#define PL R.SXP_local_mess
#define PG R.P_global_mess

#define PPC PP.PP_constants
#define PPS PP.SXPP_schema

#define TC T.T_constants
#define TN T.SXT_node_info

BOOLEAN	is_syno;
