      subroutine vcuna(ov,ov1,oper,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcuna (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : operation unaire sur les elements du VECTOR 
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
      integer     ov
      integer     ov1
      character*8 oper
      integer     status
c
      pointeur    pv.tvecto
      pointeur    pv1.tvecto
c
      character*8 type
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
c     -- creer les nouveaux elements d'apres l'operation unaire
      if(pv1.vectyp .eq. 'FLOTTANT')then
c
        if(oper .eq. 'ABS')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=abs(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'CHS')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=-pv1.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'INV')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=1./pv1.vecflo(i)
	  enddo
c
        elseif(oper .eq. 'SQRT')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=sqrt(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'LOG')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=log(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'EXP')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=exp(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'SIN')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=sin(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'COS')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=cos(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'TAN')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=tan(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'ASIN')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=asin(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'ACOS')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=acos(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'ATAN')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=atan(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'SINH')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=sinh(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'COSH')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=cosh(pv1.vecflo(i))
	  enddo
c
        elseif(oper .eq. 'TANH')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcflo
            pv.vecflo(i)=tanh(pv1.vecflo(i))
	  enddo
	else
          goto 998
	endif
c
      elseif(pv1.vectyp .eq. 'ENTIER')then
c
        if(oper .eq. 'ABS')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=abs(pv1.vecent(i))
	  enddo
c
        elseif(oper .eq. 'CHS')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=-pv1.vecent(i)
	  enddo
c
        elseif(oper .eq. 'INV')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvcent
            pv.vecent(i)=1/pv1.vecent(i)
	  enddo
	else
          goto 998
	endif
c
      elseif(pv1.vectyp .eq. 'LOGIQUE')then
c
        if(oper .eq. 'NOT')then
          segadj,pv
	  pv.vectyp = pv1.vectyp
c
	  do i=1,nvclog
            pv.veclog(i)=.not. pv1.veclog(i)
	  enddo
	else
          goto 998
	endif
c
      elseif(pv1.vectyp .eq. 'CHAINE')then
        goto 998
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
        type=pv1.vectyp
        call edmess('vcuna: operation non disponible sur ce type')
        call edmess('vcuna: operation = '//oper)
        call edmess('vcuna:      type = '//type)
      goto 999
c
 999  continue      
      call edmess('vcuna: routine en echec')
      status=1
      return
      end
