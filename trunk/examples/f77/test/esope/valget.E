      subroutine valget(
     & pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : valget (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : lire les valeurs dans un conteneur "values"
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
      integer       increm
      parameter    (increm=100)
c
      integer     ierr
      integer     size
c
      integer     icond
      integer     ireto
      character*8 ctype
c
      integer           xent
      double precision  xflo
      logical           xlog
      pointeur          xcha.string
c
      status=0
c
c     -- vider le conteneur "values" avant lecture
      call valrma(pval,ierr)
      if(ierr .ne. 0) goto 999
c
c     -- ouvrir le conteneur "values"
      segact,pval
      valnf =pval.valf(/1)
      valne =pval.vale(/1)
      valnl =pval.vall(/1)
      valnc =pval.valc(/2)
      valsmx=pval.valc(/1)
c
c     -- lire les elements du conteneur "values"
      size=0
 100  continue
        icond=0
        call quetyp(ctype,icond,ireto)
        if(ireto .ne. 1) then
	  goto 101
	else
          if(ctype .ne. pval.valtyp)then
	    goto 101
          endif
	endif
c
        if(ctype .eq. 'FLOTTANT')then
          icond=1
          call lirree(xflo,icond,ireto)
c
          size=size+1
          if(size .gt. valnf)then
            valnf=valnf+increm
            segadj,pval
          endif
c
          pval.valf(size)=xflo
c
        elseif(ctype .eq. 'ENTIER')then
          icond=1
          call lirent(xent,icond,ireto)
c
          size=size+1
          if(size .gt. valne)then
            valne=valne+increm
            segadj,pval
          endif
c
          pval.vale(size)=xent
c
        elseif(ctype .eq. 'LOGIQUE')then
          icond=1
          call lirlog(xlog,icond,ireto)
c
          size=size+1
          if(size .gt. valnl)then
            valnl=valnl+increm
            segadj,pval
          endif
c
          pval.vall(size)=xlog
c
        elseif(ctype .eq. 'CHAINE')then
          call lirobj(ctype,xcha,icond,ireto)
          xcha=abs(xcha)
          segact,xcha
            lstg=xcha.cstg(/1)
c
            size=size+1
            if(size .gt. valnc)then
              valnc=valnc+increm
              segadj,pval
            endif
c
            if(lstg .gt. valsmx)then
              valsmx=lstg
              segadj,pval
	    endif
c
            pval.vals(size)=lstg
            pval.valc(size)=xcha.cstg
          segdes,xcha*NOMOD
        endif
c
        goto 100
 101  continue
c
c     -- re-ajuster la taille du VECTOR
      if(pval.valtyp .eq. 'FLOTTANT')then
	valnf=size
c
      elseif(pval.valtyp .eq. 'ENTIER')then
	valne=size
c
      elseif(pval.valtyp .eq. 'LOGIQUE')then
	valnl=size
c
      elseif(pval.valtyp .eq. 'CHAINE')then
	valnc=size
      endif
c
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
      call edmess('valget: routine en echec')
      status=1
      return
      end
