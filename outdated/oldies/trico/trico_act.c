/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2000 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20000905 18:14 (phd):	Adaptation a MINGW			*/
/************************************************************************/
/* Dimanche 21 mai 2000 (phd) :	Creation				*/
/************************************************************************/

char WHAT_TRICO_ACT[] = "@(#)trico_act.c\t- SYNTAX [unix] - Dimanche 21 mai 2000";

#include "sxunix.h"
#include <time.h>


struct trico {
    unsigned int 	max, init_base, class_mask, stop_mask, is_suffix_mask, base_shift, partiel;
    unsigned char	*char2class;
    unsigned int	*comb_vector;
};

#define TOTAL 0
#define PARTIEL 1

#define TIME 0
#define SPACE 1


struct mots {
    int		*ste;
    int 	nombre, taille, long_tot;
};



static FILE	*trico_file = {NULL};



static SXINLINE void horodatage (int INIT_ou_ACTION)
{
  sxtimestamp (INIT_ou_ACTION, "%ldms\n");
}


static SXINLINE int min (int a, int b)
{
  return ((a)<=(b) ? (a) : (b));
}



/*
 * MULTIKEY QUICKSORT
 * from J.L.Bentley's & R.Sedgewick's
 * ssort2 -- Faster Version of Multikey Quicksort
 */


static SXINLINE void vecswap2 (SXINT *a, SXINT *b, int n)
{
  while (n-- > 0) {
    int t = *a;
    *a++ = *b;
    *b++ = t;
  }
}

static SXINLINE void swap2 (SXINT *a, SXINT *b)
{
  SXINT t = *(a); *(a) = *(b); *(b) = t;
}


static SXINLINE int p2c (SXINT *i, int d)
{
  return ((*(sxstrget(*(i)) + d)) & 0xFF);
}



static SXINLINE SXINT *med3 (SXINT *a, SXINT *b, SXINT *c, int d)
{
  int va, vb, vc;

  if ((va = p2c (a, d)) == (vb = p2c (b, d)))
    return a;
  if ((vc = p2c (c, d)) == va || vc == vb)
    return c;
  return va < vb ?
    (vb < vc ? b : (va < vc ? c : a ) )
    : (vb > vc ? b : (va < vc ? a : c ) );
}



/* Cette fonction "inf_egal" ne sert que pour l'appel de "sort_by_tree" */

static SXINLINE bool inf_egal (SXINT a, SXINT b)
{
  unsigned char *s, *t;

  SXUINT ls = sxstrlen (a), lt = sxstrlen (b);
  SXUINT len = min (ls, lt);

  for (s = (unsigned char *) sxstrget (a), t = (unsigned char *) sxstrget (b); *s==*t && --len>0; s++, t++)
    ;
  return ((len > 0) ? (*s <= *t) : (ls <= lt));
}



/* Cette fonction "inf_egal_sup" ne sert que pour l'appel de "qsort" */

static SXINLINE int inf_egal_sup (const SXINT *a, const SXINT *b)
{
  unsigned char *s, *t;

  SXUINT ls = sxstrlen (*a), lt = sxstrlen (*b);
  SXUINT len = min (ls, lt);

  for (s = (unsigned char *) sxstrget (*a), t = (unsigned char *) sxstrget (*b); *s==*t && --len>0; s++, t++)
    ;
  return ((len > 0) ? (*s - *t) : (int) (ls - lt));
}



static SXINLINE void inssort (SXINT *a, int n, int d)
{
  SXINT *pi, *pj;
  unsigned char *s, *t;

  for (pi = a + 1; --n > 0; pi++) {
    for (pj = pi; pj > a; pj--) {
	  /* Inline strcmp: break if *(pj-1) <= *pj */
      int ls = sxstrlen (*(pj-1)), lt = sxstrlen (*pj);
      int len = min (ls, lt) - d;

      for (s = (unsigned char *) sxstrget (*(pj-1))+d, t = (unsigned char *) sxstrget (*pj)+d; *s==*t && --len>0; s++, t++)
	;
      if ((len > 0) ? (*s <= *t) : (ls <= lt))
	break;
      swap2(pj, pj-1);
    }
  }
}

