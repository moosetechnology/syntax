      subroutine vcsta(
c     == entrees/sorties ==
     & ov,
c     == entrees ==
     & pval,
c     == sorties ==
     & status)
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
cb  Subroutine : vcsta (langage Esope)
cb  Module     : VECTOR
cb  Auteur     : L.Brault
cb  Date       : Octobre-2000
cb  Service    : EPDN 
cb  But        : changer tous les elements avec une valeur donnee
c xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
c
cae  ov     : un objet VECTOR
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
      pointeur pval.values
      integer  status
c
      integer       ierr
      integer       i
      character*8   vatp
      integer       vasz
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
c
c     -- verifier la concordance des types
      vatp=valtpe(pval,ierr)
      if(ierr .ne. 0) goto 999
c
      if(vatp .ne. vectyp)then
        call edmess('vcsta: VECTOR et VALUES pas meme type')
        goto 999
      endif
c
c     -- verifier la taille du conteneur "values"
      vasz=valsiz(pval,ierr)
      if(ierr .ne. 0) goto 999
c
      if(vasz .ne. 1)then
        call edmess('vcsta: VALUES pas un singleton')
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
c     -- changer les elements
      if(vectyp .eq. 'FLOTTANT')then
        do i=1,nvcflo
          vecflo(i)=pval.valf(1)
	enddo
c
      elseif(vectyp .eq. 'ENTIER')then
        do i=1,nvcent
          vecent(i)=pval.vale(1)
	enddo
c
      elseif(vectyp .eq. 'LOGIQUE')then
        do i=1,nvclog
          veclog(i)=pval.vall(1)
	enddo
c
      elseif(vectyp .eq. 'CHAINE')then
        do i=1,nvccha
          veccha(i)=pval.valc(1)
	enddo
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
 999  continue      
      call edmess('vcsta: routine en echec')
      status=1
      return
      end
