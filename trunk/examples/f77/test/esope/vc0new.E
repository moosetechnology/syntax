      subroutine vc0new(
     & ov,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vc0new (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : creer un VECTOR vide non type
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cas  ov     : un VECTOR
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
#include <vector.obj>
#include <verobj.seg>
c
      integer  ov
      integer  status
c
      integer  jord
c
      status=0
c
      call inistr(ov,'ACROPOLE',3,0)
c
      nvcflo = 0
      nvcent = 0
      nvclog = 0
      nvccha = 0
      cvccha = 0
      segini,tvecto
      tvecto.vectyp = ' '
      segdes,tvecto*MOD
      call ajpnt(ov,tvecto,'TVECTO  ',1,jord)
c
      segini,verobj
      ver = vvecto
      segdes,verobj*MOD
c
      call ajpnt(ov,verobj,'VEROBJ  ',2,jord)
      call ajpnt(ov,0     ,'NULL    ',3,jord)
c
      call desstr(ov,'MOD')
c
      return
      end
