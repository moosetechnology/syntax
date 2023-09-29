#ifndef sxsubset_h
#define sxsubset_h
/* Permet de manipuler les sous-ensembles de la partie [0:n] des entiers naturels.
   Le re'fe'rentiel peut e^tre (tous) les entiers relatifs si l'on filtre par le
   X_mngr.
   La valeur de n peut changer dynamiquement.
   
   Fonctionnalites similaires (pour des ensembles) aux packages "XH", "XT" et sxlist.

   Un subset est identifie' par un entier positif ou nul.
   Un subset d'un referentiel de cardinal n occupe a peu pres
   (5 + (n+SXBITS_PER_LONG-1)/SXBITS_PER_LONG) cellules.

   Comparaison (sur 10000 ensembles aleatoires de taille max 200 prenant leurs
                elements sur [0..499]):
                XH		XT		sxlist		sxsubset
   Temps	1.47		8.22		3.71		1
   Place	4.82		19.57		9.85		1

   Avantages :
      - Re'cupere effectivement la place occupe'e par les subsets qui ont e'te'
        "released", sans changer les identifiants des subsets actifs.
      - Supporte la construction simultane'e de plusieurs subsets.
      - Chaque sous-ensemble peut e^tre manipule' par les SXBA

   Inconve'nients :
      - Chaque subset occupe une place fixe qui ne de'pend que du
        cardinal du re'fe'rentiel

   Bug :
      - He'rite' des SXBA, les ensembles calcule's par sxba_not, sxba_fill posent des
        problemes lorsque leur taille augmente...

   Une implantation avec des sxindex matriciels a ete essayee et s'est revelee plus 
   mauvaise.
*/

#include	<stdio.h>
#include	"sxindex.h"
#include	"sxba.h"


typedef struct {
    sxindex_header	subset_hd;
    SXBA		*subsets;

    SXINT			*hash_tbl;

    struct subset_display {
	SXINT		lnk;
	SXUINT	scrmbl;
    } *display;

    SXINT	                hash_size;

    SXINT		(*subset_oflw) (SXINT, SXINT),
                        (*elem_oflw) (SXINT, SXINT);

    FILE		*stat_file;
} sxsubset_header;

sxsubset_header		*_sxsubset_header; /* Pour oflw */


extern void			sxsubset_alloc (sxsubset_header *h,
						SXINT subset_nb,
						SXINT cardinal,
						SXINT (*user_subset_oflw) (SXINT, SXINT),
						SXINT (*user_elem_oflw) (SXINT, SXINT),
						FILE *stat_file);
extern void			sxsubset_free (sxsubset_header *h);
extern void			sxsubset_elem_oflw (sxsubset_header *h, SXINT new_cardinal);
extern SXINT			sxsubset_set (sxsubset_header *h, SXINT subset_nb);
    /* subset_nb est un identifiant (entier) d'un ensemble, retourne par sxsubset_seek,
       manipule par les SXBA (via sxsubset_to_sxba) que l'on veut entrer ds la
       structure.
       Le resultat est l'identifiant de ce sous-ensemble ds la structure.
       Si resultat == subset_nb => c'est un nouveau, sinon il existait deja et on peut
       donc le liberer via sxsubset_release(h,subset_nb). */
extern BOOLEAN			sxsubset_unset (sxsubset_header *h, SXINT subset_nb);
/* Le sous-ensemble "subset_nb" redevient un simple SXBA.
   Retourne TRUE ssi il figure ds le pool des sous-ensembles. */
extern SXINT			sxsubset_seek (sxsubset_header *h);
/* Retourne l'identifiant entier d'un nouvel ensemble manipulable par les
   SXBA via sxsubset_to_sxba.  ATTENTION, les SXBA retourne's pre'ce'demment
   par "sxsubset_to_sxba" peuvent e^tre invalides (reallocation). */

extern void			sxsubset_stat (FILE *stat_file, sxsubset_header *h);
extern BOOLEAN			sxsubset_write (sxsubset_header *header, filedesc_t file_descr);
extern BOOLEAN			sxsubset_read (sxsubset_header *header,
					       filedesc_t file_descr,
					       char *name,
					       SXINT (*subset_oflw) (SXINT, SXINT), 
					       SXINT (*elem_oflw) (SXINT, SXINT),
					       FILE *stat_file);
extern void			sxsubset_array_to_c (sxsubset_header *header, FILE *file, char *name);
extern void			sxsubset_header_to_c (sxsubset_header *header, FILE *file, char *name);
extern void			sxsubset_to_c (sxsubset_header *header, 
					       FILE *file, 
					       char *name, 
					       BOOLEAN is_static);
extern void			sxsubset_reuse (sxsubset_header *header,
						SXINT (*user_subset_oflw) (SXINT, SXINT), 
						SXINT (*user_elem_oflw) (SXINT, SXINT),
						FILE *stat_file);


#define sxsubset_to_sxba(h,i)	(h).subsets [i]
#define sxsubset_release(h,i)	sxindex_release ((h).subset_hd,i)
/* L'identifiant "i" (donne' par sxsubset_seek) est rendu au pool. */
#define sxsubset_is_released(h,i)\
                                sxindex_is_released ((h).subset_hd,i)
#define sxsubset_is_set(h,i)	((h).display [i].lnk != -2)
/* TRUE ssi i == seek () && i == set () */
#define sxsubset_clear(h)	sxindex_clear ((h).subset_hd)
#define sxsubset_nb(h)		sxindex_size ((h).subset_hd)
    /* Nombre maximal de subsets */
#define sxsubset_card(h)	BASIZE ((h).subsets [0])
    /* Nombre maximal d'elements ds chaque subset. */

#define sxsubset_foreach(h,l)	sxindex_foreach((h).subset_hd,l)
#define sxsubset_foreach_subset(h,l)					\
	                        sxindex_foreach((h).subset_hd,l)	\
                                   if (sxsubset_is_set(h,l))
#define sxsubset_is_static(h)	sxindex_is_static ((h).subset_hd)


#endif
