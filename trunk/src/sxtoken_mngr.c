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

char WHAT_SXTOKEN_MNGR[] = "@(#)SYNTAX - $Id: sxtoken_mngr.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

/*	TOKEN   MANAGER

   Chaque token du source est identifie' par un entier, son numero.
   S'il n'y a pas eu de modification du flot de token par rapport au
   source (correction d'erreur par exemple), l'entier n identifiant
   un token est aussi son numero dans le source. De fac,on interne,
   Cet entier permet d'acceder a la structure le contenant. Ses poids
   forts designent un numero de buffer et ses poids faibles un index
   dans ce buffer.

   L'utilisateur dispose d'une fonction sxget_token (n) ou n est un numero de token
   et qui retourne un pointeur vers le token numero n.

   Vu des actions et predicats de l'utilisateur le token courant est suivant les cas:

   analyseur    cas		token courant                acces par

   parser	action		token suivant                sxget_token (sxplocals.atok_no)
		T-predicat	token associe au predicat    sxget_token (sxplocals.ptok_no)
		NT-predicat	token suivant                sxget_token (sxplocals.ptok_no)

   scanner      post-action     token de la post-action      sxsvar.sxlv.terminal_token

   Soit n le numero d'un token et p = SXTOKEN_PAGE (n) le numero du buffer
   (page) dans lequel est n
      Si p < min il n'est plus accessible.
      Si p >= min il est accessible par sxget_token (n).
         Si n <= Mtok_no il a deja ete lu
	 Si n > Mtok_no il faudra le lire (sxget_token s'en charge).
      Si p >= max, les structures actuelles ne permettent pas d'acceder
         a n => appel de sxtknzoom.

   Si n est le numero du token courant, sxget_token (n-look_back) est toujours accessible.
   Si look_back est nul, la quantite de look_back est non bornee.

   Si on est sur que le token n est accessible la macro SXGET_TOKEN (n) y accede.
   ATTENTION: n doit etre sans effet de bord

   Attention
	Tout appel a sxget_token (n) si n > sxplocals.Mtok_no lit des tokens par
	l'intermediaire du scanner; de plus ces appels peuvent deplacer les tokens
	dans toks_buf et donc invalider les (pointeurs vers les) tokens rendus par des
	appels precedents.


   Le parser et le traitement d'erreurs ne connaissent pas l'implantation.

   Les post actions du scanner peuvent utiliser sxget_token () a la condition de
   repositionner sxsvar.sxlv.terminal_token a sa valeur initiale en fin de traitement.
   Pour des raisons de compatibilite avec les versions de SYNTAX precedentes,
   le token courant (celui qui vient d'etre reconnu par le scanner) est accessible aussi dans
   sxsvar.sxlv.terminal_token et par sxget_token (sxplocals.Mtok_no).

   Proprietes :
   	toks_buf [min..max[
	atok_no <= ptok_no <= Mtok_no

*/


bool	sxtknzoom (SXINT n)
{
    /* gestion du debordement de toks_buf */

    SXINT	delta, size, i;

    size = sxplocals.max - sxplocals.min;

    if ((delta = SXTOKEN_PAGE (sxplocals.left_border)) > 0) {
	/* On commence par recuperer les pages disponibles du debut. */
	for (i = 0; i < delta; i++)
	    sxplocals.toks_buf [size + i] = sxplocals.toks_buf [i];

	for (i = 0; i < size; i++)
	    sxplocals.toks_buf [i] = sxplocals.toks_buf [delta + i];
	
	sxplocals.min += delta;
	sxplocals.max += delta;
    }

    if ((delta = SXTOKEN_PAGE (n)) >= size) {
	i = sxplocals.max;
	sxplocals.max = delta + sxplocals.min + 1;
	
	sxplocals.toks_buf = (struct sxtoken**) sxrealloc (sxplocals.toks_buf,
							   2 * (sxplocals.max - sxplocals.min),
							  sizeof (struct sxtoken*));
	do {
	    sxplocals.toks_buf [i - sxplocals.min] = (struct sxtoken*) sxalloc (SXTOKEN_AND + 1,
								sizeof (struct sxtoken));
	} while (++i < sxplocals.max);
    }

    return true;
}


