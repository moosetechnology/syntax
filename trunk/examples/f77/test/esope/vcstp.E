      subroutine vcstp(ov1,pshp1,pbeg1,pend1,ov2,status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcstp (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : changer une partie des elements avec un vecteur
c
cm   auteur    : L.Brault
cm   service   : EPDN
cm   date      : 31-05-2001
cm   objet     : corriger l'implementation dans le cas hors colonne
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov1    : objet VECTOR a changer
cas  ov1    : objet VECTOR a changer
cae  pshp1  : shape du VECTOR
cae  pbeg1  : position de depart
cae  pend1  : position de fin   
cas  ov2    : objet VECTOR 
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
      integer  ov2
      integer  ov1
      pointeur pshp1.values
      pointeur pbeg1.values
      pointeur pend1.values
      integer  status
c
      pointeur pv2.tvecto
      pointeur pv1.tvecto
c
      pointeur ppos1.values
      pointeur pbot1.values
      pointeur ptop1.values
      integer  ierr
      integer  v2sz
      integer  v1sz
      integer  shp1sz
      integer  ibeg1
      integer  iend1
      integer  ioff1
      integer  idim1
      integer  iposmx
      integer  ipos
      integer  i
      integer  k
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
c     -- verifier le type de v1 et v2
      if(pv1.vectyp .ne. pv2.vectyp)then
        call edmess('vcstp: arguments de type differents')
	goto 999
      endif
c
c     -- si shape fournie, verifier compatiblite avec size
      if(pshp1 .ne. 0)then
        segact,pshp1
        iposmx=1
        do i=1,pshp1.vale(/1)
          iposmx=iposmx*pshp1.vale(i)
	enddo
        segdes,pshp1*NOMOD
c
        if(iposmx .ne. v1sz)then
          call edmess('vcstp: shape et size incompatibles')
          goto 999
        endif
      endif
c
c     -- retourner begin-1D equivalente a une multi-position
      call vc0pos(pshp1,pbeg1,ibeg1,ierr)
      if(ierr .ne. 0) goto 999
c
c     -- verifier la position de begin
      if(v1sz.ge.1)then
        if(.not.(ibeg1.ge.1 .and. ibeg1.le.v1sz))then
          call edmess('vcstp: BEGIN hors domaine')
	  goto 999
	endif
      else
        if(ibeg1.ne.0)then
          call edmess('vcstp: BEGIN hors domaine')
	  goto 999
	endif
      endif
c
c     -- retourner end-1D equivalente a une multi-position
      call vc0pos(pshp1,pend1,iend1,ierr)
      if(ierr .ne. 0) goto 999
c
c     -- verifier la position de end
      if(v1sz.ge.1)then
        if(.not.(iend1.ge.1 .and. iend1.le.v1sz))then
          call edmess('vcstp: END hors domaine')
	  goto 999
	endif
      else
        if(iend1.ne.0)then
          call edmess('vcstp: END hors domaine')
	  goto 999
	endif
      endif
c
c     -- calculer la taille de intervalle [begin, end]
      if(pshp1 .ne. 0)then
        segact,pshp1
        shp1sz=pshp1.vale(/1)
c
        segact,pbeg1
        segact,pend1
        segini,pbot1=pbeg1
        segini,ptop1=pend1
c
        idim1=1
        do i=1,shp1sz
          pbot1.vale(i)=min(pbeg1.vale(i), pend1.vale(i))
          ptop1.vale(i)=max(pbeg1.vale(i), pend1.vale(i))
          idim1=idim1*(ptop1.vale(i) - pbot1.vale(i) + 1)
        enddo
c
      else
        idim1=max(0,iend1-ibeg1+1)
      endif
      if(v1sz .le. 0)then
        idim1=0
      endif
c
c     -- verifier compatibilite [begin, end] et taille de v2
      if(v2sz .ne. idim1)then
        call edmess(
     &  'vcstp: (BEGIN,END) de v1 et taille de v2 incompatibles')
	goto 999
      endif
c
c     -- changer les elements
      if(pshp1 .ne. 0)then
c       == multi-indice
c
c       -- positionner au depart tous les indices
        segini,ppos1=pshp1
        do i=1,shp1sz
          ppos1.vale(i)=pbot1.vale(i)
        enddo
c
        do k=1,idim1
c
      	  call vc0pos(pshp1,ppos1,ipos,ierr)
      	  if(ierr .ne. 0) goto 999
	  segact,pshp1
	  segact,ppos1
c
          if(pv1.vectyp .eq. 'FLOTTANT')then
            pv1.vecflo(ipos)=pv2.vecflo(k)
c
          elseif(pv1.vectyp .eq. 'ENTIER')then
            pv1.vecent(ipos)=pv2.vecent(k)
c
          elseif(pv1.vectyp .eq. 'LOGIQUE')then
            pv1.veclog(ipos)=pv2.veclog(k)
c
          elseif(pv1.vectyp .eq. 'CHAINE')then
            pv1.veccha(ipos)=pv2.veccha(k)
	  endif
c
c         -- incrementer un indice
          do 10, i=1,shp1sz
            if(ppos1.vale(i) .lt. ptop1.vale(i))then
              ppos1.vale(i)=ppos1.vale(i)+1
	      goto 11
            else
              ppos1.vale(i)=pbot1.vale(i)
            endif
 10       continue
 11       continue
c
	enddo
c
        segsup,ppos1
        segsup,pbot1
        segsup,ptop1
c
        segdes,pshp1*NOMOD
        segdes,pbeg1*NOMOD
        segdes,pend1*NOMOD
c
      else
c       == mono-indice
      	ioff1=ibeg1-1
c
      	if(pv1.vectyp .eq. 'FLOTTANT')then
	  do i=1,v2sz
	    pv1.vecflo(ioff1+i)=pv2.vecflo(i)
	  enddo
c
      	elseif(pv1.vectyp .eq. 'ENTIER')then
	  do i=1,v2sz
	    pv1.vecent(ioff1+i)=pv2.vecent(i)
	  enddo
c
      	elseif(pv1.vectyp .eq. 'LOGIQUE')then
	  do i=1,v2sz
	    pv1.veclog(ioff1+i)=pv2.veclog(i)
	  enddo
c
      	elseif(pv1.vectyp .eq. 'CHAINE')then
	  do i=1,v2sz
	    pv1.veccha(ioff1+i)=pv2.veccha(i)
	  enddo
      	endif
      endif
c
c     -- fermer le VECTOR v2
      segdes,pv2*NOMOD
      call desstr(ov2,'NOMOD')
c
c     -- fermer le VECTOR v1
      segdes,pv1*MOD
      call desstr(ov1,'MOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 999  continue      
      call edmess('vcstp: routine en echec')
      status=1
      return
      end
