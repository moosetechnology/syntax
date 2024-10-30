      subroutine valput(
     & pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : valput (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : ecrire les valeurs depuis un conteneur "values"
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cas  pval   : un conteneur "values"
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <values.seg>
#include <string.seg>
c
      pointeur      pval.values
      integer       status
c
      integer           i
c
      integer           xent
      double precision  xflo
      logical           xlog
      pointeur          xcha.string
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
c     -- ecrire les elements du conteneur "values"
      if(pval.valtyp .eq. 'FLOTTANT')then
        do i=valnf,1,-1
          xflo=pval.valf(i)
          call ecrree(xflo)
	enddo
c
      elseif(pval.valtyp .eq. 'ENTIER')then
        do i=valne,1,-1
          xent=pval.vale(i)
          call ecrent(xent)
	enddo
c
      elseif(pval.valtyp .eq. 'LOGIQUE')then
        do i=valnl,1,-1
          xlog=pval.vall(i)
          call ecrlog(xlog)
	enddo
c
      elseif(pval.valtyp .eq. 'CHAINE')then
        do i=valnc,1,-1
          lstg=pval.vals(i)
	  segini,xcha
	    xcha.cstg=pval.valc(i)
	  segdes,xcha*MOD
          call ecrobj('CHAINE  ',xcha)
	enddo
      endif
c
c     -- fermer le conteneur "values"
      segdes,pval*NOMOD
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('valput: routine en echec')
      status=1
      return
      end
