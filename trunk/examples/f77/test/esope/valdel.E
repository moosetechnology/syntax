      subroutine valdel(
     & pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : valdel (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : detruire un conteneur "values"
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  pval   : un conteneur "values"
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
c     -- detruire le conteneur "values"
      if(pval .ne. 0)then
        segsup,pval
        pval=0
      endif
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('valdel: routine en echec')
      status=1
      return
      end
