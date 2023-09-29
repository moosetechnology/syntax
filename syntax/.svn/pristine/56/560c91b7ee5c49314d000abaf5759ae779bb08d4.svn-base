/*------------------------------*/
/*    sxmatrix2vector.h        	*/
/*------------------------------*/

/* Codage par une structure lineaire de la matrice (creuse) M [0..last_line, 0..last_column] */

#define M_ERROR 0X80000000

extern void matrix2vector (/* struct matrix*, struct vector*, percent */);
/* 0 <= percent <= 100 taux de remplissage qui indique pour chaque ligne de matrix si on en fait un vecteur ou une matrice */
/* FALSE si c,a n'a pas marche' */
extern void vector2c (/* struct vector*, FILE*, char* (name), BOOLEAN (is_static) */);
/* Genere :
   static int name_disp [last_line+1] = {...}
   static struct v_elem name_list [last_index+1] = {..., {val, check}, ...};
   [[static] unsigned short name_matrix [last_mindex+1] = {..., val, ...};]
   [static] struct name_vector name = {
   last_line, last_column, last_index, last_mindex,
   name_disp, name_list, NULL,
   name_matrix
   };
*/
extern void vector_free (/* struct vector* */);

struct matrix {
  int last_line, last_col;
  int **matrix; /* 0..last_line, 0..last_col */
  /* Repere les elements valides */
  SXBA *signature; /* 0..last_line, 0..last_col */ 
};


/* struct vector ne sert qu'aux arg de matrix2vector, vector2c et vector_free.
   Pour un vector donne, la bonne struct est generee par vector2c
   La macro vector_get (instanciee en name_get) est aussi generee */

struct vector {
  int last_line, last_column, last_index, last_mindex;
  int max_vval, min_vval, max_check, min_check, max_mval, min_mval;
  int *disp /* 0..last_line */;
  struct velem {
    int val;
    int check;
  } *list /* 0..last_index */, *pelem;
  /* Si disp[i] < 0 on accede a */
  int *matrix; /* 0..last_mindex */
};

/* l'acces a l'element M[i, j] du vector name se fait comme suit :
   if ((b = vector_base (name, i)) == 0) => element errone
   sinon
   if ((val = name_vector_get (name, i, j, b)) == name_ERROR) => element errone
   sinon val est correct
*/
#define vector_base(v,i) ((v).disp[i])
/* la macro name_vector_get et name_ERROR sont generes par vector2c */

