      subroutine stgget(
c     == entrees ==
     & as,first,last,step,
c     == sorties ==
     & rs,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : stgget (langage Esope)
cb  Module     : STRING
cb  Auteur     : L.Brault
cb  Date       : 28-Octobre-1997
cb  Service    : EPNC 
cb  But        : obtenir une partie d'une chaine de caracteres
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  as     : "la" chaine
cae  first  : debut de la partie de "la" chaine
cae  last   : fin   de la partie de "la" chaine
cae  step   : pas   de la partie de "la" chaine
c
cas  rs     : partie de "la" chaine
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <string.seg>
c
      pointeur as.string
      integer  first,last,step
      pointeur rs.string
      integer  status
c
      pointeur xs.string
      integer  lxs,lrs
      integer  i,j
c
      status=0
      xs=abs(as)
      segact,xs
      lxs=xs.cstg(/1)
c
c     -- determiner la longueur de la chaine destination
      lrs=0
      if(step.ne.0)then
        do i=first,last,step
          if((i.ge.1).and.(i.le.lxs)) lrs=lrs+1
        enddo
      endif
c
c     -- creer et remplir la chaine destination
      lstg=lrs
      segini,rs
      if(step.ne.0)then
        j=0
        do i=first,last,step
          if((i.ge.1).and.(i.le.lxs)) then
            j=j+1
            rs.cstg(j:j)=xs.cstg(i:i)
          endif
        enddo
      endif
      segdes,rs
c
      segdes,xs
      return
      end
