      subroutine valnew(
     & pval,
     & type,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : valnew (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : creer a vide un conteneur "values"
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cas  pval   : un conteneur "values"
cae  type   : type des elements du conteneur "values"
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <values.seg>
c
      pointeur      pval.values
      character*8   type
      integer       status
c
      status=0
c
c     -- initialiser le conteneur "values"
      valnf=0
      valne=0
      valnl=0
      valnc=0
      valsmx=0
      segini,pval
c
c     -- typer le conteneur "values"
      if(type .eq. 'FLOTTANT')then
        pval.valtyp='FLOTTANT'
c
      elseif(type .eq. 'ENTIER')then
        pval.valtyp='ENTIER'
c
      elseif(type .eq. 'LOGIQUE')then
        pval.valtyp='LOGIQUE'
c
      elseif(type .eq. 'CHAINE')then
        pval.valtyp='CHAINE'
c
      else
        call edmess('valnew: type inattendu '//type)
	goto 999
      endif
c
c     -- fermer le conteneur "values"
      segdes,pval*MOD
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('valnew: routine en echec')
      status=1
      return
      end
