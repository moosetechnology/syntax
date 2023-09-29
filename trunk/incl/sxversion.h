#ifndef SVNVERSION
#define SVNVERSION " (revision 3610M)"
#endif /* SVNVERSION */

#ifndef SXVERSION
#define SXVERSION "6.0b7"
#endif /* SXVERSION */

#ifndef SX_COMPILE_DATE
#define SX_COMPILE_DATE "Fri Sep 29 15:38:04 CEST 2023"
#endif /* SX_COMPILE_DATE */

#ifndef SX_release_mess
#ifdef SX_DFN_EXT_VAR
char	release_mess [] = "Release " SXVERSION SVNVERSION " of " SX_COMPILE_DATE ;
#else
extern char	release_mess [];
#endif /* SX_DFN_EXT_VAR */
#define SX_release_mess
#endif /* SX_release_mess */

