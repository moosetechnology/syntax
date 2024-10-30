      subroutine vc0ckv(
     & ov,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vc0ckv (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : verifier la version d'un VECTOR
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un VECTOR
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  verver
      external verver
c
#include <vector.obj>
c
      integer  ov
      integer  status
c
      integer  jord
c
      status=0
c
      if(verver(ov,vvecto,'VECTOR  ',0) .ne. 0)then
        call edmess(  '*** vc0ckv: *** VECTOR de version inattendue ')
        call erreur(4,'*** VECTOR: *** module en echec')
      endif
c
      call desstr(ov,'NOMOD')
c
      return
      end
