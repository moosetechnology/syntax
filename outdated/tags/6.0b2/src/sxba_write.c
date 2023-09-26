/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1988 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *							  *
   ******************************************************** */





/* Ecriture de tableaux de bits pour SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 11:05 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 23-09-96 15:31 (pb):		Bug detecte sur l'alpha pour les 64 bits*/
/*				les "constantes" doivent etre typees	*/
/************************************************************************/
/* Aug 29 1996 14:21 (pb):	sxba2c et sxbm2c un peu + rapide que 	*/
/*				sxba_to_c et sxbm_to_c			*/
/************************************************************************/
/* Jul 25 1996 14:38 (pb):	sxba_to_c appelle sxba(32|64)_to_c	*/
/************************************************************************/
/* 22-05-96 11:48 (pb):		Nouvelle version de sxba_to_c		*/
/************************************************************************/
/* 15-02-94 18:19 (pb):		Ne depend plus de "sxunix.h"		*/
/************************************************************************/
/* 09-04-90 11:30 (phd):	Reecriture				*/
/************************************************************************/
/* 10-05-88 10:15 (pb):		Creation				*/
/************************************************************************/



#include "sxcommon.h"
#include "sxba.h"
#include "varstr.h"
#include <stdio.h>
#include <unistd.h>

char WHAT_SXBA_WRITE[] = "@(#)SYNTAX - $Id: sxba_write.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

BOOLEAN
sxba_write (int file, SXBA bits_array)
/*
 * "sxba_write" writes "bits_array" on "file".
 * "file" must be the result of "open" or "create".
 */

{
    int		bytes_number = NBLONGS (BASIZE (bits_array)) * sizeof (SXBA_ELT);

    sxbassert ((*(bits_array+NBLONGS (BASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "write");

    return
      (write (file, bits_array, sizeof (SXBA_ELT)) == sizeof (SXBA_ELT)) &&
      (write (file, bits_array + 1, (size_t) bytes_number) == bytes_number)
      ;
}


#if 0
/* ATTENTION : le code C produit par "sxba_to_c ()" ne peut etre reutilise' que
   sur des machines ou l'organisation des "long int" est la meme que la machine
   ayant servi a le produire. */
/* Dans l'implantation courante (si "SXBA_TARGET_SIZE_" est correctement
   positionne'), on ne peut produire du code que pour une autre machine
   dont le "long int" a une taille double de celui de la machine source. */
/* Sinon, produire une ft qui fera a la fois l'allocation dynamique et les initialisations
   et retournera le pointeur alloue' du style :

   SXBA sxba_reuse_%name ()
   {
      static int elems_%name [] = {e1, ..., en};
      SXBA set = sxba_calloc (%card);
      int i, elem;

      for (i = 0; i < n; i++) {
         elem = elems_%name [i];
         SXBA_1_bit (set, elem);
      }

      return set;
   }
   */

void sxba_to_c (set, file, name1, name2, is_static)
    SXBA 	set;
    FILE	*file;
    char	*name1, *name2;
    BOOLEAN	is_static;
{
    int	i, lgth, nbmot;

    lgth = BASIZE (set);

    fprintf (file, "\n\n%s SXBA_ELT %s%s [%i] = {%i /* size */,", is_static ? "static " : "",
	     name1, name2, NBLONGS (lgth) + 1, lgth);

    i = sxba_1_rlscan (set, lgth) + 1;
    nbmot = NBLONGS (i);

    if (_SXBA_TARGET_SIZE == 0 || _SXBA_TARGET_SIZE == SXBITS_PER_LONG)
    {
	for (i = 1; i <= nbmot; i++) {
	    if ((i & 3) == 1)
		fprintf (file, "\n /* %i */ ", i);

	    fprintf (file, "0X%X, ", set [i]);
	} 
    }
    else {
	/* On suppose SXBA_TARGET_SIZE_ == 2 * SXBITS_PER_LONG !!! */
	for (i = 1; i <= nbmot; i += 2) {
	    if ((i & 3) == 1)
		fprintf (file, "\n /* %i */ ", (i>>1) + 1);

	    if (i < nbmot)
		fprintf (file, "0X%X%X, ", set [i+1], set [i]);
	    else
		fprintf (file, "0X%X, ", set [i]);
	} 

    }

    fprintf (file, "\n} /* End %s%s */;\n", name1, name2);
}
#endif /* 0 */


void
sxba32_to_c (FILE *file, SXBA set)
{
    /* On genere pour une machine 32 bits */

    int	i, lgth, nbmot;

    lgth = BASIZE (set);
    nbmot = NBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "32_to_c");

    fprintf (file, "{%i, ", lgth);

#if SXBITS_PER_LONG == 32
    /* Execution sur une machine 32 */
    for (i = 1; i <= nbmot; i++) {
	if ((i & 7) == 7)
	    fprintf (file, "\n");

	fprintf (file, "0X%08lX, ", set [i]);
    } 
#else
    /* Execution sur une machine 64 */
    for (i = 1; i <= nbmot; i++) {
	if ((i & 3) == 3)
	    fprintf (file, "\n");

	fprintf (file, "0X%08lX, 0X%08lX, ", set [i]&((~(((SXBA_ELT)0)))>>32), set [i]>>32);
    } 
#endif

    fprintf (file, "}");
}

void
sxba64_to_c (FILE *file, SXBA set)
{
    /* On genere pour une machine 64 bits */
    int	i, lgth, nbmot;

    lgth = BASIZE (set);
    nbmot = NBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "64_to_c");

    fprintf (file, "{%i, ", lgth);

#if SXBITS_PER_LONG == 32
    /* Execution sur une machine 32 */
    for (i = 1; i <= nbmot; i += 2) {
	if ((i & 7) == 7)
	    fprintf (file, "\n");

	if (i < nbmot)
	    fprintf (file, "0X%08lX%08lX, ", set [i+1], set [i]);
	else
	    fprintf (file, "0X%016lX, ", set [i]);
    }
#else
    /* Execution sur une machine 64 */
    for (i = 1; i <= nbmot; i++) {
	if ((i & 3) == 3)
	    fprintf (file, "\n");

	fprintf (file, "0X%016lX, ", set [i]);
    }
#endif

    fprintf (file, "}");
}


void
sxba_to_c (SXBA set, FILE *file_descr, char *name1, char *name2, BOOLEAN is_static)
{
    int	lgth, nbmot;

    lgth = BASIZE (set);
    nbmot = NBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "to_c");

    fprintf (file_descr, "\n\
%s SXBA_ELT %s%s\n\
", is_static ? "static " : "",
	     name1, name2);

    /* On genere pour une machine 32 bits */
    fprintf (file_descr, "#if SXBITS_PER_LONG==32\n\
 [%i] = ",
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	     nbmot
#else /* On est sur une machine 64 bits */
	     2*nbmot
#endif
	     +1);

    sxba32_to_c (file_descr, set);

    /* On genere pour une machine 64 bits */
    fprintf (file_descr, "\n#else\n\
 [%i] = ",
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	     (nbmot+1)/2
#else /* On est sur une machine 64 bits */
	     nbmot
#endif
	     +1);

    sxba64_to_c (file_descr, set);

    fprintf (file_descr, "\n#endif\n\
/* End %s%s */;\n\
", name1, name2);
}


