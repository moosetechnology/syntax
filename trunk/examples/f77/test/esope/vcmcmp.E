      subroutine vcmcmp(ov,ov1,pshp1,ov2,pshp2,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcmcmp (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : composer : Matrix x Matrix -> Matrix 
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov1    : VECTOR/MATRIX argument
cae  pshp1  : shape ov1
cae  ov2    : VECTOR/MATRIX argument
cae  pshp2  : shape ov2
cas  ov     : VECTOR/MATRIX resultat
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
      pointeur    pshp2.values
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
      integer     il,il1,il2
      integer     ic,ic1,ic2
      integer     nl,nl1,nl2
      integer     nc,nc1,nc2
      integer     ipos,ipos1,ipos2
c
      double precision xflo      
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
        call edmess('vcmcmp: VECTOR v1 non FLOTTANT')
	goto 999
      endif
c
      if(pv2.vectyp .ne. 'FLOTTANT')then
        call edmess('vcmcmp: VECTOR v2 non FLOTTANT')
	goto 999
      endif
c
c     -- ouvrir "shape v1"
      if(pshp1 .eq. 0)then
        call edmess('vcmcmp: shape VECTOR v1 non fournie')
        goto 999
      endif
      segact,pshp1
c
c     -- verifier "shape v1"
      if(pshp1.vale(/1) .ne. 2)then
        call edmess('vcmcmp: shape VECTOR v1 pas de dimension 2')
        goto 999
      endif
c
c     -- verifier "shape v1" et "size v1"
      if(pshp1.vale(1)*pshp1.vale(2) .ne. v1sz)then
        call edmess('vcmcmp: shape et size VECTOR v1 incompatibles')
        goto 999
      endif
c
c     -- ouvrir "shape v2"
      if(pshp2 .eq. 0)then
        call edmess('vcmcmp: shape VECTOR v2 non fournie')
        goto 999
      endif
      segact,pshp2
c
c     -- verifier "shape v2"
      if(pshp2.vale(/1) .ne. 2)then
        call edmess('vcmcmp: shape VECTOR v2 pas de dimension 2')
        goto 999
      endif
c
c     -- verifier "shape v2" et "size v2"
      if(pshp2.vale(1)*pshp2.vale(2) .ne. v2sz)then
        call edmess('vcmcmp: shape et size VECTOR v2 incompatibles')
        goto 999
      endif
c
c     -- verifier compatibilite "shape v1" et "shape v2"
      if(pshp1.vale(2) .ne. pshp2.vale(1))then
        call edmess('vcmcmp: shape v1 et shape v2 incompatibles')
        goto 999
      endif
c
c     -- recuperer les nombres de lignes et de colonnees
      nl1=pshp1.vale(1)
      nc1=pshp1.vale(2)
c
      nl2=pshp2.vale(1)
      nc2=pshp2.vale(2)
c
c     -- typer et re-dimensionner le VECTOR v
      pv.vectyp = 'FLOTTANT'
      nl=nl1
      nc=nc2
      nvcflo=(nl*nc)
      segadj,pv
c
c     -- creer les nouveaux elements
      do il=1,nl
        do ic=1,nc
	  ipos=(ic -1)*nl + il
          xflo=0.
c
          il1=il
	  ic2=ic
          do ic1=1,nc1
	    il2=ic1
	    ipos1=(ic1 -1)*nl1 + il1
	    ipos2=(ic2 -1)*nl2 + il2
	    xflo=xflo + pv1.vecflo(ipos1)*pv2.vecflo(ipos2)
	  enddo
c
	  pv.vecflo(ipos)=xflo
	enddo
      enddo
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
c     -- fermer "shape v2"
      segdes,pshp2*NOMOD
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
      call edmess('vcmcmp: routine en echec')
      status=1
      return
      end
