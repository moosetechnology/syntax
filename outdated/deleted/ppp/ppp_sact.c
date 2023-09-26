/* action du scanner pour PASCAL */

#include <ctype.h>
#include "sxunix.h"
#include "stdio.h"

static char *str,*str_incl;
static int lgth = 0;
static struct sxsrcmngr *pile_include;
static int incl, incl_use;
static FILE *filemane;
pp_p_scan_act(code, act_no)
int code;
int act_no;
{
char c;
int i;
switch (code) {
  case INIT :return (0) ;

  case ACTION :
	switch (act_no) {
		case 1 :
		  sxsvar.lv_s.token_string [0] = NUL;
		  sxsvar.lv.ts_lgth = 0;
		      return ;
		case 2 :
		 if (lgth == 0)
	     	     str = (char *) sxalloc ((lgth = sxsvar.lv.ts_lgth + 1)
			,sizeof(char));
			
		 else {
	           if (lgth < sxsvar.lv.ts_lgth + 1)
	     	   str = (char *) sxrealloc (str,(lgth = sxsvar.lv.ts_lgth + 1)
			 , sizeof(char)) ;
		      }

	         for (i = 0; i < sxsvar.lv.ts_lgth; i ++) {
                  c = sxsvar.lv_s.token_string [i] ;
		  if (isascii (c) && islower (c))
			str [i] = c + ('A' - 'a'); 
		  else str [i] = c;
		 }
		 str [sxsvar.lv.ts_lgth] = NUL;

		 if (is_check_keyword (str,sxsvar.lv.ts_lgth)){
		   for (i = 0; i < sxsvar.lv.ts_lgth; i ++) {
			sxsvar.lv_s.token_string [i] = str [i] ;
		   }
		  }

		 return ;
		case 3 :

		 fclose (sxsrcmngr.infile);
		 sxfree(sxsrcmngr.source_coord.file_name);
		 sxsrc_mngr(FINAL);
		 sxsrcmngr = pile_include [ --incl ];
		 return;

		case 4 :
	        if ((filemane = sxfopen (sxsvar.lv_s.token_string, "r")) == NULL) {
        	 sxput_error(sxsrcmngr.source_coord,"%s C'ant no open this file: %s\n",
	         sxsvar.err_titles [2],sxsvar.lv_s.token_string);
        	 if ((filemane = sxfopen ("/dev/null","r")) == NULL)
			SXEXIT (SEVERITIES-1);
	        }		
	           if (pile_include == NULL) 
			    pile_include = (struct sxsrcmngr *)
				sxalloc(incl_use = 1,sizeof(struct sxsrcmngr));
		   else
			   if (incl >= incl_use++)
			    pile_include = (struct sxsrcmngr *)
	        	      sxrealloc (pile_include,incl_use,sizeof(struct sxsrcmngr));
		   pile_include [ incl++ ] = sxsrcmngr;

                   str_incl = (char *) sxalloc (sxsvar.lv.ts_lgth + 1
			,sizeof(char));
	           for (i = 0; i <= sxsvar.lv.ts_lgth; i ++) {
                      str_incl [i] = sxsvar.lv_s.token_string [i] ;
                   }

		   sxsrc_mngr(INIT,filemane,str_incl);
	      return;
		}
  default : ;
}
}
