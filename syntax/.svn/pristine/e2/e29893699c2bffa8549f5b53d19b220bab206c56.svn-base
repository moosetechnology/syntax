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
/*   R_tables.h  */

#define rt_num_version 3

struct R_tbl_size_s {
   SXINT	S_nbcart,
	S_nmax,
	S_maxlregle,
	S_maxparam,
	S_lstring_mess,
	S_last_char_code;

   SXINT	P_nbcart,
	P_maxlregle,
	P_maxparam,
	P_lstring_mess,
	P_tmax,
	P_nmax,
	P_validation_length,
	P_followers_number,
	P_sizeofpts,
	P_max_prio_X,
	P_min_prio_0;

   SXINT	E_nb_kind,
	E_maxltitles,
	E_labstract;

  };


struct string_ref {
   SXINT	param,
	index,
	length;
   };


struct local_mess {
   SXINT 			param_no;
   struct string_ref	*string_ref /* param_no */ ;
   };


struct global_mess {
   SXINT	index,
	length;
   };


struct R_tables_s {

   time_t 			bnf_modif_time;

   struct R_tbl_size_s	R_tbl_size;

   SXINT			**S_lregle;
   struct local_mess	*SXS_local_mess;
   struct global_mess	*S_global_mess;
   char			*S_string_mess;
   SXBA		S_no_delete,
			S_no_insert;

   SXINT			**P_lregle,
			*P_right_ctxt_head;
   struct local_mess	*SXP_local_mess;
   struct global_mess	*P_global_mess;
   char			*P_string_mess;
   SXBA			P_no_delete,
			P_no_insert,
			PER_tset;

   char			**E_titles,
			*E_abstract;
   };

/*   end   R_tables.h  */
