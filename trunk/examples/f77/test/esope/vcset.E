      subroutine vcset(
c     == entrees/sorties ==
     & ov,
c     == entrees ==
     & pshp,ppos,pval,
c     == sorties ==
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcset (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : changer un element d'un objet VECTOR
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
cae  pshp   : shape du VECTOR
cae  ppos   : position de l'element du VECTOR
cas  pval   : un conteneur "values"
cas  ov     : un objet VECTOR
cas  status : status d'execution (status=0 <=> succes)
c
#include <implicit.h>
c
      integer  mypnt
      external mypnt
c
      integer  valsiz
      external valsiz
c
      character*8  valtpe
      external     valtpe
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
      integer       ipos
      character*8   vatp
      integer       vasz
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
          call edmess('vcset: shape et size incompatibles')
          goto 999
        endif
      endif
c
c     -- verifier la concordance des types
      vatp=valtpe(pval,ierr)
      if(ierr .ne. 0) goto 999
c
      if(vatp .ne. vectyp)then
        call edmess('vcset: VECTOR et VALUES pas meme type')
        goto 999
      endif
c
c     -- verifier la taille du conteneur "values"
      vasz=valsiz(pval,ierr)
      if(ierr .ne. 0) goto 999
c
      if(vasz .ne. 1)then
        call edmess('vcset: VALUES pas un singleton')
        goto 999
      endif
c
c     -- ouvrir le conteneur "values"
      segact,pval
      valnf =pval.valf(/1)
      valne =pval.vale(/1)
      valnl =pval.vall(/1)
      valnc =pval.valc(/2)
      valsmx=pval.valc(/1)
c
c     -- changer l'element apres verification de la position
      if(vectyp .eq. 'FLOTTANT')then
        if(.not.(ipos.ge.1 .and. ipos.le.nvcflo))goto 998
        vecflo(ipos)=pval.valf(1)
c
      elseif(vectyp .eq. 'ENTIER')then
        if(.not.(ipos.ge.1 .and. ipos.le.nvcent))goto 998
        vecent(ipos)=pval.vale(1)
c
      elseif(vectyp .eq. 'LOGIQUE')then
        if(.not.(ipos.ge.1 .and. ipos.le.nvclog))goto 998
        veclog(ipos)=pval.vall(1)
c
      elseif(vectyp .eq. 'CHAINE')then
        if(.not.(ipos.ge.1 .and. ipos.le.nvccha))goto 998
	veccha(ipos)=pval.valc(1)
      endif
c
c     -- fermer le conteneur "values"
      segdes,pval
c
c     -- fermer le VECTOR
      segdes,tvecto*MOD
      call desstr(ov,'MOD')
c
c     -- terminer avec succes      
      return
c
c     -- terminer en echec
 998  continue  
      call edmess('vcset: position hors domaine')
      goto 999    
c
 999  continue      
      call edmess('vcset: routine en echec')
      status=1
      return
      end
