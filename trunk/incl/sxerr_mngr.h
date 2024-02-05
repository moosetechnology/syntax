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

#ifndef sxerr_mngr_h
#define sxerr_mngr_h

/*************************************/
/* ERROR MANAGER (see sxerr_mngr(3)) */
/*************************************/

/* Constantes de l'error manager */

#ifndef SXSEVERITIES
#define SXSEVERITIES	3	/* 0: comment, 1: warning, 2: error */
#endif

#define SXERR_FORMAT_DEFAULT 0
#define SXERR_FORMAT_LIGHTWEIGHT ((SXERR_FORMAT_DEFAULT)+1)
#define SXERR_FORMAT_BANNER ((SXERR_FORMAT_DEFAULT)+2)
#define SXERR_FORMAT_CUSTOM ((SXERR_FORMAT_DEFAULT)+3)
#define SXERR_FORMAT_EMPTY ((SXERR_FORMAT_DEFAULT)+4)

/* if macro SXERR_NO_TABS is #defined, sxerr_mngr won't output tabulations */

#ifndef SXERROR_COORD
#define SXERROR_COORD	"%s, line %lu: column %lu: " /* args are filename, line #, col # */
#endif

#ifndef SXHEAD_COORD
#define SXHEAD_COORD	"%s: "		/* arg is filename */
#endif

#ifndef SXTAIL_COORD
#define SXTAIL_COORD	"%s: "		/* arg is filename */
#endif

#ifndef SXNO_FILE_NAME
#define SXNO_FILE_NAME	"\"\""		/* used when file name is unknown */
#endif

/* Variables de l'error manager */

struct sxerr_info {
    SXINT	file_ste /*	string table entry of source name	*/ ;
    SXUINT	line /*		source coordinates			*/ ;
    SXUINT	column;
    off_t	beginning, end /*	offsets in scratch file			*/ ;
    int		severity /*		severity of this message		*/ ;
};

SX_GLOBAL_VAR struct sxerrmngr {
    int		nbmess [SXSEVERITIES] /*	counts messages				*/ ;
    off_t	mark /*				private...				*/ ;
    SXINT	err_kept /*			nb of messages successfully kept	*/ ;
    SXINT	err_size /*			size of err_infos			*/ ;
    FILE	*scratch_file /*		scratch file				*/ ;
    struct sxerr_info	*err_infos /*		to retrieve messages			*/ ;
    SXUINT sxerr_format /*                      error message format 
			                        default value : SXERR_FORMAT_DEFAULT
			                        other possible values : SXERR_FORMAT_LIGHTWEIGHT,
			                        SXERR_FORMAT_BANNER, SXERR_FORMAT_CUSTOM, SXERR_FORMAT_EMPTY */ ;
    char *sxerr_banner /* only used for SXERR_FORMAT_BANNER and SXERR_FORMAT_CUSTOM */ ;
}	sxerrmngr;

SX_GLOBAL_VAR void (*sxtrap_recovery) (void);

#define sxnb_warnings() (sxerrmngr.nbmess [1])
/* number of warning messages displayed */

#define sxnb_errors() (sxerrmngr.nbmess [2])
/* number of error message displayed */

extern int sxerr_max_severity (void);

#if defined (__x86_64) && (defined (__gnu_linux__) || defined (__APPLE__))
/* - 2009-03-10 : nous avons observé des erreurs de segmentation
 *   dans sxerror() si l'on utilise la fonction vfprintf() de
 *   la bibliotheque Glibc 2.6 sur architecture x64 ;
 * - 2012-04-26 : nous confirmons que l'erreur se produit encore sur
 *   x64 avec la bibliotheque Glibc 2.11.3 et le compilateur gcc 4.4.5
 *   avec l'option -O2
 * - 2018-10-24 : les erreurs de segmentation dans sxerror() se produisent
 *   aussi sous l'architecture mac64 (macOS 64 bits) en compilant avec Clang
 *   ==> soit c'est vfprintf() qui est erronee, soit c'est le code
 *   de sxerror() qui appelle vfprintf() incorrectement
 */
#define VFPRINTF_IS_BUGGED
/*
 * Note : positionner VFPRINTF_IS_BUGGED declenche des warnings a la
 * compilation "warning: function declaration isn't a prototype" ;
 * c'est bien dommage, mais c'est mieux qu'une erreur de segmentation
 */
#endif

#ifdef VFPRINTF_IS_BUGGED
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-prototypes"
/* on supprime les warnings de GCC concernant l'absence de  prototype */
extern void sxerror ();
#pragma GCC diagnostic pop
#else /* VFPRINTF_IS_BUGGED */
extern void sxerror (struct sxsource_coord source_index, SXINT severity, char *format, ...);
#endif /* VFPRINTF_IS_BUGGED */

extern void sxhmsg (char *file_name, char *format, ...);

extern void sxtmsg (char *file_name, char *format, ...);

extern void sxerr_mngr (SXINT what, ...);

extern void sxperror (char *string);

extern void sxtrap (char *caller, char *message);

#endif /* sxerr_mngr_h */