static SXINLINE void choose_median (SXINT *a, int n, int d)
{
  SXINT *pm;

  if (n > 30) { /* On big arrays, near-median of 9 */
    int r = (n-1)/8, sigma, delta_m, delta_x, i;
    SXINT *x;
    sigma = p2c (a, d) + p2c (a+r, d) + p2c (a+2*r, d) + p2c (a+3*r, d) + p2c (a+4*r, d) + p2c (a+5*r, d) + p2c (a+6*r, d) + p2c (a+7*r, d) + p2c (a+8*r, d);
    delta_m = sigma /* On peut difficilement faire pis */, pm = a /* garde-fou */ ;
    for (i = 0, x = a; i <= 8; i++, x += r) {
      delta_x = 9 * p2c (x, d) - sigma;
      if (delta_x < 0) 
        delta_x = -delta_x;
      if (delta_x < delta_m) {
	pm = x, delta_m = delta_x;
      }
    }
  } else {
    pm = med3 (a, a+(n/2), a+(n-1), d);
  }
  if (pm != a) swap2 (a, pm);
}



static SXINLINE void ssort2 (SXINT *a, int n, SXUINT d)
{
  if (n < 10) {
    inssort (a, n, d);
  } else {
    choose_median (a, n, d);

    {
      int r, partval;
      SXINT *pa, *pb, *pc, *pd, *pn = a + n;
      partval = p2c (a, d);
      for (pa = pb = a+1, pc = pd = pn-1; ; pb++, pc--) {
	while (pb <= pc && (r = p2c (pb, d) - partval) <= 0) {
	  if (r == 0) { swap2 (pa, pb); pa++; }
	  pb++;
	}
	while (pb <= pc && (r = p2c (pc, d) - partval) >= 0) {
	  if (r == 0) { swap2 (pc, pd); pd--; }
	  pc--;
	}
	if (pb > pc) break;
	swap2 (pb, pc);
      }

      r = min(pa-a, pb-pa);		vecswap2(a,  pb-r, r);
      r = min(pd-pc, pn-pd-1);		vecswap2(pb, pn-r, r);
      if ((r = pb-pa) > 1)		ssort2 (a, r, d);
      if (sxstrlen (*(a + r)) > d)	ssort2 (a + r, pa-a + pn-pd-1, d+1);
      if ((r = pd-pc) > 1)		ssort2 (pn-r, r, d);
    }
  }
}



static SXINLINE void multi_key_quick_sort (SXINT *a, int n)
{
  ssort2 (a, n, 0);
}



/*
 * MULTIKEY QUICKSORT
 * from J.L.Bentley's & R.Sedgewick's
 * phd_sort -- Faster Version of Multikey Quicksort
 */


static SXINLINE void swap_range (SXINT *a, SXINT *b, int n)
{
  while (n-- > 0) {
    SXINT t = *a; *a++ = *b; *b++ = t;
  }
}

static SXINLINE void swap (SXINT *a, SXINT *b)
{
  SXINT t = *a; *a = *b; *b = t;
}


static SXINLINE int inf_egal_sup_depth (SXINT *a, SXINT *b, int d)
{
  unsigned char *s, *t;

  int ls = sxstrlen (*a), lt = sxstrlen (*b);
  int len = min (ls, lt) - d;

  for (s = (unsigned char *) sxstrget (*a) + d, t = (unsigned char *) sxstrget (*b) + d; *s==*t && --len>0; s++, t++)
    ;
  return ((len > 0) ? (*s - *t) : (ls - lt));
}




