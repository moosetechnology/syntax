      subroutine vcunq(ov,ov1,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcunq (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : supprimer les copies adjacentes d'elements 
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
      integer     k
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
	k=0
        if(nvcflo .ge. 1)then
          pv.vecflo(1)=pv1.vecflo(1)
	  k=1
	  do i=2,nvcflo
	    if(pv1.vecflo(i) .ne. pv.vecflo(k))then
	      k=k+1
	      pv.vecflo(k)=pv1.vecflo(i)
	    endif
	  enddo
	endif
        nvcflo=k
        segadj,pv
c
      elseif(pv1.vectyp .eq. 'ENTIER')then
        segadj,pv
	pv.vectyp = pv1.vectyp
	k=0
        if(nvcent .ge. 1)then
          pv.vecent(1)=pv1.vecent(1)
	  k=1
	  do i=2,nvcent
	    if(pv1.vecent(i) .ne. pv.vecent(k))then
	      k=k+1
	      pv.vecent(k)=pv1.vecent(i)
	    endif
	  enddo
	endif
        nvcent=k
        segadj,pv
c
      elseif(pv1.vectyp .eq. 'LOGIQUE')then
        segadj,pv
	pv.vectyp = pv1.vectyp
	k=0
        if(nvclog .ge. 1)then
          pv.veclog(1)=pv1.veclog(1)
	  k=1
	  do i=2,nvclog
	    if(pv1.veclog(i) .neqv. pv.veclog(k))then
	      k=k+1
	      pv.veclog(k)=pv1.veclog(i)
	    endif
	  enddo
	endif
        nvclog=k
        segadj,pv
c
      elseif(pv1.vectyp .eq. 'CHAINE')then
        segadj,pv
	pv.vectyp = pv1.vectyp
	k=0
        if(nvccha .ge. 1)then
          pv.veccha(1)=pv1.veccha(1)
	  k=1
	  do i=2,nvccha
	    if(pv1.veccha(i) .ne. pv.veccha(k))then
	      k=k+1
	      pv.veccha(k)=pv1.veccha(i)
	    endif
	  enddo
	endif
        nvccha=k
        segadj,pv
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
      call edmess('vcunq: routine en echec')
      status=1
      return
      end
