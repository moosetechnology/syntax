      subroutine vcfndl(ov,pshp,pbeg,pend,fndinc,ppos,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcfndl (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : Rechercher la premiere valeur LOGIQUE "VRAI"
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cae  pshp   : shape du VECTOR
cae  pbeg   : position de depart de la recherche
cae  pend   : position de fin    de la recherche
cae  fndinc : sens de la recherche <=> increment +1/-1
cas  ppos   : resultat de la recherche
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
      integer  ov
      pointeur pshp.values
      pointeur pbeg.values
      pointeur pend.values
      integer  fndinc
      pointeur ppos.values
      integer  status
c
      character*8 vtp
      character*8 vatp
c
      integer  ierr
      integer  vsz
      integer  ibeg
      integer  iend
      integer  ipos
      integer  iposmx
      integer  i
c
      status=0
c
c     -- check version of vector v
      call vc0ckv(ov,ierr)
      if (ierr.ne.0) goto 999
c
c     -- ouvrir le VECTOR
      call actstr(ov)
      tvecto=mypnt(ov,1)
      segact,tvecto
      nvcflo=vecflo(/1)
      nvcent=vecent(/1)
      nvclog=veclog(/1)
      nvccha=veccha(/2)
      cvccha=veccha(/1)
      vsz=max(nvcflo,nvcent,nvclog,nvccha)
c
c     -- verifier le type de VECTOR
      if(vectyp .ne. 'LOGIQUE')then
        vtp=vectyp
        call edmess('vcfndl: operation non disponible sur ce type')
        call edmess('vcfndl: type = '//vtp)
        goto 999
      endif
c
c     -- verifier l'increment de la recherche
      if(.not.(fndinc.eq.1 .or. fndinc.eq.-1))then
         call edmess('vcfndl: fndinc pas +1 ou -1')
        goto 999
      endif
c
c     -- si shape fournie, verifier compatiblite avec size
      if(pshp .ne. 0)then
        segact,pshp
        iposmx=1
        do i=1,pshp.vale(/1)
          iposmx=iposmx*pshp.vale(i)
	enddo
        segdes,pshp*NOMOD
c
        if(iposmx .ne. vsz)then
          call edmess('vcfndl: shape et size incompatibles')
          goto 999
        endif
      endif
c
c     -- retourner begin-1D equivalente a une multi-position
      if(pbeg.ne.0)then
        call vc0pos(pshp,pbeg,ibeg,ierr)
        if(ierr .ne. 0) goto 999
      else
        ibeg=1
      endif
c
c     -- retourner end-1D equivalente a une multi-position
      if(pend.ne.0)then
        call vc0pos(pshp,pend,iend,ierr)
        if(ierr .ne. 0) goto 999
      else
        iend=vsz
      endif
c
c     -- chercher le premier ou le dernier .true.
      ipos=0
      ibeg=max(ibeg,1)
      iend=min(iend,vsz)
c
      if(fndinc.eq.1)then
	do i=ibeg,iend
          if(veclog(i))then
            ipos=i
            goto 10
          endif
	enddo
c
      elseif(fndinc.eq.-1)then
	do i=iend,ibeg,-1
          if(veclog(i))then
            ipos=i
            goto 10
          endif
	enddo
      endif
 10   continue
c
c     -- convertir le resultat en multi-position
      call vc0ppo(pshp,ppos,ipos,ierr)
      if(ierr.ne.0)goto 999
c
c     -- fermer le VECTOR
      segdes,tvecto*NOMOD
      call desstr(ov,'NOMOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcfndl: routine en echec')
      status=1
      return
      end