static SXINLINE void phd_sort (SXINT *a, int n, SXUINT d)
{
  switch (n) {
  case 0:
  case 1:
    return;

  case 2:
    if (inf_egal_sup_depth (a, a+1, d) > 0)
      swap (a, a+1);
    return;

  default:
	/* On big arrays, near-median of 9 */
  {
    SXINT *pm = a /* garde-fou */ ;
    int r = (n-1)/8, sigma, delta_m, delta_x, i;
    SXINT *x;
    sigma = p2c (a, d) + p2c (a+r, d) + p2c (a+2*r, d) + p2c (a+3*r, d) + p2c (a+4*r, d) + p2c (a+5*r, d) + p2c (a+6*r, d) + p2c (a+7*r, d) + p2c (a+8*r, d);
    delta_m = sigma /* On peut difficilement faire pis */ ;
    for (i = 0, x = a; i <= 8; i++, x += r) {
      delta_x = 9 * p2c (x, d) - sigma;
      if (delta_x < 0)
        delta_x = -delta_x;
      if (delta_x < delta_m) {
	pm = x;
	delta_m = delta_x;
      }
    }
    if (pm != a) swap (a, pm);
  }
    break;

  case 4: case 5: case 6: case 7: case 8: case 9:
  case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19:
  case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29:
  {
    int va, vb, vc;
    SXINT *b, *c;
    SXINT *pm =
      ((va = p2c (a, d)) == (vb = p2c ((b = a+(n/2)), d)))
      ? a
      : ((vc = p2c ((c = a+(n-1)), d)) == va || vc == vb)
      ? c
      : va < vb
      ? (vb < vc ? b : (va < vc ? c : a))
      : (vb > vc ? b : (va < vc ? a : c));

    if (pm != a) swap (a, pm);
  }
  break;

  case 3:
    break;
  }

  {
    int r, partval;
    SXINT *pa, *pb, *pc, *pd, *pn = a + n;
    partval = p2c (a, d);
    for (pa = pb = a+1, pc = pd = pn-1; ; pb++, pc--) {
      while (pb <= pc && (r = p2c (pb, d) - partval) <= 0) {
	if (r == 0) { swap (pa, pb); pa++; }
	pb++;
      }
      while (pb <= pc && (r = p2c (pc, d) - partval) >= 0) {
	if (r == 0) { swap (pc, pd); pd--; }
	pc--;
      }
      if (pb >= pc) break;
      swap (pb, pc);
    }

    r = min(pa-a, pb-pa);		swap_range (a,  pb-r, r);
    r = min(pd-pc, pn-pd-1);		swap_range (pb, pn-r, r);
    if ((r = pb-pa) > 1)		phd_sort (a, r, d);
    if (sxstrlen (*(a + r)) > d)	phd_sort (a + r, n-1 - (pd-pa), d+1);
    if ((r = pd-pc) > 1)		phd_sort (pn-r, r, d);
  }
}



static SXINLINE void phd_multi_key_quick_sort (SXINT *a, int n)
{
  phd_sort (a, n, 0);
}





/*
 * MULTIKEY QUICKSORT
 * from J.L.Bentley's & R.Sedgewick's
 * sxmksort -- Faster Version of Multikey Quicksort
 */


static SXINLINE void sxmkswap_range (SXINT *a, SXINT *b, int n)
{
  while (n-- > 0) {
    SXINT t = *a; *a++ = *b; *b++ = t;
  }
}

static SXINLINE void sxmkswap (SXINT *a, SXINT *b)
{
  SXINT t = *a; *a = *b; *b = t;
}

static SXINLINE int sxmkpd2c (SXINT *i, int d)
{
  return ((*(sxstrget(*(i)) + d)) & 0xFF);
}

static SXINLINE int sxmkp2l (SXINT *i)
{
  return sxstrlen (*(i));
}




