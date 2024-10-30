      subroutine vcget(
c     == entrees ==
     & ov,pshp,ppos,
c     == sorties ==
     & pval,
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcget (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : acceder a un element d'un objet VECTOR
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cae  pshp   : shape du VECTOR
cae  ppos   : position de l'element du VECTOR
cas  pval   : un conteneur "values"
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
      pointeur ppos.values
      pointeur pval.values
      integer  status
c
      integer       ierr
      character*8   vatp
      integer       ipos
      integer       v1sz
      integer       iposmx
      integer       i
c
      status=0
c
c     -- check version of vector v
      call vc0ckv(ov,ierr)
      if (ierr.ne.0) goto 999
c
c     -- retourner position 1D equivalente a une multi-position
      call vc0pos(pshp,ppos,ipos,ierr)
      if(ierr .ne. 0) goto 999
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
      v1sz=max(nvcflo,nvcent,nvclog,nvccha)
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
        if(iposmx .ne. v1sz)then
          call edmess('vcget: shape et size incompatibles')
          goto 999
        endif
      endif
c
c     -- creer a vide un conteneur "values"
      vatp=vectyp
      call valnew(pval,vatp,ierr)
      if(ierr .ne. 0) goto 999
c
c     -- ouvrir le conteneur "values"
      segact,pval
      valnf =pval.valf(/1)
      valne =pval.vale(/1)
      valnl =pval.vall(/1)
      valnc =pval.valc(/2)
      valsmx=pval.valc(/1)
c
c     -- charger l'element dans le conteneur "values"
c     -- apres verification de la position
      if(vectyp .eq. 'FLOTTANT')then
        if(.not.(ipos.ge.1 .and. ipos.le.nvcflo))goto 998
        valnf=valnf+1
	segadj,pval
        pval.valf(valnf)=vecflo(ipos)
c
      elseif(vectyp .eq. 'ENTIER')then
        if(.not.(ipos.ge.1 .and. ipos.le.nvcent))goto 998
        valne=valne+1
	segadj,pval
        pval.vale(valne)=vecent(ipos)
c
      elseif(vectyp .eq. 'LOGIQUE')then
        if(.not.(ipos.ge.1 .and. ipos.le.nvclog))goto 998
        valnl=valnl+1
	segadj,pval
        pval.vall(valnl)=veclog(ipos)
c
      elseif(vectyp .eq. 'CHAINE')then
        if(.not.(ipos.ge.1 .and. ipos.le.nvccha))goto 998
        valnc=valnc+1
	valsmx=max(valsmx,cvccha)
	segadj,pval
        pval.vals(valnc)=cvccha
        pval.valc(valnc)=veccha(ipos)
      endif
c
c     -- fermer le conteneur "values"
      segdes,pval
c
c     -- fermer le VECTOR
      segdes,tvecto*NOMOD
      call desstr(ov,'NOMOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 998  continue  
      call edmess('vcget: position hors domaine')
      goto 999    
c
 999  continue      
      call edmess('vcget: routine en echec')
      status=1
      return
      end
