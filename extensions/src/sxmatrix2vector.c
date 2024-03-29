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

#include "sxversion.h"
#include "sxunix.h"
#include "sxmatrix2vector.h"

char WHAT_SXMATRIX2VECTOR[] = "@(#)SYNTAX - $Id: sxmatrix2vector.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

/* Transforme une matrice creuse en un vecteur */
/* 2 lignes peuvent commencer au meme indice ds vecteur */

/* matrix[i, j]  <==> x = disp[i]+j, check[x]==i ? vector[x] : error_val */

static SXINT *state2trans_nb, *sorted_by_state;

static struct comb {
    SXINT		max_x, top, delta, size;
    SXINT		*vector, *check, *disp;
    SXBA	set;
} comb;

static SXINT max_vval, min_vval, max_check, min_check, max_mval, min_mval;

static bool	
state_by_lgth (SXINT i, SXINT j)
{
    return state2trans_nb [i] > state2trans_nb [j];
}

#define PUSH(s,x)	(s)[++*(s)]=(x)
#define POP(s)		(s)[(*(s))--]
#define IS_EMPTY(s)	(*(s)==0)
#define CLEAR(s)	*(s)=0
#define TOP(s)	        (*(s))

static SXINT *xstack;

static SXINT
put_in_comb (SXINT state, struct matrix *matrix)
{
  /* On cherche ds comb un emplacement vide pour inserer les transitions sortantes de state */
  /* On ne peut rien mettre ds vector [0] */
  SXINT 	xi, yi, start, x, top, i;
  SXBA	set;
  SXINT	*line;

  set = matrix->signature [state];
  xi = sxba_scan (set, -1);

  if (xi == -1) return -1;

  /* On cherche ds comb le 1er trou au dela de l'indice xi */
  CLEAR (xstack);
  x = xi;

  while ((x = sxba_scan (set, x)) > 0)
    PUSH (xstack, x);

  top = TOP (xstack);

  yi = xi;

  while ((yi = sxba_0_lrscan (comb.set, yi)) >= 0) {
    start = yi-xi;

    for (i = 1; i <= top; i++) {
      x = start+xstack [i];

      if (SXBA_bit_is_set (comb.set, x))
	  break;
    }

#if 0
    x = xi;

    while ((x = sxba_scan (set, x)) > 0 && !sxba_bit_is_set (comb.set, start+x));
#endif

    if (i > top) {
      /* C,a a marche' */
      line = matrix->matrix [state];
      x = start+xi;
      SXBA_1_bit (comb.set, x);
      comb.vector [x] = line [xi];
      comb.check [x] = state;

      while ((xi = sxba_scan (set, xi)) >= 0){
	x = start+xi;
	SXBA_1_bit (comb.set, x);
	i = comb.vector [x] = line [xi];

	if (i > max_vval)
	  max_vval = i;
	else {
	  if (i < min_vval)
	    min_vval = i;
	}

	comb.check [x] = state;

	if (state > max_check)
	  max_check = state;
	else {
	  if (state < min_check)
	    min_check = state;
	}
      }

      if (x > comb.max_x) {
	comb.max_x = x;

	if (comb.max_x + matrix->last_col + 1 >= comb.size) {
	  /* prudence pour le coup d'apres */
	  comb.size += comb.delta;
	  comb.set = sxba_resize (comb.set, comb.size+1);
	  comb.vector = (SXINT*) sxrealloc (comb.vector, comb.size+1, sizeof (SXINT));
	  comb.check = (SXINT*) sxrealloc (comb.check, comb.size+1, sizeof (SXINT));
	}
      }

      if ((x = start+matrix->last_col+1) > comb.top)
	comb.top = x;

      return start;
    }
  }

  sxtrap ("matrix2vector", "put_in_comb");
    /*NOTREACHED*/
  return -1;
}


void
vector_free (struct vector *vector)
{
  if (vector->disp)
    sxfree (vector->disp), vector->disp = NULL;

  if (vector->list)
    sxfree (vector->list), vector->pelem = vector->list = NULL;

  if (vector->matrix)
    sxfree (vector->matrix), vector->matrix = NULL;

  vector->last_line = vector->last_column = vector->last_index = vector->last_mindex = 0;
}

