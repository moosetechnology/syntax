/* ********************************************************************
   *  This program has been gracefully generated  for		      *
   *                        Michel LOYER			      *
   *  from lecl.lecl						      *
   *  on Mon May 18 13:47:52 1987				      *
   *  by the SYNTAX (*) lexical constructor LECL                      *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */


#include "sxcommon.h"
char WHAT_LECLKW[] = "@(#)SYNTAX - $Id: lecl_kw.c 856 2007-10-23 14:17:47Z rlacroix $" WHAT_DEBUG;

static SXINT hash_to_code [/* 50 */] = {
12, 47, 22, 49, 0, 40, 43, 64, 18, 29, 
66, 48, 41, 0, 31, 42, 51, 62, 18, 33, 
20, 21, 0, 68, 0, 30, 63, 19, 67, 50, 
25, 61, 0, 52, 45, 32, 53, 44, 27, 54, 
59, 60, 58, 0, 0, 65, 56, 57, 1, 16, 
};





static SXINT lecl_keyword (string, length)
char *string;
SXINT length;
{
SXUINT scramble;
#include "sxscrmbl_1.h"
switch (scramble & 31L) {
case 2:
return ((2146372470L / scramble) & 1) == 0 ? 12 : 47;
case 4:
return ((2143833440L / scramble) & 1) == 0 ? 22 : 49;
case 5:
return (hash_to_code + 4) [(2145649221L / scramble) & 3];
case 6:
return 18;
case 8:
return 29;
case 9:
return ((2144710567L / scramble) & 1) == 0 ? 66 : 48;
case 12:
return (hash_to_code + 12) [(2135406627L / scramble) & 3];
case 13:
return 51;
case 14:
return (hash_to_code + 17) [(2132045837L / scramble) & 7];
case 15:
return (hash_to_code + 25) [(2131590429L / scramble) & 3];
case 16:
return 50;
case 18:
return (hash_to_code + 30) [(2146487112L / scramble) & 3];
case 19:
return 45;
case 20:
return ((2141988780L / scramble) & 1) == 0 ? 32 : 53;
case 21:
return ((2146612779L / scramble) & 1) == 0 ? 44 : 27;
case 22:
return 54;
case 25:
return (hash_to_code + 40) [(2106348248L / scramble) & 7];
case 28:
return 1;
case 31:
return 16;
default :
return 0;
}
