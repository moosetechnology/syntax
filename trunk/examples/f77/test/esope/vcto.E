      subroutine vcto(ov,ov1,type,typlen,format,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcto (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : convertir les elements du VECTOR 
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov1    : VECTOR argument
cae  oper   : operation unaire
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
      integer      ov
      integer      ov1
      character*8  type
      integer      typlen
      character*16 format
      integer      status
c
      pointeur     pv.tvecto
      pointeur     pv1.tvecto
c
      character*8      v1tp
      integer          v1sz
      character*(16+2) fmt
      integer          ierr
      integer          i
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
      v1sz=max(nvcflo,nvcent,nvclog,nvccha)
c
c     -- verifier les options de conversion
      if(type .eq. 'CHAINE')then
        if(typlen .lt. 0)then
          call edmess('vcto: longueur de chaine non fournie')
	  goto 999
	endif
c
        if(format .eq. '')then
          call edmess('vcto: format de chaine non fournie')
	  goto 999
	endif
      else
        if(.not.(typlen .lt. 0))then
          call edmess('vcto: longueur de chaine inattendue')
	  goto 999
	endif
c
        if(.not.(format .eq. ''))then
          call edmess('vcto: format de chaine inattendu')
	  goto 999
	endif
      endif
c
c     -- creer les nouveaux elements d'apres l'operation unaire
      if(pv1.vectyp .eq. 'FLOTTANT')then
c
        if(type .eq. 'FLOTTANT')then
          nvcflo=v1sz
          nvcent=0
          nvclog=0
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            pv.vecflo(i)=pv1.vecflo(i)
	  enddo
c
        elseif(type .eq. 'ENTIER')then
          nvcflo=0
          nvcent=v1sz
          nvclog=0
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            pv.vecent(i)=pv1.vecflo(i)
	  enddo
c
        elseif(type .eq. 'LOGIQUE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            pv.veclog(i)=(pv1.vecflo(i) .ne. 0.)
	  enddo
c
        elseif(type .eq. 'CHAINE')then
          nvcflo=0
          nvcent=0
          nvclog=0
          nvccha=v1sz
          cvccha=typlen
          segadj,pv
	  pv.vectyp = type
c
          fmt='('//format//')'
	  do i=1,v1sz
            write(pv.veccha(i),fmt) pv1.vecflo(i)
	  enddo
	else
          goto 998
	endif
c
      elseif(pv1.vectyp .eq. 'ENTIER')then
c
        if(type .eq. 'FLOTTANT')then
          nvcflo=v1sz
          nvcent=0
          nvclog=0
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            pv.vecflo(i)=pv1.vecent(i)
	  enddo
c
        elseif(type .eq. 'ENTIER')then
          nvcflo=0
          nvcent=v1sz
          nvclog=0
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            pv.vecent(i)=pv1.vecent(i)
	  enddo
c
        elseif(type .eq. 'LOGIQUE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            pv.veclog(i)=(pv1.vecent(i) .ne. 0)
	  enddo
c
        elseif(type .eq. 'CHAINE')then
          nvcflo=0
          nvcent=0
          nvclog=0
          nvccha=v1sz
          cvccha=typlen
          segadj,pv
	  pv.vectyp = type
c
          fmt='('//format//')'
	  do i=1,v1sz
            write(pv.veccha(i),fmt) pv1.vecent(i)
	  enddo
	else
          goto 998
	endif
c
      elseif(pv1.vectyp .eq. 'LOGIQUE')then
c
        if(type .eq. 'FLOTTANT')then
          nvcflo=v1sz
          nvcent=0
          nvclog=0
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
	    if(pv1.veclog(i))then
              pv.vecflo(i)=1.
	    else
              pv.vecflo(i)=0.
	    endif
	  enddo
c
        elseif(type .eq. 'ENTIER')then
          nvcflo=0
          nvcent=v1sz
          nvclog=0
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
	    if(pv1.veclog(i))then
              pv.vecent(i)=1
	    else
              pv.vecent(i)=0
	    endif
	  enddo
c
        elseif(type .eq. 'LOGIQUE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            pv.veclog(i)=pv1.veclog(i)
	  enddo
c
        elseif(type .eq. 'CHAINE')then
          nvcflo=0
          nvcent=0
          nvclog=0
          nvccha=v1sz
          cvccha=typlen
          segadj,pv
	  pv.vectyp = type
c
          fmt='('//format//')'
	  do i=1,v1sz
            write(pv.veccha(i),fmt) pv1.veclog(i)
	  enddo
	else
          goto 998
	endif
c
      elseif(pv1.vectyp .eq. 'CHAINE')then
c
        if(type .eq. 'FLOTTANT')then
          nvcflo=v1sz
          nvcent=0
          nvclog=0
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            read(pv1.veccha(i),*) pv.vecflo(i)
	  enddo
c
        elseif(type .eq. 'ENTIER')then
          nvcflo=0
          nvcent=v1sz
          nvclog=0
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            read(pv1.veccha(i),*) pv.vecent(i)
	  enddo
c
        elseif(type .eq. 'LOGIQUE')then
          nvcflo=0
          nvcent=0
          nvclog=v1sz
          nvccha=0
          cvccha=0
          segadj,pv
	  pv.vectyp = type
c
	  do i=1,v1sz
            read(pv1.veccha(i),*) pv.veclog(i)
	  enddo
c
        elseif(type .eq. 'CHAINE')then
          nvcflo=0
          nvcent=0
          nvclog=0
          nvccha=v1sz
          cvccha=typlen
          segadj,pv
	  pv.vectyp = type
c
          fmt='('//format//')'
	  do i=1,v1sz
            write(pv.veccha(i),fmt) pv1.veccha(i)
	  enddo
	else
          goto 998
	endif
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
 998  continue      
        v1tp=pv1.vectyp
        call edmess('vcto: conversion non disponible sur ce type')
        call edmess('vcto:       type = '//v1tp)
        call edmess('vcto: conversion = '//type)
      goto 999
c
 999  continue      
      call edmess('vcto: routine en echec')
      status=1
      return
      end