/* Nouvelle version, percent permet de trouver la frontiere des lignes a acces (matriciel) direct
   celles dont la densite est >percent, des lignes qui sont mises dans un vecteur.
   percent == 100 => tout ds vecteur
   percent == 0   => tout dans matrice
*/
void
matrix2vector (struct matrix *matrix, struct vector *vector, SXINT percent /* 0 <= percent <= 100 */)
{
  SXINT trans_nb, i, mval, x, frontier, card, vline_nb, mline_nb;
  SXINT *pcv, *pcc, *pd, *pcd, *line, *pm;
  struct velem *pelem;
  SXBA vline_set, mline_set, set;

  min_vval = min_check = min_mval = 1;

  vector->pelem = NULL;

  state2trans_nb = (SXINT*) sxalloc (matrix->last_line+1, sizeof (SXINT));
  vline_set = sxba_calloc (matrix->last_line+1);
  mline_set = sxba_calloc (matrix->last_line+1);
    
  comb.disp = (SXINT*) sxalloc (matrix->last_line+1, sizeof (SXINT));

  frontier = (percent*(matrix->last_col+1))/100;
  mline_nb = vline_nb = trans_nb = 0;

  for (i = 0; i <= matrix->last_line; i++) {
    card = sxba_cardinal (matrix->signature [i]);

    if (card <= frontier) {
      /* On mettra ds vector */
      if (card > 0) {
	SXBA_1_bit (vline_set, i);
	trans_nb += state2trans_nb [i] = card;
	vline_nb++;
      }
      else {
	comb.disp [i] = 0;
      }
    }
    else {
      mline_nb++;
      SXBA_1_bit (mline_set, i);
    }
  }

  if (vline_nb) {
    sorted_by_state = (SXINT*) sxalloc (vline_nb, sizeof (SXINT));

    i = -1;
    x = 0;

    while ((i = sxba_scan (vline_set, i)) >= 0)
      sorted_by_state [x++] = i;

    sort_by_tree (sorted_by_state, 0, vline_nb-1, state_by_lgth);

    comb.delta = (trans_nb*10)/100+matrix->last_col+1;
    comb.size = 2*trans_nb+comb.delta;

    comb.set = sxba_calloc (comb.size+1);
    comb.vector = (SXINT*) sxalloc (comb.size+1, sizeof (SXINT));
    comb.check = (SXINT*) sxalloc (comb.size+1, sizeof (SXINT)); 

    xstack = (SXINT*) sxalloc (matrix->last_col+1, sizeof (SXINT)); /* Modif du 17/4/2002 */

    for (i = 0; i < vline_nb; i++) {
      x = sorted_by_state [i];
      comb.disp [x] = put_in_comb (x, matrix);
    }

    i = -1;

    while ((i = sxba_0_lrscan (comb.set, i)) >= 0) {
      comb.vector [i] = M_ERROR;
      comb.check [i] = M_ERROR;
    }

    sxfree (xstack), xstack = NULL;
    sxfree (sorted_by_state);
    sxfree (comb.set), comb.set = NULL;

    pelem = vector->list = (struct velem*) sxalloc (comb.top+1, sizeof (struct velem));

    pcv = comb.vector;
    pcc = comb.check;

    for (i = 0; i <= comb.top; i++) {
      pelem->val = *pcv++;
      pelem->check = *pcc++;
      pelem++;
    }

    sxfree (comb.vector), comb.vector = NULL;
    sxfree (comb.check), comb.check = NULL;
  }
  else
    vector->list = NULL;

  sxfree (state2trans_nb), state2trans_nb = NULL;

  if (mline_nb > 0) {
    x = i = -1;

    while ((i = sxba_scan (mline_set, i)) >= 0) {
      comb.disp [i] = x;
      x -= (matrix->last_col+1);
    }

    x = -x;

    vector->matrix = (SXINT *) sxalloc (x, sizeof (SXINT));
    vector->last_mindex = x-1;
    vector->matrix [0] = M_ERROR;
    pm = vector->matrix+1;
    i = -1;

    while ((i = sxba_scan (mline_set, i)) >= 0) {
      set = matrix->signature [i];
      line = matrix->matrix [i];

      for (x = 0; x <= matrix->last_col; x++) {
	if (SXBA_bit_is_set (set, x)) {
	  mval = pm [x] = line [x];
	}
	else {
	  pm [x] = M_ERROR;
	  mval = 0;
	}

	if (mval > max_mval)
	  max_mval = mval;
	else {
	  if (mval < min_mval)
	    min_mval = mval;
	}
      }

      pm += matrix->last_col+1;
    }
  }
  else {
    vector->matrix = (SXINT *) NULL;
    vector->last_mindex = 0;
  }

  sxfree (vline_set);
  sxfree (mline_set);

  vector->last_line = matrix->last_line;
  vector->last_column = matrix->last_col;
  vector->last_index = comb.top;
  vector->max_vval = max_vval;
  vector->min_vval = min_vval;
  vector->max_check = max_check;
  vector->min_check = min_check;
  vector->max_mval = max_mval;
  vector->min_mval = min_mval;

  pd = vector->disp = (SXINT*) sxalloc (vector->last_line+1, sizeof (SXINT));
  pcd = comb.disp;

  for (i = 0; i <= vector->last_line; i++) {
    *pd++ = *pcd++;
  }

  sxfree (comb.disp), comb.disp = NULL;
  comb.max_x = comb.top = comb.delta = comb.size = 0;
}

