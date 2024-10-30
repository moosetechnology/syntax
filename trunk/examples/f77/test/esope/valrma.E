      subroutine valrma(
     & pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : valrma (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : effacer toutes les valeurs d'un conteneur "values"
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cas  pval   : un conteneur "values"
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <values.seg>
c
      pointeur      pval.values
      integer       status
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
c     -- vider le conteneur "values"
      valnf = 0
      valne = 0
      valnl = 0
      valnc = 0
      valsmx= 0
      segadj,pval
c
c     -- fermer le conteneur "values"
      segdes,pval*MOD
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('valrma: routine en echec')
      status=1
      return
      end
