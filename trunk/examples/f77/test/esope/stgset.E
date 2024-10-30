      subroutine stgset(
c     == entrees/sorties ==
     & as,
c     == entrees ==
     & first,last,step,vs,
c     == sorties ==
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : stgset (langage Esope)
cb  Module     : STRING
cb  Auteur     : L.Brault
cb  Date       : 28-Octobre-1997
cb  Service    : EPNC 
cb  But        : modifier une partie d'une chaine de caracteres
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  as     : "la" chaine
cae  first  : debut de la partie de "la" chaine
cae  last   : fin   de la partie de "la" chaine
cae  step   : pas   de la partie de "la" chaine
cae  vs     : valeur modifiant la partie de "la" chaine
c
cae  as     : "la" chaine
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <string.seg>
c
      pointeur as.string
      integer  first,last,step
      pointeur vs.string
      integer  status
c
      pointeur xs.string
      pointeur ys.string
      integer  lxs,lys
      integer  i,j
c
      status=0
      xs=as
      if(xs.lt.0)then
        call edmess('*** stgset *** cannot modify constant string')
        call edmess('*** stgset *** routine failed')
        status=1
        return
      endif
      segact,xs
      lxs=xs.cstg(/1)
c
      ys=abs(vs)
      segact,ys
      lys=ys.cstg(/1)
c
      if(step.ne.0)then
        j=0
        do i=first,last,step
          if((i.ge.1).and.(i.le.lxs)) then
            j=j+1
            if(j.le.lys)then
              xs.cstg(i:i)=ys.cstg(j:j)
            endif
          endif
        enddo
      endif
c
      segdes,ys
      segdes,xs
c
      return
      end
