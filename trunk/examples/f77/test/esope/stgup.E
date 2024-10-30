      subroutine stgup(
c     == entrees ==
     & as,
c     == sorties ==
     & rs,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : stgup (langage Esope)
cb  Module     : STRING
cb  Auteur     : L.Brault
cb  Date       : 28-Octobre-1997
cb  Service    : EPNC 
cb  But        : convertir les lettres d'une chaine en majuscules
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  as     : "la" chaine
c
cas  rs     : conversion de "la" chaine
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <string.seg>
c
      pointeur as.string
      pointeur rs.string
      integer  status
c
      pointeur xs.string
c
      status=0
      xs=abs(as)
      segini,rs=xs
        call uprtxt(rs.cstg)
      segdes,rs
      return
      end