struct sxtoken	*sxget_token (SXINT n)
{
    /* Acces a un token */

    SXINT			page_no;
    struct sxtoken	*atok;

    if ((page_no = SXTOKEN_PAGE (n)) < 0)
	return NULL;

    if (n > sxplocals.Mtok_no) {
	if (page_no >= sxplocals.max)
	    sxtknzoom (n);

	do {
	    if ((atok = &SXGET_TOKEN(sxplocals.Mtok_no))->lahead == sxplocals.SXP_tables.P_tmax) {
		do {
		    sxput_token (*atok);
		    /* Modifie le Mer 3 Nov 1999 10:28:42
		       ++sxplocals.Mtok_no;
		       SXGET_TOKEN(sxplocals.Mtok_no) = *atok;  */ 
		} while (n > sxplocals.Mtok_no);
	    }
	    else
		(*(sxplocals.SXP_tables.scanit)) ();
	    /* Le scanner a appele' sxput_token */
	    /* Mtok_no peut etre > a n, les post-actions du scanner ayant pu appeler
	       sxput_token plusieurs fois. */
	} while (n > sxplocals.Mtok_no);
    }
    
    return &SXGET_TOKEN(n);
}


SXINT	sxtknmdf (struct sxtoken *new_seq, SXINT lgth1, SXINT old_tok_no, SXINT lgth2)
{
    /* Replace a sequence of tokens:
          toks_buf [old_tok_no..old_tok_no+lgth2-1]
       by an other one:
          new_seq [0..lgth1-1] */
    SXINT	delta, newMtok_no, i, j, mtok_no;

    if ((delta = lgth1 - lgth2) != 0) {
	newMtok_no = sxplocals.Mtok_no + delta;
	mtok_no = old_tok_no + lgth2;

	if (delta < 0) {
	    for (j = mtok_no, i = j + delta; j <= sxplocals.Mtok_no; i++, j++)
		SXGET_TOKEN (i) = SXGET_TOKEN (j);
	}
	else {
	    if (SXTOKEN_PAGE (newMtok_no) >= sxplocals.max)
		sxtknzoom (newMtok_no);

	    for (j = sxplocals.Mtok_no, i = newMtok_no; j >= mtok_no; i--, j--)
		SXGET_TOKEN (i) = SXGET_TOKEN (j);
	}

	sxplocals.Mtok_no = newMtok_no;
    }

    for (j = 0, i = old_tok_no; j < lgth1; j++, i++)
	SXGET_TOKEN (i) = new_seq [j];

    return old_tok_no;
}

void	sxtkn_mngr (SXINT sxtkn_mngr_what, SXINT size)
{
    SXINT i;

    switch (sxtkn_mngr_what) {
    case SXOPEN:
	/* size = pages number */
	/* Extra space is use by sxtknzoom. */
	sxplocals.toks_buf = (struct sxtoken**) sxalloc (2*size, sizeof (struct sxtoken*));
	sxplocals.max = size /* assume size > 0 */ ;
	sxplocals.min = 0;

	for (i = sxplocals.min; i < sxplocals.max; i++)
	    sxplocals.toks_buf [i] = (struct sxtoken*) sxalloc (SXTOKEN_AND + 1,
								sizeof (struct sxtoken));
	sxplocals.left_border = 0;
	break;

    case SXINIT:
	sxplocals.Mtok_no = sxplocals.atok_no = sxplocals.ptok_no = 0;
	break;

    case SXFINAL:
	sxplocals.max -= sxplocals.min;
	sxplocals.min = 0;
	break;

    case SXCLOSE:
	for (i = sxplocals.min; i < sxplocals.max; i++)
	    sxfree (sxplocals.toks_buf [i]);

	sxfree (sxplocals.toks_buf), sxplocals.toks_buf = NULL;

	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxtoken_mngr","unknown switch case #1");
#endif
      break;
    }
}

