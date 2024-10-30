      subroutine vc0pos(pshp,ppos,ipos,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vc0pos (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : retourner position 1D equivalente a une multi-position
c
cm   auteur    : L.Brault
cm   service   : EPDN
cm   date      : 31-05-2001
cm   objet     : corriger l'implementation indice hors domaine
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  pshp   : shape
cae  ppos   : multi-position
cas  ipos   : position 1D equivalente
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <values.seg>
c
      pointeur pshp.values
      pointeur ppos.values
      integer  shpsiz
      integer  possiz
      integer  status
      integer  ipos
c
      integer i
      integer p
      integer d
      integer daccum
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
c     -- ouvrir "position"
      if(ppos .ne. 0)then
        segact,ppos
        possiz =ppos.vale(/1)
      else
        possiz =0
      endif
c
c     -- verifier la coherence entre shape et position
      if(shpsiz .ne. 0)then
        if(possiz .ne. shpsiz)then
          call edmess('vc0pos: position et shape non compatibles')
          goto 999
        endif
      else
        if(possiz .ne. 1)then
          call edmess('vc0pos: position non singleton')
          goto 999
        endif
      endif
c
c     -- determiner la position 1D equivalente
      if(shpsiz .ne. 0)then
c       // avec emulation multi-indice
        ipos=1
        daccum=1
        do i=1,possiz
          if(ipos .gt. 0)then
            p=ppos.vale(i)
            d=pshp.vale(i)
c
            if(.not. (p.ge.1 .and. p.le.d))then
c             -- position hors domaine
              ipos=0
            else
              ipos=ipos+(p-1)*daccum
              daccum=daccum*d
            endif
          endif
c
        enddo
      else
c       // sans emulation multi-indice
        ipos=ppos.vale(1)
      endif
c
c     -- fermer "shape"
      if(pshp .ne. 0) segdes,pshp*NOMOD
c
c     -- fermer "position"
      if(ppos .ne. 0) segdes,ppos*NOMOD
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vc0pos: routine en echec')
      ipos=0
      status=1
      return
      end
