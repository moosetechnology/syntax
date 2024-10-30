      function valsiz(
     & pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : valsiz (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : nombre d'elements d'un conteneur "values"
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cas  pval   : un conteneur "values"
cas  status : status d'execution (status=0 <=> succes)
cas  valsiz : nombre d'elements d'un conteneur "values"
c
#include <implicit.h>
#include <values.seg>
c
      pointeur      pval.values
      integer       status
      integer       valsiz
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
c     -- taille du conteneur "values"
      valsiz=max(valnf,valne,valnl,valnc)
c
c     -- fermer le conteneur "values"
      segdes,pval*NOMOD
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('valsiz: routine en echec')
      status=1
      return
      end