static SXINLINE void sxmksort_depth (SXINT *a, int n, int d)
{
  switch (n) {
  case 0:
  case 1:
    return;

  case 2:
  {
    unsigned char c0, c1;

    int l0 = sxmkp2l (a), l1 = sxmkp2l (a+1);
    int len = min (l0, l1) - d;

    while ((c0 = sxmkpd2c (a, d)) == (c1 = sxmkpd2c (a+1, d)) && --len>0)
      d++;
    if ((len > 0) ? (c0 > c1) : (l0 > l1))
      sxmkswap (a, a+1);
    return;
  }

  default:
  {	/* On big arrays, near-median of 9 */
    SXINT *x, *pm = a /* garde-fou */ ;
    int r = (n-1)/8, sigma, delta_m, delta_x, i;

    sigma = sxmkpd2c (a, d) + sxmkpd2c (a+r, d) + sxmkpd2c (a+2*r, d) + sxmkpd2c (a+3*r, d) + sxmkpd2c (a+4*r, d) + sxmkpd2c (a+5*r, d) + sxmkpd2c (a+6*r, d) + sxmkpd2c (a+7*r, d) + sxmkpd2c (a+8*r, d);
    delta_m = sigma /* On peut difficilement faire pis */ ;
    for (i = 0, x = a; i <= 8; i++, x += r) {
      delta_x = 9 * sxmkpd2c (x, d) - sigma;
      if (delta_x < 0)
        delta_x = -delta_x;
      if (delta_x < delta_m) {
	pm = x;
	delta_m = delta_x;
      }
    }
    if (pm != a) sxmkswap (a, pm);
    break;
  }

  case 4: case 5: case 6: case 7: case 8: case 9:
  case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19:
  case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29:
  {
    int va, vb, vc;
    SXINT *b, *c;
    SXINT *pm =
      ((va = sxmkpd2c (a, d)) == (vb = sxmkpd2c ((b = a+(n/2)), d)))
      ? a
      : ((vc = sxmkpd2c ((c = a+(n-1)), d)) == va || vc == vb)
      ? c
      : va < vb
      ? (vb < vc ? b : (va < vc ? c : a))
      : (vb > vc ? b : (va < vc ? a : c));

    if (pm != a) sxmkswap (a, pm);
    break;
  }
  case 3:
    break;
  }

  {
    int r, partval;
    SXINT *pa, *pb, *pc, *pd, *pn = a + n;
    partval = sxmkpd2c (a, d);
    for (pa = pb = a+1, pc = pd = pn-1; ; pb++, pc--) {
      while (pb <= pc && (r = sxmkpd2c (pb, d) - partval) <= 0) {
	if (r == 0) { sxmkswap (pa, pb); pa++; }
	pb++;
      }
      while (pb <= pc && (r = sxmkpd2c (pc, d) - partval) >= 0) {
	if (r == 0) { sxmkswap (pc, pd); pd--; }
	pc--;
      }
      if (pb >= pc) break;
      sxmkswap (pb, pc);
    }

    r = min(pa-a, pb-pa);		sxmkswap_range (a,  pb-r, r);
    r = min(pd-pc, pn-pd-1);		sxmkswap_range (pb, pn-r, r);
    if ((r = pb-pa) > 1)		sxmksort_depth (a, r, d);
    if (sxmkp2l (a + r) > d)		sxmksort_depth (a + r, n-1 - (pd-pa), d+1);
    if ((r = pd-pc) > 1)		sxmksort_depth (pn-r, r, d);
  }
}



static SXINLINE void sxmksort (SXINT *a, int n)
{
  sxmksort_depth (a, n, 0);
}




static struct mots mots;
static int	*pste;
// static struct trico trico;

static SXBA	ensemble;
static int	taille_ensemble;

static SXINLINE void agrandir_mots ()
{
  mots.ste = (int*) sxrealloc (mots.ste, (mots.taille *= 2), sizeof (int));
  pste = &(mots.ste [mots.nombre]);
}

static SXINLINE void agrandir_ensemble ()
{
  ensemble = sxba_resize (ensemble, (taille_ensemble *= 2));
}

static void gripe ()
{
  fputs ("\nUne fonction de \"trico\" n'est pas a jour.\n", sxstderr);
  abort ();
}




#if 0
static void gen_header ()
{
  long		date_time = time (0);

  fprintf (trico_file,
	   "\n\
/*********************************************************************\n\
\tCet automate pour les mots \"%s\" a ete produit\n\
\tle %s\
\tpar le constructeur TRICO de SYNTAX(r)\n\
********************************************************************\n\
\t(c) SYNTAX est une marque deposee de l'INRIA.\n\
*********************************************************************/\n\n\n",
	   prgentname, ctime (&date_time));

}



static void gen_includes ()
{
  fputs ("\n/************** I N C L U D E S **********************/\n", trico_file);
  fputs ("#include \"sxtrico.h\"\n", trico_file);
}


