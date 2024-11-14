      subroutine vcconc(
c     == entrees ==
     & ov,ov1,
c     == sorties ==
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcconc (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : M.Fraczkiewicz
cb  Date       : Aout-2014
cb  Service    : PEPD 
cb  But        : concatener deux vecteurs
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cae  ov1    : un objet VECTOR
cas  status : status d'execution (status=0 <=> succes)
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
#include <vector.obj>
#include <values.seg>
c
      integer     ov
      integer     ov1
      integer     status
      pointeur  pv.tvecto
      pointeur pv1.tvecto
      integer  nvcflo1, nvcent1, nvclog1, nvccha1, cvccha1
      integer  ierr, i
      status=0
c
c     -- check version of vector v
      call vc0ckv(ov,ierr)
      if (ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR
      call actstr(ov)
      pv=mypnt(ov,1)
      segact,pv
      nvcflo=pv.vecflo(/1)
      nvcent=pv.vecent(/1)
      nvclog=pv.veclog(/1)
      nvccha=pv.veccha(/2)
      cvccha=pv.veccha(/1)

c
c     -- check version of vector v1
      call vc0ckv(ov1,ierr)
      if (ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR
      call actstr(ov1)
      pv1=mypnt(ov1,1)
      segact,pv1
      nvcflo1=pv1.vecflo(/1)
      nvcent1=pv1.vecent(/1)
      nvclog1=pv1.veclog(/1)
      nvccha1=pv1.veccha(/2)
      cvccha1=pv1.veccha(/1)
      
*     on ne gere pas le cas ou` les chaines de caractere sont
*        de longueurs differentes
      if (cvccha .ne. cvccha1) then
	 call edmess('vcconc: valeurs de lenght differentes')
	 status=1
	 return
      endif
*     les deux vecteurs doivent contenir le meme type de donnees
      if (pv.vectyp .ne. pv1.vectyp) then
	 call edmess('vcconc: types de donnees differentes')
	 status=1
	 return
      endif


c     -- Agrandissement du vecteur ov
      nvcflo = nvcflo + nvcflo1
      nvcent = nvcent + nvcent1
      nvclog = nvclog + nvclog1
      nvccha = nvccha + nvccha1
      segadj,pv

c     -- Copie des donnees de ov1 dans ov
      do i=1, nvcflo1
         pv.vecflo(nvcflo-nvcflo1+i) = pv1.vecflo(i)
      enddo
      do i=1, nvcent1
         pv.vecent(nvcent-nvcent1+i) = pv1.vecent(i)
      enddo
      do i=1, nvclog1
         pv.veclog(nvclog-nvclog1+i) = pv1.veclog(i)
      enddo
      do i=1, nvccha1
         pv.veccha(nvccha-nvccha1+i) = pv1.veccha(i)
      enddo

c     -- fermer les VECTOR
      segdes,pv*MOD
      call desstr(ov,'MOD')
      segdes,pv1*NOMOD
      call desstr(ov1,'NOMOD')
c
c     -- terminer avec succes      
      return

 999  continue      
      call edmess('vcconc: routine en echec')
      status=1
      return
      end
