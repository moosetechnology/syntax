      subroutine vcpck(ovp,ovx,ovi,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcpck (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Aout-2001
cb  Service    : EPDN 
cb  But        : compacter un VECTOR suivant un VECTOR index
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cas ovp    : VECTOR compacte
cae ovp    : VECTOR compacte initial (facultatif)
cae ovx    : VECTOR expanse
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
      integer     ovp
      integer     ovx
      integer     ovi
      integer     status
c
      integer     ovpnew
c
      pointeur    pvp.tvecto
      pointeur    pvx.tvecto
      pointeur    pvi.tvecto
c
      integer     vxsz
      integer     visz
      integer     vpsz
      integer     ierr
      integer     ip
      integer     ix
      integer     ii
      integer     k
c
      status=0
c
c     -- check version of vector vx
      call vc0ckv(ovx,ierr)
      if (ierr.ne.0) goto 999
c
c     -- check version of vector vi
      call vc0ckv(ovi,ierr)
      if (ierr.ne.0) goto 999
c
c     -- check version of vector vp
      if(ovp .ne. 0)then
        call vc0ckv(ovp,ierr)
        if (ierr.ne.0) goto 999
      endif
c
c     -- ouvrir le VECTOR vx
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
c     -- dupliquer et ouvrir le VECTOR vp
c     -- ou 
c     -- creer le VECTOR vp
      if(ovp .ne. 0)then
        call copstr(ovp,ovpnew)
	ovp=ovpnew
c
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
      else
        call vc0new(ovp,ierr)
        if(ierr.ne.0) goto 999
c
      	call actstr(ovp)
      	pvp=mypnt(ovp,1)
      	segact,pvp
c
      	nvcflo=pvp.vecflo(/1)
      	nvcent=pvp.vecent(/1)
      	nvclog=pvp.veclog(/1)
      	nvccha=pvp.veccha(/2)
      	cvccha=pvp.veccha(/1)
c
        vpsz=0
	do ii=1,visz
          k=pvi.vecent(ii)
	  if(k. gt. 0)then
	    vpsz=max(k,vpsz)
	  endif
	enddo
c
        pvp.vectyp=pvx.vectyp
c
        if(pvp.vectyp .eq. 'FLOTTANT')then
      	  nvcflo=vpsz
      	  segadj,pvp
	  do ip=1,vpsz
	    pvp.vecflo(ip)=0.
	  enddo
c
	elseif(pvp.vectyp .eq. 'ENTIER')then
      	  nvcent=vpsz
      	  segadj,pvp
	  do ip=1,vpsz
	    pvp.vecent(ip)=0
	  enddo
c
	elseif(pvp.vectyp .eq. 'LOGIQUE')then
      	  nvclog=vpsz
      	  segadj,pvp
	  do ip=1,vpsz
	    pvp.veclog(ip)=.false.
	  enddo
c
	elseif(pvp.vectyp .eq. 'CHAINE')then
      	  nvccha=vpsz
      	  cvccha=pvx.veccha(/1)
      	  segadj,pvp
	  do ip=1,vpsz
	    pvp.veccha(ip)=' '
	  enddo
	endif
      endif
c
c     -- verifier les types de vx, vp, et vi
      if(pvi.vectyp .ne. 'ENTIER')then
        call edmess('vcpck: VECTOR vi non ENTIER')
        goto 999
      endif
c
      if(pvp.vectyp .ne. pvx.vectyp)then
        call edmess('vcpck: VECTORs vp et vx de types differents')
        goto 999
      endif
c
c     -- verifier "size vx" et "size vi"
      if(vxsz .ne. visz)then
        call edmess('vcpck: VECTOR vi et vx de tailles incompatibles')
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
          call edmess('vcpck: valeur VECTOR vi hors vp')
          goto 999
        endif
c
        if(ip .gt. 0)then
c
          if(pvp.vectyp .eq. 'FLOTTANT')then
             pvp.vecflo(ip) = pvx.vecflo(ix)
          elseif
     &      (pvp.vectyp .eq. 'ENTIER')then
             pvp.vecent(ip) = pvx.vecent(ix)
          elseif
     &      (pvp.vectyp .eq. 'LOGIQUE')then
             pvp.veclog(ip) = pvx.veclog(ix)
          elseif
     &      (pvp.vectyp .eq. 'CHAINE')then
             pvp.veccha(ip) = pvx.veccha(ix)
          endif
c
        endif
      enddo
c
c     -- fermer le VECTOR vx
      segdes,pvx*NOMOD
      call desstr(ovx,'NOMOD')
c
c     -- fermer le VECTOR vi
      segdes,pvi*NOMOD
      call desstr(ovi,'NOMOD')
c
c     -- fermer le VECTOR vp
      segdes,pvp*MOD
      call desstr(ovp,'MOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcpck: routine en echec')
      status=1
      return
      end
