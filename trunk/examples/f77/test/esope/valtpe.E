      function valtpe(
     & pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : valtpe (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : type des elements d'un conteneur "values"
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cas  pval   : un conteneur "values"
cas  status : status d'execution (status=0 <=> succes)
cas  valtpe : type des elements d'un conteneur "values"
c
#include <implicit.h>
#include <values.seg>
c
      pointeur      pval.values
      integer       status
      character*8   valtpe
c
      status=0
c
c     -- ouvrir le conteneur "values"
      segact,pval
      valnf =pval.valf(/1)
      valne =pval.vale(/1)
      valnl =pval.vall(/1)
      valnc =pval.valc(/2)
      valsmx=pval.valc(/1)
c
c     -- type des elements d'un conteneur "values"
      valtpe=pval.valtyp
c
c     -- fermer le conteneur "values"
      segdes,pval*NOMOD
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('valtpe: routine en echec')
      status=1
      return
      end