static void gen_trico ()
{
  int 		i;
  unsigned char	class;

  if (trico.max) {
    fprintf (trico_file, "\n\nstatic unsigned int trico_comb_vector [%i] = {\n", trico.max+1);

    for (i = 0; i <= trico.max; i++) {
      if ((i % 10) == 9)
	fputs ("\n", trico_file);

      fprintf (trico_file, "%i, ", trico.comb_vector [i]);
    }

    fputs ("};\n", trico_file);
    fputs ("\n\nstatic unsigned char trico_char2class [256] = {\n", trico_file);

    for (i = 0; i <= 255; i++) {
      if ((i % 8) == 7)
	fputs ("\n", trico_file);

      class = trico.char2class [i];
      fprintf (trico_file, class == '\'' ? "'\\%s', " : "'%s', ", SXCHAR_TO_STRING (class));
    }

    fputs ("};\n", trico_file);
  }

  fputs ("\n\nstruct trico trico = {\n", trico_file);
  fprintf (trico_file, "%i, %i, %i, %i, %i, %i, %i,\n",
	   trico.max, trico.init_base, trico.class_mask, trico.stop_mask, trico.is_suffix_mask, trico.base_shift, trico.partiel);
  fputs (trico.max ? "trico_char2class,\ntrico_comb_vector\n" : "NULL,\nNULL\n", trico_file);
  fputs ("};\n", trico_file);
}
#endif

static SXINT *pointeurs_de_mots, *pointeurs_prefixe, *pointeurs_supplementaires, *pointeurs_rajoutes, *pointeurs_en_plus;
static SXINT *pointeurs_suffixe;

static void trico_process ()
{
  register int		 i;
  register SXINT	*p, *q, *r, *s, *t;

  pointeurs_de_mots = (SXINT *) sxalloc (5 * mots.nombre, sizeof (int));
  pointeurs_prefixe = &(pointeurs_de_mots [0]);
  pointeurs_suffixe = &(pointeurs_de_mots [mots.nombre]);
  pointeurs_supplementaires = &(pointeurs_de_mots [2*mots.nombre]);
  pointeurs_rajoutes = &(pointeurs_de_mots [3*mots.nombre]);
  pointeurs_en_plus = &(pointeurs_de_mots [4*mots.nombre]);

  for (i = mots.nombre - 1, p = pointeurs_prefixe + i, q = pointeurs_suffixe + i, r = pointeurs_supplementaires + i, s = pointeurs_rajoutes + i, t = pointeurs_en_plus + i; i >= 0; i--) {
    *p-- = *q-- = *r-- = *s-- = *t-- = i;
  }

  fputs ("Initialisation : ", sxtty);
  horodatage (SXACTION);
  fputs ("Tri MKQS...", sxtty);
  multi_key_quick_sort (pointeurs_prefixe, mots.nombre);
  horodatage (SXACTION);
  fputs ("Tri SBT...", sxtty);
  sort_by_tree (pointeurs_suffixe, 0, mots.nombre - 1, inf_egal);
  horodatage (SXACTION);
  fputs ("Tri QS...", sxtty);
  qsort (pointeurs_supplementaires, mots.nombre, sizeof (*pointeurs_supplementaires), (int (*)(const void *, const void *)) inf_egal_sup);
  horodatage (SXACTION);
  fputs ("Tri PhDMKQS...", sxtty);
  phd_multi_key_quick_sort (pointeurs_rajoutes, mots.nombre);
  horodatage (SXACTION);
  fputs ("Tri SXMKS...", sxtty);
  sxmksort (pointeurs_en_plus, mots.nombre);
  horodatage (SXACTION);
  fputs ("Verifications...", sxtty);
  for (i = mots.nombre - 1, p = pointeurs_prefixe + i, q = pointeurs_suffixe + i, r = pointeurs_supplementaires + i, s = pointeurs_rajoutes + i, t = pointeurs_en_plus + i; i >= 0; i--,p--,q--,r--,s--,t--) {
    if (*p != *q || *q != *r || *r != *s || *s != *t) {
      if (!((*p == 0 || *p == 1) && (*q == 0 || *q == 1) && (*r == 0 || *r == 1) && (*s == 0 || *s == 1) && (*t == 0 || *t == 1))) {
	fprintf (sxstderr, "\nErreur de tri a l'indice %d : \"%s\" --- \"%s\" --- \"%s\" --- \"%s\" --- \"%s\".", i, sxstrget (*p), sxstrget (*q), sxstrget (*r), sxstrget (*s), sxstrget (*t));
	break;
      }
    }
  }
  horodatage (SXACTION);
  fputs ("Sortie...", sxtty);
  for (i = mots.nombre - 1, p = pointeurs_prefixe; i >= 0; i--,p++) {
    fprintf (sxstdout, "\"%s\"\n", sxstrget (*p));
  }
}