/* A mettre eventuellement ds sxba_write et sxbitsmatrix */

static char hexa [] = "0123456789ABCDEF";

static VARSTR
sxba32_to_vstr (SXBA set, VARSTR vstr)
{
    /* On genere pour une machine 32 bits */
    SXBA_ELT	mot;
    int	i, lgth, nbmot;
    char	*l, *r;
#if SXBITS_PER_LONG == 32
    /* Execution sur une machine 32 */
    static char str [] = "0X........, ";
#else
    /* Execution sur une machine 64 */
    static char str [] = "0X........, 0X........, ";
#endif

    lgth = BASIZE (set);
    nbmot = NBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "32_to_vstr");

    for (i = 1; i <= nbmot; i++) {
	mot = set [i];

#if SXBITS_PER_LONG == 32
	/* Execution sur une machine 32 */
	if ((i & 7) == 7)
#else
    /* Execution sur une machine 64 */
	if ((i & 3) == 3)
#endif

	    varstr_catchar (vstr, '\n');

	l = str + 2;
	r = l + 8;
	
	while (--r >= l) {
	    *r = hexa [mot & 0xF];
	    mot >>= 4;
	}

#if SXBITS_PER_LONG == 64
	l = str + 14;
	r = l + 8;
	
	while (--r >= l) {
	    *r = hexa [mot & 0xF];
	    mot >>= 4;
	}
#endif

#if SXBITS_PER_LONG == 32
	/* Execution sur une machine 32 */
	varstr_lcatenate (vstr, str, 12);
#else
	/* Execution sur une machine 64 */
	varstr_lcatenate (vstr, str, 24);
#endif
    } 

    return vstr;
}