static char*
val2type (SXINT min, SXINT max, char **error)
{
  SXUINT size;
  char *type;

  if (min < 0)
    size = max-min+1;
  else
    size = max+1;

  if (size < 0XFF) {
    if (min >= 0) {
      type = "unsigned char";

      if (min > 0)
	*error = "0X00";
      else
	*error = "0XFF";
    }
    else {
      type = "char";
      *error = "0X80";
    }
  }
  else {
    if (size < 0XFFFF) {
      if (min >= 0) {
	type = "unsigned short";

	if (min > 0)
	  *error = "0X0000";
	else
	  *error = "0XFFFF";
      }
      else {
	type = "short";
	*error = "0X8000";
      }
    }
    else {
      if (min >= 0) {
	type = "SXUINT";

	if (min > 0)
	  *error = "0X00000000";
	else
	  *error = "0XFFFFFFFF";
      }
      else {
	type = "SXINT";
	*error = "0X80000000";
      }
    }
  }

  return type;
}


void
vector2c (struct vector *vector,
	  FILE   *cfile,
	  char   *name,
	  bool is_static)
{
  SXINT i, min, max, val, check;
  char *vector_error, *check_error;
  char *vector_type, *check_type;

  /* Il faut que vector_error == matrix_error pour pouvoir tester le resultat de vector_get facilement */

  if (vector->min_vval < vector->min_mval)
    min = vector->min_vval;
  else
    min = vector->min_mval;

  if (vector->max_vval > vector->max_mval)
    max = vector->max_vval;
  else
    max = vector->max_mval;

  vector_type = val2type (min, max, &vector_error);
  check_type = val2type (vector->min_check, vector->max_check, &check_error);

  fprintf (cfile, "\n#define %s_ERROR %s\n", name, vector_error);

  fprintf (cfile, "\n\n\
struct %s_vector {\n\
  SXINT last_line, last_column, last_index, last_mindex;\n\
  SXINT *disp /* 0..last_line */;\n\
  struct %s_velem {\n\
    %s val;\n\
    %s check;\n\
  } *list /* 0..last_index */, *pelem;\n\
  %s *matrix; /* 0..last_mindex */\n\
};\n", name, name, vector_type, check_type, vector_type);

  fprintf (cfile, "\n\nstatic SXINT %s_disp [%ld] = {", name, (SXINT) vector->last_line + 1);

  for (i = 0; i <= vector->last_line; i++) {
    if ((i % 10) == 0)
      fprintf (cfile, "\n/* %ld */ ", (SXINT) i);

    fprintf (cfile, "%ld, ", (SXINT) vector->disp [i]);
  } 

  fprintf (cfile, "\n};\n");

  if (vector->list) {
    fprintf (cfile, "\n\nstatic struct %s_velem %s_list [%ld] = {", name, name, (SXINT) vector->last_index + 1);

    for (i = 0; i <= vector->last_index; i++) {
      if ((i % 10) == 0)
	fprintf (cfile, "\n/* %ld */ ", (SXINT) i);

      val = vector->list [i].val;

      if (val == (SXINT)M_ERROR)
	fprintf (cfile, "{%s, ", vector_error);
      else
	fprintf (cfile, "{%ld, ", val);

      check = vector->list [i].check;

      if (check == (SXINT)M_ERROR)
	fprintf (cfile, "%s}, ", check_error);
      else
	fprintf (cfile, "%ld}, ", (SXINT) check);
    } 

    fprintf (cfile, "\n};\n");
  }

  if (vector->matrix) {
    fprintf (cfile, "\n\nstatic %s %s_matrix [%ld] = {", vector_type, name, (SXINT) vector->last_mindex + 1);

    for (i = 0; i <= vector->last_mindex; i++) {
      if ((i % 10) == 0)
	fprintf (cfile, "\n/* %ld */ ", (SXINT) i);

      val = vector->matrix [i];

      if (val == (SXINT)M_ERROR)
	fprintf (cfile, "%s, ", vector_error);
      else
	fprintf (cfile, "%ld, ", (SXINT) val);
    } 

    fprintf (cfile, "\n};\n");
  }

  fprintf (cfile, "\n\n\
%sstruct %s_vector %s = {\n\
%ld, %ld, %ld, %ld,\n\
%s_disp,\n\
%s%s, NULL,\n\
%s%s\n\
};\n",
	   is_static ? "static " : "",
	   name, name,
	   (SXINT) vector->last_line, (SXINT) vector->last_column, (SXINT) vector->last_index, (SXINT) vector->last_mindex,
	   name,
	   vector->list ? name : "NULL", vector->list ? "_list" : "",
	   vector->matrix ? name : "NULL", vector->matrix ? "_matrix" : "");


  if (vector->list && vector->matrix) {
fprintf (cfile, "\n\n\
#define %s_vector_get(i,j,b) ((b > 0) ? ((%s.pelem = %s.list + (b) + (j))->check == (i) ? %s.pelem->val : %s_ERROR) : (%s.matrix[(j)-(b)]))\n",
	 name, name, name, name, name, name);
  }
  else {
    if (vector->list)
      fprintf (cfile, "\n\n\
#define %s_vector_get(i,j,b) ((%s.pelem = %s.list + (b) + (j))->check == (i) ? %s.pelem->val : %s_ERROR)\n",
	 name, name, name, name, name);
    else
      fprintf (cfile, "\n\n\
#define %s_vector_get(i,j,b) (%s.matrix[(j)-(b)])\n",
	 name, name);
  }
}