void trico_sem_act (int code, int numact)
{
  int			ste;

  switch (code) {
  case SXOPEN:
    if (trico_file == NULL)
      trico_file = stdout;

  case SXCLOSE:
  case SXSEMPASS:
  case SXERROR:
    return;

  case SXINIT:
    horodatage (SXINIT);
    fputs ("Lecture des chaines... ", sxtty);
    mots.ste = (int*) sxalloc ((mots.taille = 1020), sizeof (int));
    pste = &(mots.ste [mots.nombre = 0]);
    mots.long_tot = 0;
    return;

  case SXFINAL:
    sxfree (mots.ste), mots.ste = NULL;
    return;

  case SXACTION:
    switch (numact) {
    case 3:
	  /*	<mots>		= %MOT 				; 3	*/
      ensemble = sxba_calloc (taille_ensemble = 1020);
	  /* On poursuit vers le cas 2 */
          /* FALLTHROUGH */

    case 2:
	  /*	<mots>		= <mots> %MOT	 		; 2	*/

      ste = SXSTACKtoken (SXSTACKtop ()).string_table_entry;
      while (ste >= taille_ensemble) agrandir_ensemble ();

      if (sxba_bit_is_set (ensemble, ste))
	sxerror (SXSTACKtoken (SXSTACKtop ()).source_index,
		     sxplocals.sxtables->err_titles [1][0] /* Warning */,
		     "%sCe texte est deja apparu plus haut : on l'ignore.",
		     sxplocals.sxtables->err_titles [1]+1 /* Warning */);
      else {
	sxba_1_bit (ensemble, ste);
	if (mots.nombre >= mots.taille) agrandir_mots ();
	mots.nombre++, *pste++ = ste;
	mots.long_tot += sxstrlen (ste);
      }

      return;

    case 1:
	  /*	<trico>		= <mots> 			; 1	*/
      horodatage (SXACTION);
      sxfree (ensemble), ensemble = NULL;

      if (sxerrmngr.nbmess [1] + sxerrmngr.nbmess [2] == 0) {
	fputs ("Calcul de l'automate... ", sxtty);
	trico_process ();
	horodatage (SXACTION);
#if 0
	if ((((unsigned int)(~0))>>trico.base_shift) >= trico.max) {
	  fputs ("ecriture de l'automate... ", sxtty);
	  gen_header ();
	  gen_includes ();
	  gen_trico ();
	  horodatage (SXACTION);
	}
	else
	  sxtmsg ("%sIl y a trop d'entrees, l'automate ne peut etre produit.\n", sxplocals.sxtables->err_titles [2] /* Error */);
#endif
      }
      return;

    default:
      break /* continue in sequence to execute gripe() */ ;
    }
    /* FALLTHROUGH */

  default:
    gripe ();
  }
}


bool sxscanner_action (SXINT code, SXINT act_no)
{
  switch (code) {
  case SXOPEN:
  case SXCLOSE:
  case SXINIT:
  case SXFINAL:
    return SXANY_BOOL;

  case SXACTION:
    switch (act_no) {
    case 1:
	  /* \nnn => char */
    {
      register int	val;
      register char	c, *s, *t;

      t = s = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index;

      for (val = *s++ - '0'; (c = *s++) != SXNUL;) {
	val = (val << 3) + c - '0';
      }

      *t = val;
      sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index + 1;
      sxsvar.sxlv.mark.index = -1;
    }
    return SXANY_BOOL;

    default:
      break /* continue in sequence to execute gripe() */ ;
    }
    /* FALLTHROUGH */

  default:
    gripe ();
    /* NOTREACHED */
    return SXANY_BOOL;
  }
}
