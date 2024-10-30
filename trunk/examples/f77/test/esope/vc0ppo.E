      subroutine vc0ppo(pshp,ppos,ipos,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vc0ppo (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : retourner une multi-position equivalente a une pos. 1D 
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  pshp   : shape
cae  ipos   : position 1D equivalente
cas  ppos   : multi-position
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <values.seg>
c
      pointeur pshp.values
      pointeur ppos.values
      integer  shpsiz
      integer  status
      integer  ipos
c
      character*8 vatp
      integer     ierr
c
      integer iposmx
      integer ipm1
      integer pm1
      integer d
      integer i
c
      status=0
c
c     -- ouvrir "shape"
      if(pshp .ne. 0)then
        segact,pshp
        shpsiz =pshp.vale(/1)
      else
        shpsiz =0
      endif
c
c     -- creer a vide un conteneur "values"
      vatp='ENTIER'
      call valnew(ppos,vatp,ierr)
      if(ierr .ne. 0) goto 999
c
c     -- ouvrir le conteneur "values"
      segact,ppos
      valnf =ppos.valf(/1)
      valne =ppos.vale(/1)
      valnl =ppos.vall(/1)
      valnc =ppos.valc(/2)
      valsmx=ppos.valc(/1)
c
c     -- determiner les indices de la position
      if(shpsiz .ne. 0)then
c
c       // avec emulation multi-indice
        valne=shpsiz
        segadj,ppos
c
        iposmx=1
        do i=1,shpsiz
          iposmx=iposmx*pshp.vale(i)
	enddo
c
	if(.not.(ipos.ge.1 .and. ipos.le.iposmx))then
c         // multi-indice nul
          do i=1,shpsiz
            ppos.vale(i)=0
	  enddo
	else
c         // multi-indice non-nul
          ipm1=ipos-1
          do i=1,shpsiz
	    d=pshp.vale(i)
	    pm1=mod(ipm1,d)
            ppos.vale(i)=pm1+1
	    ipm1=(ipm1-pm1)/d
	  enddo
	endif
c
      else
c       // sans emulation multi-indice
        valne=1
        segadj,ppos
        ppos.vale(valne)=max(ipos,0)
      endif
c
c     -- fermer "shape"
      if(pshp .ne. 0) segdes,pshp*NOMOD
c
c     -- fermer "position"
      segdes,ppos*MOD
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vc0ppo: routine en echec')
      ipos=0
      status=1
      return
      end
