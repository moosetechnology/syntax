      subroutine vcsiz(
c     == entrees ==
     & ov,
c     == sorties ==
     & size,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcsiz (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : retourner le nombre d'elements d'un objet VECTOR
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cas  size   : nombre d'elements d'un objet VECTOR
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
#include <vector.obj>
c
      integer  ov
      integer  size
      integer  status
      integer  ierr
c
      status=0
c
c     -- check version of vector v
      call vc0ckv(ov,ierr)
      if (ierr.ne.0) goto 999
c
      call actstr(ov)
      tvecto=mypnt(ov,1)
      segact,tvecto
      nvcflo=vecflo(/1)
      nvcent=vecent(/1)
      nvclog=veclog(/1)
      nvccha=veccha(/2)
      cvccha=veccha(/1)
c
      size=max(nvcflo,nvcent,nvclog,nvccha)
c
      segdes,tvecto*NOMOD
      call desstr(ov,'NOMOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcsiz: routine en echec')
      status=1
      return
      end
