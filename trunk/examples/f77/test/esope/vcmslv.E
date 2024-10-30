      subroutine vcmslv(ov,ov1,pshp1,ov2,isolve,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcmslv (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : resoudre : Matrix x Vector -> Vector 
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov1    : VECTOR/MATRIX argument
cae  pshp1  : shape ov1
cae  ov2    : VECTOR argument
cae  isolve : option de resolution
cas  ov     : VECTOR resultat
cas  status : status d'execution (status=0 <=> succes)
c
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
      pointeur    pshp1.values
      integer     ov2
      integer     isolve
      integer     status
c
      pointeur    pv.tvecto
      pointeur    pv1.tvecto
      pointeur    pv2.tvecto
c
      character*8 type
      integer     v1sz
      integer     v2sz
      integer     ierr
      integer     il
      integer     ic
      integer     nl
      integer     nc
      integer     ipos
c
      double precision xflo      
c
c     sln : ordre du systeme lineaire
      integer sln      
c
      segment,sl
c       sla : coefficients de la matrice
        real sla(sln,sln)
c
c       slb : second membre du systeme
        real slb(sln)
c
c       sltmp : zone de travail
        integer sltmp(sln)
c
c       sltyp : type de pivot 
c               0: pivot max
c               1: sans pivot
c               2: pivot max par colonne
c              -1: sans pivot ( on bascule en 0 si pivot nul )
c              -2: pivot colonne (  on bascule en 0 si pivot nul )
        integer sltyp
c
c       slx : solution du systeme
        real slx(sln)
c
c       slerr : status  
c               0: correct
c               1: matrice singuliere
c               2: pivot nul
c               3: dimension incorrecte
        integer slerr
      endsegment
c
      status=0
c
c     -- check version of vector v1
      call vc0ckv(ov1,ierr)
      if (ierr.ne.0) goto 999
c
c     -- check version of vector v2
      call vc0ckv(ov2,ierr)
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
c     -- ouvrir le VECTOR v2
      call actstr(ov2)
      pv2=mypnt(ov2,1)
      segact,pv2
      nvcflo=pv2.vecflo(/1)
      nvcent=pv2.vecent(/1)
      nvclog=pv2.veclog(/1)
      nvccha=pv2.veccha(/2)
      cvccha=pv2.veccha(/1)
      v2sz=max(nvcflo,nvcent,nvclog,nvccha)
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
      v1sz=max(nvcflo,nvcent,nvclog,nvccha)
c
c     -- verifier les types de v1 et v2
      if(pv1.vectyp .ne. 'FLOTTANT')then
        call edmess('vcmslv: VECTOR v1 non FLOTTANT')
	goto 999
      endif
c
      if(pv2.vectyp .ne. 'FLOTTANT')then
        call edmess('vcmslv: VECTOR v2 non FLOTTANT')
	goto 999
      endif
c
c     -- ouvrir "shape v1"
      if(pshp1 .eq. 0)then
        call edmess('vcmslv: shape VECTOR v1 non fournie')
        goto 999
      endif
      segact,pshp1
c
c     -- verifier "shape v1"
      if(pshp1.vale(/1) .ne. 2)then
        call edmess('vcmslv: shape VECTOR v1 pas de dimension 2')
        goto 999
      endif
c
      if(pshp1.vale(1)*pshp1.vale(2) .ne. v1sz)then
        call edmess('vcmslv: shape et size VECTOR v1 incompatibles')
        goto 999
      endif
c
      if(pshp1.vale(1) .ne. pshp1.vale(2))then
        call edmess('vcmslv: shape VECTOR v1 non carree')
        goto 999
      endif
c
c     -- verifier "size v2 et shape v1"
      if(pshp1.vale(2) .ne. v2sz)then
        call edmess('vcmslv: shape v1 et size v2 incompatibles')
        goto 999
      endif
c
c     -- recuperer les nombres de lignes et de colonnees
      nl=pshp1.vale(1)
      nc=pshp1.vale(2)
c
c     -- typer et re-dimensionner le VECTOR v
      pv.vectyp = 'FLOTTANT'
      nvcflo=nl
      segadj,pv
c
c     -- sous-traiter la resolution du systeme par Gauss
      sln=nl
      segini,sl
c
      do il=1,nl
        do ic=1,nc
	  ipos=(ic -1)*nl + il
	  sla(il,ic)=pv1.vecflo(ipos)
	enddo
      enddo
c
      do il=1,nl
	slb(il)=pv2.vecflo(il)
      enddo
c
      sltyp=isolve
      call rslpm(sln,sla,slx,slb,sltmp,sltyp,slerr) 
      if(slerr .ne. 0)then
        call edmess('vcmslv: routine rslpm en echec')
c
	if(slerr .eq. 1)then
          call edmess('vcmslv: matrice singuliere')
c
	elseif(slerr .eq. 2)then
          call edmess('vcmslv: pivot nul')
c
	elseif(slerr .eq. 3)then
          call edmess('vcmslv: dimension incorrecte')
	endif
c
        goto 999
      endif
c
      do il=1,nl
	pv.vecflo(il)=slx(il)
      enddo
c
      segsup,sl
c
c     -- fermer le VECTOR v1
      segdes,pv1*NOMOD
      call desstr(ov1,'NOMOD')
c
c     -- fermer "shape v1"
      segdes,pshp1*NOMOD
c
c     -- fermer le VECTOR v2
      segdes,pv2*NOMOD
      call desstr(ov2,'NOMOD')
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
      call edmess('vcmslv: routine en echec')
      status=1
      return
      end
