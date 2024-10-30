      subroutine vcupck(ovx,ovp,ovi,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcupck (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Aout-2001
cb  Service    : EPDN 
cb  But        : decompacter un VECTOR suivant un VECTOR index
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cas ovx    : VECTOR expanse
cae ovx    : VECTOR expanse initial (facultatif)
cae ovp    : VECTOR compacte
cae ovi    : VECTOR index
cas status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
#include <vector.obj>
#include <values.seg>
c
      integer     ovx
      integer     ovp
      integer     ovi
      integer     status
c
      integer     ovxnew
c
      pointeur    pvx.tvecto
      pointeur    pvp.tvecto
      pointeur    pvi.tvecto
c
      integer     vxsz
      integer     visz
      integer     vpsz
      integer     ierr
      integer     ip
      integer     ix
      integer     ii
c
      status=0
c
c     -- check version of vector vx
      if(ovx .ne. 0)then
        call vc0ckv(ovx,ierr)
        if (ierr.ne.0) goto 999
      endif
c
c     -- check version of vector vp
      call vc0ckv(ovp,ierr)
      if (ierr.ne.0) goto 999
c
c     -- check version of vector vi
      call vc0ckv(ovi,ierr)
      if (ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR vp
      call actstr(ovp)
      pvp=mypnt(ovp,1)
      segact,pvp
      nvcflo=pvp.vecflo(/1)
      nvcent=pvp.vecent(/1)
      nvclog=pvp.veclog(/1)
      nvccha=pvp.veccha(/2)
      cvccha=pvp.veccha(/1)
      vpsz=max(nvcflo,nvcent,nvclog,nvccha)
c
c     -- ouvrir le VECTOR vi
      call actstr(ovi)
      pvi=mypnt(ovi,1)
      segact,pvi
      nvcflo=pvi.vecflo(/1)
      nvcent=pvi.vecent(/1)
      nvclog=pvi.veclog(/1)
      nvccha=pvi.veccha(/2)
      cvccha=pvi.veccha(/1)
      visz=max(nvcflo,nvcent,nvclog,nvccha)
c
c     -- dupliquer et ouvrir le VECTOR vx
c     -- ou 
c     -- creer le VECTOR vx
      if(ovx .ne. 0)then
        call copstr(ovx,ovxnew)
	ovx=ovxnew
c
      	call actstr(ovx)
      	pvx=mypnt(ovx,1)
      	segact,pvx
      	nvcflo=pvx.vecflo(/1)
      	nvcent=pvx.vecent(/1)
      	nvclog=pvx.veclog(/1)
      	nvccha=pvx.veccha(/2)
      	cvccha=pvx.veccha(/1)
      	vxsz=max(nvcflo,nvcent,nvclog,nvccha)
c
      else
        call vc0new(ovx,ierr)
        if(ierr.ne.0) goto 999
c
      	call actstr(ovx)
      	pvx=mypnt(ovx,1)
      	segact,pvx
c
      	nvcflo=pvx.vecflo(/1)
      	nvcent=pvx.vecent(/1)
      	nvclog=pvx.veclog(/1)
      	nvccha=pvx.veccha(/2)
      	cvccha=pvx.veccha(/1)
c
        vxsz=visz
c
        pvx.vectyp=pvp.vectyp
c
        if(pvx.vectyp .eq. 'FLOTTANT')then
      	  nvcflo=vxsz
      	  segadj,pvx
	  do ix=1,vxsz
	    pvx.vecflo(ix)=0.
	  enddo
c
	elseif(pvx.vectyp .eq. 'ENTIER')then
      	  nvcent=vxsz
      	  segadj,pvx
	  do ix=1,vxsz
	    pvx.vecent(ix)=0
	  enddo
c
	elseif(pvx.vectyp .eq. 'LOGIQUE')then
      	  nvclog=vxsz
      	  segadj,pvx
	  do ix=1,vxsz
	    pvx.veclog(ix)=.false.
	  enddo
c
	elseif(pvx.vectyp .eq. 'CHAINE')then
      	  nvccha=vxsz
      	  cvccha=pvp.veccha(/1)
      	  segadj,pvx
	  do ix=1,vxsz
	    pvx.veccha(ix)=' '
	  enddo
	endif
      endif
c
c     -- verifier les types de vx, vp, et vi
      if(pvi.vectyp .ne. 'ENTIER')then
        call edmess('vcupck: VECTOR vi non ENTIER')
        goto 999
      endif
c
      if(pvp.vectyp .ne. pvx.vectyp)then
        call edmess('vcupck: VECTORs vp et vx de types differents')
        goto 999
      endif
c
c     -- verifier "size vx" et "size vi"
      if(vxsz .ne. visz)then
        call edmess('vcupck: VECTOR vi et vx de tailles incompatibles')
        goto 999
      endif
c
c     -- compacter les elements
      do ii=1,visz
c
        ix=ii
        ip=pvi.vecent(ii)
c
        if(ip .gt. vpsz)then
          call edmess('vcupck: valeur VECTOR vi hors vp')
          goto 999
        endif
c
        if(ip .gt. 0)then
c
          if(pvx.vectyp .eq. 'FLOTTANT')then
             pvx.vecflo(ix) = pvp.vecflo(ip)
          elseif
     &      (pvx.vectyp .eq. 'ENTIER')then
             pvx.vecent(ix) = pvp.vecent(ip)
          elseif
     &      (pvx.vectyp .eq. 'LOGIQUE')then
             pvx.veclog(ix) = pvp.veclog(ip)
          elseif
     &      (pvx.vectyp .eq. 'CHAINE')then
             pvx.veccha(ix) = pvp.veccha(ip)
          endif
c
        endif
      enddo
c
c     -- fermer le VECTOR vi
      segdes,pvi*NOMOD
      call desstr(ovi,'NOMOD')
c
c     -- fermer le VECTOR vp
      segdes,pvp*NOMOD
      call desstr(ovp,'NOMOD')
c
c     -- fermer le VECTOR vx
      segdes,pvx*MOD
      call desstr(ovx,'MOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcupck: routine en echec')
      status=1
      return
      end