static VARSTR
sxba64_to_vstr (SXBA set, VARSTR vstr)
{
    /* On genere pour une machine 64 bits */
    SXBA_ELT	mot;
    int	i, lgth, nbmot;
    char	*l, *r;
    static char str [] = "0X................, ";

    lgth = BASIZE (set);
    nbmot = NBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "64_to_vstr");

    for (i = 1; i <= nbmot; i++) {
	mot = set [i];

#if SXBITS_PER_LONG == 32
    /* Execution sur une machine 32 */
    	if ((i & 7) == 7)
	    varstr_catchar (vstr, '\n');

	l = str + 10;
	r = l + 8;
	
	while (--r >= l) {
	    *r = hexa [mot & 0xF];
	    mot >>= 4;
	}

	if (++i > nbmot)
	    mot = ((SXBA_ELT)0);
	else
	    mot = set [i];

	l = str + 2;
	r = l + 8;
#else
    /* Execution sur une machine 64 */
	if ((i & 3) == 3)
	    varstr_catchar (vstr, '\n');

	l = str + 2;
	r = l + 16;
#endif

	while (--r >= l) {
	    *r = hexa [mot & 0xF];
	    mot >>= 4;
	}

	varstr_lcatenate (vstr, str, 20);
    }

    return vstr;
}


/* C'est un peu moins mal que les autres versions ... */
/* mais ca reste d'une lenteur effrayante. */

void
sxba2c (SXBA set, FILE *file_descr, char *name1, char *name2, BOOLEAN is_static, VARSTR vstr)
{
    int	lgth, nbmot;

    lgth = BASIZE (set);
    nbmot = NBLONGS (lgth);

    sxbassert ((*(set+nbmot) & ((~((SXBA_ELT)0)) << 1 << MOD (lgth - 1))) == 0, "2c");

    fprintf (file_descr, "\n\
%s SXBA_ELT %s%s\n\
", is_static ? "static " : "",
	     name1, name2);

    /* On genere pour une machine 32 bits */
    fprintf (file_descr, "#if SXBITS_PER_LONG==32\n\
 [%i] = ",
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	     nbmot
#else /* On est sur une machine 64 bits */
	     2*nbmot
#endif
	     +1);

    varstr_raz (vstr);
    fprintf (file_descr, "{%i, %s}", lgth, varstr_tostr (sxba32_to_vstr (set, vstr)));

    /* On genere pour une machine 64 bits */
    fprintf (file_descr, "\n#else\n\
 [%i] = ",
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	     (nbmot+1)/2
#else /* On est sur une machine 64 bits */
	     nbmot
#endif
	     +1);

    varstr_raz (vstr);
    fprintf (file_descr, "{%i, %s}", lgth, varstr_tostr (sxba64_to_vstr (set, vstr)));

    fprintf (file_descr, "\n#endif\n\
/* End %s%s */;\n\
", name1, name2);
}


void
sxbm2c (FILE *file_descr, SXBA *bits_matrix, int line_nb, char *name1, char *name2, BOOLEAN is_static, VARSTR vstr)
{
    /* Genere un SXBA_ELT [] [] et non un sxbm */
    int	i, lgth, nbmot;

#if SXBITS_PER_LONG!=32 && SXBITS_PER_LONG!=64
    sxtrap ("sxba_write", "sxbm2c");
#endif

    lgth = BASIZE (bits_matrix [0]);
    nbmot = NBLONGS (lgth);
    
fprintf (file_descr, "\n\
%s SXBA_ELT %s%s [%i]\n\
", is_static ? "static " : "",
	     name1, name2, line_nb);

    /* On genere pour une machine 32 bits */
    fprintf (file_descr, "#if SXBITS_PER_LONG==32\n\
 [%i] = {\n\
",
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	 nbmot
#else /* On est sur une machine 64 bits */
	 2*nbmot
#endif
+1);

    for (i = 0; i < line_nb; i++) {
	varstr_raz (vstr);
	fprintf (file_descr, "/* %i */ {%i, %s},\n\
", i, lgth, varstr_tostr (sxba32_to_vstr (bits_matrix [i], vstr)));
    }

    /* On genere pour une machine 64 bits */
    fprintf (file_descr, "#else\n\
 [%i] = {\n\
",
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	 (nbmot+1)/2
#else /* On est sur une machine 64 bits */
	 nbmot
#endif
+1);

    for (i = 0; i < line_nb; i++) {
	varstr_raz (vstr);
	fprintf (file_descr, "/* %i */ {%i, %s},\n\
", i, lgth, varstr_tostr (sxba64_to_vstr (bits_matrix [i], vstr)));
    }

    fprintf (file_descr, "#endif\n\
} /* End %s%s */;\n\
", name1, name2);
}

