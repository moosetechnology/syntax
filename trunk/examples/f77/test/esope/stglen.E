      subroutine stglen(
c     == entrees ==
     & as,
c     == sorties ==
     & las,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : stglen (langage Esope)
cb  Module     : STRING
cb  Auteur     : L.Brault
cb  Date       : 28-Octobre-1997
cb  Service    : EPNC 
cb  But        : obtenir la longueur d'une chaine de caracteres
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  as     : "la" chaine
c
cas  las    : longueur de "la" chaine
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <string.seg>
c
      pointeur as.string
      integer  las
      integer  status
c
      pointeur xs.string
c
      status=0
      xs=abs(as)
      segact,xs
        las=xs.cstg(/1)
      segdes,xs
      return
      end
