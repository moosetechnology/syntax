      subroutine vcrvs(ov,ov1,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcrvs (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : renverser l'ordre des elements 
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov1    : VECTOR argument
cas  ov     : VECTOR resultat
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
#include <vector.obj>
c
      integer     ov
      integer     ov1
      integer     status
c
      pointeur    pv.tvecto
      pointeur    pv1.tvecto
c
      integer     ierr
      integer     i
c
      status=0
c
c     -- check version of vector v1
      call vc0ckv(ov1,ierr)
      if (ierr.ne.0) goto 999
c
c     -- creer un VECTOR vide non type
      call vc0new(ov,ierr)
      if(ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR v
      call actstr(ov)
      pv=mypnt(ov,1)
      segact,pv
      nvcflo=pv.vecflo(/1)
      nvcent=pv.vecent(/1)
      nvclog=pv.veclog(/1)
      nvccha=pv.veccha(/2)
      cvccha=pv.veccha(/1)
c
c     -- ouvrir le VECTOR v1
      call actstr(ov1)
      pv1=mypnt(ov1,1)
      segact,pv1
      nvcflo=pv1.vecflo(/1)
      nvcent=pv1.vecent(/1)
      nvclog=pv1.veclog(/1)
      nvccha=pv1.veccha(/2)
      cvccha=pv1.veccha(/1)
c
c     -- creer les nouveaux elements
      if(pv1.vectyp .eq. 'FLOTTANT')then
        segadj,pv
	pv.vectyp = pv1.vectyp
	do i=1,nvcflo
          pv.vecflo(i)=pv1.vecflo(nvcflo-i+1)
	enddo
c
      elseif(pv1.vectyp .eq. 'ENTIER')then
        segadj,pv
	pv.vectyp = pv1.vectyp
	do i=1,nvcent
          pv.vecent(i)=pv1.vecent(nvcent-i+1)
	enddo
c
      elseif(pv1.vectyp .eq. 'LOGIQUE')then
        segadj,pv
	pv.vectyp = pv1.vectyp
	do i=1,nvclog
          pv.veclog(i)=pv1.veclog(nvclog-i+1)
	enddo
c
      elseif(pv1.vectyp .eq. 'CHAINE')then
        segadj,pv
	pv.vectyp = pv1.vectyp
	do i=1,nvccha
          pv.veccha(i)=pv1.veccha(nvccha-i+1)
	enddo
      endif
c
c     -- fermer le VECTOR v1
      segdes,pv1*NOMOD
      call desstr(ov1,'NOMOD')
c
c     -- fermer le VECTOR v
      segdes,pv*MOD
      call desstr(ov,'MOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcrvs: routine en echec')
      status=1
      return
      end
