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
/*------------------------------*/
/*    sxmatrix2vector.h        	*/
/*------------------------------*/

/* Codage par une structure lineaire de la matrice (creuse) M [0..last_line, 0..last_column] */

#define M_ERROR 0X80000000

/* struct vector ne sert qu'aux arg de matrix2vector, vector2c et vector_free.
   Pour un vector donne, la bonne struct est generee par vector2c
   La macro vector_get (instanciee en name_get) est aussi generee */

struct vector {
  SXINT last_line, last_column, last_index, last_mindex;
  SXINT max_vval, min_vval, max_check, min_check, max_mval, min_mval;
  SXINT *disp /* 0..last_line */;
  struct velem {
    SXINT val;
    SXINT check;
  } *list /* 0..last_index */, *pelem;
  /* Si disp[i] < 0 on accede a */
  SXINT *matrix; /* 0..last_mindex */
};

struct matrix {
  SXINT last_line, last_col;
  SXINT **matrix; /* 0..last_line, 0..last_col */
  /* Repere les elements valides */
  SXBA *signature; /* 0..last_line, 0..last_col */ 
};

/* l'acces a l'element M[i, j] du vector name se fait comme suit :
   if ((b = vector_base (name, i)) == 0) => element errone
   sinon
   if ((val = name_vector_get (name, i, j, b)) == name_ERROR) => element errone
   sinon val est correct
*/
#define vector_base(v,i) ((v).disp[i])
/* la macro name_vector_get et name_ERROR sont generes par vector2c */

extern void matrix2vector (struct matrix*, struct vector*, SXINT);
/* 0 <= percent <= 100 taux de remplissage qui indique pour chaque ligne de matrix si on en fait un vecteur ou une matrice */
/* false si c,a n'a pas marche' */
extern void vector2c (struct vector*, FILE*, char*, bool);
/* Genere :
   static SXINT name_disp [last_line+1] = {...}
   static struct v_elem name_list [last_index+1] = {..., {val, check}, ...};
   [[static] unsigned short name_matrix [last_mindex+1] = {..., val, ...};]
   [static] struct name_vector name = {
   last_line, last_column, last_index, last_mindex,
   name_disp, name_list, NULL,
   name_matrix
   };
*/
extern void vector_free (struct vector*);

